# biNixieClock
Arduino code for a clock that uses only two Nixie tubes.

To build a biNixie clock you need:
- an Arduino board (I used Nano)
- two Nixie tubes with their sockets
- two 74141 or K155DI or similar BCD-to-decimal converter and driver
- current limiting resistor on each tube's anode
- an RTC module (like DS1307)
- an HV PSU like the one sold on www.lumos.sk
- a 12 Vdc source
- two NO pushbuttons
- wires, perfboard
 
If you don't edit the code, wiring from the Arduino to the rest of the world:
- pins D9, D7, D6, D8 are BCD of big decade, LSB first (2 of number "23")
- pins D5, D16, D14, D4 are BCD of low nibble decade, MSB first (3 of number "23")
- pin D15 on hour setting button
- pin D18 on minute  setting button

Nixie anodes are not multiplexed.
There is no HV PSU poweroff, it runs all the time, but you could use pin D10 to control it (code not provided).
There is a quick depoison routine running at power-up and every 10 minutes past the full hour.
Serial debug output has been commented out (disabled).

Adjust timings to suit your needs. Right now:
- 70 milliseconds between each biDigit display (13 : 45 : 43)
- 1 second "on" time for each biDigit
- 3.3 seconds "off" time between cycles
- 500 milliseconds between each hh/mm increase when setting them
 
The biNixie Clock schematic diagram can be seen at http://ik1zyw.blogspot.it/2016/03/binixie-clock-schematic-diagram.html .

RULES
Enjoy.
Share back your code mods.
Be careful with the high-voltage (can be lethal, you have been warned).
Let your guests be hypnotized by the biNixie clock (and guess what it is showing).
