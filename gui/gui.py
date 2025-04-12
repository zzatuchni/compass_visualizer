import sys
import serial
import datetime
import time
import traceback
import numpy as np
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt6.QtWidgets import QApplication, QSizePolicy, QWidget, QMainWindow, QVBoxLayout, QHBoxLayout, QComboBox, QPushButton, QDialogButtonBox, QLabel, QDialog
from PyQt6.QtCore import QThread, pyqtSignal

NUM_POINTS = 1000
BATCH_SIZE = 25

class CustomDialog(QDialog):
    def __init__(self, title, message, parent=None):
        super().__init__(parent)

        self.setWindowTitle(title)

        QBtn = (
            QDialogButtonBox.StandardButton.Ok
        )

        self.buttonBox = QDialogButtonBox(QBtn)
        self.buttonBox.accepted.connect(self.accept)

        layout = QVBoxLayout()
        message = QLabel(message)
        layout.addWidget(message)
        layout.addWidget(self.buttonBox)
        self.setLayout(layout)

class MyMplCanvas(FigureCanvas):
    def __init__(self, parent=None, width=10, height=7, dpi=200):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)
        self.axes.plot()
        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Policy.Expanding,
                QSizePolicy.Policy.Expanding)
        FigureCanvas.updateGeometry(self)

class LineGraph(MyMplCanvas):

    def __init__(self, parent):
        super().__init__(parent)

        self.points_t = []
        self.points_s = []
        self.points_at = []
        self.points_as = []

    def update_figure(self, i):
        self.axes.cla()

        self.points_t = self.points_t + i[0]
        self.points_s = self.points_s + i[1]

        self.points_at = self.points_at + [i[2]]
        self.points_as = self.points_as + [i[3]]

        if len(self.points_t) > NUM_POINTS:
            self.points_t = self.points_t[BATCH_SIZE-1:]
            self.points_s = self.points_s[BATCH_SIZE-1:]
            self.points_at = self.points_at[1:]
            self.points_as = self.points_as[1:]

        self.axes.plot(self.points_t, self.points_s)
        self.axes.plot(self.points_at, self.points_as)
        self.draw()

    def clear_figure(self):
        self.axes.cla()
        self.points_t = []
        self.points_s = []
        self.points_at = []
        self.points_as = []
        self.axes.plot([], [])
        self.draw()

class Compass(MyMplCanvas):

    def __init__(self, parent):
        super().__init__(parent)
        self.style_compass()

    def style_compass(self):
        self.axes.cla()
        self.axes.set_xticks([])
        self.axes.set_yticks([])
        self.axes.set_xticklabels([])
        self.axes.set_yticklabels([])
        self.axes.set_aspect("equal")
        self.axes.set_xlim(-1.1, 1.1)
        self.axes.set_ylim(-1.1, 1.1)

        self.axes.set_xlabel("S")
        self.axes.set_ylabel("W")
    
        secax = self.axes.secondary_xaxis('top')
        secax.set_xlabel('N')
        secax.set_xticklabels([])
        secax.set_xticks([])
        secay = self.axes.secondary_yaxis('right')
        secay.set_ylabel('E')
        secay.set_yticklabels([])
        secay.set_yticks([])

    def update_figure(self, angle):
        self.style_compass()

        angle = np.radians(-1*angle)

        R = np.array([[np.cos(angle), -np.sin(angle)],
                  [np.sin(angle), np.cos(angle)]])
    
        vec = R @ [0, 1]

        self.axes.quiver(0, 0, vec[0], vec[1], angles='xy', scale_units='xy', scale=1)
        self.draw()

    def clear_figure(self):
        self.style_compass()

        self.draw()

