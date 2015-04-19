/**  Quiz Button App
This application is designed for a simple quiz button system, where 
only one button can be active at once. The app has four states:
Ready - LED off, Waiting for a button press or message from another Wixel
Arm_Active - LED off, broadcasting request to go active
Active - LED on, waiting for timer to expire
Locked - LED off, waiting for locked timer to expire

The master button button is indicated by setting param_master=1. The master
controls the state for the whole network.

== Parameters ==

radio_channel: See description in radio_link.h.
lockout_ms: Duration (in ms) to lockout when a button press is detected.

packets format:
byte 0    Length
     1    Command
     2-5  Source
     6-7  Time Remaining

Master sends 0000 for source, other nodes send their serial number as source.
Currently, 'C' is the only command, which indicates that buttons should 
transition to Locked state. Messages from the master include a Winner, which
is the ID of the node whose message was received first.

// TODO: Add time stamp for packet

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
uint32 CODE param_master = 0;

//#define DEBUG
//#define PACKET_DEBUG
#define USE_USB

#ifdef DEBUG
#define DEBUG_PRINTF(...) printf( __VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

/** Definitions ***************************************************************/
typedef enum
{
    READY,
    ARM_ACTIVE,
    ACTIVE,
    LOCKED
} state_t ;

#define BUTTON_PIN 16
#define MASTER_RESENDS 10

/** Variables *****************************************************************/
static uint32 unlockTime = 0;

/** Functions *****************************************************************/
void printPacket(uint8 XDATA * pkt);

