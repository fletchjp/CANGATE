
/********************************************************************************************/
 /* CANBUFFER version using Duncan Greenwood's CBUS library
  * I have done this in such a way as to minimize changes to the event sending code.
  *******************************************************
  * Can be used in conjunction with CANGATE as an allternative way to produce a 'self consumed' event
  * Outputs of CANGATE can be fed into inputs
  * Setting Event variable 1 to 255 will invert the output event
  * Node Variable 1 is used to either use the same incoming event number  or to multiply the event number by 10
  * Setting NV1 to 1 - same event number in and our e.g. input 210 output 210
  * Setting NV1 to 10 multiplies the event number by 10 e.g. input 210 output 2100
  * Philip Silver MERG Member 4082
  */
/********************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////
// #define USE_CS_15 Not needed here
// NOTE: The CS pin for CAN has to be changed from 10 to 15 as 10 is used by the lcd.

// IMPORTANT: The external MCP2515 boards use 8Mhz
//            The CBUS shield uses 16Mhz
// Define this for the external board. This code must be compiled separately for each option.
// This applies to Phil's board
//#define USE_EXTERNAL_MCP2515
////////////////////////////////////////////////////////////////////////////////////

#define DEBUG 1       // set to 0 for no serial debug

#if DEBUG
#define DEBUG_PRINT(S) Serial << S << endl
#else
#define DEBUG_PRINT(S)
#endif

/*
All Libaries used can be downloaded from my Github account
https://github.com/philsilvers


Pins used for interface chip

           Nano  
INT         D2       
SCK   SCK   D13      
SI    MISO  D11      
SO    MOSI  D12      
CS          D10      
GND         0V       
VCC         5V      

*/
/********************************************************************************************/


/********************************************************************************************/
// Load CBUS Libraries
/********************************************************************************************/

//#include <SPI.h> //equired by the CBUS library to communicate to MCP2515 CAN Controller
//#include <MergCBUS.h> // Main CBUS Library
//#include <Message.h>  // CBUS Message Libary
//#include <EEPROM.h> //Required by the CBUS library to read / write Node Identifiction and Node Varaiables

// 3rd party libraries
#include <Streaming.h>
#include <EEPROM.h>   //Required to check EEPROM size

// CBUS library header files
#include <CBUS2515.h>            // CAN controller and CBUS class
//#include "LEDControl.h"          // CBUS LEDs
#include <CBUSconfig.h>          // module configuration
#include <cbusdefs.h>            // MERG CBUS constants
#include <CBUSParams.h>

/********************************************************************************************/

// module name
unsigned char mname[7] = { 'B', 'U', 'F', 'F', 'E', 'R', 'D' };

// constants
const byte VER_MAJ = 2;         // code major version
const char VER_MIN = ' ';       // code minor version
const byte VER_BETA = 0;        // code beta sub-version
const byte MODULE_ID = 75;      // CBUS module type 75 for CANGATE

#ifdef USE_EXTERNAL_MCP2515
const unsigned long CAN_OSC_FREQ = 8000000;     // Oscillator frequency on the CAN2515 board
#else
const unsigned long CAN_OSC_FREQ = 16000000;     // Oscillator frequency on the CAN2515 shield
#endif

//////////////////////////////////////////////////////////////////////////

//CBUS pins
const byte CAN_INT_PIN = 2;  // Only pin 2 and 3 support interrupts
const byte CAN_CS_PIN = 10;
//const byte CAN_SI_PIN = 11;  // Cannot be changed
//const byte CAN_SO_PIN = 12;  // Cannot be changed
//const byte CAN_SCK_PIN = 13;  // Cannot be changed

// CBUS objects
CBUS2515 CBUS;                      // CBUS object
CBUSConfig config;                  // configuration object

CBUSLED ledGrn, ledYlw;             // LED objects
CBUSSwitch pb_switch;               // switch object



