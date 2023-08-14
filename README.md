# CANGATE ver1b
 Two, Three or Four input AND 'gates'   Four input 0R 'gate' for CBUS Events.
  *  Also invert output event to cretae NAND and NOR
  *  No Arduino programming required - black box - similar to standard MERG CBUS kits
  *  Can use both LONG  and SHORT events as inputs to the gates  ACON / ACOF    ASON / ASOF
  *  Can use any Event on BUS - not Node Number or Event specific
  *  Input Events into 'gates' can be edited if wrongly added
  *  Copyright 2022 - Philip Silver MERG Member 4082

## Suggested modified versions

### cangate

 This version has functions to reduce the duplication in the code.
 
 Also there are new functions sendOnEvent and sendOffevent which reduce the number of direct calls to the cbus object.
 This means that the event sending code is separate from the library implementation.
 
### cangated

This new version uses DuncanGreenwood's ArduinoCBUS library instead of the MergCBUS library.

This provides the alternative way of making the FLiM/SLiM change for FCU using the Arduino serial monitor. This means the code could be run on other hardware.

John Fletcher <M6777> August 2023

P.S. I have not adjusted the version number.