void updateLeds(state_t state)
{
    static int pin = 0;
    usbShowStatusWithGreenLed();
    pin++;
    if (pin > 10000) pin = 0;
    setDigitalOutput(10, pin > 5000);

    switch(state)
    {
        case ACTIVE:
            LED_YELLOW(0);
            LED_RED(1);
            break;
        case ARM_ACTIVE:
            LED_YELLOW(1);
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

uint8 sendActive(uint8 cmd, uint8 *addr, uint32 timeLeft)
{
   uint8 XDATA * packet = radioQueueTxCurrentPacket();
   uint8 queued = radioQueueTxQueued();
   // NOTE: We don't want to queue up packets since they can cause timing problems
   if (!queued && packet != 0)
   {
       packet[0] = 7;
       packet[1] = cmd;
       packet[2] = addr[0];
       packet[3] = addr[1];
       packet[4] = addr[2];
       packet[5] = addr[3];
       packet[6] = (uint8)((timeLeft & 0xFF00) >> 8);
       packet[7] = (uint8)(timeLeft & 0xFF);

#ifdef PACKET_DEBUG
       DEBUG_PRINTF("t: %02x%02x\r\n", packet[6], packet[7]);
#endif

       radioQueueTxSendPacket();
       return 1;
   }
   return 0;
}

BIT receiveActive(void)
{
   uint8 XDATA * packet = radioQueueRxCurrentPacket();
   BIT retval = 0;
   if (packet != 0)
   {
       if (packet[0] == 2 && packet[1] == 'C' && packet[2] == 'C')
       {
           retval = 1;
       }
   }
   return retval;
}

uint32 getNextSendTime()
{
    uint8 m = param_master ? 60 : 120;
    uint32 next = getMs() + (uint32)(randomNumber() % m) + 20;
    // Dont resend if 250ms from unlock
    if ( next > (unlockTime - 250) ) 
    {
        next = ~0;
    }
    return next;
}


static uint8 active_addr[4];
#define setActive( a ) { \
    active_addr[0] = a[0];\
    active_addr[1] = a[1];\
    active_addr[2] = a[2];\
    active_addr[3] = a[3];\
}

state_t updateState(const state_t state)
{
    state_t newState = state;
    state_t oldState = state;
    uint8 XDATA *packet = radioQueueRxCurrentPacket();
    uint8 XDATA *src = NULL;
    int gotLock = 0;
    static nextSendTime;
    static BIT prevButtonPress = 0;
    uint32 remoteTime = 0;
    
    if (packet) 
    {
#ifdef PACKET_DEBUG
        if (usbComTxAvailable() >= 20) 
        {
            DEBUG_PRINTF("%d RECV %d %c %02x%02x-%02x%02x %02x%02x\r\n", state, packet[0], 
                         packet[1], packet[2], packet[3], packet[4], packet[5], 
                         packet[6], packet[7]);
        }
#endif
        // Check for a transition packet
        if ((param_master && packet[0] == 7 && packet[1] == 'C')
           || (!param_master && packet[0] == 7 && packet[1] == 'A'))
        {
            gotLock = 1;
            src = &packet[2];
            remoteTime = (uint32)packet[6] << 8 | (uint32)packet[7];
        }
        radioQueueRxDoneWithPacket();
    }

    switch(state)
    {
        case READY:
            if (gotLock && memcmp(src, serialNumber, 4) != 0)
            {
                if (param_master)
                {
                    setActive(src);
                    if (sendActive('A', active_addr, remoteTime))
                        nextSendTime = getNextSendTime();
                    else
                        nextSendTime = getMs();
                }
                newState = LOCKED;
                unlockTime = getMs() + remoteTime;
                DEBUG_PRINTF("lock\r\n");
            }
            else if (isPinHigh(BUTTON_PIN) == LOW && !prevButtonPress)
            {
                if (param_master)
                {
                    newState = ACTIVE;
                    unlockTime = getMs() + param_lockout_ms;
                    setActive(serialNumber);

                    if (sendActive('A', serialNumber, param_lockout_ms))
                        nextSendTime = getNextSendTime();
                    else
                        nextSendTime = getMs();

                }
                else
                {
                    newState = ARM_ACTIVE;
                    unlockTime = getMs() + param_lockout_ms;
                    if (sendActive('C', serialNumber, param_lockout_ms))
                        nextSendTime = getNextSendTime();
                    else
                        nextSendTime = getMs();
                }
            }
            break;
        case ARM_ACTIVE:
            // Only slaves can get into this state
            if (getMs() > unlockTime)
            {
                // Didn't hear back from the master so fail
                newState = READY;
            }
            else if (gotLock)
            { 
                if (memcmp(src, serialNumber, 4) == 0)
                {
                    DEBUG_PRINTF("L\r\n");
                    newState = ACTIVE;
                }
                else // Locked on someone else
                {
                    DEBUG_PRINTF("l\r\n");
                    newState = LOCKED;
                    unlockTime = remoteTime;
                }
            } 
            else if (getMs() > nextSendTime) // resend
            {
                sendActive('C', serialNumber, unlockTime - getMs());
                nextSendTime = getNextSendTime();
            }
            break;
        case ACTIVE:
            if (getMs() > unlockTime)
            {
                newState = READY;
            }
            else if (param_master && getMs() > nextSendTime)
            {
                if (sendActive('A', serialNumber, unlockTime - getMs()))
                    nextSendTime = getNextSendTime();
                else
                    nextSendTime = getMs();
            }
            break;
        case LOCKED:
            if (getMs() > unlockTime)
            {
                newState = READY;
            }
            else if (param_master && getMs() > nextSendTime)
            {
                if (sendActive('A', serialNumber, unlockTime - getMs()))
                    nextSendTime = getNextSendTime();
                else
                    nextSendTime = getMs();
            }
            break;
    }

    prevButtonPress = (isPinHigh(BUTTON_PIN) == LOW); 
    return newState;
}

// This is called by printf and printPacket.
void putchar(char c)
{
    usbComTxSendByte(c);
}

void main()
{
    state_t g_state = READY;
    systemInit();
#ifdef USE_USB
    usbInit();
#endif

    radioQueueInit();
    //radioQueueAllowCrcErrors = 1;
    
    randomSeedFromSerialNumber();

    while(1)
    {
        boardService();
        updateLeds(g_state);
#ifdef USE_USB
        usbComService();
#endif
        g_state = updateState(g_state);
    }
}
