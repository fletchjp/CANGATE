# CANGATE ver1b
 Two, Three or Four input AND 'gates'   Four input 0R 'gate' for CBUS Events.
  *  Also invert output event to cretae NAND and NOR
  *  No Arduino programming required - black box - similar to standard MERG CBUS kits
  *  Can use both LONG  and SHORT events as inputs to the gates  ACON / ACOF    ASON / ASOF
  *  Can use any Event on BUS - not Node Number or Event specific
  *  Input Events into 'gates' can be edited if wrongly added
  *  Copyright 2022 - Philip Silver MERG Member 4082

## Suggested modified versions

NOTE: There is conditional compilation in the codes to allow for different oscillator values and also the presence or absence of the push button and LEDs.

These should work the same as CANGATE although they have not yet been tested.

### cangate

 This version has functions to reduce the duplication in the code.
 
 Also there are new functions sendOnEvent and sendOffevent which reduce the number of direct calls to the cbus object.
 This means that the event sending code is separate from the library implementation.
 
### cangated

This new version uses DuncanGreenwood's ArduinoCBUS library instead of the MergCBUS library.

This provides the alternative way of making the FLiM/SLiM change for FCU using the Arduino serial monitor. This means the code could be run on other hardware.

The code for working with LEDs and a push button is also available. 

Details of how this works are in the README file for CANmINnOUT which can be found here: 

https://github.com/MartinDaCosta53/CANmINnOUT

### cangated2

This is an extended version of cangated which is experimentally storing output messages into the table of taught messages. 

This is done by having a framehandler routine which is set to look only at messages with on and off (short or long) opcodes. 

It then checks the node number to see if it is the same as the module node number. 

If so it checks to see if the node and event are already in the learned event table and if not it adds an entry.

The events for the module have to be read into FCU to update the information in FCU.

The new entry does not have event variables set. This can be done from FCU once there is an entry in the table.

### canbuffer

This has had the same treatment (sort of) as I did for cangate, though the changes are much less.

I think what I have from Phil is not the latest version as his code does not do the trick of changing the output event number. I have attempted to add that.

### canbufferd

This is the version of canbuffer using the ArduinoCBUS library, with similar changes to those in cangated.

## Testing

### cangated and canbufferd

I have tested cangated and canbufferd without a CBUS connection using the serial monitor to check what is happening.

My initial tests showed that the parameters used in CANGATE did not work with cangated as it is asking for more than the 1024 bytes of EEPROM available in a NANO or UNO.

I reduced the requirec EVs to 3 from 20 (as only 3 are used in CANGATE) and also reduced the number of events to 128. This reduces the EEPROM size to 946.

I have put lines in the output to the serial monitor to report the calculations.

### cangate and canbuffer

I have not tested these other than to confirm that they compile. I need to sort out hardware with push button and LEDs.

## Actions 

I have still to talk to Phil Silver to check what the actual requirements are.

I have still to do testing with the FCU.


John Fletcher <M6777> August 2023

P.S. I have not adjusted the version number or the modile ID value.


