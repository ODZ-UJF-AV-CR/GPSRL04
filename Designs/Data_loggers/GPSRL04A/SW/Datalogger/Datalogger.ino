/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SD.h>


// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;
int led = 5;
unsigned int n = 0;
char inChar;
String dataString = "";
String filename = "log.csv";
int coll = 0;

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(led, OUTPUT);     

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  dataString = "";
  ReadGPRMC();

//  filename = dataString.substring(0,5) + dataString.substring(dataString.length()-5,dataString.length()) + filename;
  int len = dataString.length()-1;
  filename = dataString.substring(len-2,len) + dataString.substring(len-4,len-2) + dataString.substring(len-6,len-4) + dataString.substring(0,2) + ".CSV ";
}

void loop()
{ 
  dataString = "";
  ReadGPRMC();
  ReadGPGGA();
  
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(20);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(20);               // wait for a second

  // make a string for assembling the data to log:
//  String dataString = "";
  
  dataString += String(n++);
  dataString += ","; 

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 2; analogPin++) 
  {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 1) 
    {
      dataString += ","; 
    }
  }

  char fileNameCharArray[filename.length()];
  filename.toCharArray(fileNameCharArray, filename.length());
  File dataFile = SD.open(fileNameCharArray, FILE_WRITE);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}


void ReadGPRMC()
{
  // $GPRMC,091451.00,A,4915.64143,N,01441.50397,E,0.053,,090215,,,A*74

  coll = 0;
  
  while(1)
  {
    // get incoming byte:
    while (!Serial.available());
    if (Serial.read() != '$') continue;
    while (!Serial.available());
    if (Serial.read() != 'G') continue;
    while (!Serial.available());
    if (Serial.read() != 'P') continue;
    while (!Serial.available());
    if (Serial.read() != 'R') continue;
    while (!Serial.available());
    if (Serial.read() != 'M') continue;
    while (!Serial.available());
    if (Serial.read() != 'C') continue;
    while (!Serial.available());
    if (Serial.read() != ',') continue;
    break;
  }  
  
  do
  {
    while (!Serial.available());
    inChar = (char)Serial.read();
    if (inChar == ',') coll++;
    dataString += inChar;
  }
  while (coll < 9);
}


void ReadGPGGA()
{
  // $GPGGA,091451.00,4915.64143,N,01441.50397,E,1,09,0.90,443.2,M,44.0,M,,*50

  coll = 0;
  
  while(1)
  {
    // get incoming byte:
    while (!Serial.available());
    if (Serial.read() != '$') continue;
    while (!Serial.available());
    if (Serial.read() != 'G') continue;
    while (!Serial.available());
    if (Serial.read() != 'P') continue;
    while (!Serial.available());
    if (Serial.read() != 'G') continue;
    while (!Serial.available());
    if (Serial.read() != 'G') continue;
    while (!Serial.available());
    if (Serial.read() != 'A') continue;
    while (!Serial.available());
    if (Serial.read() != ',') continue;
    break;
  }  
  
  do
  {
    while (!Serial.available());
    inChar = (char)Serial.read();
    if (inChar == ',') coll++;
    if (coll > 4) dataString += inChar;
  }
  while (coll < 12);
}




