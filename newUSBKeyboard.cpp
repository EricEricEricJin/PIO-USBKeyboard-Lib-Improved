#include "stdint.h"

#include "newUSBKeyboard.h"
#include "usb_phy_api.h"

#define REPORT_ID_PRIMARY 1
#define REPORT_ID_MEDIA 3

newUSBKeyboard::newUSBKeyboard(bool connect, uint16_t vendor_id, uint16_t product_id, uint16_t product_release) : USBHID(get_usb_phy(), 0, 0, vendor_id, product_id, product_release)
{
    _lock_status = 0;

    primary_latest_report.data[0] = REPORT_ID_PRIMARY;
    primary_latest_report.data[2] = 0;
    primary_latest_report.length = 9;

    media_latest_report.data[0] = REPORT_ID_MEDIA;
    media_latest_report.length = 2;

    PRIMARY_FULL_REPORT.data[0] = REPORT_ID_PRIMARY;
    PRIMARY_FULL_REPORT.data[1] = 0;
    PRIMARY_FULL_REPORT.data[2] = 0;
    PRIMARY_FULL_REPORT.data[3] = 0xff;
    PRIMARY_FULL_REPORT.data[4] = 0xff;
    PRIMARY_FULL_REPORT.data[5] = 0xff;
    PRIMARY_FULL_REPORT.data[6] = 0xff;
    PRIMARY_FULL_REPORT.data[7] = 0xff;
    PRIMARY_FULL_REPORT.data[8] = 0xff;
    PRIMARY_FULL_REPORT.length = 9;
}

newUSBKeyboard::newUSBKeyboard(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release) : USBHID(phy, 0, 0, vendor_id, product_id, product_release)
{
#warning Use constructor with default parameters!
    _lock_status = 0;
    primary_latest_report.data[0] = REPORT_ID_PRIMARY;
    primary_latest_report.data[2] = 0;
    primary_latest_report.length = 9;

    media_latest_report.data[0] = REPORT_ID_MEDIA;
    media_latest_report.length = 2;

    PRIMARY_FULL_REPORT.data[0] = REPORT_ID_PRIMARY;
    PRIMARY_FULL_REPORT.data[1] = 0;
    PRIMARY_FULL_REPORT.data[2] = 0;
    PRIMARY_FULL_REPORT.data[3] = 0xff;
    PRIMARY_FULL_REPORT.data[4] = 0xff;
    PRIMARY_FULL_REPORT.data[5] = 0xff;
    PRIMARY_FULL_REPORT.data[6] = 0xff;
    PRIMARY_FULL_REPORT.data[7] = 0xff;
    PRIMARY_FULL_REPORT.data[8] = 0xff;
    PRIMARY_FULL_REPORT.length = 9;
}

newUSBKeyboard::~newUSBKeyboard()
{
}

const uint8_t *newUSBKeyboard::report_desc()
{
    static const uint8_t reportDescriptor[] = {
        USAGE_PAGE(1),
        0x01, // Generic Desktop
        USAGE(1),
        0x06, // Keyboard
        COLLECTION(1),
        0x01, // Application
        REPORT_ID(1),
        REPORT_ID_PRIMARY,

        USAGE_PAGE(1),
        0x07, // Key Codes
        USAGE_MINIMUM(1),
        0xE0,
        USAGE_MAXIMUM(1),
        0xE7,
        LOGICAL_MINIMUM(1),
        0x00,
        LOGICAL_MAXIMUM(1),
        0x01,
        REPORT_SIZE(1),
        0x01,
        REPORT_COUNT(1),
        0x08,
        INPUT(1),
        0x02, // Data, Variable, Absolute
        REPORT_COUNT(1),
        0x01,
        REPORT_SIZE(1),
        0x08,
        INPUT(1),
        0x01, // Constant

        REPORT_COUNT(1),
        0x05,
        REPORT_SIZE(1),
        0x01,
        USAGE_PAGE(1),
        0x08, // LEDs
        USAGE_MINIMUM(1),
        0x01,
        USAGE_MAXIMUM(1),
        0x05,
        OUTPUT(1),
        0x02, // Data, Variable, Absolute
        REPORT_COUNT(1),
        0x01,
        REPORT_SIZE(1),
        0x03,
        OUTPUT(1),
        0x01, // Constant

        REPORT_COUNT(1),
        0x06,
        REPORT_SIZE(1),
        0x08,
        LOGICAL_MINIMUM(1),
        0x00,
        LOGICAL_MAXIMUM(1),
        0x65,
        USAGE_PAGE(1),
        0x07, // Key Codes
        USAGE_MINIMUM(1),
        0x00,
        USAGE_MAXIMUM(1),
        0x65,
        INPUT(1),
        0x00, // Data, Array
        END_COLLECTION(0),

        // Media Control
        USAGE_PAGE(1),
        0x0C,
        USAGE(1),
        0x01,
        COLLECTION(1),
        0x01,
        REPORT_ID(1),
        REPORT_ID_MEDIA,
        USAGE_PAGE(1),
        0x0C,
        LOGICAL_MINIMUM(1),
        0x00,
        LOGICAL_MAXIMUM(1),
        0x01,
        REPORT_SIZE(1),
        0x01,
        REPORT_COUNT(1),
        0x07,
        USAGE(1),
        0xB5, // Next Track
        USAGE(1),
        0xB6, // Previous Track
        USAGE(1),
        0xB7, // Stop
        USAGE(1),
        0xCD, // Play / Pause
        USAGE(1),
        0xE2, // Mute
        USAGE(1),
        0xE9, // Volume Up
        USAGE(1),
        0xEA, // Volume Down
        INPUT(1),
        0x02, // Input (Data, Variable, Absolute)
        REPORT_COUNT(1),
        0x01,
        INPUT(1),
        0x01,
        END_COLLECTION(0),
    };
    reportLength = sizeof(reportDescriptor);
    return reportDescriptor;
}