class Worker(QThread):
    progress_update = pyqtSignal(object)
    worker_error = pyqtSignal(object)

    def __init__(self, port_name, logs_enabled):
        super().__init__()
        self.port_name = port_name
        self.logs_enabled = logs_enabled

    def get_from_serial(self):
        i = 0
        num = b''
        while i < 2:
            byte  = self.ser.read(1)
            if byte:
                num = byte + num
                i = i + 1

        return int.from_bytes(num, signed=True)
    
    def connect_to_serial(self, port_name):
        return serial.Serial(
            port=port_name,\
            baudrate=115200,\
            parity=serial.PARITY_NONE,\
            stopbits=serial.STOPBITS_ONE,\
            bytesize=serial.EIGHTBITS,\
            timeout=0)
    
    def get_angle(self, x, y):            
        angle = np.atan2(-1*y, x) * (360 / (2 * np.pi))
        if angle < 0:
            angle = angle + 360
        return angle
    
    def worker_loop(self, file=None):
        times = []
        angles = []

        point_num = 1
        t0 = time.time()
        t_prev = t0;

        period_count = 0
        avg_freq = 0

        x_count = 0
        y_count = 0

        batch_count = 1;

        if self.logs_enabled:
            file.write("Timestamp,Angle\n")

        while True:
            # get data here
            x = self.get_from_serial();
            y = self.get_from_serial();
            t1 = time.time()

            #print(x, y)
            if abs(x) > 250:
                x = 0
            if abs(y) > 250:
                y = 0

            angle = self.get_angle(x,y)

            times.append(float(t1-t0))
            angles.append(float(angle))

            x_count = x_count + x
            y_count = y_count + y

            if batch_count == BATCH_SIZE:

                x_avg = x_count / batch_count
                y_avg = y_count / batch_count
                avg_angle = self.get_angle(x_avg,y_avg)

                self.progress_update.emit([times, angles, t1-t0, avg_angle])

                if self.logs_enabled:
                    file.write(str(t1-t0)+","+str(avg_angle)+"\n")

                times = []
                angles = []
                
                x_count = 0
                y_count = 0
                batch_count = 1

                print("avg freq:", avg_freq)
                print("avg angle:", avg_angle)

            if self.isInterruptionRequested():
                print("Interrupt received")
                break

            point_num = point_num + 1
            period_count = period_count + (t1 - t_prev)
            t_prev = t1
            avg_freq = 1 / (period_count / point_num)

            batch_count = batch_count + 1;

    def run(self):
        print("Worker started")

        if (self.port_name == "Auto"):
            ok = False
            for i in range(0, 256):
                try:
                    self.ser = self.connect_to_serial("COM"+str(i))
                    ok = True
                except:
                    pass
            if not ok:
                self.worker_error.emit(["Error", "Could not find serial port to connect to"])
                return
        else:
            try:
                self.ser = self.connect_to_serial(self.port_name)
            except:
                print(traceback.format_exc())
                self.worker_error.emit(["Error", "Could not connect to "+self.port_name])
                return


        print("Connected to: " + self.ser.portstr)

        if self.logs_enabled:
            print("Logs enabled on this run")
            filename = "sensor_log_"+datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")+".csv"
            with open(filename, "a") as f:
                self.worker_loop(file=f)
        else:
            self.worker_loop()

        self.ser.close()

class ApplicationWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setStyleSheet('font-size: 35px;')

        self.main_widget = QWidget()
        layout = QVBoxLayout(self.main_widget)
        sub_widget = QWidget()
        graph_layout = QHBoxLayout(sub_widget)

        self.line_graph = LineGraph(self.main_widget)
        self.compass = Compass(self.main_widget)

        self.com_port_selector = QComboBox()
        self.com_port_selector.addItems(['Auto', 'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7'])

        self.logs_enabled_selector = QComboBox()
        self.logs_enabled_selector.addItems(["Logs disabled", "Logs enabled"])

        self.com_port_selector.currentTextChanged.connect(self.com_port_changed)

        self.start_button = QPushButton(text="START")
        self.start_button.clicked.connect(self.start_button_clicked)

        self.stop_button = QPushButton(text="STOP")
        self.stop_button.clicked.connect(self.stop_button_clicked)
        self.stop_button.setEnabled(False)

        self.clear_button = QPushButton(text="CLEAR")
        self.clear_button.clicked.connect(self.clear_button_clicked)

        graph_layout.addWidget(self.line_graph)
        graph_layout.addWidget(self.compass)
        layout.addWidget(sub_widget)
        layout.addWidget(self.com_port_selector)
        layout.addWidget(self.start_button)
        layout.addWidget(self.stop_button)
        layout.addWidget(self.clear_button)
        layout.addWidget(self.logs_enabled_selector)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)


    def com_port_changed(self, s):
        print("COM port changed to", s)
        pass

    def start_button_clicked(self):
        print("Start button clicked")
        self.worker = Worker(self.com_port_selector.currentText(), "enabled" in self.logs_enabled_selector.currentText().lower())
        self.worker.progress_update.connect(self.worker_progress_update)
        self.worker.worker_error.connect(self.worker_error)

        self.line_graph.clear_figure()
        self.start_button.setEnabled(False)
        self.clear_button.setEnabled(False)
        self.worker.start()
        self.stop_button.setEnabled(True)
        pass

    def stop_button_clicked(self):
        print("Stop button clicked")
        self.stop_button.setEnabled(False)
        self.worker.requestInterruption()
        self.start_button.setEnabled(True)
        self.clear_button.setEnabled(True)
        pass

    def clear_button_clicked(self):
        print("Clear button clicked")
        self.line_graph.clear_figure()
        self.compass.clear_figure()
        pass

    def worker_progress_update(self, i):
        print("Worker progress update")
        #print(i)
        self.line_graph.update_figure(i)
        self.compass.update_figure(i[3])
        pass

    def worker_error(self, i):
        print("Error message")
        dlg = CustomDialog(i[0], i[1])
        dlg.exec()

        self.stop_button.setEnabled(False)
        self.start_button.setEnabled(True)
        self.clear_button.setEnabled(True)

        pass

    def worker_finished_self(self):
        print("Worker thread finished")
        pass
    

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = ApplicationWindow()
    win.setWindowTitle("PyQt Matplotlib App Demo")
    win.show()
    sys.exit(app.exec())