/**
 * This is an example of joining, sending and receiving data via LoRaWAN using a more minimal interface.
 * The example is configured for OTAA, set your keys into the variables below.
 * The example will upload a counter value periodically, and will print any downlink messages.
 * please disable AT_SUPPORT in tools menu
 * David Brodrick.
 *
 * This program was initialy developed by CubeCell. 
 * Additional Comments by F.Farahmand 
 *   This code works with Arduino IDE:
 *	In the "Preferences" of the IDE add the following: 
 *		https://github.com/HelTecAutomation/CubeCell-Arduino/releases/download/V1.5.0/package_CubeCell_index.json
 *      The random data is defined in the main loop() and is sent using LoRaWAN.send() function.
 *   For TTN setup please refer to README.
 *   The Data transmission has been modified. Please refer to the README. 
 *   Although the original program is for OTAA, the ABP parameters are given. 
 *   In this program once the switch is pressed a packet is sent to the TTN. 
 *   Each packet contain random values.
 *   The switch is connected to GPIO1 0 the transmission happens only when the switch is pressed
 *   	When the switch is pressed the module receives a high signal.	
 *   The device is registered on Farid's TTN ID: faridcubecell.
 */

#include "LoRaWanMinimal_APP.h"
#include "Arduino.h"
//#include "ttnparameters.h"

/* ABP para*/
uint8_t devEui[] = {0x70,0xB3,0xD5,0x7E,0xD0,0x06,0xB0,0x3C}; // From TTN
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // all zero 
uint8_t appKey[] = {0xB6,0x4B,0xC7,0x69,0x87,0x1E,0xD6,0xDB,0x48,0x9D,0x97,0x05,0x3A,0xA5,0x2D,0x59 }; // From TTN

/*
 * set LoraWan_RGB to Active,the RGB active in loraWan
 * RGB red means sending;
 * RGB purple means joined done;
 * RGB blue means RxWindow1;
 * RGB yellow means RxWindow2;
 * RGB green means received done;
 */

uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
static uint8_t counter=0;
const int buttonPin = GPIO1;  // the number of the pushbutton pin
int buttonState = 0;  // variable for reading the pushbutton status


///////////////////////////////////////////////////
//Some utilities for going into low power mode
TimerEvent_t sleepTimer;
//Records whether our sleep/low power timer expired
bool sleepTimerExpired;


static void wakeUp()
{
  sleepTimerExpired=true;
}

static void lowPowerSleep(uint32_t sleeptime)
{
  sleepTimerExpired=false;
  TimerInit( &sleepTimer, &wakeUp );
  TimerSetValue( &sleepTimer, sleeptime );
  TimerStart( &sleepTimer );
  //Low power handler also gets interrupted by other timers
  //So wait until our timer had expired
  while (!sleepTimerExpired) lowPowerHandler();
  TimerStop( &sleepTimer );
}

///////////////////////////////////////////////////
void setup() {
	Serial.begin(115200);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  if (ACTIVE_REGION==LORAMAC_REGION_US915) {
    //TTN uses sub-band 2 in AU915
    LoRaWAN.setSubBand2();
  }
 
  LoRaWAN.begin(LORAWAN_CLASS, ACTIVE_REGION);
  
  //Enable ADR
  LoRaWAN.setAdaptiveDR(true);

  while (1) {
    Serial.print("Joining... ");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined()) {
      //In this example we just loop until we're joined, but you could
      //also go and start doing other things and try again later
      Serial.println("JOIN FAILED! Sleeping for 30 seconds");
      lowPowerSleep(30000);
    } else {
      Serial.println("JOINED");
      break;
    }
  }
}

///////////////////////////////////////////////////
void loop()
{
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  //In this demo we use a timer to go into low power mode to kill some time.
  //You might be collecting data or doing something more interesting instead.
  //lowPowerSleep(15000);  

  //Here we send confirmed packed (ACK requested) only for the first five (remember there is a fair use policy)
  bool requestack=counter<5?true:false;

  #define NODE1 0x12345678  // Example NodeId (32-bit value)
  float Temp45 = random(0,99) + (random(0, 100) / 100.0);// a random decimal number from 0.00 to 99.99
  uint8_t Temp6 = random(0,100);   // a random integer from 0 to 100
  uint8_t Temp7 = random(0,100);   // a random integer from 0 to 100
  float Temp89 = random(0,99) + (random(0, 100) / 100.0);// a random decimal number from 0.00 to 99.99

  // Data array to hold the bytes
  uint8_t data[10];
 // Split NodeId (NODE1) into 4 bytes and store in data[0] to data[3]
  data[0] = (NODE1 >> 24) & 0xFF; // Most significant byte
  data[1] = (NODE1 >> 16) & 0xFF;
  data[2] = (NODE1 >> 8) & 0xFF;
  data[3] = NODE1 & 0xFF;         // Least significant byte

  // Convert Temp45 to integer (for example, 100.2 becomes 1002)
  int Temp45_int = Temp45 * 10;
  data[4] = (Temp45_int >> 8) & 0xFF;  // Higher byte
  data[5] = Temp45_int & 0xFF;         // Lower byte

  // Store Temp6 and Temp7 directly
  data[6] = Temp6;
  data[7] = Temp7;

   // Convert Temp89 to integer (for example, 23.32 becomes 2332)
  int Temp89_int = Temp89 * 100;
  data[8] = (Temp89_int >> 8) & 0xFF;  // Higher byte
  data[9] = Temp89_int & 0xFF;         // Lower byte

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // send
    if (LoRaWAN.send(sizeof(data), data, 1, requestack)) {
    Serial.println("Send OK");
    } else {
      Serial.println("Send FAILED");
    }
  } else {
    // do nothing
  }

}

///////////////////////////////////////////////////
//Example of handling downlink data
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  Serial.printf("Received downlink: %s, RXSIZE %d, PORT %d, DATA: ",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++) {
    Serial.printf("%02X",mcpsIndication->Buffer[i]);
  }
  Serial.println();
}

