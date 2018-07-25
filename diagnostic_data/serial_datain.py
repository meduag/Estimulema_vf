import sys
import time

from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QApplication
import serial
import threading

serPort1 = "COM13"
serPort2 = "COM14"
baudRate = 2000000

"""
ser1_Teensy_Stim = serial.Serial(serPort1, baudRate, timeout=1, xonxoff=1)
print("Serial port: " + serPort1 + ", Baud rate:" + str(baudRate))

ser2_Teensy_Accel = serial.Serial(serPort2, baudRate, timeout=1, xonxoff=1)
print("Serial port: " + serPort2 + ", Baud rate:" + str(baudRate))

"""
arrayt = []

# lock to serialize console output
lock = threading.Lock()

# lock to serialize console output
start = False
rh_ch = False  # false = rh, true = cr

msg_bytes = ''
limit_ma = 0
limit_pw = 0
Ton = 0
Toff = 0
limit_start = 0


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Set up the user interface from Designer.
        uic.loadUi("Diag_dataControl.ui", self)

        # Connect up the buttons.
        self.pushButton_rh.clicked.connect(self.btn_start_rh)
        self.pushButton_cr.clicked.connect(self.btn_start_cr)
        self.pushBtn_Stop.clicked.connect(self.btn_stop_stim)
        self.spinBox_limit_mA.valueChanged.connect(self.update_var)
        self.spinBox_limit_pW.valueChanged.connect(self.update_var)

        # Show interface
        self.show()

    def btn_start_rh(self):
        global msg_bytes
        global start
        global limit_ma
        global limit_pw
        global limit_start
        global rh_ch

        self.update_var()
        msg = "R>N>" + str(limit_ma) + ">" + str(limit_pw) + ">"
        print(msg)
        msg_bytes = str.encode(msg)
        start = True
        rh_ch = False
        limit_start = 5

    def btn_start_cr(self):
        global msg_bytes
        global start
        global limit_ma
        global limit_pw
        global limit_start
        global rh_ch

        self.update_var()
        msg = "C>N>" + str(limit_ma) + ">" + str(limit_pw) + ">"
        print(msg)
        msg_bytes = str.encode(msg)
        start = True
        rh_ch = True
        limit_start = int(limit_pw / 100)
        print(limit_start)

    @staticmethod
    def btn_stop_stim():
        global msg_bytes
        global start
        global limit_ma

        msg = "A>Y>"
        msg_bytes = str.encode(msg)
        start = False

    def update_var(self):
        global limit_ma
        global limit_pw
        limit_ma = self.spinBox_limit_mA.value()
        limit_pw = self.spinBox_limit_pW.value()


def read_serial(port, baud):
    global msg_bytes
    global start
    global arrayt
    global limit_ma

    cont = 0

    ser = serial.Serial()
    ser.port = port
    ser.timeout = 1
    ser.baudrate = baud
    ser.xonxoff = 1

    try:
        ser.open()
    except Exception as e:
        print("error open serial port: " + str(e))
        exit()

    if ser.isOpen():
        try:
            while start is False:
                pass

            ser.write(msg_bytes)

            while start:
                c = ser.readline()
                with lock:
                    # time.sleep(0)
                    if len(c) > 0:
                        str_msn = c.decode("utf-8")
                        str_msn = str_msn.rstrip()
                        print(str_msn)
                        arrayt.append(str_msn)
                    else:
                        cont = cont + 1
                        if cont == limit_ma + limit_start:
                            start = False
                            print("Terminou Captura")

            save_data()
            ser.close()

        except Exception as e1:
            print("error communicating...: " + str(e1))

    else:
        print("cannot open serial port ")
        exit()


def save_data():
    global arrayt
    global limit_ma
    global rh_ch
    if rh_ch is False:
        stim_r = open('stim_r.txt', 'w')
        acel_r = open('acel_r.txt', 'w')
    elif rh_ch is True:
        stim_c = open('stim_c.txt', 'w')
        acel_c = open('acel_c.txt', 'w')

    char = "0;"
    x = ""
    mA = ""
    tp = "0"
    strEnd = ""

    for x in arrayt:
        if "M" in x:
            y = len(x) - 1
            mA = ""
            c = 0
            close = False
            while c < y and close is False:
                if x[c+1] != ";":
                    mA = mA + x[c+1]
                    c += 1
                else:
                    close = True
                    c += 2
            if "T" in x:
                if x[c + 1] is "0":
                    tp = "0"
                else:
                    tp = ""
                    while c < y:
                        if x[c + 1] != ";":
                            tp = tp + x[c + 1]
                            c += 1
                        else:
                            c += 1
            if rh_ch is False:
                if int(mA) > limit_ma:
                    mA = str(limit_ma)
            elif rh_ch is True:
                if int(mA) > limit_pw:
                    mA = str(limit_pw)
        elif "P" in x:
            char = "1;"
            c = 2
            y = len(x) - 1
            if x[c + 1] is "0":
                tp = "0"
            else:
                tp = ""
                while c < y:
                    if x[c + 1] != ";":
                        tp = tp + x[c + 1]
                        c += 1
                    else:
                        c += 1
        elif "N" in x:
            char = "2;"
            c = 2
            y = len(x) - 1
            if x[c + 1] is "0":
                tp = "0"
            else:
                tp = ""
                while c < y:
                    if x[c + 1] != ";":
                        tp = tp + x[c + 1]
                        c += 1
                    else:
                        c += 1
        elif "R" in x:
            char = "0;"
        else:
            if "1" in char:
                char = "1;"
            elif "2" in char:
                char = "2;"
            elif "0" in char:
                char = "0;"
            else:
                char = "0;"

        if "M" in x or "P" in x or "N" in x or "R" in x or "T" in x:
            print("no se imprimió: " + x)
            strEnd = char + mA + ";" + tp
            tp = "0"
            if rh_ch is False:
                stim_r.write(strEnd)
                stim_r.write("\n")
            elif rh_ch is True:
                stim_c.write(strEnd)
                stim_c.write("\n")

            print(strEnd)
        else:
            # strEnd = char + mA + ";" + x
            #strEnd = x
            #tp = "0"
            if rh_ch is False:
                acel_r.write(x)
                acel_r.write("\n")
            elif rh_ch is True:
                acel_c.write(x)
                acel_c.write("\n")
            print(strEnd)

    acel.close()
    stim.close()
    print("Terminou generacion de archivo")

# Create two threads as follows
try:
    t1 = threading.Thread(target=read_serial, args=(serPort1, baudRate))
    t1.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
    t1.start()

    t1 = threading.Thread(target=read_serial, args=(serPort2, baudRate))
    t1.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
    t1.start()

except:
    print("Error: unable to start thread")

#save_data()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())

