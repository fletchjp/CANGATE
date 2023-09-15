// CANSYNC_0_2 modify to document EEPROM usage.
//
///
//
////////////////////////////////////////////////////////////////////////////////////
#define USE_CS_15
// NOTE: The CS pin for CAN has to be changed from 10 to 15 as 10 is used by the lcd.

// IMPORTANT: The external MCP2515 boards use 8Mhz
//            The CBUS shield uses 16Mhz
// Define this for the external board. This code must be compiled separately for each option.
// This applies to Phil's board
//#define USE_EXTERNAL_MCP2515
////////////////////////////////////////////////////////////////////////////////////

/*
  Copyright (C) Duncan Greenwood 2023 (duncan_greenwood@hotmail.com)

  This work is licensed under the:
      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   To view a copy of this license, visit:
      http://creativecommons.org/licenses/by-nc-sa/4.0/
   or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

   License summary:
    You are free to:
      Share, copy and redistribute the material in any medium or format
      Adapt, remix, transform, and build upon the material

    The licensor cannot revoke these freedoms as long as you follow the license terms.

    Attribution : You must give appropriate credit, provide a link to the license,
                  and indicate if changes were made. You may do so in any reasonable manner,
                  but not in any way that suggests the licensor endorses you or your use.

    NonCommercial : You may not use the material for commercial purposes. **(see note below)

    ShareAlike : If you remix, transform, or build upon the material, you must distribute
                 your contributions under the same license as the original.

    No additional restrictions : You may not apply legal terms or technological measures that
                                 legally restrict others from doing anything the license permits.

   ** For commercial use, please contact the original copyright holder(s) to agree licensing terms

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE

*/

/*
      3rd party libraries needed for compilation: (not for binary-only distributions)

      Streaming   -- C++ stream style output, v5, (http://arduiniana.org/libraries/streaming/)
      ACAN2515    -- library wrapper for MCP2515 CAN controller
*/

//
/// NV map
//
// 1:  startup delay, in seconds, before first pass, in seconds. default = 10
// 2:  delay between event groups, initial startup, in seconds. default = 2
// 3:  delay between event group, subsequent, in seconds, default = 10
// 4:  delay between request messages, in milliseconds. default = 20
// 5:  whether to insert new events into event table when seen, 0 = no, 1 = yes. default = 1
// 6:  default group 0 frequency, in minutes. default = 5
// 7:  group 1 frequency, in minutes. default = 6
// 8:  group 2 frequency, in minutes. default = 7
// 9:  group 3 frequency, in minutes. default = 8
// 10: group 4 frequency, in minutes. default = 9
// 11: default event group for new events. default = 0
//

/// EV map
//
// 0:  group (default 0)
// 1:  priority within the group (default 0)
//


#define DEBUG 1       // set to 0 for no serial debug

#if DEBUG
#define DEBUG_PRINT(S) Serial << S << endl
#else
#define DEBUG_PRINT(S)
#endif

// 3rd party libraries
#include <Streaming.h>
#include <EEPROM.h>   //Required to check EEPROM size

// AVR libC
#include <stdlib.h>

// CBUS library header files
#include <CBUS2515.h>               // CAN controller and CBUS class
#include <CBUSswitch.h>             // pushbutton switch
#include <CBUSLED.h>                // CBUS LEDs
#include <CBUSconfig.h>             // module configuration
#include <CBUSParams.h>             // CBUS parameters
#include <cbusdefs.h>               // MERG CBUS constants

// constants
const byte VER_MAJ = 1;             // code major version
const char VER_MIN = 'a';           // code minor version
const byte VER_BETA = 0;            // code beta sub-version
const byte MODULE_ID = 111;         // CBUS module type

#ifdef USE_EXTERNAL_MCP2515
const unsigned long CAN_OSC_FREQ = 8000000;     // Oscillator frequency on the CAN2515 board
#else
const unsigned long CAN_OSC_FREQ = 16000000;     // Oscillator frequency on the CAN2515 shield
#endif

