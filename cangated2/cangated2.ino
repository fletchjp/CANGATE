// CANGATED2 experimental to look at consuming own events

/********************************************************************************************/
 /* CANGATE version using Duncan Greenwood's CBUS library
  * I have done this in such a way as to minimize changes to the event sending code.
  *******************************************************
  *  Two, Three or Four input AND 'gates'   Four input 0R 'gate' for CBUS Events.
  *  Also invert output event to cretae NAND and NOR
  *  No Arduino programming required - black box - similar to standard MERG CBUS kits
  *  Can use both LONG  and SHORT events as inputs to the gates  ACON / ACOF    ASON / ASOF
  *  Can use any Event on BUS - not Node Number or Event specific
  *  Input Events into 'gates' can be edited if wrongly added
  *  Copyright 2022 - Philip Silver MERG Member 4082
  */
/********************************************************************************************/




////////////////////////////////////////////////////////////////////////////////////
#define USE_CS_15
// NOTE: The CS pin for CAN has to be changed from 10 to 15 as 10 is used by the lcd.

// IMPORTANT: The external MCP2515 boards use 8Mhz
//            The CBUS shield uses 16Mhz
// Define this for the external board. This code must be compiled separately for each option.
// This applies to Phil's board
//#define USE_EXTERNAL_MCP2515
////////////////////////////////////////////////////////////////////////////////////
#define CONSUME_OWN_EVENTS
/*
Pins used for interface chip

					Nano  
INT         		D2       
SCK   	SCK   		D13      
SI    	MISO  		D11      
SO    	MOSI  		D12      
CS        			D10      
GND         		0V       
VCC         		5V      

*/
/********************************************************************************************/

#define DEBUG 1       // set to 0 for no serial debug

#if DEBUG
#define DEBUG_PRINT(S) Serial << S << endl
#else
#define DEBUG_PRINT(S)
#endif

/********************************************************************************************/
// Load CBUS Libraries
/********************************************************************************************/

//#include <SPI.h>        //equired by the CBUS library to communicate to MCP2515 CAN Controller
//#include <MergCBUS.h>  // Main CBUS Library
//#include <Message.h>   // CBUS Message Libary
//#include <EEPROM.h>   //Required by the CBUS library to read / write Node Identifiction and Node Varaiables

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

////////////DEFINE MODULE/////////////////////////////////////////////////

// module name
const unsigned char mname[7] = { 'G', 'A', 'T', 'E', 'D', ' ', ' ' };

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
#ifdef USE_CS_15
const byte CAN_CS_PIN = 15;  // For hardware with display only - normally 10
#else
const byte CAN_CS_PIN = 10;  // Normally 10
#endif
//const byte CAN_SI_PIN = 11;  // Cannot be changed
//const byte CAN_SO_PIN = 12;  // Cannot be changed
//const byte CAN_SCK_PIN = 13;  // Cannot be changed

// CBUS objects
CBUS2515 CBUS;                      // CBUS object
CBUSConfig config;                  // configuration object
#ifdef CONSUME_OWN_EVENTS
CBUScoe coe(4);  // consume own events - buffer size = 4
#endif
CBUSLED ledGrn, ledYlw;             // LED objects
CBUSSwitch pb_switch;               // switch object


/********************************************************************************************/
//CBUS definitions
/********************************************************************************************/
  #define GREEN_LED 4               //MERG Green (SLIM) LED port
  #define YELLOW_LED 5              //MERG Yellow (FLIM) LED port
  #define PUSH_BUTTON 6             //std merg FLIM / SLIM push button
  //#define HAS_BUTTON 1            // Define this if there is a button
  //#define PUSH_BUTTON1 7          //debug push button
  #define NODE_VARS 1               //sets up number of NVs for module to store variables
  #define NODE_EVENTS 128           //Max Number of supported Events
  #define EVENTS_VARS 3             //number of variables per event Maximum is 20
  #define DEVICE_NUMBERS 0          //number of devices numbers connected to Arduino such as servos, relays etc. Can be used for Short events

#ifdef CONSUME_OWN_EVENTS
byte nopcodes = 4;
const byte opcodes[] = {OPC_ACON, OPC_ACOF, OPC_ASON, OPC_ASOF };
#endif

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
#ifdef CONSUME_OWN_EVENTS
  params.setFlags(PF_FLiM | PF_COMBI | PF_COE);
#else
  params.setFlags(PF_FLiM | PF_COMBI);
