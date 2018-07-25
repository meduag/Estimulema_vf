import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import time


"""
hora = time.strftime("%H")
n_dia = time.strftime("%a")
dia = time.strftime("%d")
min = time.strftime("%M")
nameFile_s = 'stim_r ' + n_dia + ' ' + dia + ' ' + hora + 'h' + min + '.txt'
nameFile_a = 'stim_r ' 
# dat = '20 132'
# nameFile_s = 'stim_r .txt'
stim_r = open(nameFile_s, 'w')
acel_r = open(nameFile_a, 'w') """




# Data for plotting
t = np.arange(0.0, 2.0, 0.01)
s = 1 + np.sin(2 * np.pi * t)

dos = [['0;7060;4;14444;0'], ['0;6992;-40;14512;0'], ['0;7032;60;14564;0'], ['0;7032;60;14564;0']]
print(dos)
print(dos[0])
ss = dos[0][0]
acx = np.zeros(len(dos))
print(acx)
y = acx
print(y)
sa = np.array(ss.rstrip().split(';')).astype(int) # //para hacer en el codigo
t = np.arange(0, 2000)
g = t[:500]
print(g)
print(sa)
print(sa[1])

print(y[0])

file = 'acel_r.txt'
data = np.loadtxt(file, delimiter=';')
#print(data)
ejeX = []

t = np.arange(0, len(data))
ejeX.append(data[:, 1])
ejeY = data[:, 2]
ejeZ = data[:, 3]

plt.figure()
a_ejeX = np.array(ejeX)
plt.plot(t, a_ejeX, 'b')
plt.plot(t, ejeY, 'r')
plt.plot(t, ejeZ, 'g')
plt.grid()
plt.show()


"""
plt.figure(1)
plt.subplot(3, 1, 1)
plt.plot(t, ejeX)
plt.xlabel = 'time (s)'
plt.ylabel = 'g(m/s^2)'
plt.title = 'X axis'
plt.grid()

plt.subplot(3, 1, 2)
plt.plot(t, ejeY, 'r')
plt.xlabel = 'time (s)'
plt.ylabel = 'g(m/s^2)'
plt.title = 'X axis'
plt.grid()

plt.subplot(3, 1, 3)
plt.plot(t, ejeZ, 'g')
plt.xlabel = 'time (s)'
plt.ylabel = 'g(m/s^2)'
plt.title = 'Z axis'
plt.grid()

plt.show()

"""

print("Acabou")



# Note that using plt.subplots below is equivalent to using
# fig = plt.figure() and then ax = fig.add_subplot(111)
#fig, ax = plt.subplots()
#ax.plot(t, s)

#ax.set(xlabel='time (s)', ylabel='voltage (mV)',
       #title='About as simple as it gets, folks')
#ax.grid()

#fig.savefig("test.png")
#plt.show()