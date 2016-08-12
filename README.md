# ShiftPoint Indicator 12LED with 7SegmentLED gear display (Fake tachometer) - V2.0

add 7Segment LED gear indication to a shiftpoint indicator and assume it ver2.0.

I used an arduino nano micro boad.

Description of connected equipment:

connect lead3 or lead8 of 7Segment LED to 5V PIN(anode common).

connect shift registers QA～QG to E,D,C,B,A,F,G segment of 7segment LED each via 240Ω resister.(D.P.segment is not used.)

connect shift registers SER(serial),RCLK(storage registor clock)、SRCLK(shift registor clock) to A0～A2 PIN（use as Digital i/o）.

connect shift registers VCC to 5V PIN, and connect VCC to 0.1μF Ceramic Condenser, and connect Ceramic Condenser to GND PIN.

connect shift registers SRCLR(direct overriding clear input) to 5V PIN.

connect shift registers GND,OE to GND PIN.

shift registers QH、QH' is not used.


Digital i/o pin 2-13 for Indicator LED Output: 

  D pin2:GREEN LED (add 180Ω resistor to the anode)
  
  D pin3:GREEN LED (add 180Ω resistor to the anode)
  
  D pin4:GREEN LED (add 180Ω resistor to the anode)
  
  D pin5:GREEN LED (add 180Ω resistor to the anode)
  
  D pin6:RED LED (add 180Ω resistor to the anode)
  
  D pin7:RED LED (add 180Ω resistor to the anode)
  
  D pin8:RED LED (add 180Ω resistor to the anode)
  
  D pin9:RED LED (add 180Ω resistor to the anode)
  
  D pin10:RED LED (add 180Ω resistor to the anode)
  
  D pin11:BLUE LED (add 120Ω resistor to the anode)
  
  D pin12:BLUE LED (add 120Ω resistor to the anode)
  
  D pin13:BLUE LED (add 120Ω resistor to the anode)
  
Digital i/o pin 14-16(=Analog i/o pin 0-2) for Shift Register control: 

  D pin14:Shift Regisiter SER(SI) (pinA0 use as Digital i/o)
  
  D pin15:Shift Regisiter RCLK(RCK) (pinA1 use as Digital i/o)

  D pin16:Shift Regisiter SRCLK(SCK) (pinA2 use as Digital i/o)
  
Analog i/o pin 3 for ADXL335 Accelerometer Input, Analog i/o pin 4 for CarSpeed Pulse Input:

  A pin 3:ADXL335 Y axis

  A pin 4:SpeedPulse from car ECU, connect to switching diode Cathode, and connect switching diode Anode to Car Speedpulse wire
  (I used 1N4148 Small signal switching diode for Backflow prevention)
  
3v3 PIN:connect to ADXL335 vdd

5v PIN: connect to 1kΩ resistor,and connect there to switching diode Cathode(for 5v PullUp)
