
/********************************************************************************************/
 /* CANGATE
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


/********************************************************************************************/
// Load CBUS Libraries
/********************************************************************************************/

#include <SPI.h>        //equired by the CBUS library to communicate to MCP2515 CAN Controller
#include <MergCBUS.h>  // Main CBUS Library
#include <Message.h>   // CBUS Message Libary
#include <EEPROM.h>   //Required by the CBUS library to read / write Node Identifiction and Node Varaiables


/********************************************************************************************/





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





     

 /********************************************************************************************/
 // flags for 2 input AND Gates  
 /********************************************************************************************/
   bool sendEvent21 = 1;
   bool sendEvent22 = 1;
   bool sendEvent23 = 1;
   bool sendEvent24 = 1;
   bool sendEvent25 = 1;
   bool sendEvent26 = 1;
   bool sendEvent27 = 1;
   bool sendEvent28 = 1;
 /********************************************************************************************/
 // flags for 3 input AND Gates  
 /********************************************************************************************/
   bool sendEvent31 = 1;
   bool sendEvent32 = 1;
   bool sendEvent33 = 1;
   bool sendEvent34 = 1;
   bool sendEvent35 = 1;
   bool sendEvent36 = 1;
   bool sendEvent37 = 1;
   bool sendEvent38 = 1;
  /********************************************************************************************/
 // flags for 4 input AND Gates  
 /********************************************************************************************/
   bool sendEvent41 = 1;
   bool sendEvent42 = 1;
   bool sendEvent43 = 1;
   bool sendEvent44 = 1;
   bool sendEvent45 = 1;
   bool sendEvent46 = 1;
   bool sendEvent47 = 1;
   bool sendEvent48 = 1;
 /********************************************************************************************/
 // flags for 4 input OR Gates  
 /********************************************************************************************/
   bool sendEvent51 = 1;
   bool sendEvent52 = 1;
   bool sendEvent53 = 1;
   bool sendEvent54 = 1;
   bool sendEvent55 = 1;
   bool sendEvent56 = 1;
   bool sendEvent57 = 1;
   bool sendEvent58 = 1;


 /********************************************************************************************/
 //  event status flags now set in a loop.
 /********************************************************************************************/

/********************************************************************************************/
//Create the MERG CBUS object - cbus
/********************************************************************************************/

MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

/********************************************************************************************/



MergNodeIdentification MergNode=MergNodeIdentification();


void setup () {




/********************************************************************************************/
//Configuration CBUS data for the node
/********************************************************************************************/

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

  cbus.setLeds(GREEN_LED,YELLOW_LED);                 //set the led ports
  cbus.setPushButton(PUSH_BUTTON);                    //set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);           //function that implements the node logic when recieving events
  cbus.initCanBus(10,CAN_125KBPS,MCP_8MHz,10,200);    //initiate the transport layer. pin=10, rate=125Kbps,10 tries,200 millis between each try
  
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

          if (eventVariable2 == 41){
            if (invert != 41 && sendEvent41 == 1 && rxEvent[41] == 1 && rxEvent[42] == 1 && rxEvent[43] == 1 && rxEvent[44] == 1){    
              cbus.sendOnEvent(true, logicEventNumber41);
               sendEvent41 = 0;
                  }
                  if (invert == 41 && sendEvent41 == 1 && rxEvent[41] == 1 && rxEvent[42] == 1 && rxEvent[43] == 1 && rxEvent[44] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber41);
                      sendEvent41 = 0;
                  }
                  
               else if (invert != 41 && sendEvent41 == 0 && (rxEvent[41] == 0 || rxEvent[42] == 0 || rxEvent[43] == 0 || rxEvent[44] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber41);
                        sendEvent41 = 1; 
                 }
                 else if (invert == 41 && sendEvent41 == 0 && (rxEvent[41] == 0 || rxEvent[42] == 0 || rxEvent[43] == 0 || rxEvent[44] == 0)){
                        cbus.sendOnEvent(true, logicEventNumber41);
                        sendEvent41 = 1; 
                 }  
                 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 2
/********************************************************************************************/

          if (eventVariable2 == 42){
            if (invert != 42 && sendEvent42 == 1 && rxEvent[45] == 1 && rxEvent[46] == 1 && rxEvent[47] == 1 && rxEvent[48] == 1){    
                cbus.sendOnEvent(true, logicEventNumber42);
                sendEvent42 = 0;
                  }
                  if (invert == 42 && sendEvent42 == 1 && rxEvent[45] == 1 && rxEvent[46] == 1 && rxEvent[47] == 1 && rxEvent[48] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber42);
                      sendEvent42 = 0;
                  }
                  
               else if (invert != 42 && sendEvent42 == 0 && (rxEvent[45] == 0 || rxEvent[46] == 0 || rxEvent[47] == 0 || rxEvent[48] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber42);
                        sendEvent42 = 1;
                  }
                  else if (invert == 42 && sendEvent42 == 0 && (rxEvent[45] == 0 || rxEvent[46] == 0 || rxEvent[47] == 0 || rxEvent[48] == 0)){
                          cbus.sendOnEvent(true, logicEventNumber42);
                          sendEvent42 = 1;
                  }
             }

