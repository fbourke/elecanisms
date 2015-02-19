import time
import os, sys
sys.path.append(os.path.relpath("../../site_scons"))

import numpy as np

import hellousb


def main():
    roughController = hellousb.hellousb()
    while(1):
        response, spacing = roughController.get_vals()
        print("Response strength (0-1023): %d, Response spacing (50-450) %d"
              %(response, spacing))
        string = raw_input("Set the spring constant: response spacing\n")
        try:
            response, spacing = [float(num) for num in string.split(' ')]
        except:
            print("Give the values as number space number")
        response = np.clip(response, 0, 1023)  # bounds value between 0, 1023
        spacing = np.clip(spacing, 1, 450)  # bounds value between 1, 450
        roughController.set_vals(response, spacing)


if __name__ == '__main__':
    main()
