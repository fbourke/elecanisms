import sys, serial, argparse
import mputest
import numpy as np
from time import sleep
from collections import deque

import matplotlib.pyplot as plt 
import matplotlib.animation as animation

# plot class
class AnalogPlot:
  # constr
  def __init__(self, maxLen, mpu):
    # open serial port
    # self.ser = serial.Serial(19200)
    self.ax = deque([0.0]*maxLen)
    self.ay = deque([0.0]*maxLen)
    self.az = deque([0.0]*maxLen)
    self.maxLen = maxLen
    self.mpu = mpu

  # add to buffer
  def addToBuf(self, buf, val):
    if len(buf) < self.maxLen:
        buf.append(val)
    else:
        buf.pop()
        buf.appendleft(val)

  # add data
  def add(self, data):
    assert(len(data) == 3)
    self.addToBuf(self.ax, data[0])
    self.addToBuf(self.ay, data[1])
    self.addToBuf(self.az, data[2])

  # # update plot
  # def update(self, frameNum, a0, a1):
  #     try:
  #         line = self.ser.readline()
  #         data = [float(val) for val in line.split()]
  #         if len(data) == 2:
  #           self.add(data)
  #           a0.set_data(range(self.maxLen), self.ax)
  #           a1.set_data(range(self.maxLen), self.ay)
  #     except KeyboardInterrupt:
  #         print('exiting')      
  #     return a0, a1

  def updateWithAccel(self, frameNum, a0, a1, a2):
    try:
      data = self.mpu.mpu_read_accel()
      self.add(data)
      print(data)
      a0.set_data(range(self.maxLen), self.ax)
      a1.set_data(range(self.maxLen), self.ay)
      a2.set_data(range(self.maxLen), self.az)
    except KeyboardInterrupt:
      print('exiting')      
    return a0, a1, a2

  def close(self):
      self.ser.flush()
      self.ser.close()

def main():
    mpu = mputest.mputest()
    mpu.mpu_init()
    analogPlot = AnalogPlot(100, mpu)

    # while 1:
    #   data = mpu.mpu_read_accel()
    #   print data
    # set up animation
    fig = plt.figure()
    ax = plt.axes(xlim=(0, 100), ylim=(-1.5, 1.5))
    a0, = ax.plot([], [], label="x")
    a1, = ax.plot([], [], label="y")
    a2, = ax.plot([], [], label="z")
    anim = animation.FuncAnimation(fig, analogPlot.updateWithAccel, 
                                   fargs=(a0, a1, a2), 
                                   interval=1)
    ax.legend()
    plt.show()
    # analogPlot.close()
    print('exiting.')

# call main
if __name__ == '__main__':
    main()
