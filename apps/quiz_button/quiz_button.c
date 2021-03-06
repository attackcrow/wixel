/**  Quiz Button App
This application is designed for a simple quiz button system, where 
only one button can be active at once. The app has three states:
Ready - LED off, Waiting for a button press or message from another Wixel
Active - LED on, broadcasting active state
Locked - LED off, waiting for locked timer to expire

== Parameters ==

radio_channel: See description in radio_link.h.
lockout_ms: Duration (in ms) to lockout when a button press is detected.
*/

/** Dependencies **************************************************************/
#include <cc2511_map.h>
#include <gpio.h>
#include <board.h>
#include <random.h>
#include <time.h>

#include <usb.h>
#include <usb_com.h>
#include <radio_queue.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
/** Parameters ****************************************************************/
uint32 CODE param_lockout_ms = 2000;

/** Definitions ***************************************************************/
typedef enum
{
    READY,
    ACTIVE,
    LOCKED
} state_t ;

#define BUTTON_PIN 16

/** Variables *****************************************************************/
static uint32 lock_time = 0;

/** Functions *****************************************************************/
void printPacket(uint8 XDATA * pkt);

void updateLeds(state_t state)
{
    usbShowStatusWithGreenLed();

    switch(state)
    {
        case ACTIVE:
            LED_YELLOW(0);
            LED_RED(1);
            break;
        case LOCKED:
            LED_YELLOW(1);
            LED_RED(0);
            break;
        default:
            LED_YELLOW(0);
            LED_RED(0);
            break;
    }
}


void sendActive(void)
{
   uint8 XDATA * packet = radioQueueTxCurrentPacket();
   if (packet != 0)
   {
       packet[0] = 2;
       packet[1] = 'C';
       packet[2] = 'C';
       radioQueueTxSendPacket();
       if (usbComTxAvailable())
       {
           putchar('.');
       }
   }
}

BIT receiveActive(void)
{
   uint8 XDATA * packet = radioQueueRxCurrentPacket();
   BIT retval = 0;
   if (packet != 0)
   {
       printPacket(packet);
       if (packet[0] == 2 && packet[1] == 'C' && packet[2] == 'C')
       {
           retval = 1;
       }
       radioQueueRxDoneWithPacket();
   }
   return retval;
}

state_t updateState(const state_t state)
{
    state_t newstate = state;
    BIT gotActive = receiveActive();
    switch(state)
    {
        case READY:
            if (gotActive)
            {
                newstate = LOCKED;
                lock_time = getMs();
            }
            if (isPinHigh(BUTTON_PIN) == LOW)
            {
                newstate = ACTIVE;
                sendActive();
                lock_time = getMs();
            }
            break;
        case ACTIVE:
            if (getMs() - lock_time > param_lockout_ms)
            {
                newstate = READY;
            }
            break;
        case LOCKED:
            if (getMs() - lock_time > param_lockout_ms)
            {
                newstate = READY;
            }
            break;
    }
    return newstate;
}


// This is called by printf and printPacket.
void putchar(char c)
{
    usbComTxSendByte(c);
}

char nibbleToAscii(uint8 nibble)
{
    nibble &= 0xF;
    if (nibble <= 0x9){ return '0' + nibble; }
    else{ return 'A' + (nibble - 0xA); }
}

void printPacket(uint8 XDATA * pkt)
{
    static uint16 pkt_count = 0;
    uint8 j, len;

    // index
    printf("%3d> ", pkt_count++);
    if (pkt_count > 999)
        pkt_count = 0;

    len = pkt[0];

    // ASCII packet data
    putchar('"');
    for (j = 1; j <= len; j++)
    {
        if (isprint(pkt[j]))
        {
            putchar(pkt[j]);
        }
        else
        {
            putchar('?');
        }
    }
    putchar('"');

    // pad with spaces
    for (; j <= RADIO_QUEUE_PAYLOAD_SIZE; j++)
    {
        putchar(' ');
    }
    putchar(' ');

    // CRC
    putchar((pkt[len + 2] & 0x80) ? ' ' : '!');
    putchar(' ');

    // RSSI, LQI
    printf("R:%4d ", (int8)(pkt[len + 1])/2 - 71);
    printf("L:%4d ", pkt[len + 2] & 0x7F);

    // sequence number
    printf("s:%1d ", pkt[1] & 0x1);

    // packet type
    switch((pkt[1] >> 6) & 3)
    {
        case 0: printf("PING  "); break;
        case 1: printf("NAK   "); break;
        case 2: printf("ACK   "); break;
        case 3: printf("RESET "); break;
    }

    // payload type
    putchar('p');
    putchar(':');
    putchar(nibbleToAscii(pkt[1] >> 1 & 0xF));
    putchar(' ');
    
    // packet contents in hex
    for(j = 0; j < len + 1; j++)  // add 1 for length byte
    {
        putchar(nibbleToAscii(pkt[j] >> 4));
        putchar(nibbleToAscii(pkt[j]));
    }
    putchar('\r');
    putchar('\n');
}

void printPacketIfNeeded()
{
    uint8 XDATA * packet;
    if ((packet = radioQueueRxCurrentPacket()) && usbComTxAvailable() >= 128)
    {
        printPacket(packet);
        radioQueueRxDoneWithPacket();
    }
}

void main()
{
    state_t state = READY;
    systemInit();
    usbInit();

    radioQueueInit();
    //radioQueueAllowCrcErrors = 1;

    while(1)
    {
        boardService();
        updateLeds(state);
        usbComService();
        state = updateState(state);
        //printPacketIfNeeded();
    }
}
