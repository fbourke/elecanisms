# Code taken from this Instructables:
#   http://www.instructables.com/id/Plotting-real-time-data-from-Arduino-using-Python-/
#   https://gist.github.com/electronut/d5e5f68c610821e311b0

import numpy as np
from time import sleep
from collections import deque
 
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 



def main():
  data = [3600,3600,4200,4200,4200,4200,4200,3600,3600,3600,3600,3600,4800,4800,4800,4800,4800,3600,3600,3600,3600,3600,4800,4800,4800,4800,4800,3600,3600,3600,3600,3600,4800,4800,4800,4800,4800,3600,3600,3600,3600,3600,4200,4200,4200,4200,4200,3600,3600,3600,3600,3600,3600,3600,3600,3600,3600,4200,4200,4200,4200,4200,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,2400,2400,2400,2400,2400,2400,2400,2400,2400,2400,1800,1800,1800,1800,1800,1200,1200,1200,1200,1200,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,0,0,0,0,0,0,0,0,0,0,0,0,0]
  dataX = np.linspace(0,len(data)*0.05,len(data))
  data2 = [1800,1800,1800,1800,2100,1800,1800,1500,900,300,0,0,0,0,0,0,0]
  data2X = np.linspace(0,len(data2)*0.25,len(data2))

  angle = [0,45,90,135,180]
  measurement = [76,382,600,738,845]

  #fig = plt.figure()
  #ax = plt.axes(xlim=(0, 125), ylim=(0, 5000))
  plt.plot(angle,measurement, 'bo')
  #plt.plot(np.log(dataX),np.log(data))
  #plt.plot(data2X,data2)
  plt.show()
  print('exiting.')


if __name__ == '__main__':
  main()
