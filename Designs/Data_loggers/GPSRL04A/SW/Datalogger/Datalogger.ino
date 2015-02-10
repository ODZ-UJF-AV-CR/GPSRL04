/*
  SD card datalogger GPSRL04A
 	
 The circuit:
 * analog sensors are connected to analog ins PC0, PC1
 * SD card attached to SPI bus as follows:
 ** MOSI - pin PB3
 ** MISO - pin PB4
 ** CLK  - pin PB5
 ** CS   - pin PD4
 
*/

#include <SD.h>

const int chipSelect = 4;     // CS is PD4
int LED = 5;                  // LED is conected from PD5 to LED at GPS module
unsigned int n = 0;           // measurement counter
char inChar;                  // input character from GPS
String dataString = "";       // concantenated string with NMEA messages and measured values
String filename = "log.csv";  // generated filename for log file
int coll = 0;                 // collons counter in NMEA messages

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {;}

  Serial.print("Initializing SD card...");  // inserting a SD Card always reset the processor and call setup
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT); // PB2
  pinMode(LED, OUTPUT);     

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  dataString = "";
  ReadGPRMC();     // read time from GPS
  // derive file name fom time (YYMMDDHH.CSV)
  int len = dataString.length()-1;
  filename = dataString.substring(len-2,len) + dataString.substring(len-4,len-2) + dataString.substring(len-6,len-4) + dataString.substring(0,2) + ".CSV ";
}

void loop()
{ 
  dataString = "";        // make a string for assembling the data to log
  ReadGPRMC();            // read NMEA sentences from GPS
  ReadGPGGA();
  
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(20);               // wait for a while
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(20);               // wait for a while

  // make a string for assembling the data to log:
  dataString += String(n++);
  dataString += ","; 

  // read analog values from sensors and append to the string:
  for (int analogPin = 0; analogPin < 2; analogPin++) 
  {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 1)   // suppress last coma
    {
      dataString += ","; 
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  char fileNameCharArray[filename.length()];
  filename.toCharArray(fileNameCharArray, filename.length());
  File dataFile = SD.open(fileNameCharArray, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) 
  {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.CSV");
  } 
}


// function for reading $GPRMC NMEA message
void ReadGPRMC()
{
  // $GPRMC,091451.00,A,4915.64143,N,01441.50397,E,0.053,,090215,,,A*74

  coll = 0;
  
  while(1)    // wait for $GPRMC
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
  while (coll < 9);  // read only 9 coma separated values
}


// function for reading $GPGGA NMEA message
void ReadGPGGA()
{
  // $GPGGA,091451.00,4915.64143,N,01441.50397,E,1,09,0.90,443.2,M,44.0,M,,*50

  coll = 0;
  
  while(1)  // wait for $GPGGA
  {
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
    if (coll > 4) dataString += inChar;  // skip first 5 coma separated values
  }
  while (coll < 12); // read only 7 coma separated values
}




