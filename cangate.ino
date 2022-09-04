
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
 //  event status flags
 /********************************************************************************************/
 /*
   bool rxEvent1 = 0;     
   bool rxEvent2 = 0;       
   bool rxEvent3 = 0;    
   bool rxEvent4 = 0;    
   bool rxEvent5 = 0;     
   bool rxEvent6 = 0;      
   bool rxEvent7 = 0;    
   bool rxEvent8 = 0;      
   bool rxEvent9 = 0;     
   bool rxEvent10 = 0;      
   bool rxEvent11 = 0;     
   bool rxEvent12 = 0;    
   bool rxEvent13 = 0;      
   bool rxEvent14 = 0;     
   bool rxEvent15 = 0;    
   bool rxEvent16 = 0;      
   bool rxEvent17 = 0;       
   bool rxEvent18 = 0;      
   bool rxEvent19 = 0;      
   bool rxEvent20 = 0;     
   bool rxEvent21 = 0;    
   bool rxEvent22 = 0;     
   bool rxEvent23 = 0;    
   bool rxEvent24 = 0;       
   bool rxEvent25 = 0;      
   bool rxEvent26 = 0;      
   bool rxEvent27 = 0;    
   bool rxEvent28 = 0;     
   bool rxEvent29 = 0;     
   bool rxEvent30 = 0;     
   bool rxEvent31 = 0;    
   bool rxEvent32 = 0;    
   bool rxEvent33 = 0;      
   bool rxEvent34 = 0;      
   bool rxEvent35 = 0;    
   bool rxEvent36 = 0;     
   bool rxEvent37 = 0;    
   bool rxEvent38 = 0;      
   bool rxEvent39 = 0;    
   bool rxEvent40 = 0;    
   bool rxEvent41 = 0;   
   bool rxEvent42 = 0;     
   bool rxEvent43 = 0;     
   bool rxEvent44 = 0;    
   bool rxEvent45 = 0;    
   bool rxEvent46 = 0;    
   bool rxEvent47 = 0;    
   bool rxEvent48 = 0;    
   bool rxEvent49 = 0;     
   bool rxEvent50 = 0;      
   bool rxEvent51 = 0;    
   bool rxEvent52 = 0;     
   bool rxEvent53 = 0;     
   bool rxEvent54 = 0;     
   bool rxEvent55 = 0;     
   bool rxEvent56 = 0;      
   bool rxEvent57 = 0;     
   bool rxEvent58 = 0;     
   bool rxEvent59 = 0;     
   bool rxEvent60 = 0;      
   bool rxEvent61 = 0;    
   bool rxEvent62 = 0;       
   bool rxEvent63 = 0;     
   bool rxEvent64 = 0; 
   bool rxEvent65 = 0;   
   bool rxEvent66 = 0;     
   bool rxEvent67 = 0;     
   bool rxEvent68 = 0;    
   bool rxEvent69 = 0;    
   bool rxEvent70 = 0;    
   bool rxEvent71 = 0;     
   bool rxEvent72 = 0;     
   bool rxEvent73 = 0;      
   bool rxEvent74 = 0;    
   bool rxEvent75 = 0;     
   bool rxEvent76 = 0;   
   bool rxEvent77 = 0;       
   bool rxEvent78 = 0;     
   bool rxEvent79 = 0;      
   bool rxEvent80 = 0;    
   bool rxEvent81 = 0;    
   bool rxEvent82 = 0;     
   bool rxEvent83 = 0;    
   bool rxEvent84 = 0;    
   bool rxEvent85 = 0;    
   bool rxEvent86 = 0;    
   bool rxEvent87 = 0;    
   bool rxEvent88 = 0;     
   bool rxEvent89 = 0;    
   bool rxEvent90 = 0;     
   bool rxEvent91 = 0;      
   bool rxEvent92 = 0;  
   bool rxEvent93 = 0;      
   bool rxEvent94 = 0;  
   bool rxEvent95 = 0;      
   bool rxEvent96 = 0;      
   bool rxEvent97 = 0;       
   bool rxEvent98 = 0;      
   bool rxEvent99 = 0;    
   bool rxEvent100 = 0;  
   bool rxEvent101 = 0; 
   bool rxEvent102 = 0;  
   bool rxEvent103 = 0;   
   bool rxEvent104 = 0;      
*/




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


} // End Of Set Up





/********************************************************************************************/
//  Functions
/********************************************************************************************/