/********************************************************************************************/





/********************************************************************************************/
// 4 Input AND Gate 3
/********************************************************************************************/

           if (eventVariable2 == 43){
            if (invert != 43 && sendEvent43 == 1 && rxEvent[49] == 1 && rxEvent[50] == 1 && rxEvent[51] == 1 && rxEvent[52] == 1){    
              cbus.sendOnEvent(true, logicEventNumber43);
               sendEvent43 = 0;
                  }
                   if (invert == 43 && sendEvent43 == 1 && rxEvent[49] == 1 && rxEvent[50] == 1 && rxEvent[51] == 1 && rxEvent[52] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber43);
                      sendEvent43 = 0;
                  }
                  
               else if (invert != 43 && sendEvent43 == 0 && (rxEvent[49] == 0 || rxEvent[50] == 0 || rxEvent[51] == 0 || rxEvent[52] == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber43);
                        sendEvent43= 1; 
                  }
                  else if (invert == 43 && sendEvent43 == 0 && (rxEvent[49] == 0 || rxEvent[50] == 0 || rxEvent[51] == 0 || rxEvent[52] == 0)) {
                           cbus.sendOnEvent(true, logicEventNumber43);
                            sendEvent43= 1; 
                  } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 4
/********************************************************************************************/

          if (eventVariable2 == 44){
            if (invert != 44 && sendEvent44 == 1 && rxEvent[53] == 1 && rxEvent[54] == 1 && rxEvent[55] == 1 && rxEvent[56] == 1){    
                cbus.sendOnEvent(true, logicEventNumber44);
                sendEvent44 = 0;
                  }
                  if (invert == 44 && sendEvent44 == 1 && rxEvent[53] == 1 && rxEvent[54] == 1 && rxEvent[55] == 1 && rxEvent[56] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber44);
                       sendEvent44 = 0;
                  }
                     
               else if (invert != 44 && sendEvent44 == 0 && (rxEvent[53] == 0 || rxEvent[54] == 0 || rxEvent[55] == 0 || rxEvent[56] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber44);
                        sendEvent44= 1; 
                  }
                  else if (invert == 44 && sendEvent44 == 0 && (rxEvent[53] == 0 || rxEvent[54] == 0 || rxEvent[55] == 0 || rxEvent[56] == 0)){
                          cbus.sendOnEvent(true, logicEventNumber44);
                          sendEvent44= 1; 
                  } 
             }

/********************************************************************************************/




/********************************************************************************************/
// 4 Input AND Gate 5
/********************************************************************************************/

          if (eventVariable2 == 45){
            if (invert != 45 && sendEvent45 == 1 && rxEvent[57] == 1 && rxEvent[58] == 1 && rxEvent[59] == 1 && rxEvent[60] == 1){    
                cbus.sendOnEvent(true, logicEventNumber45);
                sendEvent45 = 0;
                  }
                   if (invert == 45 && sendEvent45 == 1 && rxEvent[57] == 1 && rxEvent[58] == 1 && rxEvent[59] == 1 && rxEvent[60] == 1){    
                        cbus.sendOffEvent(true, logicEventNumber45);
                        sendEvent45 = 0;
                  }
                  
               else if (invert != 45 && sendEvent45 == 0 && (rxEvent[57] == 0 || rxEvent[58] == 0 || rxEvent[59] == 0 || rxEvent[60] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber45);
                        sendEvent45= 1; 
                  }
                  else if (invert == 45 && sendEvent45 == 0 && (rxEvent[57] == 0 || rxEvent[58] == 0 || rxEvent[59] == 0 || rxEvent[60] == 0)){
                          cbus.sendOnEvent(true, logicEventNumber45);
                          sendEvent45= 1; 
                  } 
             }

/********************************************************************************************/




