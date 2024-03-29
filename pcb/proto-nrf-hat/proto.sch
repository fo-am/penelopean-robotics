EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L proto-rescue:Conn_01x04 J2
U 1 1 5AB7BCE5
P 2100 2250
F 0 "J2" H 2100 2450 50  0000 C CNN
F 1 "Power/i2c" H 2100 1950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm_SMD_Pin1Left" H 2100 2250 50  0001 C CNN
F 3 "" H 2100 2250 50  0001 C CNN
	1    2100 2250
	-1   0    0    1   
$EndComp
$Comp
L proto-rescue:Conn_01x07 J4
U 1 1 5AB7C2DC
P 3100 2250
F 0 "J4" H 3100 2650 50  0000 C CNN
F 1 "Port C" H 3100 1850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x07_Pitch2.54mm_SMD_Pin1Right" H 3100 2250 50  0001 C CNN
F 3 "" H 3100 2250 50  0001 C CNN
	1    3100 2250
	-1   0    0    1   
$EndComp
$Comp
L proto-rescue:Conn_01x06 J3
U 1 1 5AB7CA31
P 6300 2750
F 0 "J3" H 6300 3050 50  0000 C CNN
F 1 "Port B" H 6300 2350 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm_SMD_Pin1Right" H 6300 2750 50  0001 C CNN
F 3 "" H 6300 2750 50  0001 C CNN
	1    6300 2750
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Conn_01x01 J9
U 1 1 5ACA1E49
P 8850 3350
F 0 "J9" H 8850 3450 50  0000 C CNN
F 1 "Conn_01x01" H 8850 3250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_Pitch2.54mm" H 8850 3350 50  0001 C CNN
F 3 "" H 8850 3350 50  0001 C CNN
	1    8850 3350
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Conn_01x01 J10
U 1 1 5ACA1EC1
P 8850 3500
F 0 "J10" H 8850 3600 50  0000 C CNN
F 1 "Conn_01x01" H 8850 3400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_Pitch2.54mm" H 8850 3500 50  0001 C CNN
F 3 "" H 8850 3500 50  0001 C CNN
	1    8850 3500
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Conn_01x03 J1
U 1 1 5BBB4667
P 4000 1850
F 0 "J1" H 4000 2050 50  0000 C CNN
F 1 "Servo 1" H 4000 1650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 4000 1850 50  0001 C CNN
F 3 "" H 4000 1850 50  0001 C CNN
	1    4000 1850
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Conn_01x03 J6
U 1 1 5BBB46C0
P 4000 2300
F 0 "J6" H 4000 2500 50  0000 C CNN
F 1 "Servo 2" H 4000 2100 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 4000 2300 50  0001 C CNN
F 3 "" H 4000 2300 50  0001 C CNN
	1    4000 2300
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Conn_01x03 J7
U 1 1 5BBB46E7
P 4000 2750
F 0 "J7" H 4000 2950 50  0000 C CNN
F 1 "Servo 3" H 4000 2550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 4000 2750 50  0001 C CNN
F 3 "" H 4000 2750 50  0001 C CNN
	1    4000 2750
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Earth #PWR01
U 1 1 5BBB4A4F
P 2300 2050
F 0 "#PWR01" H 2300 1800 50  0001 C CNN
F 1 "Earth" H 2300 1900 50  0001 C CNN
F 2 "" H 2300 2050 50  0001 C CNN
F 3 "" H 2300 2050 50  0001 C CNN
	1    2300 2050
	0    -1   -1   0   
