#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include "gas.h"

SoftwareSerial mySerial(10, 11); // TX, RX

//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);
GasSensor grove;

String loraTxt;

// the setup routine runs once when you press reset:
void setup()
{
  //output LED pin
  pinMode(13, OUTPUT);
  led_on();

  //Interruption setup
  pinMode(2, INPUT);
  attachInterrupt(0, handlerInterruption, FALLING);

  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  initialize_radio();

  grove.calibration();

  //transmit a startup message
  myLora.tx("TTN Mapper on TTN Enschede node");

  led_off();
  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;


  /*
   * ABP: initABP(String addr, String AppSKey, String NwkSKey);
   * Paste the example code from the TTN console here:
   */
  const char *devAddr = "26011F72";
  const char *nwkSKey = "266FA2424BDE0F6A7C115EA0308D1B08";
  const char *appSKey = "F07F2FEB3BED20DE136A71106DED2218";

  join_result = myLora.initABP(devAddr, appSKey, nwkSKey);

  /*
   * OTAA: initOTAA(String AppEUI, String AppKey);
   * If you are using OTAA, paste the example code from the TTN console here:
   */
  //const char *appEui = "70B3D57ED00001A6";
  //const char *appKey = "A23C96EE13804963F8C2BD6285448198";

  //join_result = myLora.initOTAA(appEui, appKey);


  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

// the loop routine runs over and over again forever:
void loop()
{
    led_on();

    Serial.println("Neux !");
    myLora.tx(loraTxt); //one byte, blocking function

    //Gas sensor
    float vol = grove.readValue();
    
    Serial.print("The gas density is ");
    Serial.println(vol);
    delay(100);

    loraTxt = "Neux";
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}

void handlerInterruption() {

  Serial.println("ATTENTION DANGER, ENVOI D'UN MESSAGE VIA LORA !!");
  loraTxt = "DANGER";
  delay(1000);

}