#endif

  // assign to CBUS
  CBUS.setParams(params.getParams());
  CBUS.setName(mname);
  
  void eventhandler(byte index, CANFrame *msg); // Advance declaration
  // register our CBUS event handler, to receive event messages of learned events
  CBUS.setEventHandler(eventhandler);

#ifdef CONSUME_OWN_EVENTS
  CBUS.consumeOwnEvents(&coe);
  CBUS.setFrameHandler(framehandler,(byte *) opcodes, nopcodes);
#endif
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

  // Explore using arrays to reduce the amount of repetitive code.
  #define INCOMING_EVENT1_VALUES 104
  bool rxEvent[INCOMING_EVENT1_VALUES+1]; // These need to be set to 0.
  #define SEND_EVENT_VALUES 58
  bool sendEvent[SEND_EVENT_VALUES+1]; // Not all of these are used. All set to 1.
  int logicEventNumber[SEND_EVENT_VALUES+1]; // Not all of these are used. Set to 10 times the index.

/********************************************************************************************/
//Variables
/********************************************************************************************/

int eventVariable1; // used for gate input
int eventVariable2; // used for gate type
int invert; // used to invert output event

/*
   int logicEventNumber21 = 210;  
   int logicEventNumber22 = 220;  
   int logicEventNumber23 = 230;  
   int logicEventNumber24 = 240;  
   int logicEventNumber25 = 250;  
   int logicEventNumber26 = 260;  
   int logicEventNumber27 = 270;  
   int logicEventNumber28 = 280;  

 
   int logicEventNumber31 = 310;  
   int logicEventNumber32 = 320;   
   int logicEventNumber33 = 330;  
   int logicEventNumber34 = 340;   
   int logicEventNumber35 = 350;  
   int logicEventNumber36 = 360;  
   int logicEventNumber37 = 370;  
   int logicEventNumber38 = 380;   

   
   int logicEventNumber41 = 410;  
   int logicEventNumber42 = 420;  
   int logicEventNumber43 = 430;  
   int logicEventNumber44 = 440;  
   int logicEventNumber45 = 450;  
   int logicEventNumber46 = 460;  
   int logicEventNumber47 = 470;  
   int logicEventNumber48 = 480;  
  


   int logicEventNumber51 = 510;  
   int logicEventNumber52 = 520;  
   int logicEventNumber53 = 530;  
   int logicEventNumber54 = 540;  
   int logicEventNumber55 = 550;  
   int logicEventNumber56 = 560;  
   int logicEventNumber57 = 570;  
   int logicEventNumber58 = 580;  

*/



     

 /********************************************************************************************/
 // flags for 2 input AND Gates  
 /********************************************************************************************/
 /*  bool sendEvent21 = 1;
   bool sendEvent22 = 1;
   bool sendEvent23 = 1;
   bool sendEvent24 = 1;
   bool sendEvent25 = 1;
   bool sendEvent26 = 1;
   bool sendEvent27 = 1;
   bool sendEvent28 = 1; */
 /********************************************************************************************/
 // flags for 3 input AND Gates  
 /********************************************************************************************/
 /*  bool sendEvent31 = 1;
   bool sendEvent32 = 1;
   bool sendEvent33 = 1;
   bool sendEvent34 = 1;
   bool sendEvent35 = 1;
   bool sendEvent36 = 1;
   bool sendEvent37 = 1;
   bool sendEvent38 = 1;*/
 /********************************************************************************************/
 // flags for 4 input AND Gates  
 /********************************************************************************************/
 /*  bool sendEvent41 = 1;
   bool sendEvent42 = 1;
   bool sendEvent43 = 1;
   bool sendEvent44 = 1;
   bool sendEvent45 = 1;
   bool sendEvent46 = 1;
   bool sendEvent47 = 1;
   bool sendEvent48 = 1; */
 /********************************************************************************************/
 // flags for 4 input OR Gates  
 /********************************************************************************************/
/*   bool sendEvent51 = 1;
   bool sendEvent52 = 1;
   bool sendEvent53 = 1;
   bool sendEvent54 = 1;
   bool sendEvent55 = 1;
   bool sendEvent56 = 1;
   bool sendEvent57 = 1;
   bool sendEvent58 = 1;
*/

 /********************************************************************************************/
 //  event status flags now set in a loop.
 /********************************************************************************************/

/********************************************************************************************/
//Create the MERG CBUS object - cbus
/********************************************************************************************/

//MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