$EndComp
$Comp
L proto-rescue:+5V #PWR02
U 1 1 5BBB4A75
P 2300 2150
F 0 "#PWR02" H 2300 2000 50  0001 C CNN
F 1 "+5V" H 2300 2290 50  0000 C CNN
F 2 "" H 2300 2150 50  0001 C CNN
F 3 "" H 2300 2150 50  0001 C CNN
	1    2300 2150
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:Earth #PWR03
U 1 1 5BBB4F2D
P 3800 1950
F 0 "#PWR03" H 3800 1700 50  0001 C CNN
F 1 "Earth" H 3800 1800 50  0001 C CNN
F 2 "" H 3800 1950 50  0001 C CNN
F 3 "" H 3800 1950 50  0001 C CNN
	1    3800 1950
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:Earth #PWR04
U 1 1 5BBB5010
P 3800 2400
F 0 "#PWR04" H 3800 2150 50  0001 C CNN
F 1 "Earth" H 3800 2250 50  0001 C CNN
F 2 "" H 3800 2400 50  0001 C CNN
F 3 "" H 3800 2400 50  0001 C CNN
	1    3800 2400
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:Earth #PWR05
U 1 1 5BBB5195
P 3800 2850
F 0 "#PWR05" H 3800 2600 50  0001 C CNN
F 1 "Earth" H 3800 2700 50  0001 C CNN
F 2 "" H 3800 2850 50  0001 C CNN
F 3 "" H 3800 2850 50  0001 C CNN
	1    3800 2850
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:+5V #PWR06
U 1 1 5BBB548B
P 3800 1850
F 0 "#PWR06" H 3800 1700 50  0001 C CNN
F 1 "+5V" H 3800 1990 50  0000 C CNN
F 2 "" H 3800 1850 50  0001 C CNN
F 3 "" H 3800 1850 50  0001 C CNN
	1    3800 1850
	0    -1   -1   0   
$EndComp
$Comp
L proto-rescue:+5V #PWR07
U 1 1 5BBB5622
P 3800 2300
F 0 "#PWR07" H 3800 2150 50  0001 C CNN
F 1 "+5V" H 3800 2440 50  0000 C CNN
F 2 "" H 3800 2300 50  0001 C CNN
F 3 "" H 3800 2300 50  0001 C CNN
	1    3800 2300
	0    -1   -1   0   
$EndComp
$Comp
L proto-rescue:+5V #PWR08
U 1 1 5BBB5705
P 3800 2750
F 0 "#PWR08" H 3800 2600 50  0001 C CNN
F 1 "+5V" H 3800 2890 50  0000 C CNN
F 2 "" H 3800 2750 50  0001 C CNN
F 3 "" H 3800 2750 50  0001 C CNN
	1    3800 2750
	0    -1   -1   0   
$EndComp
$Comp
L proto-rescue:Conn_02x04_Counter_Clockwise J8
U 1 1 5BBB6AFE
P 4950 2700
F 0 "J8" H 5000 2900 50  0000 C CNN
F 1 "Conn_02x04_Counter_Clockwise" H 5000 2400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 4950 2700 50  0001 C CNN
F 3 "" H 4950 2700 50  0001 C CNN
	1    4950 2700
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Earth #PWR09
U 1 1 5BBB71DA
P 4750 2600
F 0 "#PWR09" H 4750 2350 50  0001 C CNN
F 1 "Earth" H 4750 2450 50  0001 C CNN
F 2 "" H 4750 2600 50  0001 C CNN
F 3 "" H 4750 2600 50  0001 C CNN
	1    4750 2600
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:+5V #PWR010
U 1 1 5BBB7322
P 4450 1700
F 0 "#PWR010" H 4450 1550 50  0001 C CNN
F 1 "+5V" H 4450 1840 50  0000 C CNN
F 2 "" H 4450 1700 50  0001 C CNN
F 3 "" H 4450 1700 50  0001 C CNN
	1    4450 1700
	0    -1   -1   0   
$EndComp
$Comp
L proto-rescue:Earth #PWR011
U 1 1 5BBB7616
P 4850 1900
F 0 "#PWR011" H 4850 1650 50  0001 C CNN
F 1 "Earth" H 4850 1750 50  0001 C CNN
F 2 "" H 4850 1900 50  0001 C CNN
F 3 "" H 4850 1900 50  0001 C CNN
	1    4850 1900
	-1   0    0    1   
$EndComp
$Comp
L proto-rescue:CP C2
U 1 1 5BBB7B4C
P 5150 2150
F 0 "C2" H 5175 2250 50  0000 L CNN
F 1 "CP" H 5175 2050 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_Tantal_D5.5mm_P5.00mm" H 5188 2000 50  0001 C CNN
F 3 "" H 5150 2150 50  0001 C CNN
	1    5150 2150
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:LED D1
U 1 1 5BBB94A5
P 6050 2250
F 0 "D1" H 6050 2350 50  0000 C CNN
F 1 "LED" H 6050 2150 50  0000 C CNN
F 2 "LEDs:LED_D3.0mm_FlatTop" H 6050 2250 50  0001 C CNN
F 3 "" H 6050 2250 50  0001 C CNN
	1    6050 2250
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:R R1
U 1 1 5BBB950A
P 6050 1900
F 0 "R1" V 6130 1900 50  0000 C CNN
F 1 "R" V 6050 1900 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 5980 1900 50  0001 C CNN
F 3 "" H 6050 1900 50  0001 C CNN
	1    6050 1900
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:Earth #PWR012
U 1 1 5BBB9541
P 6050 1650
F 0 "#PWR012" H 6050 1400 50  0001 C CNN
F 1 "Earth" H 6050 1500 50  0001 C CNN
F 2 "" H 6050 1650 50  0001 C CNN
F 3 "" H 6050 1650 50  0001 C CNN
	1    6050 1650
	-1   0    0    1   
