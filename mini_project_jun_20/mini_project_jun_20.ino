
// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#define rs 12
#define en 11
#define d4 5
#define d5 4
#define d6 3
#define d7 6
#define flowsensor 2

SoftwareSerial sim800l(9, 8);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile int flow_frequency = 0;
float litres = 0;
int prev = -1, Time = 0;
String location = "";

void flow()
{
  flow_frequency++;
}


void updateSerial()
{
  delay(500);
  while (Serial.available())
    sim800l.write(Serial.read());
  while (sim800l.available())
    Serial.write(sim800l.read());
}

String getLocation()
{
  String GmapsLink = "https://www.google.com/maps/place/";
  String location = "";
  for (int i = 0; i < 10; i++) {
    sim800l.println("AT+CLBS=1,1");
    delay(1000);
    while (!sim800l.available())
      updateFuel();
    while (sim800l.available()) {
      char letter = sim800l.read();
      location += String(letter);
    }

    if (location.indexOf(",") != -1) {
      String Latitude = "", Longitude = "";
      int first_comma = location.indexOf(',');
      int second_comma = location.indexOf(',', first_comma + 1);
      int third_comma = location.indexOf(',', second_comma + 1);

      for (int i = first_comma + 1; i < second_comma; i++)
        Longitude = Longitude + location.charAt(i);

      for (int i = second_comma + 1; i < third_comma; i++)
        Latitude = Latitude + location.charAt(i);

      GmapsLink += Latitude + "," + Longitude;
      for (int i = 0; i < GmapsLink.length(); i++)
        Serial.write(GmapsLink.charAt(i));
      return GmapsLink;
    }
    location = "";
  }
  return "failed to fetch location";
}

bool sendMsg(float fuel, String location)
{
  sim800l.println("AT+CMGF=1");
  delay(1000);
  sim800l.println("AT+CMGS=\"8977558264\"");
  delay(1000);
  if (fuel != -1)
    sim800l.println("fuel filled: " + String(fuel) + " liters");
  delay(1000);
  sim800l.println("at location: " + location);
  delay(1000);
  sim800l.write(26);
  delay(1000);
}

void welcomeMsg()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     WELCOME    ");
  lcd.setCursor(0, 1);
  lcd.print("smart fuel meter");
}

void updateFuel()
{
  delay(500);
  if (Time == 8)
  {
    if (location == "") {
      Time = 8;
    }
    else {
      lcd.setCursor(0, 0);
      lcd.print("sending message..");
      sendMsg(litres, location);
      welcomeMsg();
      flow_frequency = 0;
      Time = 0;
      prev = -1;
    }
  }
  else if (flow_frequency == 0)
  {
    Time = 0;
  }
  else if (flow_frequency != 0 && prev == -1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("filling started");
    prev = flow_frequency;
    Time = 0;
    location = getLocation();
  }
  else if (flow_frequency == prev)
  {
    lcd.setCursor(0, 0);
    lcd.print("filling paused");
    Time++;
  }
  else
  {
    lcd.clear();
    litres = (((float)flow_frequency) / (310));
    lcd.setCursor(0, 0);
    lcd.print("filling");
    lcd.setCursor(0, 1);
    lcd.print(litres, 1); lcd.print(" Ltrs");
    prev = flow_frequency;
    Time = 0;
  }
  if (Time == 7)
  {
    lcd.setCursor(0, 0);
    lcd.print("filling finished");
    litres = (((float)flow_frequency) / (310));
    lcd.setCursor(0, 1);
    lcd.print(litres, 1); lcd.print(" Ltrs");
    flow_frequency = 0;
    Time = 0;
    prev = -1;
    delay(5000);
    if (location == "")
    {
      lcd.setCursor(0, 0);
      lcd.print("fetching location..");
      Time = 8;
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("sending message..");
      sendMsg(litres, location);
      welcomeMsg();
    }
  }


  //lcd.setCursor(0,1);
  //lcd.print(litres,1);lcd.print(" Ltrs");
  //updateSerial();
  delay(500);
}

void sendOnlyLocation()
{
  String ring = "", location;
  while (sim800l.available())
    ring += String((char)sim800l.read());
  if (ring.indexOf("RING") != -1)
  {
    sim800l.println("AT+CLCC");
    delay(1000);
    while (sim800l.available())
      ring += String((char)sim800l.read());
    sim800l.println("ATH");
    delay(1000);
    if (ring.indexOf("8977558264") != -1) {
      location = getLocation();
      sendMsg(-1, location);
    }
  }
}
void setup() {
  Serial.begin(9600);
  sim800l.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait..");
  Serial.println("Initializing...");
  delay(1000);
  sim800l.println("AT");
  updateSerial();
  sim800l.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  sim800l.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  sim800l.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();
  sim800l.println("ATE0"); //disable AT COMMANDS echos
  updateSerial();
  sim800l.println("AT+SAPBR=1,1"); //enable GPRS connection
  while (!sim800l.available())
    delay(1000);
  updateSerial();

  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);
  attachInterrupt(0, flow, RISING);
  sei();

  welcomeMsg();
}

void loop() {
  updateFuel();
  sendOnlyLocation();
}