/********************************************************************************************/
//CBUS definitions
/********************************************************************************************/
  #define GREEN_LED 4               //MERG Green (SLIM) LED port
  #define YELLOW_LED 7              //MERG Yellow (FLIM) LED port
  #define PUSH_BUTTON 8             //std merg FLIM / SLIM push button
 //#define HAS_BUTTON 1            // Define this if there is a button
 //#define PUSH_BUTTON1 3          //debug push button
  #define NODE_VARS 1      //sets up number of NVs for module to store variables

  #define NODE_EVENTS 128     //Max Number of supported Events is 255
  #define EVENTS_VARS 3  //number of variables per event Maximum is 20
  #define DEVICE_NUMBERS 0  //number of devices numbers connected to Arduino such as servos, relays etc. Can be used for Short events

/********************************************************************************************/





/********************************************************************************************/
//Variables made into arrays, one per event.
//Use EV2 to choose which one to use.
/********************************************************************************************/
bool nonInverting[NODE_EVENTS];

bool inverting[NODE_EVENTS];


/********************************************************************************************/

//
///  setup CBUS - runs once at power on called from setup()
//
void setupCBUS()
{
  // set config layout parameters
  config.EE_NVS_START = 10;
  config.EE_NUM_NVS = NODE_VARS;
  config.EE_EVENTS_START = 50;
  config.EE_MAX_EVENTS = NODE_EVENTS;
  config.EE_NUM_EVS = EVENTS_VARS;
  config.EE_BYTES_PER_EVENT = (config.EE_NUM_EVS + 4);

  // initialise and load configuration
  config.setEEPROMtype(EEPROM_INTERNAL);
  config.begin();
  unsigned int EEPROM_needed = config.EE_EVENTS_START + 
              (config.EE_BYTES_PER_EVENT * config.EE_MAX_EVENTS);
  Serial << F("> EEPROM required is ") << EEPROM_needed << endl;
  if (EEPROM_needed < EEPROM.length()) {
    Serial << F("> There is enough EEPROM for this case") << endl;
  } else {
    Serial << F("**** WARNING There is NOT enough EEPROM for this case ****") << endl;
  }

  Serial << F("> mode = ") << ((config.FLiM) ? "FLiM" : "SLiM") << F(", CANID = ") << config.CANID;
  Serial << F(", NN = ") << config.nodeNum << endl;

  // show code version and copyright notice
  printConfig();

  // set module parameters
  CBUSParams params(config);
  params.setVersion(VER_MAJ, VER_MIN, VER_BETA);
  params.setModuleId(MODULE_ID);
  params.setFlags(PF_FLiM | PF_COMBI);

  // assign to CBUS
  CBUS.setParams(params.getParams());
  CBUS.setName(mname);

  // register our CBUS event handler, to receive event messages of learned events
  CBUS.setEventHandler(eventhandler);

  // set LED and switch pins and assign to CBUS
  ledGrn.setPin(GREEN_LED);
  ledYlw.setPin(YELLOW_LED);
  CBUS.setLEDs(ledGrn, ledYlw);
  CBUS.setSwitch(pb_switch);

  // set CBUS LEDs to indicate mode
  CBUS.indicateMode(config.FLiM);

  // configure and start CAN bus and CBUS message processing
  CBUS.setNumBuffers(2);         // more buffers = more memory used, fewer = less
  CBUS.setOscFreq(CAN_OSC_FREQ);   // select the crystal frequency of the CAN module
  CBUS.setPins(CAN_CS_PIN, CAN_INT_PIN);           // select pins for CAN bus CE and interrupt connections
  CBUS.begin();
}


/********************************************************************************************/
//Create the MERG CBUS object - cbus
/********************************************************************************************/

//MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

/********************************************************************************************/

//MergNodeIdentification MergNode=MergNodeIdentification();