$EndComp
Text Label 4550 2000 0    60   ~ 0
+3v
Text Label 4400 3250 0    60   ~ 0
CE
Text Label 5800 3150 0    60   ~ 0
SCK
Text Label 5400 2700 0    60   ~ 0
MISO
Text Label 5350 3600 0    60   ~ 0
IRQ
Text Label 5400 2800 0    60   ~ 0
MOSI
Text Label 4850 3150 0    60   ~ 0
CSN
Text Label 5350 2000 0    60   ~ 0
Reg3v
$Comp
L proto-rescue:Conn_01x05 J11
U 1 1 5C81939A
P 2150 3150
F 0 "J11" H 2150 3450 50  0000 C CNN
F 1 "Sensors" H 2150 2850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05_Pitch2.54mm" H 2150 3150 50  0001 C CNN
F 3 "" H 2150 3150 50  0001 C CNN
	1    2150 3150
	-1   0    0    1   
$EndComp
$Comp
L proto-rescue:+5V #PWR013
U 1 1 5C819423
P 2350 2950
F 0 "#PWR013" H 2350 2800 50  0001 C CNN
F 1 "+5V" H 2350 3090 50  0000 C CNN
F 2 "" H 2350 2950 50  0001 C CNN
F 3 "" H 2350 2950 50  0001 C CNN
	1    2350 2950
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:Earth #PWR014
U 1 1 5C819457
P 2350 3150
F 0 "#PWR014" H 2350 2900 50  0001 C CNN
F 1 "Earth" H 2350 3000 50  0001 C CNN
F 2 "" H 2350 3150 50  0001 C CNN
F 3 "" H 2350 3150 50  0001 C CNN
	1    2350 3150
	0    -1   -1   0   
$EndComp
Text Label 2450 3250 0    60   ~ 0
SCL
Text Label 2450 3350 0    60   ~ 0
SDA
$Comp
L proto-rescue:Conn_01x02 J12
U 1 1 5C9285A3
P 2100 1750
F 0 "J12" H 2100 1850 50  0000 C CNN
F 1 "Conn_01x02" H 2100 1550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 2100 1750 50  0001 C CNN
F 3 "" H 2100 1750 50  0001 C CNN
	1    2100 1750
	-1   0    0    1   
$EndComp
$Comp
L proto-rescue:+5V #PWR015
U 1 1 5C92868A
P 2300 1650
F 0 "#PWR015" H 2300 1500 50  0001 C CNN
F 1 "+5V" H 2300 1790 50  0000 C CNN
F 2 "" H 2300 1650 50  0001 C CNN
F 3 "" H 2300 1650 50  0001 C CNN
	1    2300 1650
	0    1    1    0   
$EndComp
$Comp
L proto-rescue:Earth #PWR016
U 1 1 5C9286C0
P 2300 1750
F 0 "#PWR016" H 2300 1500 50  0001 C CNN
F 1 "Earth" H 2300 1600 50  0001 C CNN
F 2 "" H 2300 1750 50  0001 C CNN
F 3 "" H 2300 1750 50  0001 C CNN
	1    2300 1750
	0    -1   -1   0   
$EndComp
$Comp
L proto-rescue:Conn_01x08 J5
U 1 1 5AB7BD68
P 6700 4100
F 0 "J5" H 6700 4500 50  0000 C CNN
F 1 "Port D" H 6700 3600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x08_Pitch2.54mm_SMD_Pin1Left" H 6700 4100 50  0001 C CNN
F 3 "" H 6700 4100 50  0001 C CNN
	1    6700 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 1700 5350 1700