void myUserFunc(Message *msg,MergCBUS *mcbus){
  
   byte CBUSOpc = msg->getOpc(); // Get The OPCODE from Message
   int nodeNumber = msg->getNodeNumber(); // Get The Node Number from Message
   int eventNumber = msg->getEventNumber(); // Get The Event Number from Message
   
       
     
   
   if (mcbus->eventMatch()){  //The recived event has been taught this module

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
         /*
         switch(eventVariable1) {  

               case 1: 
                     if (mcbus->isAccOn()== true){
                        rxEvent1 = 1; 
                        }
                     else if (mcbus->isAccOff()== true) {
                        rxEvent1 = 0;   
                       }
                       break;
               case 2: 
                     if (mcbus->isAccOn()== true){
                        rxEvent2 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent2 = 0;  
                       }
                       break;
               case 3: 
                     if (mcbus->isAccOn()== true){
                        rxEvent3 = 1;  
                        }
                     else if (mcbus->isAccOff()== true){
                         rxEvent3 = 0;  
                       }
                       break;
               case 4: 
                     if (mcbus->isAccOn()== true){
                        rxEvent4 = 1; 
                         }
                     else if (mcbus->isAccOff()== true){
                        rxEvent4 = 0;  
                       }
                       break;
               case 5:  
                     if (mcbus->isAccOn()== true){
                        rxEvent5 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent5 = 0; 
                       }
                       break;
               case 6: 
                     if (mcbus->isAccOn()== true){
                        rxEvent6 = 1;
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent6 = 0;   
                       }
                       break;
               case 7: 
                     if (mcbus->isAccOn()== true){
                        rxEvent7 = 1;
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent7 = 0;  
                       }
                       break;
               case 8: 
                     if (mcbus->isAccOn()== true){
                        rxEvent8 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent8 = 0;   
                       }
                       break; 
               case 9: 
                     if (mcbus->isAccOn()== true){
                        rxEvent9 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent9 = 0;   
                       }
                       break; 
              case 10: 
                     if (mcbus->isAccOn()== true){
                        rxEvent10 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent10 = 0;   
                       }
                       break;
              case 11: 
                     if (mcbus->isAccOn()== true){
                        rxEvent11 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent11 = 0;   
                       }
                       break;
              case 12: 
                     if (mcbus->isAccOn()== true){
                        rxEvent12 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent12 = 0;   
                       }
                       break;
              case 13: 
                     if (mcbus->isAccOn()== true){
                        rxEvent13 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent13 = 0;   
                       }
                       break;
              case 14: 
                     if (mcbus->isAccOn()== true){
                        rxEvent14 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent14 = 0;   
                       }
                       break;
              case 15: 
                     if (mcbus->isAccOn()== true){
                        rxEvent15 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent15 = 0;   
                       }
                       break;
              case 16: 
                     if (mcbus->isAccOn()== true){
                        rxEvent16 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent16 = 0;   
                       }
                       break;
              case 17: 
                     if (mcbus->isAccOn()== true){
                        rxEvent17 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent17 = 0;   
                       }
                       break;
              case 18:
                     if (mcbus->isAccOn()== true){
                        rxEvent18 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent18 = 0;   
                       }
                       break;
              case 19: 
                     if (mcbus->isAccOn()== true){
                        rxEvent19 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent19 = 0;   
                       }
                       break;
              case 20: 
                     if (mcbus->isAccOn()== true){
                        rxEvent20 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent20 = 0;   
                       }
                       break;
			  case 21: 
                     if (mcbus->isAccOn()== true){
                        rxEvent21 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent21 = 0;   
                       }
                       break;
			  case 22: 
                     if (mcbus->isAccOn()== true){
                        rxEvent22 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent22 = 0;   
                       }
                       break;
			  case 23: 
                     if (mcbus->isAccOn()== true){
                        rxEvent23 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent23 = 0;   
                       }
                       break;
			  case 24: 
                     if (mcbus->isAccOn()== true){
                        rxEvent24 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent24 = 0;   
                       }
                       break;
		      case 25: 
                     if (mcbus->isAccOn()== true){
                        rxEvent25 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent25 = 0;   
                       }
                       break;
			  case 26: 
                     if (mcbus->isAccOn()== true){
                        rxEvent26 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent26 = 0;   
                       }
                       break;
			  case 27: 
                     if (mcbus->isAccOn()== true){
                        rxEvent27 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent27 = 0;   
                       }
                       break;
			  case 28: 
                     if (mcbus->isAccOn()== true){
                        rxEvent28 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent28 = 0;   
                       }
                       break;
			  case 29: 
                     if (mcbus->isAccOn()== true){
                        rxEvent29 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent29 = 0;   
                       }
                       break;
			  case 30: 
                     if (mcbus->isAccOn()== true){
                        rxEvent30 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent30 = 0;   
                       }
                       break;
			  case 31: 
                     if (mcbus->isAccOn()== true){
                        rxEvent31 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent31 = 0;   
                       }
                       break;
			  case 32: 
                     if (mcbus->isAccOn()== true){
                        rxEvent32 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent32 = 0;   
                       }
                       break;
			  case 33: 
                     if (mcbus->isAccOn()== true){
                        rxEvent33 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent33 = 0;   
                       }
                       break;
			  case 34: 
                     if (mcbus->isAccOn()== true){
                        rxEvent34 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent34 = 0;   
                       }
                       break;
			  case 35: 
                     if (mcbus->isAccOn()== true){
                        rxEvent35 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent35 = 0;   
                       }
                       break;
		      case 36: 
                     if (mcbus->isAccOn()== true){
                        rxEvent36 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent36 = 0;   
                       }
                       break;
		      case 37: 
                     if (mcbus->isAccOn()== true){
                        rxEvent37 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent37 = 0;   
                       }
                       break;
			  case 38: 
                     if (mcbus->isAccOn()== true){
                        rxEvent38 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent38 = 0;   
                       }
                       break;
			  case 39: 
                     if (mcbus->isAccOn()== true){
                        rxEvent39 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent39 = 0;   
                       }
                       break;
			  case 40: 
                     if (mcbus->isAccOn()== true){
                        rxEvent40 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent40 = 0;   
                       }
                       break;
			  case 41: 
                     if (mcbus->isAccOn()== true){
                        rxEvent41 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent41 = 0;   
                       }
                       break;
			  case 42: 
                     if (mcbus->isAccOn()== true){
                        rxEvent42 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent42 = 0;   
                       }
                       break;
			  case 43: 
                     if (mcbus->isAccOn()== true){
                        rxEvent43 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent43 = 0;   
                       }
                       break;
			  case 44: 
                     if (mcbus->isAccOn()== true){
                        rxEvent44 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent44 = 0;   
                       }
                       break;
			  case 45: 
                     if (mcbus->isAccOn()== true){
                        rxEvent45 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent45 = 0;   
                       }
                       break;
			  case 46: 
                     if (mcbus->isAccOn()== true){
                        rxEvent46 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent46 = 0;   
                       }
                       break;
			  case 47: 
                     if (mcbus->isAccOn()== true){
                        rxEvent47 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent47 = 0;   
                       }
                       break;
			  case 48: 
                     if (mcbus->isAccOn()== true){
                        rxEvent48 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent48 = 0;   
                       }
                       break;
			  case 49: 
                     if (mcbus->isAccOn()== true){
                        rxEvent49 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent49 = 0;   
                       }
                       break;
			  case 50: 
                     if (mcbus->isAccOn()== true){
                        rxEvent50 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent50 = 0;   
                       }
                       break;
			  case 51: 
                     if (mcbus->isAccOn()== true){
                        rxEvent51 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent51 = 0;   
                       }
                       break;
			  case 52: 
                     if (mcbus->isAccOn()== true){
                        rxEvent52 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent52 = 0;   
                       }
                       break;
			  case 53: 
                     if (mcbus->isAccOn()== true){
                        rxEvent53 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent53 = 0;   
                       }
                       break;
			  case 54: 
                     if (mcbus->isAccOn()== true){
                        rxEvent54 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent54 = 0;   
                       }
                       break;
			  case 55: 
                     if (mcbus->isAccOn()== true){
                        rxEvent55 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent55 = 0;   
                       }
                       break;
			  case 56: 
                     if (mcbus->isAccOn()== true){
                        rxEvent56 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent56 = 0;   
                       }
                       break;
			  case 57: 
                     if (mcbus->isAccOn()== true){
                        rxEvent57 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent57 = 0;   
                       }
                       break;
			  case 58: 
                     if (mcbus->isAccOn()== true){
                        rxEvent58 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent58 = 0;   
                       }
                       break;
			  case 59: 
                     if (mcbus->isAccOn()== true){
                        rxEvent59 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent59 = 0;   
                       }
                       break;
			  case 60: 
                     if (mcbus->isAccOn()== true){
                        rxEvent60 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent60 = 0;   
                       }
                       break;
			  case 61: 
                     if (mcbus->isAccOn()== true){
                        rxEvent61 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent61 = 0;   
                       }
                       break;
			  case 62: 
                     if (mcbus->isAccOn()== true){
                        rxEvent62 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent62 = 0;   
                       }
                       break;
			  case 63: 
                     if (mcbus->isAccOn()== true){
                        rxEvent63 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent63 = 0;   
                       }
                       break;
			  case 64: 
                     if (mcbus->isAccOn()== true){
                        rxEvent64 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent64 = 0;   
                       }
                       break;
			  case 65: 
                     if (mcbus->isAccOn()== true){
                        rxEvent65 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent65 = 0;   
                       }
                       break;
			  case 66: 
                     if (mcbus->isAccOn()== true){
                        rxEvent66 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent66 = 0;   
                       }
                       break;
			  case 67: 
                     if (mcbus->isAccOn()== true){
                        rxEvent67 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent67 = 0;   
                       }
                       break;
			  case 68: 
                     if (mcbus->isAccOn()== true){
                        rxEvent68 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent68 = 0;   
                       }
                       break;
			  case 69: 
                     if (mcbus->isAccOn()== true){
                        rxEvent69 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent69 = 0;   
                       }
                       break;
			  case 70:
                     if (mcbus->isAccOn()== true){
                        rxEvent70 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent70 = 0;   
                       }
                       break;
			  case 71:
                     if (mcbus->isAccOn()== true){
                        rxEvent71 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent71 = 0;   
                       }
                       break;
			  case 72:
                     if (mcbus->isAccOn()== true){
                        rxEvent72 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent72 = 0;   
                       }
                       break;
			  case 73: 
                     if (mcbus->isAccOn()== true){
                        rxEvent73 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent73 = 0;   
                       }
                       break;
			  case 74: 
                     if (mcbus->isAccOn()== true){
                        rxEvent74 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent74 = 0;   
                       }
                       break;
           	  case 75: 
                     if (mcbus->isAccOn()== true){
                        rxEvent75 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent75 = 0;   
                       }
                       break;
			  case 76:
                     if (mcbus->isAccOn()== true){
                        rxEvent76 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent76 = 0;   
                       }
                       break;
			  case 77:
                     if (mcbus->isAccOn()== true){
                        rxEvent77 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent77 = 0;   
                       }
                       break;
			  case 78: 
                     if (mcbus->isAccOn()== true){
                        rxEvent78 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent78 = 0;   
                       }
                       break;
			  case 79: 
                     if (mcbus->isAccOn()== true){
                        rxEvent79 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent79 = 0;   
                       }
                       break;
			  case 80: 
                     if (mcbus->isAccOn()== true){
                        rxEvent80 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent80 = 0;   
                       }
                       break;
			  case 81: 
                     if (mcbus->isAccOn()== true){
                        rxEvent81 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent81 = 0;   
                       }
                       break;
			  case 82: 
                     if (mcbus->isAccOn()== true){
                        rxEvent82 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent82 = 0;   
                       }
                       break;
			  case 83: 
                     if (mcbus->isAccOn()== true){
                        rxEvent83 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent83 = 0;   
                       }
                       break;
			  case 84: 
                     if (mcbus->isAccOn()== true){
                        rxEvent84 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent84 = 0;   
                       }
                       break;
			  case 85: 
                     if (mcbus->isAccOn()== true){
                        rxEvent85 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent85 = 0;   
                       }
                       break;
			  case 86: 
                     if (mcbus->isAccOn()== true){
                        rxEvent86 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent86 = 0;   
                       }
                       break;
			  case 87: 
                     if (mcbus->isAccOn()== true){
                        rxEvent87 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent87 = 0;   
                       }
                       break;
			  case 88: 
                     if (mcbus->isAccOn()== true){
                        rxEvent88 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent88 = 0;   
                       }
                       break;
			  case 89: 
                     if (mcbus->isAccOn()== true){
                        rxEvent89 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent89 = 0;   
                       }
                       break;
			  case 90: 
                     if (mcbus->isAccOn()== true){
                        rxEvent90 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent90 = 0;   
                       }
                       break;
			  case 91:
                     if (mcbus->isAccOn()== true){
                        rxEvent91 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent91 = 0;   
                       }
                       break;
			  case 92: 
                     if (mcbus->isAccOn()== true){
                        rxEvent92 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent92 = 0;   
                       }
                       break;
			  case 93: 
                     if (mcbus->isAccOn()== true){
                        rxEvent93 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent93 = 0;   
                       }
                       break;
			  case 94:
                     if (mcbus->isAccOn()== true){
                        rxEvent94 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent94 = 0;   
                       }
                       break;
			  case 95: 
                     if (mcbus->isAccOn()== true){
                        rxEvent95 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent95 = 0;   
                       }
                       break;
			  case 96: 
                     if (mcbus->isAccOn()== true){
                        rxEvent96 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent96 = 0;   
                       }
                       break;
			  case 97: 
                     if (mcbus->isAccOn()== true){
                        rxEvent97 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent97 = 0;   
                       }
                       break;
			  case 98: 
                     if (mcbus->isAccOn()== true){
                        rxEvent98 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent98 = 0;   
                       }
                       break;
			  case 99: 
                     if (mcbus->isAccOn()== true){
                        rxEvent99 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent99 = 0;   
                       }
                       break;
			 case 100: 
                     if (mcbus->isAccOn()== true){
                        rxEvent100 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent100 = 0;   
                       }
                       break;
			 case 101: 
                     if (mcbus->isAccOn()== true){
                        rxEvent101 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent101 = 0;   
                       }
                       break;
			 case 102: 
                     if (mcbus->isAccOn()== true){
                        rxEvent102 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent102 = 0;   
                       }
                       break;
			 case 103: 
                     if (mcbus->isAccOn()== true){
                        rxEvent103 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent103 = 0;   
                       }
                       break;
		     case 104: 
                     if (mcbus->isAccOn()== true){
                        rxEvent104 = 1; 
                        }
                     else if (mcbus->isAccOff()== true){
                        rxEvent104 = 0;   
                       }
                       break;
					   
         }
         */      
   
    
/********************************************************************************************/         








    
/********************************************************************************************/
//  2 Input AND Gate 1
/********************************************************************************************/
        if (eventVariable2 == 21){
         if (invert != 21 && sendEvent21 == 1 && rxEvent[1] == 1 && rxEvent[2] == 1){    
              cbus.sendOnEvent(true, logicEventNumber21);
               sendEvent21 = 0;
                  }
              if (invert == 21 && sendEvent21 == 1 && rxEvent[1] == 1 && rxEvent[2] == 1){    
                 cbus.sendOffEvent(true, logicEventNumber21);
                  sendEvent21 = 0;
                  }
                  
           else if (invert != 21 && sendEvent21 == 0 && (rxEvent[1] == 0 || rxEvent[2] == 0)) {
                    cbus.sendOffEvent(true, logicEventNumber21);
                    sendEvent21 = 1; 
                    }
              else if (invert == 21 && sendEvent21 == 0 && (rxEvent[1] == 0 || rxEvent[2] == 0)) {
                      cbus.sendOnEvent(true, logicEventNumber21);
                      sendEvent21 = 1; 
                    }
            }
/********************************************************************************************/



/********************************************************************************************/
// 2 Input AND Gate 2
/********************************************************************************************/
        if (eventVariable2 == 22){
         if (invert != 22 && sendEvent22 == 1 && rxEvent[3] == 1 && rxEvent[4] == 1){    
              cbus.sendOnEvent(true, logicEventNumber22);
              sendEvent22 = 0;
                  }
                  if (invert == 22 && sendEvent22 == 1 && rxEvent[3] == 1 && rxEvent[4] == 1){    
                     cbus.sendOffEvent(true, logicEventNumber21);
                     sendEvent22 = 0;
                  }
                  
               else if (invert != 22 && sendEvent22 == 0 && (rxEvent[3] == 0 || rxEvent[4] == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber22);
                        sendEvent22 = 1; 
                   } 
                     else if (invert == 22 && sendEvent22 == 0 && (rxEvent[3] == 0 || rxEvent[4] == 0)) {
                              cbus.sendOnEvent(true, logicEventNumber22);
                              sendEvent22 = 1; 
                      } 
             }
/********************************************************************************************/



/********************************************************************************************/
// 2 Input AND Gate 3
/********************************************************************************************/
        if (eventVariable2 == 23){
         if (invert != 23 && sendEvent23 == 1 && rxEvent[5] == 1 && rxEvent[6] == 1){    
              cbus.sendOnEvent(true, logicEventNumber23);
               sendEvent23 = 0;
                  }
                  if  (invert == 23 && sendEvent23 == 1 && rxEvent[5] == 1 && rxEvent[6] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber23);
                      sendEvent23 = 0;
                  }
                 
               else if (invert != 23 && sendEvent23 == 0 && (rxEvent[5] == 0 || rxEvent[6] == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber23);
                        sendEvent23 = 1; 
                   } 
                   else if (invert == 23 && sendEvent23 == 0 && (rxEvent[5] == 0 || rxEvent[6] == 0)) {
                          cbus.sendOnEvent(true, logicEventNumber23);
                          sendEvent23 = 1; 
                   } 
            }
/********************************************************************************************/



/********************************************************************************************/
// 2 Input AND Gate 4
/********************************************************************************************/
        if (eventVariable2 == 24){
         if (invert != 24 && sendEvent24 == 1 && rxEvent[7] == 1 && rxEvent[8] == 1){    
              cbus.sendOnEvent(true, logicEventNumber24);
               sendEvent24 = 0;
                  }
                  if (invert == 24 && sendEvent24 == 1 && rxEvent[7] == 1 && rxEvent[8] == 1){    
                      cbus.sendOffEvent(true, logicEventNumber24);
                      sendEvent24 = 0;
                  }

               else if (invert != 24 && sendEvent24 == 0 && (rxEvent[7] == 0 || rxEvent[8] == 0)){
                        cbus.sendOffEvent(true, logicEventNumber24);
                        sendEvent24 = 1; 
                    } 
                   else if (invert == 24 && sendEvent24 == 0 && (rxEvent[7] == 0 || rxEvent[8] == 0)){
                            cbus.sendOnEvent(true, logicEventNumber24);
                            sendEvent24 = 1; 
                    }
           }
/********************************************************************************************/


/********************************************************************************************/
// 2 Input AND Gate 5
/********************************************************************************************/
        if (eventVariable2 == 25){
         if (invert != 25 && sendEvent25 == 1 && rxEvent9 == 1 && rxEvent10 == 1){    
              cbus.sendOnEvent(true, logicEventNumber25);
               sendEvent25 = 0;
                  }
                   if  (invert == 25 && sendEvent25 == 1 && rxEvent9 == 1 && rxEvent10 == 1){    
                        cbus.sendOffEvent(true, logicEventNumber25);
                        sendEvent25 = 0;
                  }

               else if (invert != 25 && sendEvent25 == 0 && (rxEvent9 == 0 || rxEvent10 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber25);
                        sendEvent25 = 1; 
                   }
                   else if (invert == 25 && sendEvent25 == 0 && (rxEvent9 == 0 || rxEvent10 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber25);
                          sendEvent25 = 1; 
                   }
             }
/********************************************************************************************/


/********************************************************************************************/
// 2 Input AND Gate 6
/********************************************************************************************/
        if (eventVariable2 == 26){
         if  (invert != 26 && sendEvent26 == 1 && rxEvent11 == 1 && rxEvent12 == 1){    
              cbus.sendOnEvent(true, logicEventNumber26);
               sendEvent26 = 0;
                  }
                  if  (invert == 26 && sendEvent26 == 1 && rxEvent11 == 1 && rxEvent12 == 1){    
                       cbus.sendOffEvent(true, logicEventNumber26);
                        sendEvent26 = 0;
                  }
                  
               else if (invert != 26 && sendEvent26 == 0 && (rxEvent11 == 0 || rxEvent12 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber26);
                        sendEvent26 = 1; 
                   }
                   else if (invert == 26 && sendEvent26 == 0 && (rxEvent11 == 0 || rxEvent12 == 0)){
                            cbus.sendOnEvent(true, logicEventNumber26);
                            sendEvent26 = 1; 
                    }
            }
/********************************************************************************************/



/********************************************************************************************/
// 2 Input AND Gate 7
/********************************************************************************************/
        if (eventVariable2 == 27){
         if (invert != 27 && sendEvent27 == 1 && rxEvent13 == 1 && rxEvent14 == 1){    
              cbus.sendOnEvent(true, logicEventNumber27);
               sendEvent27 = 0;
                  }
                if (invert == 27 && sendEvent27 == 1 && rxEvent13 == 1 && rxEvent14 == 1){    
                    cbus.sendOffEvent(true, logicEventNumber27);
                    sendEvent27 = 0;
                  }
                  
               else if (invert != 27 && sendEvent27 == 0 && (rxEvent13 == 0 || rxEvent14 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber27);
                        sendEvent27 = 1; 
                    }
                   else if (invert == 27 && sendEvent27 == 0 && (rxEvent13 == 0 || rxEvent14 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber27);
                          sendEvent27 = 1; 
                     }  
          }
/********************************************************************************************/



/********************************************************************************************/
// 2 Input AND Gate 8
/********************************************************************************************/
        if (eventVariable2 == 28){
         if (invert != 28 && sendEvent28 == 1 && rxEvent15 == 1 && rxEvent16 == 1){    
              cbus.sendOnEvent(true, logicEventNumber28);
               sendEvent28 = 0;
                  }
                   if (invert == 28 && sendEvent28 == 1 && rxEvent15 == 1 && rxEvent16 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber28);
                      sendEvent28 = 0;
                  }
                  
               else if (invert != 28 && sendEvent28 == 0  && (rxEvent15 == 0 || rxEvent16 == 0)){
                       cbus.sendOffEvent(true, logicEventNumber28);
                        sendEvent28 = 1; 
                    } 
                   else if (invert == 28 && sendEvent28 == 0  && (rxEvent15 == 0 || rxEvent16 == 0)){
                            cbus.sendOnEvent(true, logicEventNumber28);
                            sendEvent28 = 1; 
                     } 
          }
