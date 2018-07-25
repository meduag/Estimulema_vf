import sys
from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QApplication
import serial


# First String communication - global variables
valC = 0
max_pulse_neg = 1330
max_pulse_pos = 1432
pos_neg = 0
ch1_ch2 = 0


# ------------------------------------------------------------------------------------------ #
# Serial communication configurations
# serial port
serPort = "COM13"
baudRate = 2000000
# Open the serial port
ser = serial.Serial(serPort, baudRate, timeout=1, xonxoff=1)
# Print configurations
print("Serial port: " + serPort + ", Baud rate:" + str(baudRate))


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Set up the user interface from Designer.
        uic.loadUi("gui_diag_direct.ui", self)

        # Connect up the buttons.
        # self.pushBtnStart.clicked.connect(self.btn_clk_start)
        self.pushBtnStop.clicked.connect(self.btn_clk_stop)
        self.Slider_CurrentCH1.sliderReleased.connect(self.update_value)
        self.spinBox_mACH1.valueChanged.connect(self.val_change_ma1)

        # Show interface
        self.show()

    @staticmethod
    def btn_clk_start():
        print("Hola")

    @staticmethod
    def btn_clk_stop():
        msg = "0>0>2>"
        print("Teste Paro - STOP")
        msg_bytes = str.encode(msg)
        ser.write(msg_bytes)
        ser.close()

    def update_value(self):
        global pos_neg, ch1_ch2

        self.spinBox_mACH1.setValue(self.Slider_CurrentCH1.value())

        if self.rb_pos.isChecked() is True:
            pos_neg = 1
        else:
            pos_neg = 0

        if self.rb_ch1.isChecked() is True:
            ch1_ch2 = 0

        if self.rb_ch2.isChecked() is True:
            ch1_ch2 = 1

        if self.rb_chx.isChecked() is True:
            ch1_ch2 = 2

    def val_change_ma1(self):
        global valC, pos_neg, ch1_ch2
        self.update_value()

        valC = self.spinBox_mACH1.value()

        # get global value of Therapy for Channel 1
        # print("Valor em milliamps: " + str(valC))
        valf = self.map_ar(valC, 0, 100, 0, 2047)
        val = int(valf)
        print("Valor para o DAC: " + str(val))

        msg = str(valC) + ">" + str(pos_neg) + ">" + str(ch1_ch2) + ">"
        print("Valor enviado: " + msg)
        msg_bytes = str.encode(msg)
        ser.write(msg_bytes)
        """ msn = ser.readline()
        print(msn)
        msn = ser.readline()
        print(msn)
        msn = ser.readline()
        print(msn) """

    # map function Arduino on Python
    @staticmethod
    def map_ar(x, in_min, in_max, out_min, out_max):
        y = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
        return y


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())
