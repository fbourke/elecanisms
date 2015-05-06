import os, sys
lib_path = os.path.abspath(os.path.join('..', 'spitest'))
sys.path.append(lib_path)
import spitest
import time

foo = spitest.spitest()
foo.xl_writeReg(0x2D, 0x02)

while 1:
    ret = foo.xl_readRegs(0x0E, 6) 

    try:
        x = ret[0] + 256*ret[1]
        y = ret[2] + 256*ret[3]
        z = ret[4] + 256*ret[5]

        x = x - 65536 if x>32767 else x
        y = y - 65536 if y>32767 else y
        z = z - 65536 if z>32767 else z
    except:
        pass

    sys.stdout.write('\x1b[2J\x1b[1;1f')
    print 'x = {0:+05.3f}g'.format(float(x)*0.001)
    print 'y = {0:+05.3f}g'.format(float(y)*0.001)
    print 'z = {0:+05.3f}g'.format(float(z)*0.001)
    print 'squares = {0:+05.3f}g'.format(float(x)*float(x) + float(y)*float(y) + float(z)*float(z))

    t0 = time.clock()
    while time.clock()<t0+0.05:
        pass
