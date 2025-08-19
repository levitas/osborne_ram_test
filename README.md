# Osborne Ram Test
Osborne Executive 64k RAM Board Test

For use with Osborne Computer Corp. RD1140 REV. 02  
Testing done with Arduino MEGA 2560

This program tests all addresses using a "Walking Ones" pattern, running through all addresses 8 times to test all bits.

Arduino Wiring:  
| RAM Board Pin | Function | Arduino Pin |
| --- | --- | --- |
| 1 | GND | GND |
5 | RFSH* | GND
7 | RAS | 46
9 | GND | GND
13 | CAS* | 48
15 | WR* | 40 
16 | RD* | 8
17 | CAE* | 42
19 | RAE* | 44
27 | A15 | 23
28 | A14 | 25
29 | A13 | 27
30 | A12 | 29
31 | A11 | 31
33 | A10 | 33
34 | A9 | 35
35 | A8 | 37
36 | A7 | 39
37 | A6 | 41
39 | A5 | 43
40 | A4 | 45
41 | A3 | 47
42 | A2 | 49
43 | A1 | 51
44 | A0 | 53
47 | D7 | 22
48 | D6 | 24
49 | D5 | 26
50 | D4 | 28
51 | D3 | 30
53 | D2 | 32
54 | D1 | 34
55 | D0 | 36
65 | 5V | 5V
