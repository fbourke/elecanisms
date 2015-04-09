
import usb.core

class spitest:

    def __init__(self):
        self.TOGGLE_LED1 = 1
        self.TOGGLE_LED2 = 2
        self.READ_SW1 = 3
        self.XL_WRITE_REG = 10
        self.XL_READ_REG = 11
        self.XL_WRITE_REGS = 12
        self.XL_READ_REGS = 13
        self.XL_READ_FIFO = 14
        self.TOGGLE_SHOW_XLINT1 = 15
        self.TOGGLE_SHOW_XLINT2 = 16
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def toggle_led1(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED1)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED1 vendor request."

    def toggle_led2(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED2)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED2 vendor request."

    def read_sw1(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW1, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send EEPROM_READ_BYTE vendor request."
        else:
            return int(ret[0])

    def xl_writeReg(self, address, value):
        try:
            self.dev.ctrl_transfer(0x40, self.XL_WRITE_REG, address, value)
        except usb.core.USBError:
            print "Could not send XL_WRITE_REG vendor request."

    def xl_readReg(self, address):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.XL_READ_REG, address, 0, 1)
        except usb.core.USBError:
            print "Could not send XL_READ_REG vendor request."
        else:
            return int(ret[0])

    def xl_writeRegs(self, address, values):
        try:
            self.dev.ctrl_transfer(0x40, self.XL_WRITE_REGS, address, 0, values)
        except usb.core.USBError:
            print "Could not send XL_WRITE_REGS vendor request."

    def xl_readRegs(self, address, num_bytes):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.XL_READ_REGS, address, 0, num_bytes)
        except usb.core.USBError:
            print "Could not send XL_READ_REGS vendor request."
        else:
            return [int(val) for val in ret]

    def xl_readFIFO(self, num_bytes):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.XL_READ_FIFO, 0, 0, num_bytes)
        except usb.core.USBError:
            print "Could not send XL_READ_FIFO vendor request."
        else:
            return [int(val) for val in ret]

    def xl_toggleShowXLINT1(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_SHOW_XLINT1)
        except usb.core.USBError:
            print "Could not send TOGGLE_SHOW_XLINT1 vendor request."

    def xl_toggleShowXLINT2(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_SHOW_XLINT2)
        except usb.core.USBError:
            print "Could not send TOGGLE_SHOW_XLINT2 vendor request."
