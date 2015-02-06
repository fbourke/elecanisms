import matplotlib.pyplot as plt 


def main():
  angle = [0,45,90,135,180]
  measurement = [76,382,600,738,845]
  fig = plt.figure()
  ax = plt.axes(xlim=(-5, 185), ylim=(0, 900))
  plt.plot(angle,measurement, 'bo')
  plt.show()


if __name__ == '__main__':
  main()