void newUSBKeyboard::report_rx()
{
    assert_locked();

    HID_REPORT report;
    read_nb(&report);

    // we take [1] because [0] is the report ID
    _lock_status = report.data[1] & 0x07;
}

uint8_t newUSBKeyboard::lock_status()
{
    return _lock_status;
}

int newUSBKeyboard::_putc(int c)
{
    return -1;
}

bool newUSBKeyboard::send_key(uint8_t *primary_key, uint8_t *modifier_key)
{
    _mutex.lock();
    for (int i = 0; i < PRIMARY_KEY_LIST_LEN; i++)
    {
        primary_latest_report.data[i + 3] = primary_key[i];
    }
    primary_latest_report.data[1] = 0;
    for (int i = 0; i < MODIFIER_LEY_LIST_LEN; i++)
    {
        primary_latest_report.data[1] |= modifier_key[i];
    }
    if (!send(&primary_latest_report))
    {
        _mutex.unlock();
        return true;
    }
    _mutex.unlock();
    return false;
}

bool newUSBKeyboard::send_media(uint8_t *key)
{
    _mutex.lock();
    media_latest_report.data[1] = 0;
    for (int i = 0; i < MEDIA_LEY_LIST_LEN; i++)
    {
        media_latest_report.data[1] |= key[i];
    }
    if (!send(&media_latest_report))
    {
        _mutex.unlock();
        return true;
    }
    _mutex.unlock();
    return false;
}

bool newUSBKeyboard::send_full()
{
    _mutex.lock();
    if (!send(&PRIMARY_FULL_REPORT))
    {
        _mutex.unlock();
        return false;
    }
    _mutex.unlock();
    return true;
}

#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) + (1 * INTERFACE_DESCRIPTOR_LENGTH) + (1 * HID_DESCRIPTOR_LENGTH) + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

const uint8_t *newUSBKeyboard::configuration_desc(uint8_t index)
{
    if (index != 0)
    {
        return NULL;
    }
    uint8_t configuration_descriptor_temp[] = {
        CONFIGURATION_DESCRIPTOR_LENGTH, // bLength
        CONFIGURATION_DESCRIPTOR,        // bDescriptorType
        LSB(TOTAL_DESCRIPTOR_LENGTH),    // wTotalLength (LSB)
        MSB(TOTAL_DESCRIPTOR_LENGTH),    // wTotalLength (MSB)
        0x01,                            // bNumInterfaces
        DEFAULT_CONFIGURATION,           // bConfigurationValue
        0x00,                            // iConfiguration
        C_RESERVED | C_SELF_POWERED,     // bmAttributes
        C_POWER(0),                      // bMaxPower

        INTERFACE_DESCRIPTOR_LENGTH, // bLength
        INTERFACE_DESCRIPTOR,        // bDescriptorType
        0x00,                        // bInterfaceNumber
        0x00,                        // bAlternateSetting
        0x02,                        // bNumEndpoints
        HID_CLASS,                   // bInterfaceClass
        HID_SUBCLASS_BOOT,           // bInterfaceSubClass
        HID_PROTOCOL_KEYBOARD,       // bInterfaceProtocol
        0x00,                        // iInterface

        HID_DESCRIPTOR_LENGTH,                // bLength
        HID_DESCRIPTOR,                       // bDescriptorType
        LSB(HID_VERSION_1_11),                // bcdHID (LSB)
        MSB(HID_VERSION_1_11),                // bcdHID (MSB)
        0x00,                                 // bCountryCode
        0x01,                                 // bNumDescriptors
        REPORT_DESCRIPTOR,                    // bDescriptorType
        (uint8_t)(LSB(report_desc_length())), // wDescriptorLength (LSB)
        (uint8_t)(MSB(report_desc_length())), // wDescriptorLength (MSB)

        ENDPOINT_DESCRIPTOR_LENGTH, // bLength
        ENDPOINT_DESCRIPTOR,        // bDescriptorType
        _int_in,                    // bEndpointAddress
        E_INTERRUPT,                // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (MSB)
        1,                          // bInterval (milliseconds)

        ENDPOINT_DESCRIPTOR_LENGTH, // bLength
        ENDPOINT_DESCRIPTOR,        // bDescriptorType
        _int_out,                   // bEndpointAddress
        E_INTERRUPT,                // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (MSB)
        1,                          // bInterval (milliseconds)
    };
    MBED_ASSERT(sizeof(configuration_descriptor_temp) == sizeof(_configuration_descriptor));
    memcpy(_configuration_descriptor, configuration_descriptor_temp, sizeof(_configuration_descriptor));
    return _configuration_descriptor;
}

int newUSBKeyboard::_getc()
{
    return -1;
}
