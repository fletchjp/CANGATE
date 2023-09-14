# CANBUFFER ver1a
  * Can be used in conjunction with CANGATE as an allternative way to produce a 'self consumed' event
  * Outputs of CANGATE can be fed into inputs
  * Setting Event variable 1 to 255 will invert the output event
  * Node Variable 1 is used to either use the same incoming event number  or to multiply the event number by 10
  * Setting NV1 to 1 - same event number in and our e.g. input 210 output 210
  * Setting NV1 to 10 multiplies the event number by 10 e.g. input 210 output 2100
  * Philip Silver MERG Member 4082

### canbufferd

This is the version of canbuffer using the ArduinoCBUS library, with similar changes to those in cangated.

This now has the ability to send more than one event from any input event.

The idea is to allow for events from CANBUFFER to be used by more than one CANGATE buffer.

This is implemented by a value in EV3 of between 2 and 5 (the maximum).

For EV3 = 2 this means that an event 1 input will generate events 10 and 11 if the multiplier is set to 10.

This new version uses DuncanGreenwood's ArduinoCBUS library instead of the MergCBUS library.

This provides the alternative way of making the FLiM/SLiM change for FCU using the Arduino serial monitor. This means the code could be run on other hardware.

The code for working with LEDs and a push button is also available. 

Details of how this works are in the README file for CANmINnOUT which can be found here: 

https://github.com/MartinDaCosta53/CANmINnOUT

## Testing

### cangated and canbufferd

I have tested cangated and canbufferd without a CBUS connection using the serial monitor to check what is happening.

My initial tests showed that the parameters used in CANGATE did not work with cangated as it is asking for more than the 1024 bytes of EEPROM available in a NANO or UNO.

I reduced the requirec EVs to 3 from 20 (as only 3 are used in CANGATE) and also reduced the number of events to 128. This reduces the EEPROM size to 946.

I have put lines in the output to the serial monitor to report the calculations.

I have also done quite a lot of testing using events from a CANPAN working with FCU 1.5.3

John Fletcher <M6777> August/September 2023

P.S. I have not adjusted the version number or the modile ID value.


