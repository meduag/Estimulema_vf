import sys
import matplotlib.pyplot as plt
from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QApplication
import serial
import threading
import numpy as np
import datetime
import time

serPort1 = "COM13"
serPort2 = "COM14"
baudRate = 2000000

arrayt = []
arrayt2 = []

# lock to serialize console output
lock = threading.Lock()

# lock to serialize console output
start = False
rh = False  # activate test
cr = False  # activate test
s_ch1 = False  # Start Channel 1  = true
s_ch2 = False  # Start Channel 2  = true
s_c = False  # Start capture
upd = False  # update value
ctr_upd = 0
stop_chx = True

solo_mode = 0  # para controlar testes 0 - abre comunicacion !-! 1 solo imprime los datos

msg_bytes = ''
limit_ma = 0
limit_pw = 0
limit_start = 0  # To count the final message

# Stim parameters
ts = 0
freq = 0
pw = 0
msg = ""

cs = ">"


# First String communication - global variables
class stim_param_channel:
    def __init__(self):
        pass


# For channel 1
ch1 = stim_param_channel()
ch1.tn = 0  # Save de Frequency value
ch1.tf = 0  # Save de PulseWidth value
ch1.r = 0  # Save de TOff value
ch1.ma = 0  # Save de Current value

# For channel 2
ch2 = stim_param_channel()
ch2.tn = 0  # Save de Frequency value
ch2.tf = 0  # Save de PulseWidth value
ch2.r = 0  # Save de TOff value
ch2.ma = 0  # Save de Current value


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Set up the user interface from Designer.
        uic.loadUi("Estimulema V0.3 Rv1.ui", self)

        # Connect up the buttons.
        self.pushButton_rh.clicked.connect(self.btn_start_rh)
        self.pushButton_cr.clicked.connect(self.btn_start_cr)
        self.pushButton_sp.clicked.connect(self.btn_stop_stim)
        self.pushButton_ch1.clicked.connect(self.btn_start_ch1)
        self.pushButton_ch2.clicked.connect(self.btn_start_ch2)

        # for two channels
        self.pushButton_ch1_ch2.clicked.connect(self.btn_start_ch1_ch2)

        # Stop buttons
        self.pushButton_sp_ch1.clicked.connect(self.btn_stop_ch1)
        self.pushButton_sp_ch2.clicked.connect(self.btn_stop_ch2)

        # limits for tests
        self.spinBox_limit_mA.valueChanged.connect(self.update_var)
        self.spinBox_limit_pW.valueChanged.connect(self.update_var)

        # for general parameters
        self.spinBox_ts.valueChanged.connect(self.update_var)
        self.spinBox_f.valueChanged.connect(self.update_var)
        self.spinBox_pw.valueChanged.connect(self.update_var)

        # for channel 1
        self.spinBox_tn_1.valueChanged.connect(self.update_var)
        self.spinBox_tf_1.valueChanged.connect(self.update_var)
        self.spinBox_r_1.valueChanged.connect(self.update_var)
        self.spinBox_ma_1.valueChanged.connect(self.btn_change_ma1)

        # for channel 2
        self.spinBox_tn_2.valueChanged.connect(self.update_var)
        self.spinBox_tf_2.valueChanged.connect(self.update_var)
        self.spinBox_r_2.valueChanged.connect(self.update_var)
        self.spinBox_ma_2.valueChanged.connect(self.btn_change_ma2)

        # Show interface
        self.show()

    # buttons ---------------------------------------------------------------
    def btn_start_rh(self):
        global rh, cr, s_c, start, limit_start, arrayt, limit_ma, s_ch1, s_ch2

        arrayt = []
        rh = start = True
        cr = s_c = s_ch1 = s_ch2 = False
        limit_start = 5

        self.update_var()

        if solo_mode == 0:
            start_test()

        self.lineEdit_terminal.setText("Iniciando Captura de dados")
        #save_data()

    def btn_start_cr(self):
        global rh, cr, s_c, start, limit_start, arrayt, limit_pw, s_ch1, s_ch2

        arrayt = []
        rh = s_c = s_ch1 = s_ch2 = False
        cr = start = True
        limit_start = int(limit_pw / 100) + 10  # Para ver el limite de captura de datos

        self.update_var()

        if solo_mode == 0:
            start_test()

        self.lineEdit_terminal.setText("Iniciando Captura de dados")
        #save_data()

    def btn_start_ch1(self):
        global s_ch1, s_c, start, s_ch2, ctr_upd, stop_chx

        s_ch1 = start = stop_chx = True
        s_c = s_ch2 = False

        self.update_var()

    def btn_start_ch2(self):
        global s_ch2, s_c, start, s_ch1, ctr_upd, stop_chx

        s_ch2 = start = stop_chx = True
        s_c = s_ch1 = False

        self.update_var()

    def btn_start_ch1_ch2(self):
        global s_ch2, s_c, start, s_ch1, ctr_upd, stop_chx

        s_ch2 = s_ch1 = start = stop_chx = True
        s_c = False

        self.update_var()

    def btn_stop_stim(self):
        global rh, cr, s_ch1, s_ch2, s_c, start, upd, stop_chx

        upd = False
        start = True

        rh = cr = s_ch1 = s_ch2 = False
        s_c = stop_chx = upd = True

        self.spinBox_ma_1.setValue(0)
        self.spinBox_ma_2.setValue(0)
        self.update_var()
        upd = start = False

    def btn_stop_ch1(self):
        global upd, start, stop_chx
        upd = stop_chx = False
        start = True

        self.spinBox_ma_1.setValue(0)
        self.update_var()
        upd = start = False

    def btn_stop_ch2(self):
        global upd, start, stop_chx
        upd = stop_chx = False
        start = True

        self.spinBox_ma_2.setValue(0)
        self.update_var()
        upd = start = False

    def btn_change_ma1(self):
        global upd, start, stop_chx

        if start is True:
            upd = True

        if stop_chx is True:
            self.update_var()
        else:
            stop_chx = True

    def btn_change_ma2(self):
        global start, upd, stop_chx

        if start is True:
            upd = True

        if stop_chx is True:
            self.update_var()
        else:
            stop_chx = True

    def upd_terminal(self, msn):
        self.lineEdit_terminal.setText(msn)

    def update_var(self):
        global limit_ma, limit_pw, ch1, ch2, ts, freq, pw, msg, cs
        global rh, cr, s_ch1, s_ch2, s_c, msg_bytes, upd, ctr_upd, start

        # general parameters
        ts = self.spinBox_ts.value()
        freq = self.spinBox_f.value()
        pw = self.spinBox_pw.value()

        # for channel 1
        ch1.tn = self.spinBox_tn_1.value()
        ch1.tf = self.spinBox_tf_1.value()
        ch1.r = self.spinBox_r_1.value()
        ch1.ma = self.spinBox_ma_1.value()

        # for channel 2
        ch2.tn = self.spinBox_tn_2.value()
        ch2.tf = self.spinBox_tf_2.value()
        ch2.r = self.spinBox_r_2.value()
        ch2.ma = self.spinBox_ma_2.value()

        # limits for tests
        limit_ma = self.spinBox_limit_mA.value()
        limit_pw = self.spinBox_limit_pW.value()

        # general parameters = 3
        msg = str(ts) + cs + str(freq) + cs + str(pw)

        # parameters for channel 1 = 4
        msg = msg + cs + str(ch1.tn) + cs + str(ch1.tf) + cs + str(ch1.r) + cs + str(ch1.ma)

        # parameters for channel 2 = 4
        msg = msg + cs + str(ch2.tn) + cs + str(ch2.tf) + cs + str(ch2.r) + cs + str(ch2.ma)

        # parameters for tests = 2
        msg = msg + cs + str(limit_ma) + cs + str(limit_pw) + cs

        # msg of activation control = 4
        if rh is True:
            rh_str = "1"
        else:
            rh_str = "0"

        if cr is True:
            cr_str = "1"
        else:
            cr_str = "0"

        if s_ch1 is True:
            s_ch1_str = "1"
        else:
            s_ch1_str = "0"

        if s_ch2 is True:
            s_ch2_str = "1"
        else:
            s_ch2_str = "0"

        if s_c is True:
            s_c_str = "0"
        else:
            s_c_str = "1"

        if upd is True:
            upd_str = "1"
        else:
            upd_str = "0"

        msg = msg + rh_str + cs + cr_str + cs + s_ch1_str + cs + s_ch2_str + cs + s_c_str + cs + upd_str + cs

        if ctr_upd == 0 and start is True:
            upd = True
            ctr_upd = 2

        self.lineEdit_terminal.setText(msg)
        print("Para enviar: " + msg)
        msg_bytes = str.encode(msg)

        if start is True and solo_mode == 0:
            # print("Cambiando Valor")
            stim_training()
        else:
            print("Solo mode:   " + msg + "<<<<")


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

            # ser.close()

        except Exception as e1:
            print("error communicating...: " + str(e1))

    else:
        print("cannot open serial port ")
        exit()

    print("Uno")

    #save_data()