/********************************************************************************************/
// 4 Input AND Gate 6
/********************************************************************************************/

          if (eventVariable2 == 46){
            if (invert != 46 && sendEvent46 == 1 && rxEvent[61] == 1 && rxEvent[62] == 1 && rxEvent[63] == 1 && rxEvent[64] == 1){    
                cbus.sendOnEvent(true, logicEventNumber46);
                sendEvent46 = 0;
                  }
                  if (invert == 46 && sendEvent46 == 1 && rxEvent[61] == 1 && rxEvent[62] == 1 && rxEvent[63] == 1 && rxEvent[64] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber46);
                      sendEvent46 = 0;
                  }
                  
               else if (invert != 46 && sendEvent46 == 0 && (rxEvent[61] == 0 || rxEvent[62] == 0 || rxEvent[63] == 0 || rxEvent[64] == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber46);
                        sendEvent46= 1; 
                  }
                  else if (invert == 46 && sendEvent46 == 0 && (rxEvent[61] == 0 || rxEvent[62] == 0 || rxEvent[63] == 0 || rxEvent[64] == 0)) {
                          cbus.sendOnEvent(true, logicEventNumber46);
                          sendEvent46= 1; 
                  } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 7
/********************************************************************************************/

           if (eventVariable2 == 47){
            if (invert != 47 && sendEvent47 == 1 && rxEvent[65] == 1 && rxEvent[66] == 1 && rxEvent[67] == 1 && rxEvent[68] == 1 ){    
                cbus.sendOnEvent(true, logicEventNumber47);
                sendEvent47 = 0;
                  }
                  if (invert == 47 && sendEvent47 == 1 && rxEvent[65] == 1 && rxEvent[66] == 1 && rxEvent[67] == 1 && rxEvent[68] == 1 ){    
                      cbus.sendOffEvent(true, logicEventNumber47);
                      sendEvent47 = 0;
                  }
                  
               else if (invert != 47 && sendEvent47 == 0 && (rxEvent[65] == 0 || rxEvent[66] == 0 || rxEvent[67] == 0 || rxEvent[68] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber47);
                        sendEvent47= 1; 
                 }
                 else if (invert == 47 && sendEvent47 == 0 && (rxEvent[65] == 0 || rxEvent[66] == 0 || rxEvent[67] == 0 || rxEvent[68] == 0)){
                          cbus.sendOnEvent(true, logicEventNumber47);
                          sendEvent47= 1; 
                 } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 8
/********************************************************************************************/

          if (eventVariable2 == 48){
           if (invert != 48 && sendEvent48 == 1 && rxEvent[69] == 1 && rxEvent[70] == 1 && rxEvent[71] == 1 && rxEvent[72] == 1){    
              cbus.sendOnEvent(true, logicEventNumber48);
               sendEvent48 = 0;
                  }
                  if (invert == 48 && sendEvent48 == 1 && rxEvent[69] == 1 && rxEvent[70] == 1 && rxEvent[71] == 1 && rxEvent[72] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber48);
                      sendEvent48 = 0;
                  }
                  
               else if (invert !=48 && sendEvent48 == 0 && (rxEvent[69] == 0 || rxEvent[70] == 0 || rxEvent[71] == 0 || rxEvent[72] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber48);
                        sendEvent48= 1; 
                  }
                  else if (invert ==48 && sendEvent48 == 0 && (rxEvent[69] == 0 || rxEvent[70] == 0 || rxEvent[71] == 0 || rxEvent[72] == 0)){
                        cbus.sendOnEvent(true, logicEventNumber48);
                        sendEvent48= 1; 
                  }  
             }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 1
/********************************************************************************************/

          if  (eventVariable2 == 51){
             if (invert != 51 && sendEvent51 == 1 && (rxEvent[73] == 1 || rxEvent[74] == 1 || rxEvent[75] == 1 || rxEvent[76] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber51);
                  sendEvent51 = 0;
                  }
                if (invert == 51 && sendEvent51 == 1 && (rxEvent[73] == 1 || rxEvent[74] == 1 || rxEvent[75] == 1 || rxEvent[76] == 1)){
                 cbus.sendOffEvent(true, logicEventNumber51);
                  sendEvent51 = 0;
                 }
                 
                  else if (invert != 51 && sendEvent51 == 0 && rxEvent[73] == 0 && rxEvent[74] == 0 && rxEvent[75] == 0 && rxEvent[76] == 0){
                            cbus.sendOffEvent(true, logicEventNumber51);
                              sendEvent51 = 1; 
                 } 
                 else if (invert == 51 && sendEvent51 == 0 && rxEvent[73] == 0 && rxEvent[74] == 0 && rxEvent[75] == 0 && rxEvent[76] == 0){
                            cbus.sendOnEvent(true, logicEventNumber51);
                              sendEvent51 = 1; 
                 } 
              }
      
