#include <ThingSpeak.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


#define OLED_RESET     D0
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D1, /* data=*/ D2, /* reset=*/ OLED_RESET);

#if defined(ESP8266)
// For ESP8266, use software serial
#define RX_PIN D6   // Connect R307 TX pin to ESP8266 pin D6 (GPIO12)
#define TX_PIN D7   // Connect R307 RX pin to ESP8266 pin D7 (GPIO13)
SoftwareSerial mySerial(RX_PIN, TX_PIN);
#else
#define mySerial Serial1
#endif
#define MAX_STUDENTS 100


//----------------------------------------SSID and Password of your WiFi router.
const char* ssid = ""; //--> Your wifi name or SSID.
const char* password = "";  //--> Your wifi password.
//----------------------------------------

//----------------------------------------Host & httpsPort
const char* host = "api.thingspeak.com";

//----------------------------------------

unsigned long myChannelNumber = ; //--> channel number.
const char * apikey = ""; //--> api key

WiFiClient client; //--> Create a WiFiClientSecure object.
const int httpsPort = 80;

String myStatus = "";
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

struct Student {
  int id;
  String name;
};

Student students[MAX_STUDENTS];

int numStudents = 0;

void enrollStudent(int id, String name) {
  if (numStudents < MAX_STUDENTS) {
    // Add student to the array
    students[numStudents].id = id;
    students[numStudents].name = name;
    numStudents++;
    Serial.println("Student enrolled successfully.");
  } else {
    Serial.println("Maximum number of students reached. Cannot enroll new student.");
  }
}

void printStudents() {
  Serial.println("Enrolled Students:");
  for (int i = 0; i < numStudents; i++) {
    Serial.print("ID: ");
    Serial.print(students[i].id);
    Serial.print(", Name: ");
    Serial.println(students[i].name);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(57600); 
  ThingSpeak.begin(client);
  

  while (!Serial); // Wait for Serial to be available
  delay(100);
  //Connect to host, host (web site) is define at top
  
  
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr); // Change font size to smaller
  u8g2.clearBuffer();
  u8g2.drawStr(0, 20, "Place your finger");
  u8g2.drawStr(0, 40, "on the sensor");
  u8g2.sendBuffer();
  delay(7000);

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(fibnger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

}

int FingerID;
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


char* serialString()
{
  static char str[21]; // For strings of max length=20
  if (!Serial.available()) return NULL;
  delay(64); // wait for all characters to arrive
  memset(str,0,sizeof(str)); // clear str
  byte count=0;
  while (Serial.available())
  {
    char c=Serial.read();
    if (c>=32 && count<sizeof(str)-1)
    {
      str[count]=c;
      count++;
    }
  }
  str[count]='\0'; // make it a zero terminated string
  return str;
}


void loop() {
  // put your main code here, to run repeatedly:
  FingerID = getFingerprintID();  // Get the Fingerprint ID from the Scanner
  delay(50);

  //deleteFingerprint();
  DisplayFingerprintID();
  checkfinger();

  if(FingerID == -1){
    EnrollFingerprint();
  }
}

int id;
int DisplayFingerprintID(){
  //Fingerprint has been detected 
  if (FingerID > 0){
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "Finger detected");
    u8g2.sendBuffer();
    return id;      
  }
  //---------------------------------------------
  //No finger detected
  else if (FingerID == 0){
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "No finger detected");
    u8g2.sendBuffer();
  }
  //---------------------------------------------
  //Didn't find a match
  else if (FingerID == -1){
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "No match found");
    u8g2.sendBuffer();
  }
  //---------------------------------------------
  //Didn't find the scanner or there an error
  else if (FingerID == -2){
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "Error occured");
    u8g2.sendBuffer();
  }
  return 0;
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return -2;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println("Imaging error");
      return -2;
    default:
      //Serial.println("Unknown error");
      return -2;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return -2;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return -2;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return -2;
    default:
      //Serial.println("Unknown error");
      return -2;
  }
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return -2;
  } else if (p == FINGERPRINT_NOTFOUND) {
    //Serial.println("Did not find a match");
    return -1;
  } else {
    //Serial.println("Unknown error");
    return -2;
  }   
  // found a match!
  //Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  //Serial.print(" with confidence of "); Serial.println(finger.confidence); 
 
  return finger.fingerID;
}


String readStringFromSerial() {
  String inputString = ""; // Initialize an empty string to hold the input

  // Wait until a newline character is received
  while (!Serial.available() && Serial.peek() == '\n') {
    delay(100); // Wait for serial data to be available
  }
  return inputString;
}