/********************************************************************************************/









/********************************************************************************************/
// 3 Input AND Gate 1
/********************************************************************************************/

          if  (eventVariable2 == 31){
              if (invert != 31 && sendEvent31 == 1 && rxEvent17 == 1 && rxEvent18 == 1 && rxEvent19 == 1){    
                    cbus.sendOnEvent(true, logicEventNumber31);
                    sendEvent31 = 0;
                  }
                   if (invert == 31 && sendEvent31 == 1 && rxEvent17 == 1 && rxEvent18 == 1 && rxEvent19 == 1){    
                        cbus.sendOffEvent(true, logicEventNumber31);
                        sendEvent31 = 0;
                  } 
                    
               else if (invert != 31 && sendEvent31 == 0 && (rxEvent17 == 0 || rxEvent18 == 0 || rxEvent19 == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber31);
                        sendEvent31 = 1; 
                  }
                 else if (invert == 31 && sendEvent31 == 0 && (rxEvent17 == 0 || rxEvent18 == 0 || rxEvent19 == 0)) {
                          cbus.sendOnEvent(true, logicEventNumber31);
                          sendEvent31 = 1; 
                  }     
            }

/********************************************************************************************/


/********************************************************************************************/
// 3 Input AND Gate 2
/********************************************************************************************/

          if  (eventVariable2 == 32){
             if (invert != 32 && sendEvent32 == 1 && rxEvent20 == 1 && rxEvent21 == 1 && rxEvent22 == 1){    
                cbus.sendOnEvent(true, logicEventNumber32);
                sendEvent32 = 0;
                  }
                  if (invert == 32 && sendEvent32 == 1 && rxEvent20 == 1 && rxEvent21 == 1 && rxEvent22 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber32);
                      sendEvent32 = 0;
                  }
                  
               else if (invert != 32 && sendEvent32 == 0 && (rxEvent20 == 0 || rxEvent21 == 0 || rxEvent22 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber32);
                        sendEvent32 = 1; 
                  } 
                 else if (invert == 32 && sendEvent32 == 0 && (rxEvent20 == 0 || rxEvent21 == 0 || rxEvent22 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber32);
                          sendEvent32 = 1; 
                  } 
             }