/********************************************************************************************/



/********************************************************************************************/
// 4 Input OR Gate 2
/********************************************************************************************/
          if  (eventVariable2 == 52){
             if (invert != 52 && sendEvent52 == 1 && (rxEvent[77] == 1 || rxEvent[78] == 1 || rxEvent[79] == 1 || rxEvent[80] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber52);
                  sendEvent52 = 0;
                  }
                if (invert == 52 && sendEvent52 == 1 && (rxEvent[77] == 1 || rxEvent[78] == 1 || rxEvent[79] == 1 || rxEvent[80] == 1)){
                 cbus.sendOffEvent(true, logicEventNumber52);
                  sendEvent52 = 0;
                 }
                 
                  else if (invert != 52 && sendEvent52 == 0 && rxEvent[77] == 0 && rxEvent[78] == 0 && rxEvent[79] == 0 && rxEvent[80] == 0){
                            cbus.sendOffEvent(true, logicEventNumber52);
                              sendEvent51 = 1; 
                 } 
                 else if (invert == 52 && sendEvent52 == 0 && rxEvent[77] == 0 && rxEvent[78] == 0 && rxEvent[79] == 0 && rxEvent[80] == 0){
                            cbus.sendOnEvent(true, logicEventNumber52);
                              sendEvent52 = 1; 
                 } 
              }
/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 3
/********************************************************************************************/

           if  (eventVariable2 == 53){
             if (invert != 53 && sendEvent53 == 1 && (rxEvent[81] == 1 || rxEvent[82] == 1 || rxEvent[83] == 1 || rxEvent[84] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber53);
                  sendEvent53 = 0;
                  }
                if (invert == 53 && sendEvent53 == 1 && (rxEvent[81] == 1 || rxEvent[82] == 1 || rxEvent[83] == 1 || rxEvent[84] == 1)){
                    cbus.sendOffEvent(true, logicEventNumber53);
                      sendEvent53 = 0;
                 }
                 
                  else if (invert != 53 && sendEvent53 == 0 && rxEvent[81] == 0 && rxEvent[82] == 0 && rxEvent[83] == 0 && rxEvent[84] == 0){
                            cbus.sendOffEvent(true, logicEventNumber53);
                              sendEvent53 = 1; 
                 } 
                    else if (invert == 53 && sendEvent53 == 0 && rxEvent[81] == 0 && rxEvent[82] == 0 && rxEvent[83] == 0 && rxEvent[84] == 0){
                            cbus.sendOnEvent(true, logicEventNumber53);
                              sendEvent53 = 1; 
                 } 
              }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 4
/********************************************************************************************/

   if  (eventVariable2 == 54){
             if (invert != 54 && sendEvent54 == 1 && (rxEvent[85] == 1 || rxEvent[86] == 1 || rxEvent[87] == 1 || rxEvent[88] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber54);
                  sendEvent54 = 0;
                  }
                if (invert == 54 && sendEvent54 == 1 && (rxEvent[85] == 1 || rxEvent[86] == 1 || rxEvent[87] == 1 || rxEvent[88] == 1)){
                    cbus.sendOffEvent(true, logicEventNumber54);
                      sendEvent54 = 0;
                 }
                 
                  else if (invert != 54 && sendEvent54 == 0 && rxEvent[85] == 0 && rxEvent[86] == 0 && rxEvent[87] == 0 && rxEvent[88] == 0){
                            cbus.sendOffEvent(true, logicEventNumber54);
                              sendEvent54 = 1; 
                 } 
                    else if (invert == 54 && sendEvent54 == 0 && rxEvent[85] == 0 && rxEvent[86] == 0 && rxEvent[87] == 0 && rxEvent[88] == 0){
                            cbus.sendOnEvent(true, logicEventNumber54);
                              sendEvent54 = 1; 
                 } 
              }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input OR Gate 5
/********************************************************************************************/
          if  (eventVariable2 == 55){
             if (invert != 55 && sendEvent55 == 1 && (rxEvent[89] == 1 || rxEvent[90] == 1 || rxEvent[91] == 1 || rxEvent[92] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber55);
                  sendEvent55 = 0;
                  }
                if (invert == 55 && sendEvent55 == 1 && (rxEvent[89] == 1 || rxEvent[90] == 1 || rxEvent[91] == 1 || rxEvent[92] == 1)){
                    cbus.sendOffEvent(true, logicEventNumber55);
                      sendEvent55 = 0;
                 }
                 
                  else if (invert != 55 && sendEvent55 == 0 && rxEvent[89] == 0 && rxEvent[90] == 0 && rxEvent[91] == 0 && rxEvent[92] == 0){
                            cbus.sendOffEvent(true, logicEventNumber55);
                              sendEvent55 = 1; 
                 } 
                    else if (invert == 55 && sendEvent55 == 0 && rxEvent[89] == 0 && rxEvent[90] == 0 && rxEvent[91] == 0 && rxEvent[92] == 0){
                            cbus.sendOnEvent(true, logicEventNumber55);
                              sendEvent55 = 1; 
                 } 
              }
