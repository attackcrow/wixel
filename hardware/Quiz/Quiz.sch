EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 25 0
EELAYER END
$Descr USLetter 8500 11000 portrait
encoding utf-8
Sheet 1 1
Title "Wixel Quiz Button"
Date ""
Rev ""
Comp ""
Comment1 "For use with https://github.com/attackcrow/wixel"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Battery BT1
U 1 1 553AA46D
P 2400 2450
F 0 "BT1" H 2500 2500 50  0000 L CNN
F 1 "6V" H 2500 2400 50  0000 L CNN
F 2 "" V 2400 2490 60  0000 C CNN
F 3 "" V 2400 2490 60  0000 C CNN
	1    2400 2450
	-1   0    0    1   
$EndComp
$Comp
L Wixel U1
U 1 1 553ACCBA
P 3850 2950
F 0 "U1" H 3850 3100 70  0000 C CNN
F 1 "WIXEL" H 3850 2900 70  0000 C CNN
F 2 "" H 3850 3000 60  0000 C CNN
F 3 "" H 3850 3000 60  0000 C CNN
	1    3850 2950
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 553ACCFF
P 5300 3000
F 0 "SW1" H 5450 3110 50  0000 C CNN
F 1 "PUSH" H 5300 2920 50  0000 C CNN
F 2 "" H 5300 3000 60  0000 C CNN
F 3 "" H 5300 3000 60  0000 C CNN
	1    5300 3000
	1    0    0    -1  
$EndComp
$Comp
L LED D1
U 1 1 553ACD68
P 5400 2650
F 0 "D1" H 5400 2750 50  0000 C CNN
F 1 "LED" H 5400 2550 50  0000 C CNN
F 2 "" H 5400 2650 60  0000 C CNN
F 3 "" H 5400 2650 60  0000 C CNN
	1    5400 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 2300 5600 2300
Connection ~ 5600 2650
Wire Wire Line
	5000 3000 4800 3000
Wire Wire Line
	4800 2400 5200 2400
Wire Wire Line
	5200 2400 5200 2650
Wire Wire Line
	2400 2300 2900 2300
Wire Wire Line
	2900 2400 2650 2400
Wire Wire Line
	2650 2400 2650 2600
Wire Wire Line
	2650 2600 2400 2600
Wire Wire Line
	5600 2300 5600 3000
NoConn ~ 4800 2500
NoConn ~ 4800 2600
NoConn ~ 4800 2700
NoConn ~ 4800 2800
NoConn ~ 4800 2900
NoConn ~ 4800 3100
NoConn ~ 4800 3200
NoConn ~ 4800 3300
NoConn ~ 4800 3400
NoConn ~ 2900 3300
NoConn ~ 2900 3200
NoConn ~ 2900 3100
NoConn ~ 2900 3000
NoConn ~ 2900 2900
NoConn ~ 2900 2800
NoConn ~ 2900 2700
NoConn ~ 2900 2600
NoConn ~ 2900 2500
$EndSCHEMATC
