#include <p24FJ64GB002.h>
#include "usb.h"

BYTE __attribute__ ((space(auto_psv))) Device[] = {
    0x12,       // bLength
    DEVICE,     // bDescriptorType
    0x00,       // bcdUSB (low byte)
    0x02,       // bcdUSB (high byte)
    0x00,       // bDeviceClass
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    MAX_PACKET_SIZE,    // bMaxPacketSize
    0x66,       // idVendor (low byte)
    0x66,       // idVendor (high byte)
    0x99,       // idProduct (low byte)
    0x99,       // idProduct (high byte)
    0x00,       // bcdDevice (low byte)
    0x00,       // bcdDevice (high byte)
    0x01,       // iManufacturer
    0x02,       // iProduct
    0x00,       // iSerialNumber (none)
    NUM_CONFIGURATIONS  // bNumConfigurations
};

BYTE __attribute__ ((space(auto_psv))) Configuration1[] = {
    0x09,       // bLength
    CONFIGURATION,  // bDescriptorType
    0x3B,       // wTotalLength (low byte)
    0x00,       // wTotalLength (high byte)
    NUM_INTERFACES, // bNumInterfaces
    0x01,       // bConfigurationValue
    0x00,       // iConfiguration (none)
    0xA0,       // bmAttributes
    0x32,       // bMaxPower (100 mA)
    0x09,       // bLength (Interface1 descriptor starts here)
    INTERFACE,  // bDescriptorType
    0x00,       // bInterfaceNumber
    0x00,       // bAlternateSetting
    0x01,       // bNumEndpoints (excluding EP0)
    0x03,       // bInterfaceClass (HID code)
    0x01,       // bInterfaceSubClass (Boot subclass)
    0x01,       // bInterfaceProtocol (Keyboard protocol)
    0x00,       // iInterface (none)
    0x09,       // bLength (HID1 descriptor starts here)
    HID,        // bDescriptorType
    0x00,       // bcdHID (low byte)
    0x01,       // bcdHID (high byte)
    0x00,       // bCountryCode (none)
    0x01,       // bNumDescriptors
    REPORT,     // bDescriptorType
    0x3F,       // wDescriptorLength (low byte)
    0x00,       // wDescriptorLength (high byte)
    0x07,       // bLength (Endpoint1 descriptor starts here)
    ENDPOINT,   // bDescriptorType
    0x81,       // bEndpointAddress (EP1 IN)
    0x03,       // bmAttributes (Interrupt)
    0x08,       // wMaxPacketSize (low byte)
    0x00,       // wMaxPacketSize (high byte)
    0x0A,       // bInterval (10 ms)
    0x09,       // bLength (Interface2 descriptor starts here)
    INTERFACE,  // bDescriptorType
    0x01,       // bInterfaceNumber
    0x00,       // bAlternateSetting
    0x01,       // bNumEndpoints (excluding EP0)
    0x03,       // bInterfaceClass (HID code)
    0x01,       // bInterfaceSubClass (Boot subclass)
    0x02,       // bInterfaceProtocol (Mouse protocol)
    0x00,       // iInterface (none)
    0x09,       // bLength (HID2 descriptor starts here)
    HID,        // bDescriptorType
    0x00,       // bcdHID (low byte)
    0x01,       // bcdHID (high byte)
    0x00,       // bCountryCode (none)
    0x01,       // bNumDescriptors
    REPORT,     // bDescriptorType
    0x32,       // wDescriptorLength (low byte)
    0x00,       // wDescriptorLength (high byte)
    0x07,       // bLength (Endpoint1 descriptor starts here)
    ENDPOINT,   // bDescriptorType
    0x82,       // bEndpointAddress (EP2 IN)
    0x03,       // bmAttributes (Interrupt)
    0x03,       // wMaxPacketSize (low byte)
    0x00,       // wMaxPacketSize (high byte)
    0x0A        // bInterval (10 ms)
};