/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 6
/********************************************************************************************/
           if  (eventVariable2 == 56){
             if (invert != 56 && sendEvent56 == 1 && (rxEvent[93] == 1 || rxEvent[94] == 1 || rxEvent[95] == 1 || rxEvent[96] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber56);
                  sendEvent56 = 0;
                  }
                if (invert == 56 && sendEvent56 == 1 && (rxEvent[93] == 1 || rxEvent[94] == 1 || rxEvent[95] == 1 || rxEvent[96] == 1)){
                    cbus.sendOffEvent(true, logicEventNumber56);
                      sendEvent56 = 0;
                 }
                 
                  else if (invert != 56 && sendEvent56 == 0 && rxEvent[93] == 0 && rxEvent[94] == 0 && rxEvent[95] == 0 && rxEvent[96] == 0){
                            cbus.sendOffEvent(true, logicEventNumber56);
                              sendEvent56 = 1; 
                 } 
                    else if (invert == 56 && sendEvent56 == 0 && rxEvent[93] == 0 && rxEvent[94] == 0 && rxEvent[95] == 0 && rxEvent[96] == 0){
                            cbus.sendOnEvent(true, logicEventNumber56);
                              sendEvent56 = 1; 
                 } 
              }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 7
/********************************************************************************************/

            if  (eventVariable2 == 57){
             if (invert != 57 && sendEvent57 == 1 && (rxEvent[97] == 1 || rxEvent[98] == 1 || rxEvent[99] == 1 || rxEvent[100] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber57);
                  sendEvent57 = 0;
                  }
                if (invert == 57 && sendEvent57 == 1 && (rxEvent[97] == 1 || rxEvent[98] == 1 || rxEvent[99] == 1 || rxEvent[100] == 1)){
                    cbus.sendOffEvent(true, logicEventNumber57);
                      sendEvent57 = 0;
                 }
                 
                  else if (invert != 57 && sendEvent57 == 0 && rxEvent[97] == 0 && rxEvent[98] == 0 && rxEvent[99] == 0 && rxEvent[100] == 0){
                            cbus.sendOffEvent(true, logicEventNumber57);
                              sendEvent57 = 1; 
                 } 
                    else if (invert == 57 && sendEvent57 == 0 && rxEvent[97] == 0 && rxEvent[98] == 0 && rxEvent[99] == 0 && rxEvent[100] == 0){
                            cbus.sendOnEvent(true, logicEventNumber57);
                              sendEvent57 = 1; 
                 } 
              }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 8
/********************************************************************************************/
           if  (eventVariable2 == 58){
             if (invert != 58 && sendEvent58 == 1 && (rxEvent[101] == 1 || rxEvent[102] == 1 || rxEvent[103] == 1 || rxEvent[104] == 1)){
                 cbus.sendOnEvent(true, logicEventNumber58);
                  sendEvent58 = 0;
                  }
                if (invert == 58 && sendEvent58 == 1 && (rxEvent[101] == 1 || rxEvent[102] == 1 || rxEvent[103] == 1 || rxEvent[104] == 1)){
                    cbus.sendOffEvent(true, logicEventNumber58);
                      sendEvent58 = 0;
                 }
                 
                  else if (invert != 58 && sendEvent58 == 0 && rxEvent[101] == 0 && rxEvent[102] == 0 && rxEvent[103] == 0 && rxEvent[104] == 0){
                            cbus.sendOffEvent(true, logicEventNumber58);
                              sendEvent58 = 1; 
                 } 
                    else if (invert == 58 && sendEvent58 == 0 && rxEvent[101] == 0 && rxEvent[102] == 0 && rxEvent[103] == 0 && rxEvent[104] == 0){
                            cbus.sendOnEvent(true, logicEventNumber58);
                              sendEvent58 = 1; 
                 } 
              }

/********************************************************************************************/












        } // End OF Recieved Events
          
    
           
  } // end function
                


/*FUNCTIONS FINISH ***********************************************************************************/



void loop() {

    cbus.run();// Run CBUS
    cbus.cbusRead(); // Check CBUS Buffers for any activity

}
