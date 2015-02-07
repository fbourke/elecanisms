import numpy as np
from time import sleep
from collections import deque
 
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 



def main():
    # 128 PWM tri-stating signal
    # 20 Hz reporting, 125 data points
    data_PWM128_20Hz   = [3600, 3600, 4200, 4200, 4200, 4200, 4200, 3600, 3600, 3600, 3600, 3600, 4800, 4800, 4800, 4800, 4800, 3600, 3600, 3600, 3600, 3600, 4800, 4800, 4800, 4800, 4800, 3600, 3600, 3600, 3600, 3600, 4800, 4800, 4800, 4800, 4800, 3600, 3600, 3600, 3600, 3600, 4200, 4200, 4200, 4200, 4200, 3600, 3600, 3600, 3600, 3600, 3600, 3600, 3600, 3600, 3600, 4200, 4200, 4200, 4200, 4200, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 2400, 2400, 2400, 2400, 2400, 2400, 2400, 2400, 2400, 2400, 1800, 1800, 1800, 1800, 1800, 1200, 1200, 1200, 1200, 1200, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001]
    data_PWM128_20Hz_X = np.linspace(0, len(data_PWM128_20Hz) * 0.05, len(data_PWM128_20Hz))

    # 384 PWM tri-stating signal
    # 10 Hz reporting, 17 data points
    data_PWM384_10Hz   = [1800, 1800, 1800, 1800, 2100, 1800, 1800, 1500, 900, 300, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001]
    data_PWM384_10Hz_X = np.linspace(0, len(data_PWM384_10Hz) * 0.1, len(data_PWM384_10Hz))

    fig = plt.figure(0)
    ax = plt.axes(xlim=(0, 7), ylim=(0, 5000))
    plt.plot(data_PWM128_20Hz_X, data_PWM128_20Hz, label='PWM 128, 20Hz Measurement')
    plt.plot(data_PWM128_20Hz_X, data_PWM128_20Hz, 'bo')
    # highlight_point(data_PWM128_20Hz_X, 57, 4800, 'b--')
    # highlight_point(data_PWM128_20Hz_X, 111, 4800, 'b--')

    plt.plot(data_PWM384_10Hz_X, data_PWM384_10Hz, label='PWM 384, 10Hz Measurement')
    plt.plot(data_PWM384_10Hz_X, data_PWM384_10Hz, 'go')
    # highlight_point(data_PWM384_10Hz_X, 6, 1800, 'g--')
    # highlight_point(data_PWM384_10Hz_X, 9, 1800, 'g--')
    plt.title('Tracked velocity as the motor is tri-stated (spindown)')
    plt.xlabel('Time (s)')
    plt.ylabel('Motor Velocity (RPM)')
    plt.legend()
    plt.show()

    fig = plt.figure(1)
    log_data_PWM128_20Hz = convert_to_log_time_units(data_PWM128_20Hz, 57, 111, 4200)
    log_data_PWM384_10Hz = convert_to_log_time_units(data_PWM384_10Hz, 6, 9, 1800)

    plt.subplot(2, 1, 1)
    plt.plot(data_PWM128_20Hz_X[57:111], log_data_PWM128_20Hz, label='Data for -(B/J)t')
    plt.plot(data_PWM128_20Hz_X[57:111], log_data_PWM128_20Hz, 'bo')
    plot_best_fit_line(data_PWM128_20Hz_X[57:111], log_data_PWM128_20Hz, 2.75, 5.75)

    plt.title('Log of tracked velocity for a 128 PWM signal')
    plt.xlabel('Time (s)')
    plt.ylabel('Log of (radians/second) velocity,\nin unitless (-B/J)t form')
    plt.legend()

    plt.subplot(2, 1, 2)
    plt.plot(data_PWM384_10Hz_X[6:9], log_data_PWM384_10Hz, 'g', label='Data for -(B/J)t')
    plt.plot(data_PWM384_10Hz_X[6:9], log_data_PWM384_10Hz, 'go')
    plot_best_fit_line(data_PWM384_10Hz_X[6:9], log_data_PWM384_10Hz, 0.63, 0.86)
    plt.title('Log of tracked velocity for a 384 PWM signal')
    plt.xlabel('Time (s)')
    plt.ylabel('Log of (radians/second) velocity,\nin unitless (-B/J)t form')
    plt.legend()
    plt.show()

def highlight_point(data, x1, height, modifier='r-'):
    plt.plot([data[x1], data[x1]], [0, height], modifier)

def convert_to_log_time_units(data, lower_idx, upper_idx, init_v):
    '''
    1. Convert RPM to rad/second
    2. Takes the log of that data, bounded by the given indices
    '''
    data_rad_per_s = [pt * 2 * np.pi / 60. for pt in data]
    return [np.log(pt / float(init_v)) for pt in data_rad_per_s[lower_idx:upper_idx]]

def plot_best_fit_line(xData, yData, x1, x2):
    m, b = np.polyfit(xData, yData, 1)
    label = 'Best fit, slope: %f' % m
    plt.plot([x1, x2], [m * x1 + b, m*x2 + b], 'r', label=label)


if __name__ == '__main__':
  main()