/********************************************************************************************/


/********************************************************************************************/
// 3 Input AND Gate 3
/********************************************************************************************/

          if  (eventVariable2 == 33){
            if (invert != 33 && sendEvent33 == 1 && rxEvent23 == 1 && rxEvent24 == 1 && rxEvent25 == 1){    
                cbus.sendOnEvent(true, logicEventNumber33);
                sendEvent33 = 0;
                  }
                  if (invert == 33 && sendEvent33 == 1 && rxEvent23 == 1 && rxEvent24 == 1 && rxEvent25 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber33);
                      sendEvent33 = 0;
                  }
                  
               else if (invert != 33 && sendEvent33 == 0 && (rxEvent23 == 0 || rxEvent24 == 0 || rxEvent25 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber33);
                        sendEvent33 = 1; 
                  } 
                 else if (invert == 33 && sendEvent33 == 0 && (rxEvent23 == 0 || rxEvent24 == 0 || rxEvent25 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber33);
                          sendEvent33 = 1; 
                  } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 3 Input AND Gate 4
/********************************************************************************************/

          if  (eventVariable2 == 34){
            if (invert != 34 && sendEvent34 == 1 && rxEvent26 == 1 && rxEvent27 == 1 && rxEvent28 == 1){    
                cbus.sendOnEvent(true, logicEventNumber34);
                sendEvent34 = 0;
                  }
                  if (invert == 34 && sendEvent34 == 1 && rxEvent26 == 1 && rxEvent27 == 1 && rxEvent28 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber34);
                      sendEvent34 = 0;
                  }
                  
               else if (invert != 34 && sendEvent34 == 0 && (rxEvent26 == 0 || rxEvent27 == 0 || rxEvent28 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber34);
                        sendEvent34 = 1; 
                  }
                 else if (invert == 34 && sendEvent34 == 0 && (rxEvent26 == 0 || rxEvent27 == 0 || rxEvent28 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber34);
                          sendEvent34 = 1; 
                  }  
           }

