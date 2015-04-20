Pololu Wixel Software Development Kit (SDK)

This package contains code and Makefiles that will help you create
your own applications for the Pololu Wixel.  The Wixel is a general-
purpose programmable module featuring a 2.4 GHz radio and USB.  The
Wixel is based on the CC2511F32 microcontroller from Texas Instruments,
which has an integrated radio transceiver, 32 KB of flash memory, 4 KB
of RAM, and a full-speed USB interface.

For documentation of this SDK, see:
http://pololu.github.com/wixel-sdk

(You can also generate the documentation yourself by running
Doxygen on libraries/Doxyfile.)

For more information about the Wixel, see the Wixel User's Guide:
http://www.pololu.com/docs/0J46

This contains the following additional apps:

quiz_button:
A peer-to-peer quiz button app that attempts to allow only one Wixel to
activate (turn on its red LED) at once. This app does a poor job of
handling simultaneous button presses. The parameter lockout_ms controls
how long (in ms) buttons are locked out after one button goes active.

Connect a button between P1_6 and Ground.

quiz_network:
A master-slave quiz button app, that works better than quiz_button but
requires a "master" button that all other buttons transmit to and receive
from. The master button is set by setting the configuration parameter
param_master to 1.

Connect a button between P1_6 and Ground. Connect status LED or LEDs (with 
current limiting resistor, if needed) between P1_0 and ground. Exactly one
Wixel must have config_master set to 1; all others must be 0.