Wire Wire Line
	4450 1700 4550 1700
Connection ~ 4550 1700
Wire Wire Line
	3300 2550 3400 2550
Wire Wire Line
	3400 1750 3800 1750
Wire Wire Line
	3300 2450 3550 2450
Wire Wire Line
	3550 2450 3550 2200
Wire Wire Line
	3550 2200 3800 2200
Wire Wire Line
	3300 2350 3500 2350
Wire Wire Line
	3500 2350 3500 2650
Wire Wire Line
	3500 2650 3800 2650
Wire Wire Line
	6100 2550 6050 2550
Wire Wire Line
	6050 2550 6050 2400
Wire Wire Line
	6050 2100 6050 2050
Wire Wire Line
	6050 1750 6050 1650
Wire Wire Line
	5750 3250 4350 3250
Wire Wire Line
	6000 3050 6100 3050
Wire Wire Line
	5650 2950 6100 2950
Wire Wire Line
	5250 2800 5900 2800
Wire Wire Line
	5900 2850 6100 2850
Wire Wire Line
	5900 2800 5900 2850
Wire Wire Line
	5750 2650 5750 3250
Wire Wire Line
	5350 2500 4600 2500
Wire Wire Line
	4600 2500 4600 2700
Wire Wire Line
	4600 2700 4750 2700
Wire Wire Line
	5350 1700 5350 2150
Wire Wire Line
	4350 3250 4350 2800
Wire Wire Line
	4350 2800 4750 2800
Wire Wire Line
	5700 3050 4750 3050
Wire Wire Line
	4750 3050 4750 2900
Wire Wire Line
	5650 2950 5650 2700
Wire Wire Line
	5650 2700 5250 2700
Wire Wire Line
	6500 4000 5350 4000
Wire Wire Line
	5350 4000 5350 2600
Wire Wire Line
	5350 2600 5250 2600
Wire Wire Line
	6000 3050 6000 3150
Wire Wire Line
	6000 3150 5550 3150
Wire Wire Line
	5550 3150 5550 2900
Wire Wire Line
	5550 2900 5250 2900
Wire Wire Line
	3350 3250 2350 3250
Wire Wire Line
	3450 3350 2350 3350
Wire Wire Line
	3450 2150 3450 3350
Wire Wire Line
	3350 2050 3350 3250
$Comp
L proto-rescue:LP2950-3.0_TO92 U1
U 1 1 5CABB898
P 4950 1700
F 0 "U1" H 4800 1825 50  0000 C CNN
F 1 "LP2950-3.0_TO92" H 4950 1825 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Wide" H 4950 1925 50  0001 C CIN
F 3 "" H 4950 1650 50  0001 C CNN
	1    4950 1700
	1    0    0    -1  
$EndComp
$Comp
L proto-rescue:CP C1
U 1 1 5BBB776B
P 4700 2150
F 0 "C1" H 4725 2250 50  0000 L CNN
F 1 "CP" H 4725 2050 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_Tantal_D5.5mm_P5.00mm" H 4738 2000 50  0001 C CNN
F 3 "" H 4700 2150 50  0001 C CNN
	1    4700 2150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4550 1700 4550 2150
Wire Wire Line
	4950 2000 4950 2150
Wire Wire Line
	4850 2150 4950 2150
Connection ~ 4950 2150
Wire Wire Line
	5300 2150 5350 2150
Connection ~ 5350 2150
Wire Wire Line
	5700 3050 5700 2750
Wire Wire Line
	5700 2750 6100 2750
Wire Wire Line
	5750 2650 6100 2650
Wire Wire Line
	3350 2050 3300 2050
Wire Wire Line
	3400 2550 3400 1750
Wire Wire Line
	3450 2150 3300 2150
$Comp
L proto-rescue:Earth #PWR?
U 1 1 5CABEB6C
P 4950 2250
F 0 "#PWR?" H 4950 2000 50  0001 C CNN
F 1 "Earth" H 4950 2100 50  0001 C CNN
F 2 "" H 4950 2250 50  0001 C CNN
F 3 "" H 4950 2250 50  0001 C CNN
	1    4950 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 1700 4650 1700
Wire Wire Line
	4950 2150 4950 2250
Wire Wire Line
	4950 2150 5000 2150
Wire Wire Line
	5350 2150 5350 2500
$EndSCHEMATC
