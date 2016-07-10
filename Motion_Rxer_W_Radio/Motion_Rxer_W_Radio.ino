/* Feather 32u4 Motion detector RXer
 *  
 *  Proudly hand coded by: Clayton Allen
 *  
 *  Future todos with this firmware include
 *  
 *  [] Include method to gain battery level report from TXers
 *  [] Test methods to distinguish North South East West
 *  [] Experiment with the Long+Slow radio setting for greater range and reliability
 *  [] Add/Refine the function of panic
 *   - [] Add green LED for setup success indication
 *   - [] Develop differnt flash pattern to better debug what the radio is doing without a computer around
 *  [] Get rid of Serial Prints once deployable
 */

/* Includes */
#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

/* IOs in use */
#define txLED 12
#define rxLED 13

/* Set Frequency */
#define RF95_FREQ 915.0

/* Singleton instance of the radio driver */
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
void setup() {
/* Radio Pin Assignments */  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
/* IO Pin Assignments */
  pinMode(rxLED, OUTPUT);
  pinMode(txLED, OUTPUT);

/* Serial start */
  Serial.begin(9600);delay(100);

/* Announce LoRa */
  Serial.println("Starting LoRa Radio");

/* Manual reset of LoRa */
  digitalWrite(RFM95_RST, LOW);delay(10);
  digitalWrite(RFM95_RST, HIGH);delay(10);

/* If LoRa init fails */
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

/* If LoRa init succeeds */
  Serial.println("LoRa radio init OK!");delay(500);

/* If LoRa frequency can't be set */
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

/* If LoRa frequency set succeeded */
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

/* Set LoRa power (range = 5 to 23) */
  rf95.setTxPower(23, false);delay(500);

/* Use the panic function to show physically the init is done */
  panic(txLED);
  panic(rxLED);

/* Once setup is complete */
  Serial.println("Motion detection is ready");
}

void loop() {
/* If message becomes available */
  if (rf95.available()){
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf,&len)){
      panic(rxLED);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char *)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

/* Send the acknowlegement */
      uint8_t ackMent[]= "Good Copy";
      rf95.send(ackMent, sizeof(ackMent));
      rf95.waitPacketSent();
      panic(txLED);delay(1000);
    }
    else{
      Serial.println("Receive Failed");
    }
  }
}

void panic(int direction){
  for(int i = 0;i<3;i++){
    digitalWrite(direction,HIGH);delay(100);
    digitalWrite(direction,LOW);delay(100);
  }
  Serial.println("Motion was detected!");
}
