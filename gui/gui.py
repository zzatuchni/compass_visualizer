import sys
import serial
import time
import random
import numpy as np
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt6.QtWidgets import QApplication, QSizePolicy, QWidget, QMainWindow, QVBoxLayout, QComboBox, QPushButton
from PyQt6.QtCore import QRunnable, QThreadPool, QThread, pyqtSignal

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

class MyStaticMplCanvas(MyMplCanvas):

    def __init__(self, parent):
        super().__init__(parent)

        self.num_points = 45;
        self.points_t = []
        self.points_s = []

    def update_figure(self, i):
        self.axes.cla()

        self.points_t.append(float(i[1]))
        self.points_s.append(float(i[2]))

        if len(self.points_t) > self.num_points:
            self.points_t = self.points_t[1:]
            self.points_s = self.points_s[1:]

        print(self.points_t)
        print(self.points_s)

        self.axes.plot(self.points_t, self.points_s)
        self.draw()

    def clear_figure(self):
        self.axes.cla()
        self.points_t = []
        self.points_s = []
        self.axes.plot([], [])
        self.draw()

class Worker(QThread):
    progress_update = pyqtSignal(object)

    def __init__(self):
        super().__init__()

    def get_from_serial(self):
        i = 0
        num = b''
        while i < 2:
            byte  = self.ser.read(1)
            if byte:
                num = byte + num
                i = i + 1

        return int.from_bytes(num, signed=True)

    def run(self):
        print("Worker started")

        self.ser = serial.Serial(
            port='COM4',\
            baudrate=115200,\
            parity=serial.PARITY_NONE,\
            stopbits=serial.STOPBITS_ONE,\
            bytesize=serial.EIGHTBITS,\
                timeout=0)

        print("Connected to: " + self.ser.portstr)

        point_num = 0
        t0 = time.time()
        while True:
            # get data here
            x = self.get_from_serial();
            y = self.get_from_serial();
            t1 = time.time()

            angle = np.atan2(y, x) * (360 / (2 * np.pi))

            self.progress_update.emit([point_num, t1-t0, angle])

            if self.isInterruptionRequested():
                print("Interrupt received")
                break

            point_num = point_num + 1
            #time.sleep(0.5)

class ApplicationWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setStyleSheet('font-size: 35px;')

        self.main_widget = QWidget()
        layout = QVBoxLayout(self.main_widget)

        self.worker = Worker()
        self.worker.progress_update.connect(self.worker_progress_update)

        self.mpl_static_canvas = MyStaticMplCanvas(self.main_widget)

        self.com_port_selector = QComboBox()
        self.com_port_selector.addItems(['Auto', 'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7'])
        self.com_port_selector.currentTextChanged.connect(self.com_port_changed)

        self.start_button = QPushButton(text="START")
        self.start_button.clicked.connect(self.start_button_clicked)

        self.stop_button = QPushButton(text="STOP")
        self.stop_button.clicked.connect(self.stop_button_clicked)
        self.stop_button.setEnabled(False)

        self.clear_button = QPushButton(text="CLEAR")
        self.clear_button.clicked.connect(self.clear_button_clicked)

        layout.addWidget(self.mpl_static_canvas)
        layout.addWidget(self.com_port_selector)
        layout.addWidget(self.start_button)
        layout.addWidget(self.stop_button)
        layout.addWidget(self.clear_button)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)


    def com_port_changed(self, s):
        print("COM port changed to", s)
        pass

    def start_button_clicked(self):
        print("Start button clicked")
        self.mpl_static_canvas.clear_figure()
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
        self.mpl_static_canvas.clear_figure()
        pass

    def worker_progress_update(self, i):
        print("Worker progress update", i[0])
        self.mpl_static_canvas.update_figure(i)

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