/********************************************************************************************/



//MergNodeIdentification MergNode=MergNodeIdentification();


void setup () {
  Serial.begin (115200);
  Serial << endl << endl << F("> ** CANGATED ** ") << __FILE__ << endl;
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
  cbus.getNodeId()->setNodeName("GATE",4);       	 	// node name shows in FCU when first detected set your own name for each module - max 7 characters
  cbus.getNodeId()->setModuleId(75);               	    // module number - see cbusdefs
  cbus.getNodeId()->setManufacturerId(0xA5);        	// MERG code
  cbus.getNodeId()->setMinCodeVersion(0x62);      		// Version a
  cbus.getNodeId()->setMaxCodeVersion(1);           	// version 1
  cbus.getNodeId()->setProducerNode(true);          	// Produce Node - Set True if Producer Module
  cbus.getNodeId()->setConsumerNode(true);        		// Consumer Node = Set True if Consumer Module
  cbus.getNodeId()->setConsumeEvents(false);      	    // consume own event COE
  cbus.getNodeId()->setTransportType(1);            	// 1 CAN  2 ETHERNET 3 MiWi
  cbus.getNodeId()->setBetaRelease(1);             		// Beta Release set to zero for release version
  cbus.getNodeId()->setSuportBootLoading(false);        // Allways set false - no bootloading
  cbus.getNodeId()->setCpuManufacturer(2);              // 1 MICROCHIP  2  ATMEL  3 ARM


  cbus.setStdNN(999);                               // Node Number in SLIM Mode. The default is 0 for Consumers or 1 - 99 for Producers.
 */ 

  

/********************************************************************************************/
/* Certain node numbers are presently allocated to specific devices and should not be taught to any
    modules. These are:
    0xFFFF (65535) Used by all CABs
    0xFFFE (65534) - Used by command station
    0x007F (127) - Used by CAN_USB modules
    0x007E (126) - Used by CAN_RS modules
    0x0064 to 0x007D (101 to 125) -   Reserved for modules with fixed NNs.
*/
/********************************************************************************************/



/********************************************************************************************/
// Set ports and CAN Transport Layer
/********************************************************************************************/
/*
  cbus.setLeds(GREEN_LED,YELLOW_LED);                 //set the led ports
  cbus.setPushButton(PUSH_BUTTON);                    //set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);           //function that implements the node logic when recieving events
  cbus.initCanBus(10,CAN_125KBPS,MCP_8MHz,10,200);    //initiate the transport layer. pin=10, rate=125Kbps,10 tries,200 millis between each try
 */ 
  //Note the clock speed 8Mhz. If 16Mhz crystal fitted change above to MCP_16Mhz
/********************************************************************************************/

// Set all these to zero.
  for (int i = 1; i <= INCOMING_EVENT1_VALUES; i++) {
    rxEvent[i] = 0;
  }
// Set all these to one. Not all are used.
  for (int i = 1; i <= SEND_EVENT_VALUES; i++) {
    sendEvent[i] = 1;
    logicEventNumber[i] = i*10;
  }

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
#ifdef CONSUME_OWN_EVENTS
  Serial << F("> Testing consuming own events") << endl;
#endif
}



/********************************************************************************************/
//  Functions
/********************************************************************************************/

void twoInputAndGate(int ev1,int in1,int in2)
{
         bool test_output = (ev1 == 21) || (ev1 == 22) || (ev1 == 25);
         if (test_output) {
            DEBUG_PRINT(F("Processing Gate ") << ev1);
            DEBUG_PRINT(sendEvent[ev1] << " " << rxEvent[in1] << " " << rxEvent[in2]);
         }
         if (invert != ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1){    
                sendOnEvent(true, logicEventNumber[ev1]);
                if (test_output) DEBUG_PRINT(F("Sent ON event ") << logicEventNumber[ev1]);
                sendEvent[ev1] = 0;
         }
         if (invert == ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1){    
                sendOffEvent(true, logicEventNumber[ev1]);
                sendEvent[ev1] = 0;
         }
                  
         if (invert != ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0)) {
                    sendOffEvent(true, logicEventNumber[ev1]);
                    if (test_output) DEBUG_PRINT(F("Sent OFF event ") << logicEventNumber[ev1]);
                    sendEvent[ev1] = 1; 
         }
         else if (invert == ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0)) {
                      sendOnEvent(true, logicEventNumber[ev1]);
                      sendEvent[ev1] = 1; 
         }
}