def read_serial2(port, baud):
    global msg_bytes
    global start
    global arrayt2
    global limit_ma

    cont2 = 0

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
                        arrayt2.append(str_msn)
                    else:
                        cont2 = cont2 + 1
                        if cont2 == limit_ma + limit_start:
                            start = False
                            print("Terminou Captura")

            # ser.close()

        except Exception as e1:
            print("error communicating...: " + str(e1))

    else:
        print("cannot open serial port ")
        exit()

    #save_data()
    print("Dos")


def save_data():
    global arrayt, rh, cr

    if rh is True:
        hora = time.strftime("%H")
        n_dia = time.strftime("%a")
        dia = time.strftime("%d")
        min = time.strftime("%M")
        nameFile_s = 'stim_r ' + n_dia + ' ' + dia + ' ' + hora + 'h' + min + '.txt'
        nameFile_a = 'acel_r ' + n_dia + ' ' + dia + ' ' + hora + 'h' + min + '.txt'
        stim_r = open(nameFile_s, 'w')
        acel_r = open(nameFile_a, 'w')
    elif cr is True:
        date_file = datetime.datetime.now()
        stim_c = open('stim_c.txt', 'w')
        acel_c = open('acel_c.txt', 'w')

    x = ""
    tam_array = len(arrayt)
    t = np.arange(0, tam_array)
    # y = np.zeros(tam_array)

    """rh_acx = np.arange(0, tam_array)
    rh_acy = np.arange(0, tam_array)
    rh_acz = np.arange(0, tam_array)
    cr_acx = np.arange(0, tam_array)
    cr_acy = np.arange(0, tam_array)
    cr_acz = np.arange(0, tam_array)"""

    rh_acx = []
    rh_acy = []
    rh_acz = []
    cr_acx = []
    cr_acy = []
    cr_acz = []

    #cont = 0
    sel_test = True

    for x in arrayt:
        if "s" in x:
            x = x[1:]
            if rh is True:
                stim_r.write(x)
                stim_r.write("\n")
            elif cr is True:
                stim_c.write(x)
                stim_c.write("\n")
            print(x)
        elif "a" in x:
            x = x[1:]
            if rh is True:
                acel_r.write(x)
                acel_r.write("\n")
                # para guardar el array en int y graficar despues
                y = np.array(x.rstrip().split(';')).astype(int)
                rh_acx.append(int(y[1]))
                rh_acy.append(int(y[2]))
                rh_acz.append(int(y[3]))
                sel_test = True

            elif cr is True:
                acel_c.write(x)
                acel_c.write("\n")
                # para guardar el array en int y graficar despues
                y = np.array(x.rstrip().split(';')).astype(int)
                cr_acx.append(y[1])
                cr_acy.append(y[2])
                cr_acz.append(y[3])
                sel_test = False
            print(x)
        #cont = cont + 1

    if rh is True:
        acel_r.close()
        stim_r.close()
    elif cr is True:
        acel_c.close()
        stim_c.close()

    # graficar vectores
    ##
    plt.figure()
    tam_array2 = len(rh_acx)
    t = np.arange(0, tam_array2)
    if sel_test is True:
        axisX = np.array(rh_acx)
        ttt = len(axisX)
        tx = np.arange(0, len(axisX))
        plt.plot(tx, axisX, 'b')
        """axisY = np.array(rh_acy)
        plt.plot(t, axisY, 'r')
        axisZ = np.array(rh_acz)
        plt.plot(t, axisZ, 'g')"""
    elif sel_test is False:
        plt.plot(t, cr_acx, 'b')
        plt.plot(t, cr_acy, 'r')
        plt.plot(t, cr_acz, 'g')

    plt.xlabel('time[s]')
    plt.ylabel('g m/s^2')
    plt.grid()
    plt.show()

    # filtrar señal
    ##
    med_x = np.mean(rh_acx[:500])


    # propor e enviar nuevos valores para el valor de terapia
    ##
    print("Terminou a geração do archivo")
    ex.upd_terminal("Terminou a geração do archivo")

    # para plotar


def stim_training():
    global msg_bytes, start, serPort1, baudRate

    ser = serial.Serial()
    ser.port = serPort1
    ser.timeout = 1
    ser.baudrate = baudRate
    ser.xonxoff = 1

    try:
        ser.open()
    except Exception as e:
        print("error open serial port: " + str(e))
        exit()

    if ser.isOpen():
        try:
            ser.write(msg_bytes)
            """c = ser.readline()
            str_msn = c.decode("utf-8")
            str_msn = str_msn.rstrip()
            print("Recibido: " + str_msn)"""

        except Exception as e1:
            print("error communicating...: " + str(e1))


# Create two threads as follows
def start_test():
    try:
        t1 = threading.Thread(target=read_serial, args=(serPort1, baudRate))
        t1.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
        t1.start()

        t1 = threading.Thread(target=read_serial2, args=(serPort2, baudRate))
        t1.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
        t1.start()

    except:
        print("Error: unable to start thread")


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())
