#include <LWiFi.h>
#include <LBattery.h>
#include <LWiFiClient.h>
#include <SimpleTimer.h>
#include <BlynkSimpleLinkItONE.h>

char auth[] = "";

char ssid[] = "";
char pass[] = "";
int wifi_sec = LWIFI_WPA;

#define vBAT_PIN V2     //Set battery level to virtual pin 2
#define vCHARGE_PIN V3  //Set charge status to virtual pin 3
#define vTEMP_PIN V5    //Set temperature value to virtual pin 5
#define vLIGHT_PIN V6   //Set light value to virtual pin 6
#define vPIR_PIN V7     //Set pir value to virtual pin 7
#define vHUMI_PIN V8    //Set humidity value to virtual pin 8
#define vPROX_PIN V9    //Set proximity value to virtual pin 9

//Define HEX value of colors for widgets
#define WHITE   "#FFFFFF"
#define YELLOW  "#FFFF00"
#define RED     "#FF0000"
#define BLUE    "#00BBFF"
#define GRAY    "#D6D9DB"

//Define email address you need to set for IFTTT
#define emailAddress  ""

//Define light sensor threshold
#define lightThreshold  ""

//Define temperature maximum and minimum values
#define maxTemp ""
#define minTemp ""

//Define humidity maximum and minimum values
#define maxHumi ""
#define minHumi ""

//Declare variables
float t, h;
int p, i, b, l;
boolean batteryStatus;

//Set charge indicator LED pin as 8
int ledPin = 8;

SimpleTimer timer;

void setup() {

  Serial1.begin(9600);    // Use 9600 baud rate
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass, wifi_sec);

  //Setup the timed fuctions
  timer.setInterval(80L, getValuesEvent);
  timer.setInterval(1000L, sendValuesEvent);

  //Set all sensor widget colors
  setSensorWidgets();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  Blynk.run();
  timer.run(); // Initiates SimpleTimer
}

void getValuesEvent() {
  getSensorValues();
  getChargeStatus();
}

void sendValuesEvent() {
  //Send sensor values to BLYNK server
  Blynk.virtualWrite(vTEMP_PIN, t);
  Blynk.virtualWrite(vLIGHT_PIN, l);
  Blynk.virtualWrite(vPIR_PIN, p);
  Blynk.virtualWrite(vHUMI_PIN, h);
  Blynk.virtualWrite(vPROX_PIN, i);

  //Set charge widget and send charging status
  setChargeWidget();
  Blynk.virtualWrite(vBAT_PIN, b);
  Blynk.virtualWrite(vCHARGE_PIN, batteryStatus);
}

void getSensorValues() {
  Serial1.print("s");
  String val = Serial1.readString();

  //Convert recieved value to separate variables

  int i1, i2, i3, i4, i5;
  String temp, humi, pir, prox, light;

  i1 = val.indexOf(',');
  temp = val.substring(0, i1);
  i2 = val.indexOf(',', i1 + 1);
  light = val.substring(i1 + 1, i2);
  i3 = val.indexOf(',', i2 + 1);
  pir = val.substring(i2 + 1, i3);
  i4 = val.indexOf(',', i3 + 1);
  humi = val.substring(i3 + 1, i4);
  i5 = val.indexOf('.', i4 + 1 );
  prox = val.substring(i4 + 1, i5);

  t = temp.toFloat();  //Covert temp to float
  t = t / 10;     //Divide by 10 for decimal place

  h = humi.toFloat();  //Covert humidity to float
  h = h / 10;     //Divide by 10 for percentage

  p = pir.toInt();  //Convert pir value to int
  i = prox.toInt(); //Convert prox value to int
  l = light.toInt(); //Convert light value to int

  //Value is either 0 or 255 for the LED Widgets
  p = p * 255;
  i = i * 255;
}

void getChargeStatus() {
  //Check if battery is charging
  batteryStatus = LBattery.isCharging();
  batteryStatus = batteryStatus * 255;

  //If battery is charging, set led to high
  if (batteryStatus == 255) {
    digitalWrite(ledPin, HIGH);
  }
  else if (batteryStatus == 0) {
    digitalWrite(ledPin, LOW);
  }

  //Get charge level
  b = LBattery.level();
}

void setSensorWidgets() {
  //Set temperature widget color to white
  Blynk.setProperty(vTEMP_PIN, "color", WHITE);

  //Set light widget color to yellow
  Blynk.setProperty(vLIGHT_PIN, "color", YELLOW);

  //If motion detected, set LED to red
  Blynk.setProperty(vPIR_PIN, "color", RED);

  //Set humidity widget color to red
  Blynk.setProperty(vHUMI_PIN, "color", BLUE);

  //If motion detected, set LED to red
  Blynk.setProperty(vPROX_PIN, "color", GRAY);
}

void setChargeWidget() {
  //Set color of charge indicator LED
  Blynk.setProperty(vCHARGE_PIN, "color", "#FF9000");

  //Set color of battery percentage, based on value
  if (b == 100) {
    Blynk.setProperty(vBAT_PIN, "color", "#1EBF00");
  }
  else if (b == 66) {
    Blynk.setProperty(vBAT_PIN, "color", "#A5CB00");
  }
  else if (b == 33) {
    Blynk.setProperty(vBAT_PIN, "color", "#D87200");
  }
  else if (b == 0) {
    Blynk.setProperty(vBAT_PIN, "color", "#E5001E");
  }
}