void threeInputAndGate(int ev1,int in1,int in2,int in3)
{
                  if (invert != ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1){    
                    sendOnEvent(true, logicEventNumber[ev1]);
                    sendEvent[ev1] = 0;
                  }
                  if (invert == ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1){    
                        sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 0;
                  } 
                    
                  if (invert != ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0)) {
                        sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 1; 
                  }
                  else if (invert == ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0)) {
                          sendOnEvent(true, logicEventNumber[ev1]);
                          sendEvent[ev1] = 1; 
                  }     
}

void fourInputAndGate(int ev1,int in1,int in2,int in3,int in4)
{
                if (invert != ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1 && rxEvent[in4] == 1){    
                    sendOnEvent(true, logicEventNumber[ev1]);
                    sendEvent[ev1] = 0;
                  }
                   if (invert == ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1 && rxEvent[in4] == 1){    
                        sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 0;
                  } 
                    
                  if (invert != ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0 || rxEvent[in4] == 0)) {
                        sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 1; 
                  }
                 else if (invert == ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0 || rxEvent[in4] == 0)) {
                          sendOnEvent(true, logicEventNumber[ev1]);
                          sendEvent[ev1] = 1; 
                  }     
}

void fourInputOrGate(int ev1,int in1,int in2,int in3,int in4)
{
             if (invert != ev1 && sendEvent[ev1] == 1 && (rxEvent[in1] == 1 || rxEvent[in2] == 1 || rxEvent[in3] == 1 || rxEvent[in4] == 1)){
                  sendOnEvent(true  , logicEventNumber[ev1]);
                  sendEvent[ev1] = 0;
                  }
                if (invert == ev1 && sendEvent[ev1] == 1 && (rxEvent[in1] == 1 || rxEvent[in2] == 1 || rxEvent[in3] == 1 || rxEvent[in4] == 1)){
                  sendOffEvent(true, logicEventNumber[ev1]);
                  sendEvent[ev1] = 0;
                 }
                 
                 if (invert != ev1 && sendEvent[ev1] == 0 && rxEvent[in1] == 0 && rxEvent[in2] == 0 && rxEvent[in3] == 0 && rxEvent[in4] == 0){
                            sendOffEvent(true, logicEventNumber[ev1]);
                            sendEvent[ev1] = 1; 
                 } 
                 else if (invert == ev1 && sendEvent[ev1] == 0 && rxEvent[in1] == 0 && rxEvent[in2] == 0 && rxEvent[in3] == 0 && rxEvent[in4] == 0){
                            sendOnEvent(true, logicEventNumber[ev1]);
                            sendEvent[ev1] = 1; 
                 } 
}



