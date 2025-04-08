import serial
import traceback

ser = serial.Serial(
    port='COM4',\
    baudrate=115200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

print("connected to: " + ser.portstr)
count=1

def get_num(s):
    i = 0
    num = b''
    while i < 2:
        byte  = s.read(1)
        if byte:
            num = byte + num
            i = i + 1

    return int.from_bytes(num, signed=True)

while True:
    try:
        x = get_num(ser)
        y = get_num(ser)
        print(x, y)
    except:
        print("error")
        print(traceback.format_exc())
        break

ser.close()
