
import spitest
import sys, time

foo = spitest.spitest()
foo.gy_writeReg(0x20, 0x0F)

while 1:
    ret = foo.gy_readRegs(0x28, 6)

    x = ret[0] + 256*ret[1]
    y = ret[2] + 256*ret[3]
    z = ret[4] + 256*ret[5]

    x = x - 65536 if x>32767 else x
    y = y - 65536 if y>32767 else y
    z = z - 65536 if z>32767 else z

    sys.stdout.write('\x1b[2J\x1b[1;1f')
    print 'x = {0:+08.3f}dps'.format(float(x)*8.75e-3)
    print 'y = {0:+08.3f}dps'.format(float(y)*8.75e-3)
    print 'z = {0:+08.3f}dps'.format(float(z)*8.75e-3)

    t0 = time.clock()
    while time.clock()<t0+0.05:
        pass

