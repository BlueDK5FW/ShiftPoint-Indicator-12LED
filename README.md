# ShiftPoint-Indicator-12LED (Fake tachometer)-V1.0

Shift Point Indicator (Fake tachometer) using a Speedpulse and a tripple axis Accelerometer

I used an arduino nano micro boad.

Description of connected equipment:

Digital i/o pin 2-13 for Indicator LED Output: 

  D pin2:GREEN LED (add 180Ω resistor to the anode)
  
  D pin3:GREEN LED (add 180Ω resistor to the anode)
  
  D pin3:GREEN LED (add 180Ω resistor to the anode)
  
  D pin4:GREEN LED (add 180Ω resistor to the anode)
  
  D pin5:RED LED (add 180Ω resistor to the anode)
  
  D pin6:RED LED (add 180Ω resistor to the anode)
  
  D pin7:RED LED (add 180Ω resistor to the anode)
  
  D pin8:RED LED (add 180Ω resistor to the anode)
  
  D pin9:RED LED (add 180Ω resistor to the anode)
  
  D pin10:BLUE LED (add 120Ω resistor to the anode)
  
  D pin10:BLUE LED (add 120Ω resistor to the anode)
  
  D pin10:BLUE LED (add 120Ω resistor to the anode)
  
Analog i/o pin 3,5,7 for ADXL335 Accelerometer Input, Analog i/o pin 4 for CarSpeed Pulse Input:

  A pin 3:ADXL335 X axis (Not used in this version)
  
  A pin 5:ADXL335 Y axis
  
  A pin 7:ADXL335 Z axis (Not used in this version)
  
  A pin 4:SpeedPulse from car ECU, connect to switching diode Cathode, and connect switching diode Anode to Car Speedpulse wire
  (I used 1N4148 Small signal switching diode for Backflow prevention)
  
3v3 PIN:connect to ADXL335 vdd

5v PIN: connect to 1kΩ resistor,and connect there to switching diode Cathode(for 5v PullUp)


