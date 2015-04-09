
import spitest
import sys, time

foo = spitest.spitest()
foo.xl_writeReg(0x2D, 0x02)
foo.gy_writeReg(0x20, 0x0F)

xl_min_x = 0
xl_max_x = 0
xl_min_y = 0
xl_max_y = 0
xl_min_z = 0
xl_max_z = 0
gy_min_x = 0
gy_max_x = 0
gy_min_y = 0
gy_max_y = 0
gy_min_z = 0
gy_max_z = 0

while 1:
    sys.stdout.write('\x1b[2J\x1b[1;1f')

    ret = foo.xl_readRegs(0x0E, 6)

    x = ret[0] + 256*ret[1]
    y = ret[2] + 256*ret[3]
    z = ret[4] + 256*ret[5]

    x = x - 65536 if x>32767 else x
    y = y - 65536 if y>32767 else y
    z = z - 65536 if z>32767 else z

    xl_min_x = x if x<xl_min_x else xl_min_x
    xl_max_x = x if x>xl_max_x else xl_max_x
    xl_min_y = y if y<xl_min_y else xl_min_y
    xl_max_y = y if y>xl_max_y else xl_max_y
    xl_min_z = z if z<xl_min_z else xl_min_z
    xl_max_z = z if z>xl_max_z else xl_max_z

    print 'Accelerometer:'
    print '  x = {0:+05.3f}g [{1!s}, {2!s}]'.format(float(x)*0.001, xl_min_x, xl_max_x)
    print '  y = {0:+05.3f}g [{1!s}, {2!s}]'.format(float(y)*0.001, xl_min_y, xl_max_y)
    print '  z = {0:+05.3f}g [{1!s}, {2!s}]'.format(float(z)*0.001, xl_min_z, xl_max_z)

    ret = foo.gy_readRegs(0x28, 6)

    x = ret[0] + 256*ret[1]
    y = ret[2] + 256*ret[3]
    z = ret[4] + 256*ret[5]

    x = x - 65536 if x>32767 else x
    y = y - 65536 if y>32767 else y
    z = z - 65536 if z>32767 else z

    gy_min_x = x if x<gy_min_x else gy_min_x
    gy_max_x = x if x>gy_max_x else gy_max_x
    gy_min_y = y if y<gy_min_y else gy_min_y
    gy_max_y = y if y>gy_max_y else gy_max_y
    gy_min_z = z if z<gy_min_z else gy_min_z
    gy_max_z = z if z>gy_max_z else gy_max_z

    print '\nGyroscope:'
    print '  x = {0:+08.3f}dps [{1!s}, {2!s}]'.format(float(x)*8.75e-3, gy_min_x, gy_max_x)
    print '  y = {0:+08.3f}dps [{1!s}, {2!s}]'.format(float(y)*8.75e-3, gy_min_y, gy_max_y)
    print '  z = {0:+08.3f}dps [{1!s}, {2!s}]'.format(float(z)*8.75e-3, gy_min_z, gy_max_z)

    t0 = time.clock()
    while time.clock()<t0+0.05:
        pass

