import time
import os, sys
sys.path.append(os.path.relpath("../../site_scons"))

import numpy as np

import hellousb


def main():
    dampingController = hellousb.hellousb()
    while(1):
        strength, dampingDirection = dampingController.get_vals()
        print("Damping strength (0-20): %d, Damping positive? %s"
              %(strength, bool(dampingDirection)))
        string = raw_input("Set the spring constant: strength(0-20) positive(0/1)\n")
        try:
            strength, dampingDirection = [float(num) for num in string.split(' ')]
        except:
            print("Give the values as number space number")
        strength = np.clip(strength, 0, 20)  # bounds value between 0, 20
        if dampingDirection != 0:
            dampingDirection = 1;
        dampingController.set_vals(strength, dampingDirection)


if __name__ == '__main__':
    main()