//////////////////////////////////////////////////////////////////////////

//CBUS pins
const byte CAN_INT_PIN = 2;  // Only pin 2 and 3 support interrupts
#ifdef USE_CS_15
const byte CAN_CS_PIN = 15;  // For hardware with display only - normally 10
#else
const byte CAN_CS_PIN = 10;  // Normally 10
#endif

const byte LED_GRN = 8;             // CBUS green SLiM LED pin
const byte LED_YLW = 9;             // CBUS yellow FLiM LED pin
const byte SWITCH0 = 10;            // CBUS push button switch pin

const byte NUM_GROUPS = 5;          // number of event groups
const byte MAX_EVENTS = 128;        // max size of event table & worklist

// CBUS objects
CBUSConfig module_config;           // configuration object
CBUS2515 CBUS(&module_config);      // CBUS object
CBUSLED ledGrn, ledYlw;             // two LED objects
CBUSSwitch pb_switch;               // switch object

// module name, must be 7 characters, space padded.
unsigned char module_name[7] = { 'S', 'Y', 'N', 'C', ' ', ' ', ' ' };

// forward function declarations
void framehandler(CANFrame *msg);
int sort_comparison_func(const void* p1, const void* p2);

// globals vars
typedef struct {
  byte index, priority;
} workitem;

workitem worklist[MAX_EVENTS];

//
/// setup CBUS - runs once at power on from setup()
//

void setupCBUS() {

  // set config layout parameters
  module_config.EE_NVS_START = 10;
  module_config.EE_NUM_NVS = 15;
  module_config.EE_EVENTS_START = 25;
  module_config.EE_MAX_EVENTS = MAX_EVENTS;
  module_config.EE_NUM_EVS = 2;
  module_config.EE_BYTES_PER_EVENT = (module_config.EE_NUM_EVS + 4);

  // initialise and load configuration
  module_config.setEEPROMtype(EEPROM_INTERNAL);
  module_config.begin();
  unsigned int EEPROM_needed = module_config.EE_EVENTS_START + 
              (module_config.EE_BYTES_PER_EVENT * module_config.EE_MAX_EVENTS);
  Serial << F("> EEPROM required is ") << EEPROM_needed << endl;
  if (EEPROM_needed < EEPROM.length()) {
    Serial << F("> There is enough EEPROM for this case") << endl;
  } else {
    Serial << F("**** WARNING There is NOT enough EEPROM for this case ****") << endl;
  }

  Serial << F("> mode = ") << ((module_config.FLiM) ? "FLiM" : "SLiM") << F(", CANID = ") << module_config.CANID;
  Serial << F(", NN = ") << module_config.nodeNum << endl;

  // show code version and copyright notice
  printConfig();

  // set module parameters
  CBUSParams params(module_config);
  params.setVersion(VER_MAJ, VER_MIN, VER_BETA);
  params.setModuleId(MODULE_ID);
  params.setFlags(PF_FLiM | PF_COMBI);

  // assign to CBUS
  CBUS.setParams(params.getParams());
  CBUS.setName(module_name);

  // set CBUS LED pins and assign to CBUS
  ledGrn.setPin(LED_GRN);
  ledYlw.setPin(LED_YLW);
  CBUS.setLEDs(ledGrn, ledYlw);

  // initialise CBUS switch and assign to CBUS
  pb_switch.setPin(SWITCH0, LOW);
  pb_switch.run();
  CBUS.setSwitch(pb_switch);

  // module reset - if switch is depressed at startup and module is in SLiM mode
  if (pb_switch.isPressed() && !module_config.FLiM) {
    Serial << F("> switch was pressed at startup in SLiM mode") << endl;
    module_config.resetModule(ledGrn, ledYlw, pb_switch);
  }

  // opportunity to set default NVs after module reset
  if (module_config.isResetFlagSet()) {
    Serial << F("> module has been reset") << endl;

    module_config.writeNV(1, 10);     // startup delay, seconds
    module_config.writeNV(2, 2);      // delay between groups, initial, seconds
    module_config.writeNV(3, 10);     // delay between groups, subsequent, seconds
    module_config.writeNV(4, 20);     // delay between request messages, milliseconds
    module_config.writeNV(5, 1);      // automatically add newly discovered events ?
    module_config.writeNV(6, 5);      // sync freq, mins, group 0
    module_config.writeNV(7, 6);
    module_config.writeNV(8, 7);
    module_config.writeNV(9, 8);
    module_config.writeNV(10, 9);
    module_config.writeNV(11, 0);     // default group for newly discovered events

    module_config.clearResetFlag();
  }

  // register our CBUS frame handler
  CBUS.setFrameHandler(framehandler);

  // set CBUS LEDs to indicate mode
  CBUS.indicateMode(module_config.FLiM);

  // configure and start CAN bus and CBUS message processing
  CBUS.setNumBuffers(32, 4);     // more buffers = more memory used, fewer = less
  CBUS.setOscFreq(CAN_OSC_FREQ);   // select the crystal frequency of the CAN module
  CBUS.setPins(CAN_CS_PIN, CAN_INT_PIN);           // select pins for CAN bus CE and interrupt connections

  if (!CBUS.begin()) {
    Serial << F("> CAN init fail") << endl;
  } else {
    Serial << F("> CAN init ok") << endl;
  }

}

