import time
import os, sys
sys.path.append(os.path.relpath("../../site_scons"))

import hellousb


def main():
    wallController = hellousb.hellousb()
    while(1):
        neg, pos = wallController.get_vals()
        print("Negative wall: %d flips, Positive wall: %d flips" %(-neg, pos))
        string = raw_input("Set the walls: (neg pos)\n")
        try:
            setNeg, setPos = [float(num) for num in string.split(' ')]
        except:
            print("Give walls as number space number")
        wallController.set_vals(abs(setNeg), setPos)


if __name__ == '__main__':
    main()