void setup () {

  Serial.begin (115200);
  Serial << endl << endl << F("> ** CANBUFFERD ** ") << __FILE__ << endl;
  Serial << F("> This version uses the ArduinoCBUS libraries") << endl;
  Serial << F("> EEPROM available is ") << EEPROM.length() << endl;

  setupCBUS();

  pb_switch.setPin(PUSH_BUTTON, LOW);
  Serial << F("> Switch set to go LOW when pressed") << endl;

#ifdef HAS_BUTTON
  if (pb_switch.isPressed() && !config.FLiM) {
//#if DEBUG
    Serial << F("> switch was pressed at startup in SLiM mode") << endl;
//#endif
    config.resetModule(ledGrn, ledYlw, pb_switch);
  }
#endif


/********************************************************************************************/
//Configuration CBUS data for the node
/********************************************************************************************/
  
/*
  cbus.getNodeId()->setNodeName("CANBUFF",7);       //node name shows in FCU when first detected set your own name for each module - max 8 characters
  cbus.getNodeId()->setModuleId(129);               //module number - set above 100 to avoid conflict with other MERG CBUS modules
  cbus.getNodeId()->setManufacturerId(0xA5);        //MERG code
  cbus.getNodeId()->setMinCodeVersion(0x61);        //Version a
  cbus.getNodeId()->setMaxCodeVersion(1);           // version 1
  cbus.getNodeId()->setProducerNode(true);          //Produce Node - Set True if Producer Module
  cbus.getNodeId()->setConsumerNode(true);          //Consumer Node = Set True if Consumer Module
  cbus.getNodeId()->setConsumeEvents(false);         // consume own event COE
  cbus.getNodeId()->setTransportType(1);            // 1 CAN  2 ETHERNET 3 MiWi
  cbus.getNodeId()->setBetaRelease(0);              // Beta Release set to zero for release version
  cbus.getNodeId()->setSuportBootLoading(false);    // Allways set false - no bootloading
  cbus.getNodeId()->setCpuManufacturer(2);          //1 MICROCHIP  2  ATMEL  3 ARM


  cbus.setStdNN(999);                               // Node Number in SLIM Mode. The default is 0 for Consumers or 1 - 99 for Producers.
*/

  for (unsigned int i = 0; i < NODE_EVENTS; i++) {
     inverting[i] = true;
     nonInverting[i] = true;
  }


/********************************************************************************************/
// Set ports and CAN Transport Layer
/********************************************************************************************/
/*
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic when recieving events
  cbus.initCanBus(10,CAN_125KBPS,MCP_16MHz,10,200);  //initiate the transport layer. pin=10, rate=125Kbps,10 tries,200 millis between each try
*/  
  //Note the clock speed 8Mhz. If 16Mhz crystal fitted change above to MCP_16Mhz
/********************************************************************************************/

  // end of setup
  DEBUG_PRINT(F("> ready"));

} // End Of Set Up


void printConfig(void)
{
  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << F("> © Phil Silver (MERG M4082) 2022") << endl;
  Serial << F("> © Martin Da Costa (MERG M6223) 2021") << endl;
  Serial << F("> © Duncan Greenwood (MERG M5767) 2021") << endl;
  Serial << F("> © John Fletcher (MERG M6777) 2023") << endl;
  Serial << F("> © Sven Rosvall (MERG M3777) 2021") << endl;
}



/********************************************************************************************/
//  Functions
/********************************************************************************************/

// Send an event routine according to Module Switch
// Renamed to avoid name clash
bool sendAnEvent(byte opCode, unsigned int eventNo)
{
  CANFrame msg;
  msg.id = config.CANID;
  msg.len = 5;
  msg.data[0] = opCode;
  msg.data[1] = highByte(config.nodeNum);
  msg.data[2] = lowByte(config.nodeNum);
  msg.data[3] = highByte(eventNo);
  msg.data[4] = lowByte(eventNo);

  bool success = CBUS.sendMessage(&msg);
  if (success) {
    DEBUG_PRINT(F("> sent CBUS message with Event Number ") << eventNo);
  } else {
    DEBUG_PRINT(F("> error sending CBUS message"));
  }
  return success;
}

