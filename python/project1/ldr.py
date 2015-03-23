"""
ldr.py

Display analog data from Arduino using Python (matplotlib)

Author: Mahesh Venkitachalam
Website: electronut.in
"""

import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque

import matplotlib.pyplot as plt 
import matplotlib.animation as animation


# plot class
class AnalogPlot:
  # constr
  def __init__(self, strPort, maxLen):
      # open serial port
      self.ser = serial.Serial(strPort, 19200)
      self.ax = deque([0.0]*maxLen)
      self.ay = deque([0.0]*maxLen)
      self.maxLen = maxLen

  # add to buffer
  def addToBuf(self, buf, val):
      if len(buf) < self.maxLen:
          buf.append(val)
      else:
          buf.pop()
          buf.appendleft(val)

  # add data
  def add(self, data):
      assert(len(data) == 2)
      self.addToBuf(self.ax, data[0])
      self.addToBuf(self.ay, data[1])

  # update plot
  def update(self, frameNum, a0, a1):
      try:
          line = self.ser.readline()
          data = [float(val) for val in line.split()]
          if len(data) == 2:
            self.add(data)
            a0.set_data(range(self.maxLen), self.ax)
            a1.set_data(range(self.maxLen), self.ay)
      except KeyboardInterrupt:
          print('exiting')      
      return a0, a1

  def close(self):
      self.ser.flush()
      self.ser.close()


def main():
  parser = argparse.ArgumentParser(description="LDR serial")
  parser.add_argument('--port', dest='port', required=True)
  args = parser.parse_args()  
  strPort = args.port
  print('reading from serial port %s...' % strPort)

  analogPlot = AnalogPlot(strPort, 100)
  print('plotting data...')

  # set up animation
  fig = plt.figure()
  ax = plt.axes(xlim=(0, 100), ylim=(0, 5000))
  a0, = ax.plot([], [])
  a1, = ax.plot([], [])
  anim = animation.FuncAnimation(fig, analogPlot.update, 
                                 fargs=(a0, a1), 
                                 interval=1)
  plt.show()
  analogPlot.close()
  print('exiting.')
  

# call main
if __name__ == '__main__':
  main()