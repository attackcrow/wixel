/** example_blink_led app:
This app blinks the red LED.

For a precompiled version of this app and a tutorial on how to load this app
onto your Wixel, see the Pololu Wixel User's Guide:
http://www.pololu.com/docs/0J46
*/

#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <stdio.h>

#define BUTTON_PIN 16
#define LOCKOUT_MS 2000

static uint32 lockout = 0;

void updateLeds()
{
    usbShowStatusWithGreenLed();

    LED_YELLOW(0);

    if (isPinHigh(BUTTON_PIN) == LOW)
    {
        LED_RED(1);
    }
    else
    {
        LED_RED(0);
    }
}

void init(void)
{
    setDigitalInput(BUTTON_PIN, HIGH);
}

void main()
{
    systemInit();
    usbInit();

    while(1)
    {
        boardService();
        updateLeds();
        usbComService();
    }
}