BYTE __attribute__ ((space(auto_psv))) Report1[] = {
    0x05, 0x01, // Usage Page (Generic Desktop),
    0x09, 0x06, // Usage (Keyboard),
    0xA1, 0x01, // Collection (Application),
    0x05, 0x07, //     Usage Page (Key Codes);
    0x19, 0xE0, //     Usage Minimum (224),
    0x29, 0xE7, //     Usage Maximum (231),
    0x15, 0x00, //     Logical Minimum (0),
    0x25, 0x01, //     Logical Maximum (1),
    0x75, 0x01, //     Report Size (1),
    0x95, 0x08, //     Report Count (8),
    0x81, 0x02, //     Input (Data, Variable, Absolute),   ; Modifier byte
    0x95, 0x01, //     Report Count (1),
    0x75, 0x08, //     Report Size (8),
    0x81, 0x01, //     Input (Constant),                   ; Reserved byte
    0x95, 0x05, //     Report Count (5),
    0x75, 0x01, //     Report Size (1),
    0x05, 0x08, //     Usage Page (Page# for LEDs),
    0x19, 0x01, //     Usage Minimum (1),
    0x29, 0x05, //     Usage Maxmimum (5),
    0x91, 0x02, //     Output (Data, Variable, Absolute),  ; LED report
    0x95, 0x01, //     Report Count (1),
    0x75, 0x03, //     Report Size (3),
    0x91, 0x01, //     Output (Constant),                  ; LED report padding
    0x95, 0x06, //     Report Count (6),
    0x75, 0x08, //     Report Size (8),
    0x15, 0x00, //     Logical Minimum (0),
    0x25, 0x65, //     Logical Maximum (101),
    0x05, 0x07, //     Usage Page (Key Codes),
    0x19, 0x00, //     Usage Minimum (0),
    0x29, 0x65, //     Usage Maximum (101),
    0x81, 0x00, //     Input (Data, Array),                ; Key arrays (6 bytes)
    0xC0        // End Collection
};

BYTE __attribute__ ((space(auto_psv))) Report2[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x02, // Usage (Mouse)
    0xA1, 0x01, // Collection (Application)
    0x09, 0x01, //     Usage (Pointer)
    0xA1, 0x00, //     Collection (Linked)
    0x05, 0x09, //         Usage Page (Buttons)
    0x19, 0x01, //         Usage Minimum (1)
    0x29, 0x03, //         Usage Maximum (3)
    0x15, 0x00, //         Logical Minimum (0)
    0x25, 0x01, //         Logical Maximum (1)
    0x95, 0x03, //         Report Count (3)
    0x75, 0x01, //         Report Size (1)
    0x81, 0x02, //         Input (3 Button Bits)
    0x95, 0x01, //         Report Count (1)
    0x75, 0x05, //         Report Size (5)
    0x81, 0x01, //         Input (Constant 5-Bit Padding)
    0x05, 0x01, //         Usage Page (Generic Desktop)
    0x09, 0x30, //         Usage (X)
    0x09, 0x31, //         Usage (Y)
    0x15, 0x81, //         Logical Minimum (-127)
    0x25, 0x7F, //         Logical Maximum (127)
    0x75, 0x08, //         Report Size (8)
    0x95, 0x02, //         Report Count (2)
    0x81, 0x06, //         Input (2 Position Bytes X & Y)
    0xC0,       //     End Collection
    0xC0        // End Collection
};

BYTE __attribute__ ((space(auto_psv))) String0[] = {
    0x04,       // bLength
    STRING,     // bDescriptorType
    0x09,       // wLANGID[0] (low byte)
    0x04        // wLANGID[0] (high byte)
};

BYTE __attribute__ ((space(auto_psv))) String1[] = {
    34,         // bLength
    STRING,     // bDescriptorType
    'B', 0x00, 'r', 0x00, 'a', 0x00, 'd', 0x00, 'l', 0x00, 'e', 0x00, 'y', 0x00, ' ', 0x00, 
    'A', 0x00, '.', 0x00, ' ', 0x00, 
    'M', 0x00, 'i', 0x00, 'n', 0x00, 'c', 0x00, 'h', 0x00
};

BYTE __attribute__ ((space(auto_psv))) String2[] = {
    84,         // bLength
    STRING,     // bDescriptorType
    'W', 0x00, 'i', 0x00, 'i', 0x00, ' ', 0x00, 
    'N', 0x00, 'u', 0x00, 'n', 0x00, 'c', 0x00, 'h', 0x00, 'u', 0x00, 'c', 0x00, 'k', 0x00, ' ', 0x00, 
    'U', 0x00, 'S', 0x00, 'B', 0x00, ' ', 0x00, 
    'K', 0x00, 'e', 0x00, 'y', 0x00, 'b', 0x00, 'o', 0x00, 'a', 0x00, 'r', 0x00, 'd', 0x00, '/', 0x00, 
    'M', 0x00, 'o', 0x00, 'u', 0x00, 's', 0x00, 'e', 0x00, ' ', 0x00, 
    'C', 0x00, 'o', 0x00, 'm', 0x00, 'p', 0x00, 'o', 0x00, 's', 0x00, 'i', 0x00, 't', 0x00, 'e', 0x00
};