bool sendOnEvent(bool longEvent, unsigned int eventNo)
{
   if(longEvent) return sendAnEvent(OPC_ACON,eventNo);
   else return sendAnEvent(OPC_ASON,eventNo);
}

bool sendOffEvent(bool longEvent, unsigned int eventNo)
{
   if(longEvent) return sendAnEvent(OPC_ACOF,eventNo);
   else return sendAnEvent(OPC_ASOF,eventNo);
}


//
/// called from the CBUS library when a learned event is received
//
void eventhandler(byte index, CANFrame *msg)
{
  byte CBUSOpc = msg->data[0];

  DEBUG_PRINT(F("> event handler: index = ") << index << F(", opcode = 0x") << _HEX(msg->data[0]));
  DEBUG_PRINT(F("> event handler: length = ") << msg->len);

  unsigned int nodeNumber = (msg->data[1] << 8 ) + msg->data[2];
  unsigned int eventNumber = (msg->data[3] << 8 ) + msg->data[4];
  DEBUG_PRINT(F("> NN = ") << nodeNumber << F(", EN = ") << eventNumber);
  DEBUG_PRINT(F("> op_code = ") << CBUSOpc);

//void myUserFunc(Message *msg,MergCBUS *mcbus){
  
  bool isAccOn  = ((CBUSOpc == OPC_ACON) || (CBUSOpc == OPC_ASON) );
  bool isAccOff = ((CBUSOpc == OPC_ACOF) || (CBUSOpc == OPC_ASOF) );

  // byte CBUSOpc = msg->getOpc(); // Get The OPCODE from Message
   //int nodeNumber = msg->getNodeNumber(); // Get The Node Number from Message
   //int eventNumber = msg->getEventNumber(); // Get The Event Number from Message
    int eventVariable1 = config.getEventEVval(index, 1); //mcbus->getEventVar(msg,1);
    int eventVariable2 = config.getEventEVval(index, 2); //mcbus->getEventVar(msg,2);
    unsigned int eventVariable3 = config.getEventEVval(index, 3); //mcbus->getEventVar(msg,3);
  
    byte nodeVariable1 = config.readNV(1); 
    // New code for eventfactor using NV(1). Experimental
    unsigned int eventFactor = 1;
    if (nodeVariable1 > 1 && nodeVariable1 < 11) eventFactor = nodeVariable1;
    unsigned int eventNumberOut0 = eventNumber*eventFactor;
    // New code to send multiple copies of an event depending on eventVariable3
    // Default is to send one event only 
       if (eventVariable3 == 0 || eventVariable3 > 4)
          eventVariable3 = 1;
       unsigned int i;
       for (i = 0; i < eventVariable3; i++) {
           unsigned int eventNumberOut =  eventNumberOut0 + i;
           bool last = (i+1 == eventVariable3);
           DEBUG_PRINT(F("> Sending event number ") << eventNumberOut );     
   //if (mcbus->eventMatch()){  //The recived event has been taught this module

                    if (nonInverting[eventVariable2]){
                      if (isAccOn){
                        sendOnEvent(true, eventNumberOut);
                        if (last) nonInverting[eventVariable2]=false;
                      }
                    }
                    if (!nonInverting[eventVariable2]){ 
                      if (isAccOff) {
                         sendOffEvent(true, eventNumberOut);
                         if (last) nonInverting[eventVariable2]=true;
                       }
                    }

              if (eventVariable1 == 255){
                    inverting[eventVariable2]=true;
                    if (isAccOn){
                        sendOffEvent(true, eventNumberOut);
                        if (last) inverting[eventVariable2]=false;
                        }
                    if (!inverting[eventVariable2]){
                       if (isAccOff) {
                        sendOnEvent(true, eventNumber);
                        if (last) inverting[eventVariable2]=true;
                       }
                    }
              }
                  
       }
        //} // End OF Recieved Events
          
    
           
  } // end function
                


/*FUNCTIONS FINISH ***********************************************************************************/



