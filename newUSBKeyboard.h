#ifndef NEWUSBKEYBOARD_H
#define NEWUSBKEYBOARD_H

#include <Arduino.h>
#include <USBHID.h>
#include <platform/Stream.h>
#include <PlatformMutex.h>

#define PRIMARY_KEY_LIST_LEN 6
#define MODIFIER_LEY_LIST_LEN 8
#define MEDIA_LEY_LIST_LEN 7

#define REPORT_ID_PRIMARY 1
#define REPORT_ID_MEDIA 3

enum MODIFIER_KEY
{
    KEY_CTRL = 0b00000001,
    KEY_SHIFT = 0b00000010,
    KEY_ALT = 0b00000100,
    KEY_LOGO = 0b00001000,
    KEY_RCTRL = 0b00010000,
    KEY_RSHIFT = 0b00100000,
    KEY_RALT = 0b01000000,
    KEY_RLOGO = 0x10000000,
};

enum MEDIA_KEY
{
    KEY_NEXT_TRACK = 0b00000001,     /*!< next Track Button */
    KEY_PREVIOUS_TRACK = 0b00000010, /*!< Previous track Button */
    KEY_STOP = 0b00000100,           /*!< Stop Button */
    KEY_PLAY_PAUSE = 0b00001000,     /*!< Play/Pause Button */
    KEY_MUTE = 0b00010000,           /*!< Mute Button */
    KEY_VOLUME_UP = 0b00100000,      /*!< Volume Up Button */
    KEY_VOLUME_DOWN = 0b01000000,    /*!< Volume Down Button */
};

enum PRIMARY_KEY
{

    KEY_F1 = 0x3a,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_PRTSC,
    KEY_SCRLK,

    KEY_A = 0x04,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_1 = 0x1e,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,

    KEY_BACKSPACE = 0x2a,
    KEY_TAB = 0x2b,
    KEY_ENTER = 0x28,
    KEY_ESC = 0x29,
    KEY_SPACE = 0x2c,
    KEY_QUOTE = 0x34,
    KEY_COMMA = 0x36,
    KEY_MINUS = 0x2d,
    KEY_DOT = 0x37,
    KEY_SLASH = 0x38,
    KEY_SEMICOLON = 0x33,
    KEY_EQUAL = 0x2e,
    KEY_BRACKET_L = 0x2f,
    KEY_BRACKET_R = 0x30,
    KEY_BACKSLASH = 0x31,
    KEY_BACKQUOTE = 0x35,

    KEY_CAPSLOCK = 0x39,
    KEY_NUMLOCK = 0x53,
    KEY_INSERT = 0x49,
    KEY_HOME = 0x4a,
    KEY_PAGEUP = 0x4b,
    KEY_DELETE = 0x4c,
    KEY_PAGEDOWN = 0x4e,

    KEY_ARROW_R = 0x4f,
    KEY_ARROW_L,
    KEY_ARROW_D,
    KEY_ARROW_U,
};

class newUSBKeyboard : public USBHID, public ::mbed::Stream
{
public:
    newUSBKeyboard(bool connect_blocking = true, uint16_t vendor_id = 0x1235, uint16_t product_id = 0x0050, uint16_t product_release = 0x0001);
    newUSBKeyboard(USBPhy *phy, uint16_t vendor_id = 0x1235, uint16_t product_id = 0x0050, uint16_t product_release = 0x0001);
    virtual ~newUSBKeyboard();

    virtual int _putc(int c);

    bool send_key(uint8_t *primary_key, uint8_t *modifier_key);
    bool send_media(uint8_t *key);
    bool send_full();
    virtual const uint8_t *report_desc();
    virtual void report_rx();
    uint8_t lock_status();

protected:
    virtual const uint8_t *configuration_desc(uint8_t index);

private:
    virtual int _getc();

    uint8_t _configuration_descriptor[41];
    uint8_t _lock_status;
    PlatformMutex _mutex;

    HID_REPORT primary_latest_report;
    HID_REPORT media_latest_report;
    HID_REPORT PRIMARY_FULL_REPORT;
};

#endif