uint8_t EnrollFingerprint(){
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  int id = readnumber();
  int time = 0;
  static boolean needPrompt=true;
  if (needPrompt)
    {
      Serial.print("Please enter inputs and press enter at the end:\n");
      needPrompt=false;
    }
  const char* name= serialString();
  if (name!=NULL)
  {
    Serial.print("You entered: ");
    Serial.println(name);
    Serial.println();
    needPrompt=true;
  }
  if (id == 0) { // ID #0 not allowed, try again!
    return 0;
  }
  delay(2000);
  Serial.print("Enrolling ID #");
  Serial.println(id);
  Serial.print("Name: ");
  Serial.println(name);
  enrollStudent(id, name);

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  u8g2.clearBuffer();
  Serial.println("Remove finger");
  u8g2.drawStr(0, 20, "Remove finger");
  u8g2.sendBuffer();
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  u8g2.clearBuffer();
  Serial.println("Place same finger again");
  u8g2.drawStr(0, 20, "Place same finger again");
  u8g2.sendBuffer();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  Serial.print("Name "); Serial.println(name);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    while (Serial.available()) {
    char name = Serial.read(); // Read the next character
    if (name == '\n') {
      break; // Stop reading if newline is encountered
    }
    
  }
    Serial.println("Stored!");
    // Update OLED display only when fingerprint is stored successfully
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr); // Change font size to smaller
    u8g2.drawStr(0, 20, "Scan");
    u8g2.drawStr(0, 40, "completed");
    u8g2.sendBuffer();
    delay(2000);
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "Welcome ! ");
    u8g2.drawStr(0, 40, name);
    u8g2.sendBuffer();
    delay(2000);
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "Attendance Marked");
    sendData(id, name);
    thingspeak(id, name);
    u8g2.sendBuffer();
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return true;

}

void checkfinger(){
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER)  goto NoFinger;
  else if (p == FINGERPRINT_OK) {
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  goto NoMatch;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  goto NoMatch;
  String studentName;
    bool found = false;
    for (int i = 0; i < numStudents; i++) {
      if (students[i].id == finger.fingerID) {
        studentName = students[i].name;
        found = true;
        break;
      }
    }
   //if (!found) {
    // Name not found for the given ID
    //Serial.println("Error: Name not found for the ID");
    //return;
  //}
  Serial.print("Welcome! Your ID is ");
  Serial.print(finger.fingerID);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 20, "Welcome! Your ID is"); 
  u8g2.drawStr(0, 40, String(finger.fingerID).c_str());
  u8g2.sendBuffer();
  delay(7000);
  u8g2.clearBuffer();
  Serial.println("Access Granted");
  sendData(finger.fingerID, studentName);
  thingspeak(finger.fingerID, studentName);
  u8g2.drawStr(0, 20, "Attendance Marked");
  u8g2.sendBuffer();
  delay(1500);
  return; 
}
NoMatch: 
{
u8g2.clearBuffer();
Serial.println("Access  Denied");
u8g2.drawStr(0, 20, "Access  Denied");
u8g2.sendBuffer();
delay(1500); 
return;
} 
NoFinger: 
{
Serial.println("No finger detected");
delay(1500);
} 
}

//******************Delete Finpgerprint ID*****************
uint8_t deleteFingerprint( int id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);
 
  if (p == FINGERPRINT_OK) {
    //Serial.println("Deleted!");
    u8g2.clearBuffer();
    u8g2.drawStr(0,0, "Deleted!\n");             
    u8g2.sendBuffer();
    return 0;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    u8g2.clearBuffer();
    u8g2.drawStr(0,0, "Communication error!\n");             
    u8g2.sendBuffer();           
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println("Could not delete in that location");
    u8g2.clearBuffer();
    u8g2.drawStr(0,0, "Could not delete in that location!\n");             
    u8g2.sendBuffer(); 
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println("Error writing to flash");
     u8g2.clearBuffer();
    u8g2.drawStr(0,0, "Error writing to flash!\n");             
    u8g2.sendBuffer(); 
    return p;
  } else {
    //Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    u8g2.clearBuffer();
    u8g2.drawStr(0,0, "Unknown error:\n");             
    u8g2.sendBuffer(); 
    return p;
  }
  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
}



void sendData(int studentId, String studentName) 
{
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data

  String url = "/update.json?api_key=" + String(apikey) + "&field1=" + studentId + "&field2=" + studentName;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}
//===============================================================

void thingspeak(int studentId, String studentName){
ThingSpeak.setField(1, studentId);
ThingSpeak.setField(2, studentName);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, apikey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(5000); // Wait 20 seconds to update the channel again
}

