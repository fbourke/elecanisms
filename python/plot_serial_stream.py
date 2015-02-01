# Code taken from this Instructables:
#   http://www.instructables.com/id/Plotting-real-time-data-from-Arduino-using-Python-/
#   https://gist.github.com/electronut/d5e5f68c610821e311b0

import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque
 
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 


class AnalogPlot:
  def __init__(self, strPort, maxLen):
      self.ser = serial.Serial(strPort, 19200)
      self.a = deque([0.0]*maxLen)
      self.maxLen = maxLen

  def addToBuf(self, buf, val):
      if len(buf) < self.maxLen:
          buf.append(val)
      else:
          buf.pop()
          buf.appendleft(val)

  def add(self, data):
      assert(len(data) == 1)
      self.addToBuf(self.a, data[0])

  def update(self, frameNum, a0):
      try:
          line = self.ser.readline()
          data = float(line.split()[0])
          if(len(data) == 1):
              self.add(data)
              a0.set_data(range(self.maxLen), self.a)
      except KeyboardInterrupt:
          print('exiting')
      return a0

  def close(self):
      self.ser.flush()
      self.ser.close()    


def main():
  parser = argparse.ArgumentParser(description="PIC serial")
  parser.add_argument('--port', dest='port', required=True)
  args = parser.parse_args()
  strPort = args.port
  print('reading from serial port %s...' % strPort)
 
  analogPlot = AnalogPlot(strPort, 100)
  print('plotting data...')

  fig = plt.figure()
  ax = plt.axes(xlim=(0, 100), ylim=(0, 1023))
  a0, = ax.plot([], [])
  anim = animation.FuncAnimation(fig, analogPlot.update, 
                                 fargs=a0, 
                                 interval=50)
  plt.show()
  analogPlot.close()
  print('exiting.')


if __name__ == '__main__':
  main()
