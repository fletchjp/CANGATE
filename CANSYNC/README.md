# CANSYNC codes

These codes are from Duncan Greenwood

## Duncan's description from the Forum.

I spent a couple of hours on this earlier and have something working. I'm interested to know if people would find it useful. If so, I'll put the code on the wiki.

It's based on the Arduino CBUS library, so would work on any platform supported by the library. The code is simple enough that someone could port it to the PIC C library.

## How it works 

- you populate the device's event table with ('teach') the events representing producer module inputs that you'd like to periodically know the state of
- and/or, the device can populate its event table when it sees new events on the bus
- there are two EVs
- EV1 is for the 'event group'; events can be classified into groups to (i) reduce traffic, and/or (ii) resync groups at different frequencies
- there a 5 groups, numbered 0 to 4
- within a group, events can be ordered using EV2, if you'd find this useful, otherwise, by default, it's the order from the event table
- each group has a separate processing frequency (in minutes)
- discovered events by default go into group 0, but this is configurable
- groups are processed at startup, after a configurable delay
- then periodically afterwards, according to the resulting schedule
- NVs control frequency, message throttling, etc.
- message transmission priority could be configurable (current it's 'medium')
- the device has no inputs or outputs. You could add LEDs for status, etc
- older Arduino boards (Uno, Nano, ...) would be limited to 128 events, but you could have multiple devices on a layout, or use a bigger processor

Per the resulting schedule, for each event in each group, according to event priority (if set), the device produces an AREQ message. Clearly, the appropriate producer module will need to respond to this, and consumer modules will need to react to the producer's responses.

It's important to understand the 'meaning' of the AREQ message. It asks for the current state of a producer's input. i.e. per the CBUS Developers' Guide:

Indicates a ‘request’ event using the full event number of 4 bytes. (long event)
A request event is used to elicit a status response from a producer when it is required to know the ‘state’ of the producer without producing an ON or OFF event and to trigger an event from a ‘combi’ node.

As such, it's just a device for generating periodic, scheduled AREQ messages.

It is not a replacement for the current SoD mechanism, which does/should/might return the layout to a desired state at power up.