//
/// setup - runs once at power on
//

void setup() {

  Serial.begin (115200);
  while (!Serial);
  Serial << endl << endl << F("> ** CANSYNC layout state synchroniser 0 1 ** ") << __FILE__ << endl;

  // configure and start CBUS
  setupCBUS();

  // end of setup
  Serial << F("> ready") << endl << endl;
}

//
/// loop - runs forever
//

void loop() {

  static bool first_sync_done = false;
  static byte next_group_to_process = 0;
  static unsigned long last_process_time = 0, last_event_group_process_time[NUM_GROUPS] = {0, 0, 0, 0, 0};

  //
  /// initial startup processing - process each group once
  //

  if ((!first_sync_done && millis() >= (module_config.readNV(1) * 1000)) && (millis() - last_process_time >= (module_config.readNV(2) * 1000))) {
    process_event_group(next_group_to_process);                                 // process this group
    last_process_time = millis();                                               // time of last run
    last_event_group_process_time[next_group_to_process] = last_process_time;   // time of last run, for this group
    next_group_to_process = (next_group_to_process + 1) % NUM_GROUPS;           // increment next event group to process

    if (next_group_to_process == 0) {                                           // we have wrapped back to zero, so all initial event groups have been processed
      first_sync_done = true;
    }
  }

  //
  /// subsequent repeating event group processing - will start at group zero, once initial startup processing has completed
  //

  if (first_sync_done) {
    if (millis() - last_process_time >= (module_config.readNV(3) * 1000) && millis() - last_event_group_process_time[next_group_to_process] >= (module_config.readNV(next_group_to_process + 6) * 60000)) {
      process_event_group(next_group_to_process);
      last_process_time = millis();
      last_event_group_process_time[next_group_to_process] = last_process_time;
      next_group_to_process = (next_group_to_process + 1) % NUM_GROUPS;
    }
  }

  //
  /// do CBUS message, switch and LED processing
  //

  CBUS.process();

  //
  /// process console commands
  //

  processSerialInput();

  // bottom of loop()
}

//
/// process an event group
//

