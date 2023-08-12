
/********************************************************************************************/
 /* CANGATE version using Duncan Greenwood's CBUS library
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

#define DEBUG 0       // set to 0 for no serial debug

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

// CBUS library header files
#include <CBUS2515.h>            // CAN controller and CBUS class
#include "LEDControl.h"          // CBUS LEDs
#include <CBUSconfig.h>          // module configuration
#include <cbusdefs.h>            // MERG CBUS constants
#include <CBUSParams.h>

/********************************************************************************************/

////////////DEFINE MODULE/////////////////////////////////////////////////

// module name
unsigned char mname[7] = { 'G', 'A', 'T', 'E', ' ', ' ', ' ' };

// constants
const byte VER_MAJ = 2;         // code major version
const char VER_MIN = ' ';       // code minor version
const byte VER_BETA = 0;        // code beta sub-version
const byte MODULE_ID = 99;      // CBUS module type

const unsigned long CAN_OSC_FREQ = 8000000;     // Oscillator frequency on the CAN2515 board

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



/********************************************************************************************/
//CBUS definitions
/********************************************************************************************/
  #define GREEN_LED 4               //MERG Green (SLIM) LED port
  #define YELLOW_LED 5              //MERG Yellow (FLIM) LED port
  #define PUSH_BUTTON 6             //std merg FLIM / SLIM push button
  //#define PUSH_BUTTON1 7          //debug push button
  #define NODE_VARS 1               //sets up number of NVs for module to store variables
  #define NODE_EVENTS 136           //Max Number of supported Events
  #define EVENTS_VARS 20            //number of variables per event Maximum is 20
  #define DEVICE_NUMBERS 0          //number of devices numbers connected to Arduino such as servos, relays etc. Can be used for Short events

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
  config.EE_NUM_EVS = EVENT_VARS;
  config.EE_BYTES_PER_EVENT = (config.EE_NUM_EVS + 4);

  // initialise and load configuration
  config.setEEPROMtype(EEPROM_INTERNAL);
  config.begin();

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
  Serial << endl << endl << F("> ** CANGATE ** ") << __FILE__ << endl;

  setupCBUS();




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

void twoInputAndGate(int ev1,int in1,int in2)
{
         if (invert != ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1){    
                cbus.sendOnEvent(true, logicEventNumber[ev1]);
                sendEvent[ev1] = 0;
              }
              if (invert == ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1){    
                 cbus.sendOffEvent(true, logicEventNumber[21]);
                  sendEvent[ev1] = 0;
                  }
                  
           else if (invert != ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0)) {
                    cbus.sendOffEvent(true, logicEventNumber[ev1]);
                    sendEvent[ev1] = 1; 
                    }
              else if (invert == ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0)) {
                      cbus.sendOnEvent(true, logicEventNumber[ev1]);
                      sendEvent[ev1] = 1; 
                    }
}

void threeInputAndGate(int ev1,int in1,int in2,int in3)
{
                if (invert != ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1){    
                    cbus.sendOnEvent(true, logicEventNumber[ev1]);
                    sendEvent[ev1] = 0;
                  }
                   if (invert == ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1){    
                        cbus.sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 0;
                  } 
                    
               else if (invert != ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 1; 
                  }
                 else if (invert == ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0)) {
                          cbus.sendOnEvent(true, logicEventNumber[ev1]);
                          sendEvent[ev1] = 1; 
                  }     
}

void fourInputAndGate(int ev1,int in1,int in2,int in3,int in4)
{
                if (invert != ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1 && rxEvent[in4] == 1){    
                    cbus.sendOnEvent(true, logicEventNumber[ev1]);
                    sendEvent[ev1] = 0;
                  }
                   if (invert == ev1 && sendEvent[ev1] == 1 && rxEvent[in1] == 1 && rxEvent[in2] == 1 && rxEvent[in3] == 1 && rxEvent[in4] == 1){    
                        cbus.sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 0;
                  } 
                    
               else if (invert != ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0 || rxEvent[in4] == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber[ev1]);
                        sendEvent[ev1] = 1; 
                  }
                 else if (invert == ev1 && sendEvent[ev1] == 0 && (rxEvent[in1] == 0 || rxEvent[in2] == 0 || rxEvent[in3] == 0 || rxEvent[in4] == 0)) {
                          cbus.sendOnEvent(true, logicEventNumber[ev1]);
                          sendEvent[ev1] = 1; 
                  }     
}

void fourInputOrGate(int ev1,int in1,int in2,int in3,int in4)
{
             if (invert != ev1 && sendEvent[ev1] == 1 && (rxEvent[in1] == 1 || rxEvent[in2] == 1 || rxEvent[in3] == 1 || rxEvent[in4] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber[ev1]);
                  sendEvent[ev1] = 0;
                  }
                if (invert == ev1 && sendEvent[ev1] == 1 && (rxEvent[in1] == 1 || rxEvent[in2] == 1 || rxEvent[in3] == 1 || rxEvent[in4] == 1)){
                 cbus.sendOffEvent(true, logicEventNumber[ev1]);
                  sendEvent[ev1] = 0;
                 }
                 
                  else if (invert != ev1 && sendEvent[ev1] == 0 && rxEvent[in1] == 0 && rxEvent[in2] == 0 && rxEvent[in3] == 0 && rxEvent[in4] == 0){
                            cbus.sendOffEvent(true, logicEventNumber[ev1]);
                              sendEvent[ev1] = 1; 
                 } 
                 else if (invert == ev1 && sendEvent[ev1] == 0 && rxEvent[in1] == 0 && rxEvent[in2] == 0 && rxEvent[in3] == 0 && rxEvent[in4] == 0){
                            cbus.sendOnEvent(true, logicEventNumber[ev1]);
                              sendEvent[ev1] = 1; 
                 } 
}

void myUserFunc(Message *msg,MergCBUS *mcbus){
  
   byte CBUSOpc = msg->getOpc(); // Get The OPCODE from Message
   int nodeNumber = msg->getNodeNumber(); // Get The Node Number from Message
   int eventNumber = msg->getEventNumber(); // Get The Event Number from Message
   
       
     
   
   if (mcbus->eventMatch()){  //The received event has been taught this module

        eventVariable1 = mcbus->getEventVar(msg,1);  // Use The Event Variables to  select which event to use for the logic
    
        eventVariable2 = mcbus->getEventVar(msg,2); // Use the Event variable to select gate type

        invert = mcbus->getEventVar(msg,3); // used to invert event

/********************************************************************************************/
         if(eventVariable1 > 0 && eventVariable1 <= INCOMING_EVENT1_VALUES) {
                     if (mcbus->isAccOn()== true){
                        rxEvent[eventVariable1] = 1; 
                        }
                     else if (mcbus->isAccOff()== true) {
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


        } // End OF Recieved Events
          
    
           
  } // end function
                


/*FUNCTIONS FINISH ***********************************************************************************/



void loop() {

    cbus.run();// Run CBUS
    cbus.cbusRead(); // Check CBUS Buffers for any activity

}