#ifdef CONSUME_OWN_EVENTS
// Converted to get an opcode array now done when calling SetFrameHandler.
void framehandler(CANFrame *msg) {
  // as an example, format and display the received frame
  byte index, ev1, ev2, ev3;
  byte data[4];
#if DEBUG
  Serial << F("[ ") << (msg->id & 0x7f) << F("] [") << msg->len << F("] [");
  if ( msg->len > 0) {
    for (byte d = 0; d < msg->len; d++) {
      Serial << F(" 0x") << _HEX(msg->data[d]);
    }
  Serial << F(" ]") << endl;
  }
  
  if (nopcodes > 0) {
    Serial << F("Opcodes [ ");
    for(byte i = 0;  i < nopcodes; i++)
    {
       Serial << F(" 0x") << _HEX(opcodes[i]);
    }
    Serial << F(" ]") << endl;
  }
  byte CBUSOpc = msg->data[0];
  bool isAccOn  = ((CBUSOpc == OPC_ACON) || (CBUSOpc == OPC_ASON) );
  bool isAccOff = ((CBUSOpc == OPC_ACOF) || (CBUSOpc == OPC_ASOF) ); 

  unsigned int nodeNumber = (msg->data[1] << 8 ) + msg->data[2];
  unsigned int eventNumber = (msg->data[3] << 8 ) + msg->data[4];
  DEBUG_PRINT(F("> NN = ") << nodeNumber << F(", EN = ") << eventNumber);
  DEBUG_PRINT(F("> op_code = ") << CBUSOpc);
  if (nodeNumber == config.nodeNum) {
    DEBUG_PRINT(F("> Event from this node ") << nodeNumber);
    index = config.findExistingEvent(nodeNumber, eventNumber);
    if (index >= config.EE_MAX_EVENTS) {
      DEBUG_PRINT(F("> Event not found in the table"));
      index = config.findEventSpace();
      DEBUG_PRINT(F("> Next free space is ") << index);
      data[0] = highByte(nodeNumber);
      data[1] = lowByte(nodeNumber);
      data[2] = highByte(eventNumber);
      data[3] = lowByte(eventNumber);
      config.writeEvent(index, data);
      config.updateEvHashEntry(index);
      DEBUG_PRINT(F("> Event written at ") << index);
    } else {
      DEBUG_PRINT(F("> Event found in the table as entry ") << index);
      ev1 = config.getEventEVval(index, 1);
      ev2 = config.getEventEVval(index, 2);
      ev3 = config.getEventEVval(index, 3);
      DEBUG_PRINT(F("> EVs are ") << ev1 << " " << ev2 << " " << ev3);
      if ((ev2 > 0) &&  (ev2 <= SEND_EVENT_VALUES)) {
        if (sendEvent[ev2]) {
          DEBUG_PRINT(F("SendEvent True"));
          //sendOffEvent(true, logicEventNumber[ev2]);
          if (ev1 == ev3)
            sendEvent[ev2] = 1; 
          else 
            sendEvent[ev2] = 0; 
        } else {
          DEBUG_PRINT(F("SendEvent False"));
          //sendOffEvent(true, logicEventNumber[ev2]);
          if (ev1 == ev3)
            sendEvent[ev2] = 0;
          else
            sendEvent[ev2] = 1; 
        }
        eventhandler(index,msg);
      }
    }
  }
#endif // DEBUG

}
#endif // CONSUME_OWN_EVENTS

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
  
   //byte CBUSOpc = msg->getOpc(); // Get The OPCODE from Message
   //int nodeNumber = msg->getNodeNumber(); // Get The Node Number from Message
   //int eventNumber = msg->getEventNumber(); // Get The Event Number from Message
   
   bool isAccOn  = ((CBUSOpc == OPC_ACON) || (CBUSOpc == OPC_ASON) );
   bool isAccOff = ((CBUSOpc == OPC_ACOF) || (CBUSOpc == OPC_ASOF) ); 

     
   // This is not needed as only taught events are seen here
   //if (mcbus->eventMatch()){  //The received event has been taught this module

        eventVariable1 = config.getEventEVval(index, 1);// mcbus->getEventVar(msg,1);  // Use The Event Variables to  select which event to use for the logic
    
        eventVariable2 = config.getEventEVval(index, 2);//mcbus->getEventVar(msg,2); // Use the Event variable to select gate type

        invert = config.getEventEVval(index, 3);        //mcbus->getEventVar(msg,3); // used to invert event

/********************************************************************************************/
         if(eventVariable1 > 0 && eventVariable1 <= INCOMING_EVENT1_VALUES) {
                     if (isAccOn){
                        rxEvent[eventVariable1] = 1; 
                        }
                     else if (isAccOff) {
                        rxEvent[eventVariable1] = 0;   
                       }
         }
      
/********************************************************************************************/         

    
/********************************************************************************************/
//  2 Input AND Gate 1
/********************************************************************************************/
        if (eventVariable2 == 21) twoInputAndGate(21,1,2);
/********************************************************************************************/

/********************************************************************************************/
// 2 Input AND Gate 2
/********************************************************************************************/
        if (eventVariable2 == 22) twoInputAndGate(22,3,4);
/********************************************************************************************/

/********************************************************************************************/
// 2 Input AND Gate 3
/********************************************************************************************/
         if (eventVariable2 == 23) twoInputAndGate(23,5,6);
/********************************************************************************************/

/********************************************************************************************/
// 2 Input AND Gate 4
/********************************************************************************************/
          if (eventVariable2 == 24) twoInputAndGate(24,7,8);
/********************************************************************************************/


/********************************************************************************************/
// 2 Input AND Gate 5
/********************************************************************************************/
          if (eventVariable2 == 25) twoInputAndGate(25,9,10);
/********************************************************************************************/


/********************************************************************************************/
// 2 Input AND Gate 6
/********************************************************************************************/
          if (eventVariable2 == 26) twoInputAndGate(26,11,12);
/********************************************************************************************/

/********************************************************************************************/
// 2 Input AND Gate 7
/********************************************************************************************/
          if (eventVariable2 == 27) twoInputAndGate(27,13,14);
/********************************************************************************************/