void process_event_group(const byte event_group_num) {

  byte tarray[4], num_events_found = 0, num_request_msgs_sent = 0, i = 0;
  CANFrame msg;

  Serial << F("> processing event group = ") << event_group_num << endl;

  // build a worklist of events to process

  for (i = 0; i < module_config.EE_MAX_EVENTS; i++) {                               // iterate over the event table
    if (module_config.evhashtbl[i] > 0) {                                           // if table slot is in use
      if (module_config.getEventEVval(i, 1) == event_group_num) {                   // if EV1 value matches this event group
        worklist[num_events_found].index = i;                                       // add it to the worklist
        worklist[num_events_found].priority = module_config.getEventEVval(i, 2);    // with priority value from EV2
        ++num_events_found;
      }
    }
  }

  // sort the worklist by event priority

  qsort(worklist, num_events_found, sizeof(workitem), sort_comparison_func);

  // send a request message for each event in the worklist

  msg.len = 5;
  msg.data[0] = OPC_AREQ;                                                           // 0x92

  for (i = 0; i < num_events_found; i++) {

    module_config.readEvent(worklist[i].index, tarray);                             // read in the event NN and EN from the sorted worklist table index

    unsigned int nn = (tarray[0] << 8) + tarray[1];                                 // debug
    unsigned int en = (tarray[2] << 8) + tarray[3];
    Serial << F("> sending item index = ") << worklist[i].index << F(", nn = ") << nn << F(", en = ") << en << F(", priority = ") << worklist[i].priority << endl;

    memcpy(&(msg.data[1]), tarray, 4);

    CBUS.sendMessage(&msg);                                                         // send the event request message
    delay(module_config.readNV(4));                                                 // inter-message throttling delay
    ++num_request_msgs_sent;
  }

  Serial << F("> events found = ") << num_events_found << F(", sent = ") << num_request_msgs_sent << endl;
}

//
/// comparison function for qsort
//

int sort_comparison_func(const void *p1, const void *p2) {

  const workitem *x = (const workitem *)p1;
  const workitem *y = (const workitem *)p2;

  if (x->priority == y->priority) {
    return 0;
  } else if (x->priority < y->priority) {
    return -1;
  } else {
    return 1;
  }
}

//
/// frame handler - called for every CBUS message received
//

void framehandler(CANFrame *msg) {

  byte index = 0;

  // insert new events into event table
  if (is_event(msg)) {                                                                          // only interested in event opcodes
    if (module_config.readNV(5)) {                                                              // insert discovered events ?
      uint16_t nn = is_short_event(msg) ? 0 : (msg->data[1] << 8) + msg->data[2];
      uint16_t en = (msg->data[3] << 8) + msg->data[4];

      if (module_config.findExistingEvent(nn, en) == module_config.EE_MAX_EVENTS) {             // not already in the event table
        if ((index = module_config.findEventSpace()) < module_config.EE_MAX_EVENTS) {           // find free event table slot
          module_config.writeEvent(index, (byte *)(msg->data + 1));                             // write the event at this slot
          module_config.writeEventEV(index, 1, module_config.readNV(11));                       // default group for new events
          module_config.writeEventEV(index, 2, 0);                                              // default priority - use FCU to change
          module_config.updateEvHashEntry(index);
        }
      }
    }
  }
}

//
/// determine whether message opcode is an accessory event
//

bool is_event(const CANFrame *msg) {

  switch (msg->data[0]) {

    case OPC_ACON:
    case OPC_ACON1:
    case OPC_ACON2:
    case OPC_ACON3:

    case OPC_ACOF:
    case OPC_ACOF1:
    case OPC_ACOF2:
    case OPC_ACOF3:

    case OPC_ASON:
    case OPC_ASON1:
    case OPC_ASON2:
    case OPC_ASON3:

    case OPC_ASOF:
    case OPC_ASOF1:
    case OPC_ASOF2:
    case OPC_ASOF3:

      return true;
      break;

    default:
      return false;
  }
}

//
/// determine whether message opcode is a short accessory event
//

inline bool is_short_event(const CANFrame *msg) {

  return (msg->data[0] & (1 << 3));
}

