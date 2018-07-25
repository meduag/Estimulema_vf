import sys

from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QApplication
import serial

serPort1 = "COM13"
baudRate = 2000000

ser_stim = serial.Serial()
ser_stim.port = serPort1
ser_stim.baudrate = baudRate
ser_stim.timeout = 1
ser_stim.xonxoff = 1

try:
    ser_stim.open()
except Exception as e:
    print("error open serial port: " + str(e))
    exit()

print("Serial port: " + serPort1 + ", Baud rate:" + str(baudRate))

limit_ma = 0
start = False
arrayt = []


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Set up the user interface from Designer.
        uic.loadUi("Diag_dataControl.ui", self)

        # Connect up the buttons.
        self.pushBtnRh.clicked.connect(self.btn_start_rh)
        self.pushBtnCr.clicked.connect(self.btn_start_cr)
        self.pushBtn_Stop.clicked.connect(self.btn_stop_stim)
        self.spinBox_limit_mA.valueChanged.connect(self.update_var)

        # Show interface
        self.show()

    def btn_start_rh(self):
        global msg_bytes
        global limit_ma
        global start
        self.update_var()
        msg = "R>N>" + str(limit_ma) + ">"
        print(msg)
        msg_bytes = str.encode(msg)
        ser_stim.write(msg_bytes)
        start = True
        self.read_data()

    def btn_start_cr(self):
        global msg_bytes
        global limit_ma
        self.update_var()
        msg = "C>N>" + str(limit_ma) + ">"
        print(msg)
        msg_bytes = str.encode(msg)
        ser_stim.write(msg_bytes)

    @staticmethod
    def btn_stop_stim():
        global msg_bytes
        msg = "A>Y>0>"
        msg_bytes = str.encode(msg)
        ser_stim.write(msg_bytes)

    def update_var(self):
        global limit_ma
        limit_ma = self.spinBox_limit_mA.value()

    def read_data(self):
        global start
        global limit_ma

        str_msn = ""
        cont = 0

        while start:
            c = ser_stim.readline()
            if len(c) > 0:
                str_msn = c.decode("utf-8")
                str_msn = str_msn.rstrip()
                print(str_msn)
                arrayt.append(str_msn)
            else:
                cont = cont + 1
                if cont == limit_ma:
                    start = False
                    print("Terminou Captura")

        self.save_data()

    def save_data(self):
        global arrayt
        global limit_ma

        stim = open('stim.txt', 'w')

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
                    if x[c + 1] != ";":
                        mA = mA + x[c + 1]
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

                if int(mA) > limit_ma:
                    mA = str(limit_ma)
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
                stim.write(strEnd)
                stim.write("\n")
                print(strEnd)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())