/********************************************************************************************/
// 2 Input AND Gate 8
/********************************************************************************************/
          if (eventVariable2 == 28) twoInputAndGate(28,15,16);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 1
/********************************************************************************************/
        if (eventVariable2 == 31) threeInputAndGate(31,17,18,19);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 2
/********************************************************************************************/
        if (eventVariable2 == 32) threeInputAndGate(32,20,21,22);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 3
/********************************************************************************************/
        if (eventVariable2 == 33) threeInputAndGate(33,23,24,25);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 4
/********************************************************************************************/
        if (eventVariable2 == 34) threeInputAndGate(34,26,27,28);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 5
/********************************************************************************************/
        if (eventVariable2 == 35) threeInputAndGate(35,29,30,31);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 6
/********************************************************************************************/
        if (eventVariable2 == 36) threeInputAndGate(36,32,33,34);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 7
/********************************************************************************************/
        if (eventVariable2 == 37) threeInputAndGate(37,35,36,37);
/********************************************************************************************/

/********************************************************************************************/
// 3 Input AND Gate 8
/********************************************************************************************/
        if (eventVariable2 == 38) threeInputAndGate(38,38,39,40);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 1
/********************************************************************************************/
        if (eventVariable2 == 41) fourInputAndGate(41,41,42,43,44);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 2
/********************************************************************************************/
        if (eventVariable2 == 42) fourInputAndGate(42,45,46,47,48);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 3
/********************************************************************************************/
        if (eventVariable2 == 43) fourInputAndGate(43,49,50,51,52);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 4
/********************************************************************************************/
        if (eventVariable2 == 44) fourInputAndGate(44,53,54,55,56);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 5
/********************************************************************************************/
        if (eventVariable2 == 45) fourInputAndGate(45,57,58,59,60);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 6
/********************************************************************************************/
        if (eventVariable2 == 46) fourInputAndGate(46,61,62,63,64);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 7
/********************************************************************************************/
        if (eventVariable2 == 47) fourInputAndGate(47,65,66,67,68);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input AND Gate 8
/********************************************************************************************/
        if (eventVariable2 == 48) fourInputAndGate(44,69,70,71,72);
/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 1
/********************************************************************************************/
        if (eventVariable2 == 51) fourInputOrGate(51,73,74,75,76);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 2
/********************************************************************************************/
        if (eventVariable2 == 52) fourInputOrGate(52,77,78,79,80);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 3
/********************************************************************************************/
        if (eventVariable2 == 53) fourInputOrGate(53,81,82,83,84);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 4
/********************************************************************************************/
        if (eventVariable2 == 54) fourInputOrGate(54,85,86,87,88);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 5
/********************************************************************************************/
        if (eventVariable2 == 55) fourInputOrGate(55,89,90,91,92);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 6
/********************************************************************************************/
        if (eventVariable2 == 56) fourInputOrGate(56,93,94,95,96);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 7
/********************************************************************************************/
        if (eventVariable2 == 57) fourInputOrGate(57,97,98,99,100);
/********************************************************************************************/

/********************************************************************************************/
// 4 Input OR Gate 8
/********************************************************************************************/
          if (eventVariable2 == 58) fourInputOrGate(58,101,102,103,104);
/********************************************************************************************/


       // } // End OF Recieved Events Not needed
          
    
           
  } // end function
                


/*FUNCTIONS FINISH ***********************************************************************************/

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
#ifdef CONSUME_OWN_EVENTS
    coe.put(&msg);
#endif
    DEBUG_PRINT(F("> sent CBUS message with Event Number ") << eventNo);
  } else {
    DEBUG_PRINT(F("> error sending CBUS message"));
  }
  return success;
}

bool sendOnEvent(bool longEvent, unsigned int eventNo)
{
   sendEvent[eventNo] = 0;
   if(longEvent) return sendAnEvent(OPC_ACON,eventNo);
   else return sendAnEvent(OPC_ASON,eventNo);
}

bool sendOffEvent(bool longEvent, unsigned int eventNo)
{
   sendEvent[eventNo] = 1;
   if(longEvent) return sendAnEvent(OPC_ACOF,eventNo);
   else return sendAnEvent(OPC_ASOF,eventNo);
}



void loop() {

   // cbus.run();// Run CBUS
   // cbus.cbusRead(); // Check CBUS Buffers for any activity
  // do CBUS message, switch and LED processing
  CBUS.process();

  // process console commands
  processSerialInput();

}

//
/// command interpreter for serial console input
//

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