//
/// print code version config details and copyright notice
//

void printConfig(void) {

  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << F("> © Duncan Greenwood (MERG M5767) 2023") << endl;
}

//
/// command interpreter for serial console input
//

void processSerialInput(void) {

  byte uev = 0;
  char msgstr[32], dstr[32];

  if (Serial.available()) {

    char c = Serial.read();

    switch (c) {

      case 'n':

        // node config
        printConfig();

        // node identity
        Serial << F("> CBUS node configuration") << endl;
        Serial << F("> mode = ") << (module_config.FLiM ? "FLiM" : "SLiM") << F(", CANID = ") << module_config.CANID << F(", node number = ") << module_config.nodeNum << endl;
        Serial << endl;
        break;

      case 'e':

        // EEPROM learned event data table
        Serial << F("> stored events ") << endl;
        Serial << F("  max events = ") << module_config.EE_MAX_EVENTS << F(" EVs per event = ") << module_config.EE_NUM_EVS << F(" bytes per event = ") << module_config.EE_BYTES_PER_EVENT << endl;

        for (byte j = 0; j < module_config.EE_MAX_EVENTS; j++) {
          if (module_config.getEvTableEntry(j) != 0) {
            ++uev;
          }
        }

        Serial << F("  stored events = ") << uev << F(", free = ") << (module_config.EE_MAX_EVENTS - uev) << endl;
        Serial << F("  using ") << (uev * module_config.EE_BYTES_PER_EVENT) << F(" of ") << (module_config.EE_MAX_EVENTS * module_config.EE_BYTES_PER_EVENT) << F(" bytes") << endl << endl;

        Serial << F("  Ev#  |  NNhi |  NNlo |  ENhi |  ENlo | ");

        for (byte j = 0; j < (module_config.EE_NUM_EVS); j++) {
          sprintf(dstr, "EV%03d | ", j + 1);
          Serial << dstr;
        }

        Serial << F("Hash |") << endl;

        Serial << F(" --------------------------------------------------------------") << endl;

        // for each event data line
        for (byte j = 0; j < module_config.EE_MAX_EVENTS; j++) {

          if (module_config.getEvTableEntry(j) != 0) {
            sprintf(dstr, "  %03d  | ", j);
            Serial << dstr;

            // for each data byte of this event
            for (byte e = 0; e < (module_config.EE_NUM_EVS + 4); e++) {
              sprintf(dstr, " 0x%02hx | ", module_config.readEEPROM(module_config.EE_EVENTS_START + (j * module_config.EE_BYTES_PER_EVENT) + e));
              Serial << dstr;
            }

            sprintf(dstr, "%4d |", module_config.getEvTableEntry(j));
            Serial << dstr << endl;
          }
        }

        Serial << endl;

        break;

      // NVs
      case 'v':

        // note NVs number from 1, not 0
        Serial << "> Node variables" << endl;
        Serial << F("   NV   Val") << endl;
        Serial << F("  --------------------") << endl;

        for (byte j = 1; j <= module_config.EE_NUM_NVS; j++) {
          byte v = module_config.readNV(j);
          sprintf(msgstr, " - %02d : %3hd | 0x%02hx", j, v, v);
          Serial << msgstr << endl;
        }

        Serial << endl << endl;

        break;

      // CAN bus status
      case 'c':

        CBUS.printStatus();
        break;

      case 'h':
        // event hash table
        module_config.printEvHashTable(false);
        break;

      case 'y':
        // reset CAN bus and CBUS message processing
        CBUS.reset();
        break;

      case '*':
        // reboot
        module_config.reboot();
        break;

      case 'm':
        // free memory
        Serial << F("> free SRAM = ") << module_config.freeSRAM() << F(" bytes") << endl;
        break;

      case '\r':
      case '\n':
        Serial << endl;
        break;

      default:
        // Serial << F("> unknown command ") << c << endl;
        break;
    }
  }
}
