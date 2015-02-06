import numpy as np
from time import sleep
from collections import deque
 
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 



def main():
    # 128 PWM tri-stating signal
    # 20 Hz reporting
    data_PWM128_20Hz   = [3600,3600,4200,4200,4200,4200,4200,3600,3600,3600,3600,3600,4800,4800,4800,4800,4800,3600,3600,3600,3600,3600,4800,4800,4800,4800,4800,3600,3600,3600,3600,3600,4800,4800,4800,4800,4800,3600,3600,3600,3600,3600,4200,4200,4200,4200,4200,3600,3600,3600,3600,3600,3600,3600,3600,3600,3600,4200,4200,4200,4200,4200,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,2400,2400,2400,2400,2400,2400,2400,2400,2400,2400,1800,1800,1800,1800,1800,1200,1200,1200,1200,1200,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,0,0,0,0,0,0,0,0,0,0,0,0,0]
    data_PWM128_20Hz_X = np.linspace(0, len(data_PWM128_20Hz) * 0.05, len(data_PWM128_20Hz))
    
    # 386 PWM tri-stating signal
    # 10 Hz reporting
    data_PWM386_10Hz   = [1800,1800,1800,1800,2100,1800,1800,1500,900,300,0,0,0,0,0,0,0]
    data_PWM386_10Hz_X = np.linspace(0, len(data_PWM386_10Hz) * 0.1, len(data_PWM386_10Hz))

    fig = plt.figure(1)
    ax = plt.axes(xlim=(0, 7), ylim=(0, 5000))
    plt.plot(data_PWM128_20Hz_X, data_PWM128_20Hz, label='PWM 128, 20Hz Measurement')
    plt.plot(data_PWM386_10Hz_X, data_PWM386_10Hz, label='PWM 386, 10Hz Measurement')
    plt.ylabel('Time (s)')
    plt.xlabel('Motor Velocity (RPM)')
    plt.legend()
    plt.show()

    fig = plt.figure(1)
    plt.plot(np.log(data_PWM128_20Hz_X),np.log(data_PWM128_20Hz), label='PWM 128, 20Hz Measurement')
    plt.plot(np.log(data_PWM386_10Hz_X),np.log(data_PWM386_10Hz), label='PWM 386, 10Hz Measurement')
    plt.ylabel('Log of Time (s)')
    plt.xlabel('Log of Motor Velocity (RPM)')
    plt.legend(loc='lower left')
    plt.show()


if __name__ == '__main__':
  main()
