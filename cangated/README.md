# CANGATE ver1b

 This is the original README
 
 Two, Three or Four input AND 'gates'   Four input 0R 'gate' for CBUS Events.
  *  Also invert output event to cretae NAND and NOR
  *  No Arduino programming required - black box - similar to standard MERG CBUS kits
  *  Can use both LONG  and SHORT events as inputs to the gates  ACON / ACOF    ASON / ASOF
  *  Can use any Event on BUS - not Node Number or Event specific
  *  Input Events into 'gates' can be edited if wrongly added
  *  Copyright 2022 - Philip Silver MERG Member 4082

NOTE: I beleive that the CANGATE and CANBUFFER codes released by Phil risk running out of EEPROM as they allocate 20 EVs per event and use 3.

## Suggested modified versions

NOTE: There is conditional compilation in the codes to allow for different oscillator values and also the presence or absence of the push button and LEDs.

These should work the same as CANGATE although they have not yet been tested.

 
### cangated

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

All of the testing has been done using Arduino UNOs with prototypes of the Arduino shield (Kit 110)

John Fletcher <M6777> August/September 2023

P.S. I have not adjusted the version number or the modile ID value.