/********************************************************************************************/



/********************************************************************************************/
// 3 Input AND Gate 5
/********************************************************************************************/

          if  (eventVariable2 == 35){
            if (invert != 35 && sendEvent35 == 1 && rxEvent29 == 1 && rxEvent30 == 1 && rxEvent31 == 1){    
                cbus.sendOnEvent(true, logicEventNumber35);
                sendEvent35 = 0;
                  }
                  if (invert == 35 && sendEvent35 == 1 && rxEvent29 == 1 && rxEvent30 == 1 && rxEvent31 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber35);
                      sendEvent35 = 0;
                  }
                  
               else if (invert != 35 && sendEvent35 == 0 && (rxEvent29 == 0 || rxEvent30 == 0 || rxEvent31 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber35);
                        sendEvent35 = 1; 
                  } 
                 else if (invert == 35 && sendEvent35 == 0 && (rxEvent29 == 0 || rxEvent30 == 0 || rxEvent31 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber35);
                          sendEvent35 = 1; 
                  }
             }

/********************************************************************************************/


/********************************************************************************************/
// 3 Input AND Gate 6
/********************************************************************************************/

          if  (eventVariable2 == 36){
            if (invert != 36 && sendEvent36 == 1 && rxEvent32 == 1 && rxEvent33 == 1 && rxEvent34 == 1){    
                cbus.sendOnEvent(true, logicEventNumber36);
                sendEvent36 = 0;
                  }
                  if (invert == 36 && sendEvent36 == 1 && rxEvent32 == 1 && rxEvent33 == 1 && rxEvent34 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber36);
                      sendEvent36 = 0;
                  }
               else if (invert != 36 && sendEvent36 == 0 && (rxEvent32 == 0 || rxEvent33 == 0 || rxEvent34 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber36);
                        sendEvent36 = 1; 
                  } 
                 else if (invert == 36 && sendEvent36 == 0 && (rxEvent32 == 0 || rxEvent33 == 0 || rxEvent34 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber36);
                          sendEvent36 = 1; 
                  } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 3 Input AND Gate 7
/********************************************************************************************/

          if (eventVariable2 == 37){
            if (invert != 37 && sendEvent37 == 1 && rxEvent35 == 1 && rxEvent36 == 1 && rxEvent37 == 1){    
              cbus.sendOnEvent(true, logicEventNumber37);
               sendEvent37 = 0;
                  }
                  if (invert == 37 && sendEvent37 == 1 && rxEvent35 == 1 && rxEvent36 == 1 && rxEvent37 == 1){    
              cbus.sendOffEvent(true, logicEventNumber37);
               sendEvent37 = 0;
                  }
                  
               else if (invert != 37 && sendEvent37 == 0 && (rxEvent35 == 0 || rxEvent36 == 0 || rxEvent37 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber37);
                        sendEvent37 = 1; 
                  } 
                  else if (invert == 37 && sendEvent37 == 0 && (rxEvent35 == 0 || rxEvent36 == 0 || rxEvent37 == 0)){
                        cbus.sendOnEvent(true, logicEventNumber37);
                        sendEvent37 = 1; 
                  }
             }

