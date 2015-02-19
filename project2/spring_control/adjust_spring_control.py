import time
import os, sys
sys.path.append(os.path.relpath("../../site_scons"))

import hellousb


def main():
    springController = hellousb.hellousb()
    while(1):
        Kp, temp = springController.get_vals()
        print("The current spring constant is %d" %(Kp))
        string = raw_input("Set the spring constant: ")
        try:
            Kp = float(string)
        except:
            print("Give spring constant as a float")
        springController.set_vals(Kp, Kp)


if __name__ == '__main__':
    main()
