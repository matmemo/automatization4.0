EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
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
$Descr A4 11693 8268
encoding utf-8
Sheet 2 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LM741 U1
U 1 1 5A8CA75B
P 6400 1750
AR Path="/5A8CA49C/5A8CA75B" Ref="U1"  Part="1" 
AR Path="/5A8CAC64/5A8CA75B" Ref="U2"  Part="1" 
F 0 "U1" H 6400 2000 50  0000 L CNN
F 1 "LM741" H 6400 1900 50  0000 L CNN
F 2 "Housings_DIP:DIP-8_W7.62mm_LongPads" H 6450 1800 50  0001 C CNN
F 3 "" H 6550 1900 50  0001 C CNN
	1    6400 1750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 5A8CA768
P 6300 2100
AR Path="/5A8CA49C/5A8CA768" Ref="#PWR08"  Part="1" 
AR Path="/5A8CAC64/5A8CA768" Ref="#PWR012"  Part="1" 
F 0 "#PWR08" H 6300 1850 50  0001 C CNN
F 1 "GND" H 6300 1950 50  0000 C CNN
F 2 "" H 6300 2100 50  0001 C CNN
F 3 "" H 6300 2100 50  0001 C CNN
	1    6300 2100
	1    0    0    -1  
$EndComp
$Comp
L R_Small 10k
U 1 1 5A8CA76E
P 6350 2450
AR Path="/5A8CA49C/5A8CA76E" Ref="10k"  Part="1" 
AR Path="/5A8CAC64/5A8CA76E" Ref="R8"  Part="1" 
F 0 "10k" H 6380 2470 50  0000 L CNN
F 1 "R_Small" H 6380 2410 50  0000 L CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 6350 2450 50  0001 C CNN
F 3 "" H 6350 2450 50  0001 C CNN
	1    6350 2450
	0    1    1    0   
$EndComp
$Comp
L R_Small R3
U 1 1 5A8CA775
P 5700 1850
AR Path="/5A8CA49C/5A8CA775" Ref="R3"  Part="1" 
AR Path="/5A8CAC64/5A8CA775" Ref="R7"  Part="1" 
F 0 "R3" H 5730 1870 50  0000 L CNN
F 1 "5k" H 5730 1810 50  0000 L CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 5700 1850 50  0001 C CNN
F 3 "" H 5700 1850 50  0001 C CNN
	1    5700 1850
	0    1    1    0   
$EndComp
$Comp
L C_Small C1
U 1 1 5A8CA77C
P 5400 1750
AR Path="/5A8CA49C/5A8CA77C" Ref="C1"  Part="1" 
AR Path="/5A8CAC64/5A8CA77C" Ref="C2"  Part="1" 
F 0 "C1" H 5410 1820 50  0000 L CNN
F 1 "C_Small" H 5350 1950 50  0000 L CNN
F 2 "Capacitors_THT:C_Rect_L7.0mm_W2.5mm_P5.00mm" H 5400 1750 50  0001 C CNN
F 3 "" H 5400 1750 50  0001 C CNN
	1    5400 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 2100 6300 2050
Wire Wire Line
	6300 1450 6300 1450
Wire Wire Line
	7250 1750 6700 1750
Wire Wire Line
	6250 2450 5950 2450
Wire Wire Line
	5950 2450 5950 1850
Wire Wire Line
	5800 1850 6100 1850
Wire Wire Line
	6450 2450 6850 2450
Connection ~ 5950 1850
Wire Wire Line
	5150 1650 6100 1650
Wire Wire Line
	5600 1850 5400 1850
$Comp
L GND #PWR09
U 1 1 5A8CA78D
P 5150 1750
AR Path="/5A8CA49C/5A8CA78D" Ref="#PWR09"  Part="1" 
AR Path="/5A8CAC64/5A8CA78D" Ref="#PWR013"  Part="1" 
F 0 "#PWR09" H 5150 1500 50  0001 C CNN
F 1 "GND" H 5150 1600 50  0000 C CNN
F 2 "" H 5150 1750 50  0001 C CNN
F 3 "" H 5150 1750 50  0001 C CNN
	1    5150 1750
	1    0    0    -1  
$EndComp
Connection ~ 5400 1650
$Comp
L R_Small R2
U 1 1 5A8CA794
P 5150 2150
AR Path="/5A8CA49C/5A8CA794" Ref="R2"  Part="1" 
AR Path="/5A8CAC64/5A8CA794" Ref="R6"  Part="1" 
F 0 "R2" H 5180 2170 50  0000 L CNN
F 1 "22k" H 5180 2110 50  0000 L CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 5150 2150 50  0001 C CNN
F 3 "" H 5150 2150 50  0001 C CNN
	1    5150 2150
	0    1    1    0   
$EndComp
Wire Wire Line
	5250 2150 5400 2150
Wire Wire Line
	5400 2150 5400 1850
Wire Wire Line
	5150 1750 5150 1650
$Comp
L +5V #PWR010
U 1 1 5A8CA79E
P 4750 1850
AR Path="/5A8CA49C/5A8CA79E" Ref="#PWR010"  Part="1" 
AR Path="/5A8CAC64/5A8CA79E" Ref="#PWR014"  Part="1" 
F 0 "#PWR010" H 4750 1700 50  0001 C CNN
F 1 "+5V" H 4750 1990 50  0000 C CNN
F 2 "" H 4750 1850 50  0001 C CNN
F 3 "" H 4750 1850 50  0001 C CNN
	1    4750 1850
	1    0    0    -1  
$EndComp
$Comp
L R_Small R1
U 1 1 5A8CA7A4
P 4750 2000
AR Path="/5A8CA49C/5A8CA7A4" Ref="R1"  Part="1" 
AR Path="/5A8CAC64/5A8CA7A4" Ref="R5"  Part="1" 
F 0 "R1" H 4780 2020 50  0000 L CNN
F 1 "1k5" H 4780 1960 50  0000 L CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4750 2000 50  0001 C CNN
F 3 "" H 4750 2000 50  0001 C CNN
	1    4750 2000
	-1   0    0    1   
$EndComp
Wire Wire Line
	4750 1900 4750 1850
Wire Wire Line
	4750 2100 4750 2150
Wire Wire Line
	4400 2150 5050 2150
Wire Wire Line
	6850 2450 6850 1750
Connection ~ 6850 1750
Text HLabel 7250 1750 2    60   Input ~ 0
out
Text HLabel 4400 2150 0    60   Input ~ 0
in
Connection ~ 4750 2150
$Comp
L +12V #PWR011
U 1 1 5A8CCA1B
P 6300 1350
AR Path="/5A8CA49C/5A8CCA1B" Ref="#PWR011"  Part="1" 
AR Path="/5A8CAC64/5A8CCA1B" Ref="#PWR015"  Part="1" 
F 0 "#PWR011" H 6300 1200 50  0001 C CNN
F 1 "+12V" H 6300 1490 50  0000 C CNN
F 2 "" H 6300 1350 50  0001 C CNN
F 3 "" H 6300 1350 50  0001 C CNN
	1    6300 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 1350 6300 1450
$EndSCHEMATC