/********************************************************************************************/



/********************************************************************************************/
// 3 Input AND Gate 8
/********************************************************************************************/

          if  (eventVariable2 == 38){
            if (invert != 38 && sendEvent38 == 1 && rxEvent38 == 1 && rxEvent39 == 1 && rxEvent40 == 1){    
                cbus.sendOnEvent(true, logicEventNumber38);
                sendEvent38 = 0;
                  }
                  if (invert == 38 && sendEvent38 == 1 && rxEvent38 == 1 && rxEvent39 == 1 && rxEvent40 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber38);
                      sendEvent38 = 0;
                  }
                  
             else if (invert != 38 && sendEvent38 == 0 && (rxEvent38 == 0 || rxEvent39 == 0 || rxEvent40 == 0)){
                      cbus.sendOffEvent(true, logicEventNumber38);
                      sendEvent38 = 1; 
                  } 
                else if (invert == 38 && sendEvent38 == 0 && (rxEvent38 == 0 || rxEvent39 == 0 || rxEvent40 == 0)){
                         cbus.sendOnEvent(true, logicEventNumber38);
                         sendEvent38 = 1; 
                  } 
             }

/********************************************************************************************/








/********************************************************************************************/
// 4 Input AND Gate 1
/********************************************************************************************/

          if (eventVariable2 == 41){
            if (invert != 41 && sendEvent41 == 1 && rxEvent41 == 1 && rxEvent42 == 1 && rxEvent43 == 1 && rxEvent44 == 1){    
              cbus.sendOnEvent(true, logicEventNumber41);
               sendEvent41 = 0;
                  }
                  if (invert == 41 && sendEvent41 == 1 && rxEvent41 == 1 && rxEvent42 == 1 && rxEvent43 == 1 && rxEvent44 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber41);
                      sendEvent41 = 0;
                  }
                  
               else if (invert != 41 && sendEvent41 == 0 && (rxEvent41 == 0 || rxEvent42 == 0 || rxEvent43 == 0 || rxEvent44 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber41);
                        sendEvent41 = 1; 
                 }
                 else if (invert == 41 && sendEvent41 == 0 && (rxEvent41 == 0 || rxEvent42 == 0 || rxEvent43 == 0 || rxEvent44 == 0)){
                        cbus.sendOnEvent(true, logicEventNumber41);
                        sendEvent41 = 1; 
                 }  
                 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 2
/********************************************************************************************/

          if (eventVariable2 == 42){
            if (invert != 42 && sendEvent42 == 1 && rxEvent45 == 1 && rxEvent46 == 1 && rxEvent47 == 1 && rxEvent48 == 1){    
                cbus.sendOnEvent(true, logicEventNumber42);
                sendEvent42 = 0;
                  }
                  if (invert == 42 && sendEvent42 == 1 && rxEvent45 == 1 && rxEvent46 == 1 && rxEvent47 == 1 && rxEvent48 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber42);
                      sendEvent42 = 0;
                  }
                  
               else if (invert != 42 && sendEvent42 == 0 && (rxEvent45 == 0 || rxEvent46 == 0 || rxEvent47 == 0 || rxEvent48 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber42);
                        sendEvent42 = 1;
                  }
                  else if (invert == 42 && sendEvent42 == 0 && (rxEvent45 == 0 || rxEvent46 == 0 || rxEvent47 == 0 || rxEvent48 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber42);
                          sendEvent42 = 1;
                  }
             }

/********************************************************************************************/





/********************************************************************************************/
// 4 Input AND Gate 3
/********************************************************************************************/

           if (eventVariable2 == 43){
            if (invert != 43 && sendEvent43 == 1 && rxEvent49 == 1 && rxEvent50 == 1 && rxEvent51 == 1 && rxEvent52 == 1){    
              cbus.sendOnEvent(true, logicEventNumber43);
               sendEvent43 = 0;
                  }
                   if (invert == 43 && sendEvent43 == 1 && rxEvent49 == 1 && rxEvent50 == 1 && rxEvent51 == 1 && rxEvent52 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber43);
                      sendEvent43 = 0;
                  }
                  
               else if (invert != 43 && sendEvent43 == 0 && (rxEvent49 == 0 || rxEvent50 == 0 || rxEvent51 == 0 || rxEvent52 == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber43);
                        sendEvent43= 1; 
                  }
                  else if (invert == 43 && sendEvent43 == 0 && (rxEvent49 == 0 || rxEvent50 == 0 || rxEvent51 == 0 || rxEvent52 == 0)) {
                           cbus.sendOnEvent(true, logicEventNumber43);
                            sendEvent43= 1; 
                  } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 4
/********************************************************************************************/

          if (eventVariable2 == 44){
            if (invert != 44 && sendEvent44 == 1 && rxEvent53 == 1 && rxEvent54 == 1 && rxEvent55 == 1 && rxEvent56 == 1){    
                cbus.sendOnEvent(true, logicEventNumber44);
                sendEvent44 = 0;
                  }
                  if (invert == 44 && sendEvent44 == 1 && rxEvent53 == 1 && rxEvent54 == 1 && rxEvent55 == 1 && rxEvent56 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber44);
                       sendEvent44 = 0;
                  }
                     
               else if (invert != 44 && sendEvent44 == 0 && (rxEvent53 == 0 || rxEvent54 == 0 || rxEvent55 == 0 || rxEvent56 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber44);
                        sendEvent44= 1; 
                  }
                  else if (invert == 44 && sendEvent44 == 0 && (rxEvent53 == 0 || rxEvent54 == 0 || rxEvent55 == 0 || rxEvent56 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber44);
                          sendEvent44= 1; 
                  } 
             }

/********************************************************************************************/




/********************************************************************************************/
// 4 Input AND Gate 5
/********************************************************************************************/

          if (eventVariable2 == 45){
            if (invert != 45 && sendEvent45 == 1 && rxEvent57 == 1 && rxEvent58 == 1 && rxEvent59 == 1 && rxEvent60 == 1){    
                cbus.sendOnEvent(true, logicEventNumber45);
                sendEvent45 = 0;
                  }
                   if (invert == 45 && sendEvent45 == 1 && rxEvent57 == 1 && rxEvent58 == 1 && rxEvent59 == 1 && rxEvent60 == 1){    
                        cbus.sendOffEvent(true, logicEventNumber45);
                        sendEvent45 = 0;
                  }
                  
               else if (invert != 45 && sendEvent45 == 0 && (rxEvent57 == 0 || rxEvent58 == 0 || rxEvent59 == 0 || rxEvent60 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber45);
                        sendEvent45= 1; 
                  }
                  else if (invert == 45 && sendEvent45 == 0 && (rxEvent57 == 0 || rxEvent58 == 0 || rxEvent59 == 0 || rxEvent60 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber45);
                          sendEvent45= 1; 
                  } 
             }