void loop() {

    //cbus.run();// Run CBUS
    //cbus.cbusRead(); // Check CBUS Buffers for any activity
  // do CBUS message, switch and LED processing
  CBUS.process();

  // process console commands
  processSerialInput();

}


void processSerialInput(void)
{
  byte uev = 0;
  char msgstr[32];

  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {

      case 'n':
        // node config
        printConfig();

        // node identity
        Serial << F("> CBUS node configuration") << endl;
        Serial << F("> mode = ") << (config.FLiM ? "FLiM" : "SLiM") << F(", CANID = ") << config.CANID << F(", node number = ") << config.nodeNum << endl;
        Serial << endl;
        break;

      case 'e':
        // EEPROM learned event data table
        Serial << F("> stored events ") << endl;
        Serial << F("  max events = ") << config.EE_MAX_EVENTS << F(" EVs per event = ") << config.EE_NUM_EVS << F(" bytes per event = ") << config.EE_BYTES_PER_EVENT << endl;

        for (byte j = 0; j < config.EE_MAX_EVENTS; j++) {
          if (config.getEvTableEntry(j) != 0) {
            ++uev;
          }
        }

        Serial << F("  stored events = ") << uev << F(", free = ") << (config.EE_MAX_EVENTS - uev) << endl;
        Serial << F("  using ") << (uev * config.EE_BYTES_PER_EVENT) << F(" of ") << (config.EE_MAX_EVENTS * config.EE_BYTES_PER_EVENT) << F(" bytes") << endl << endl;

        Serial << F("  Ev#  |  NNhi |  NNlo |  ENhi |  ENlo | ");

        for (byte j = 0; j < (config.EE_NUM_EVS); j++) {
          sprintf(msgstr, "EV%03d | ", j + 1);
          Serial << msgstr;
        }

        Serial << F("Hash |") << endl;

        Serial << F(" --------------------------------------------------------------") << endl;

        // for each event data line
        for (byte j = 0; j < config.EE_MAX_EVENTS; j++) {
          if (config.getEvTableEntry(j) != 0) {
            sprintf(msgstr, "  %03d  | ", j);
            Serial << msgstr;

            // for each data byte of this event
            for (byte e = 0; e < (config.EE_NUM_EVS + 4); e++) {
              sprintf(msgstr, " 0x%02hx | ", config.readEEPROM(config.EE_EVENTS_START + (j * config.EE_BYTES_PER_EVENT) + e));
              Serial << msgstr;
            }

            sprintf(msgstr, "%4d |", config.getEvTableEntry(j));
            Serial << msgstr << endl;
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

        for (byte j = 1; j <= config.EE_NUM_NVS; j++) {
          byte v = config.readNV(j);
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
        config.printEvHashTable(false);
        break;

      case 'y':
        // reset CAN bus and CBUS message processing
        CBUS.reset();
        break;

      case '*':
        // reboot
        config.reboot();
        break;

      case 'm':
        // free memory
        Serial << F("> free SRAM = ") << config.freeSRAM() << F(" bytes") << endl;
        break;

      case 'r':
        // renegotiate
        CBUS.renegotiate();
        break;

      case 'z':
        // Reset module, clear EEPROM
        static bool ResetRq = false;
        static unsigned long ResWaitTime;
        if (!ResetRq) {
          // start timeout timer
          Serial << F(">Reset & EEPROM wipe requested. Press 'z' again within 2 secs to confirm") << endl;
          ResWaitTime = millis();
          ResetRq = true;
        }
        else {
          // This is a confirmed request
          // 2 sec timeout
          if (ResetRq && ((millis() - ResWaitTime) > 2000)) {
            Serial << F(">timeout expired, reset not performed") << endl;
            ResetRq = false;
          }
          else {
            //Request confirmed within timeout
            Serial << F(">RESETTING AND WIPING EEPROM") << endl;
            config.resetModule();
            ResetRq = false;
          }
        }
        break;

      default:
        // Serial << F("> unknown command ") << c << endl;
        break;
    }
  }
}