/********************************************************************************************/




/********************************************************************************************/
// 4 Input AND Gate 6
/********************************************************************************************/

          if (eventVariable2 == 46){
            if (invert != 46 && sendEvent46 == 1 && rxEvent61 == 1 && rxEvent62 == 1 && rxEvent63 == 1 && rxEvent64 == 1){    
                cbus.sendOnEvent(true, logicEventNumber46);
                sendEvent46 = 0;
                  }
                  if (invert == 46 && sendEvent46 == 1 && rxEvent61 == 1 && rxEvent62 == 1 && rxEvent63 == 1 && rxEvent64 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber46);
                      sendEvent46 = 0;
                  }
                  
               else if (invert != 46 && sendEvent46 == 0 && (rxEvent61 == 0 || rxEvent62 == 0 || rxEvent63 == 0 || rxEvent64 == 0)) {
                        cbus.sendOffEvent(true, logicEventNumber46);
                        sendEvent46= 1; 
                  }
                  else if (invert == 46 && sendEvent46 == 0 && (rxEvent61 == 0 || rxEvent62 == 0 || rxEvent63 == 0 || rxEvent64 == 0)) {
                          cbus.sendOnEvent(true, logicEventNumber46);
                          sendEvent46= 1; 
                  } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 7
/********************************************************************************************/

           if (eventVariable2 == 47){
            if (invert != 47 && sendEvent47 == 1 && rxEvent65 == 1 && rxEvent66 == 1 && rxEvent67 == 1 && rxEvent68 == 1 ){    
                cbus.sendOnEvent(true, logicEventNumber47);
                sendEvent47 = 0;
                  }
                  if (invert == 47 && sendEvent47 == 1 && rxEvent65 == 1 && rxEvent66 == 1 && rxEvent67 == 1 && rxEvent68 == 1 ){    
                      cbus.sendOffEvent(true, logicEventNumber47);
                      sendEvent47 = 0;
                  }
                  
               else if (invert != 47 && sendEvent47 == 0 && (rxEvent65 == 0 || rxEvent66 == 0 || rxEvent67 == 0 || rxEvent68 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber47);
                        sendEvent47= 1; 
                 }
                 else if (invert == 47 && sendEvent47 == 0 && (rxEvent65 == 0 || rxEvent66 == 0 || rxEvent67 == 0 || rxEvent68 == 0)){
                          cbus.sendOnEvent(true, logicEventNumber47);
                          sendEvent47= 1; 
                 } 
             }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input AND Gate 8
/********************************************************************************************/

          if (eventVariable2 == 48){
           if (invert != 48 && sendEvent48 == 1 && rxEvent69 == 1 && rxEvent70 == 1 && rxEvent71 == 1 && rxEvent72 == 1){    
              cbus.sendOnEvent(true, logicEventNumber48);
               sendEvent48 = 0;
                  }
                  if (invert == 48 && sendEvent48 == 1 && rxEvent69 == 1 && rxEvent70 == 1 && rxEvent71 == 1 && rxEvent72 == 1){    
                      cbus.sendOffEvent(true, logicEventNumber48);
                      sendEvent48 = 0;
                  }
                  
               else if (invert !=48 && sendEvent48 == 0 && (rxEvent69 == 0 || rxEvent70 == 0 || rxEvent71 == 0 || rxEvent72 == 0)){
                        cbus.sendOffEvent(true, logicEventNumber48);
                        sendEvent48= 1; 
                  }
                  else if (invert ==48 && sendEvent48 == 0 && (rxEvent69 == 0 || rxEvent70 == 0 || rxEvent71 == 0 || rxEvent72 == 0)){
                        cbus.sendOnEvent(true, logicEventNumber48);
                        sendEvent48= 1; 
                  }  
             }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 1
/********************************************************************************************/

          if  (eventVariable2 == 51){
             if (invert != 51 && sendEvent51 == 1 && (rxEvent73 == 1 || rxEvent74 == 1 || rxEvent75 == 1 || rxEvent76 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber51);
                  sendEvent51 = 0;
                  }
                if (invert == 51 && sendEvent51 == 1 && (rxEvent73 == 1 || rxEvent74 == 1 || rxEvent75 == 1 || rxEvent76 == 1)){
                 cbus.sendOffEvent(true, logicEventNumber51);
                  sendEvent51 = 0;
                 }
                 
                  else if (invert != 51 && sendEvent51 == 0 && rxEvent73 == 0 && rxEvent74 == 0 && rxEvent75 == 0 && rxEvent76 == 0){
                            cbus.sendOffEvent(true, logicEventNumber51);
                              sendEvent51 = 1; 
                 } 
                 else if (invert == 51 && sendEvent51 == 0 && rxEvent73 == 0 && rxEvent74 == 0 && rxEvent75 == 0 && rxEvent76 == 0){
                            cbus.sendOnEvent(true, logicEventNumber51);
                              sendEvent51 = 1; 
                 } 
              }
      
/********************************************************************************************/



/********************************************************************************************/
// 4 Input OR Gate 2
/********************************************************************************************/
          if  (eventVariable2 == 52){
             if (invert != 52 && sendEvent52 == 1 && (rxEvent77 == 1 || rxEvent78 == 1 || rxEvent79 == 1 || rxEvent80 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber52);
                  sendEvent52 = 0;
                  }
                if (invert == 52 && sendEvent52 == 1 && (rxEvent77 == 1 || rxEvent78 == 1 || rxEvent79 == 1 || rxEvent80 == 1)){
                 cbus.sendOffEvent(true, logicEventNumber52);
                  sendEvent52 = 0;
                 }
                 
                  else if (invert != 52 && sendEvent52 == 0 && rxEvent77 == 0 && rxEvent78 == 0 && rxEvent79 == 0 && rxEvent80 == 0){
                            cbus.sendOffEvent(true, logicEventNumber52);
                              sendEvent51 = 1; 
                 } 
                 else if (invert == 52 && sendEvent52 == 0 && rxEvent77 == 0 && rxEvent78 == 0 && rxEvent79 == 0 && rxEvent80 == 0){
                            cbus.sendOnEvent(true, logicEventNumber52);
                              sendEvent52 = 1; 
                 } 
              }
/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 3
/********************************************************************************************/

           if  (eventVariable2 == 53){
             if (invert != 53 && sendEvent53 == 1 && (rxEvent81 == 1 || rxEvent82 == 1 || rxEvent83 == 1 || rxEvent84 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber53);
                  sendEvent53 = 0;
                  }
                if (invert == 53 && sendEvent53 == 1 && (rxEvent81 == 1 || rxEvent82 == 1 || rxEvent83 == 1 || rxEvent84 == 1)){
                    cbus.sendOffEvent(true, logicEventNumber53);
                      sendEvent53 = 0;
                 }
                 
                  else if (invert != 53 && sendEvent53 == 0 && rxEvent81 == 0 && rxEvent82 == 0 && rxEvent83 == 0 && rxEvent84 == 0){
                            cbus.sendOffEvent(true, logicEventNumber53);
                              sendEvent53 = 1; 
                 } 
                    else if (invert == 53 && sendEvent53 == 0 && rxEvent81 == 0 && rxEvent82 == 0 && rxEvent83 == 0 && rxEvent84 == 0){
                            cbus.sendOnEvent(true, logicEventNumber53);
                              sendEvent53 = 1; 
                 } 
              }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 4
/********************************************************************************************/

   if  (eventVariable2 == 54){
             if (invert != 54 && sendEvent54 == 1 && (rxEvent85 == 1 || rxEvent86 == 1 || rxEvent87 == 1 || rxEvent88 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber54);
                  sendEvent54 = 0;
                  }
                if (invert == 54 && sendEvent54 == 1 && (rxEvent85 == 1 || rxEvent86 == 1 || rxEvent87 == 1 || rxEvent88 == 1)){
                    cbus.sendOffEvent(true, logicEventNumber54);
                      sendEvent54 = 0;
                 }
                 
                  else if (invert != 54 && sendEvent54 == 0 && rxEvent85 == 0 && rxEvent86 == 0 && rxEvent87 == 0 && rxEvent88 == 0){
                            cbus.sendOffEvent(true, logicEventNumber54);
                              sendEvent54 = 1; 
                 } 
                    else if (invert == 54 && sendEvent54 == 0 && rxEvent85 == 0 && rxEvent86 == 0 && rxEvent87 == 0 && rxEvent88 == 0){
                            cbus.sendOnEvent(true, logicEventNumber54);
                              sendEvent54 = 1; 
                 } 
              }

/********************************************************************************************/



/********************************************************************************************/
// 4 Input OR Gate 5
/********************************************************************************************/
          if  (eventVariable2 == 55){
             if (invert != 55 && sendEvent55 == 1 && (rxEvent89 == 1 || rxEvent90 == 1 || rxEvent91 == 1 || rxEvent92 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber55);
                  sendEvent55 = 0;
                  }
                if (invert == 55 && sendEvent55 == 1 && (rxEvent89 == 1 || rxEvent90 == 1 || rxEvent91 == 1 || rxEvent92 == 1)){
                    cbus.sendOffEvent(true, logicEventNumber55);
                      sendEvent55 = 0;
                 }
                 
                  else if (invert != 55 && sendEvent55 == 0 && rxEvent89 == 0 && rxEvent90 == 0 && rxEvent91 == 0 && rxEvent92 == 0){
                            cbus.sendOffEvent(true, logicEventNumber55);
                              sendEvent55 = 1; 
                 } 
                    else if (invert == 55 && sendEvent55 == 0 && rxEvent89 == 0 && rxEvent90 == 0 && rxEvent91 == 0 && rxEvent92 == 0){
                            cbus.sendOnEvent(true, logicEventNumber55);
                              sendEvent55 = 1; 
                 } 
              }
/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 6
/********************************************************************************************/
           if  (eventVariable2 == 56){
             if (invert != 56 && sendEvent56 == 1 && (rxEvent93 == 1 || rxEvent94 == 1 || rxEvent95 == 1 || rxEvent96 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber56);
                  sendEvent56 = 0;
                  }
                if (invert == 56 && sendEvent56 == 1 && (rxEvent93 == 1 || rxEvent94 == 1 || rxEvent95 == 1 || rxEvent96 == 1)){
                    cbus.sendOffEvent(true, logicEventNumber56);
                      sendEvent56 = 0;
                 }
                 
                  else if (invert != 56 && sendEvent56 == 0 && rxEvent93 == 0 && rxEvent94 == 0 && rxEvent95 == 0 && rxEvent96 == 0){
                            cbus.sendOffEvent(true, logicEventNumber56);
                              sendEvent56 = 1; 
                 } 
                    else if (invert == 56 && sendEvent56 == 0 && rxEvent93 == 0 && rxEvent94 == 0 && rxEvent95 == 0 && rxEvent96 == 0){
                            cbus.sendOnEvent(true, logicEventNumber56);
                              sendEvent56 = 1; 
                 } 
              }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 7
/********************************************************************************************/

            if  (eventVariable2 == 57){
             if (invert != 57 && sendEvent57 == 1 && (rxEvent97 == 1 || rxEvent98 == 1 || rxEvent99 == 1 || rxEvent100 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber57);
                  sendEvent57 = 0;
                  }
                if (invert == 57 && sendEvent57 == 1 && (rxEvent97 == 1 || rxEvent98 == 1 || rxEvent99 == 1 || rxEvent100 == 1)){
                    cbus.sendOffEvent(true, logicEventNumber57);
                      sendEvent57 = 0;
                 }
                 
                  else if (invert != 57 && sendEvent57 == 0 && rxEvent97 == 0 && rxEvent98 == 0 && rxEvent99 == 0 && rxEvent100 == 0){
                            cbus.sendOffEvent(true, logicEventNumber57);
                              sendEvent57 = 1; 
                 } 
                    else if (invert == 57 && sendEvent57 == 0 && rxEvent97 == 0 && rxEvent98 == 0 && rxEvent99 == 0 && rxEvent100 == 0){
                            cbus.sendOnEvent(true, logicEventNumber57);
                              sendEvent57 = 1; 
                 } 
              }

/********************************************************************************************/


/********************************************************************************************/
// 4 Input OR Gate 8
/********************************************************************************************/
           if  (eventVariable2 == 58){
             if (invert != 58 && sendEvent58 == 1 && (rxEvent101 == 1 || rxEvent102 == 1 || rxEvent102 == 1 || rxEvent104 == 1)){
                 cbus.sendOnEvent(true, logicEventNumber58);
                  sendEvent58 = 0;
                  }
                if (invert == 58 && sendEvent58 == 1 && (rxEvent101 == 1 || rxEvent102 == 1 || rxEvent103 == 1 || rxEvent104 == 1)){
                    cbus.sendOffEvent(true, logicEventNumber58);
                      sendEvent58 = 0;
                 }
                 
                  else if (invert != 58 && sendEvent58 == 0 && rxEvent101 == 0 && rxEvent102 == 0 && rxEvent103 == 0 && rxEvent104 == 0){
                            cbus.sendOffEvent(true, logicEventNumber58);
                              sendEvent58 = 1; 
                 } 
                    else if (invert == 58 && sendEvent58 == 0 && rxEvent101 == 0 && rxEvent102 == 0 && rxEvent102 == 0 && rxEvent104 == 0){
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
