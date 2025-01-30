
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3qnkUNdrM"
#define BLYNK_TEMPLATE_NAME "Hybrid Energy"
#define BLYNK_AUTH_TOKEN "64XeCQ3mI_faAQlAXRS4LGDL5qO8AJOG"

#include <LiquidCrystal.h>     // include the library
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  // datapin, latchpin, clockpin
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Peterparker";  // type your wifi name
char pass[] = "0987654321";  // type your wifi password

#define EspSerial Serial
#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);

int mvAmp_sol20  = 148;          
int offset_sol20 = 1935;

const int I_in = A0;    //  Current
const int v_in = A1;    //  Voltage
const int v_in2 = A2;    //  Voltage
const int v_in3 = A3;    //  Voltage

const int G_rly = 9;
const int W_rly = 8;
const int S_rly = 10;
const int L_rly = 11;

//Measuring DC Voltage Using voltage divider n/w
float Vout = 0.00;
float Vin = 0.00;
float R1 = 32600.00; // resistance of R1 (100K) 
float R2 = 6650.00; // resistance of R2 (10K) 
int val = 0;

//Measuring DC Voltage Using voltage divider n/w
float Vout2 = 0.00;
float Vin2 = 0.00;
float R11 = 32600.00; // resistance of R1 (100K) 
float R22 = 6650.00; // resistance of R2 (10K) 
int val2 = 0;

float Power_Value = 0.00;
int RawValue  = 0;
float sen_v = 0;
float Amps = 0;

void setup()
{
  // Debug console
   EspSerial.begin(ESP8266_BAUD);  delay(10);
   lcd.begin(16, 2);  lcd.clear();
   lcd.home (); 

   pinMode(G_rly, OUTPUT); digitalWrite(G_rly, HIGH);
   pinMode(W_rly, OUTPUT); digitalWrite(W_rly, HIGH);
   pinMode(S_rly, OUTPUT); digitalWrite(S_rly, HIGH);
   pinMode(L_rly, OUTPUT); digitalWrite(L_rly, HIGH);
  
   pinMode(v_in, INPUT); pinMode(v_in2, INPUT); pinMode(v_in3, INPUT);
   pinMode(I_in, INPUT);

   lcd.setCursor (0, 0);  lcd.print ("  Smart hybrid  ");
   lcd.setCursor (0, 1);  lcd.print ("renewable energy");
   delay(3000);
   lcd.clear();

   Blynk.begin(auth, wifi, ssid, pass);
   timer.setInterval(500L, send_I);    //send_I();  send_V(); send_V2();
   timer.setInterval(1000L, send_V);
   timer.setInterval(1500L, send_V2);
   delay(200);
}

/
void loop()
  {
   Blynk.run();  timer.run();
   if(Vin >= 10) 
    { 
     lcd.setCursor(9, 0);  lcd.print("L-on:So ");
     digitalWrite(G_rly, HIGH);
     digitalWrite(W_rly, HIGH);
     digitalWrite(S_rly, LOW); 
    }
   else if(Vin2 >= 3) 
     { 
      lcd.setCursor(9, 0);  lcd.print("L-on:Wi ");
      digitalWrite(G_rly, HIGH);
      digitalWrite(W_rly, LOW);
      digitalWrite(S_rly, HIGH); 
     }
   else
     { 
      lcd.setCursor(9, 0);  lcd.print("L-on:Gr ");
      digitalWrite(G_rly, LOW);
      digitalWrite(W_rly, HIGH);
      digitalWrite(S_rly, HIGH); 
     }
  }
 

void send_I()   
  {
    int average = 0;
    for(int i = 0; i < 15; i++) 
     {
      average = average + analogRead(I_in);
      delay(20);
     }
    RawValue = average/15;
    sen_v = (RawValue / 1023.0) * 5000;       // Gets you mV
    //Serial.println(sen_v);
    Amps = ((sen_v - offset_sol20) / mvAmp_sol20);
    if(Amps <= 0.06)  { Amps = 0.00; }
    lcd.setCursor(0, 0);  
    lcd.print("I:");
    lcd.print(Amps);
    lcd.print("  ");
    Blynk.virtualWrite(V0, Amps);
    delay (50);
  }

void send_V()   
  {
    int average2 = 0;
    for(int i = 0; i < 10; i++) 
     {
      average2 = average2 + analogRead(v_in);
      delay(20);
     }                                     //reads the analog input
    val = average2/10;
    Vout = (val * 5.00) / 1023.00;  // formula for calculating voltage out i.e. V+, here 5.00
    Vin = Vout / (R2/(R1+R2));      // formula for calculating voltage in i.e. GND
    if(Vin < 0.10)  { Vin = 0.00; }

    lcd.setCursor(0, 1);   lcd.print("SV:");
    lcd.print(Vin,1);   lcd.print("  ");
    Blynk.virtualWrite(V1, Vin);
    delay (100);
  }

void send_V2()   
  {
    int average3 = 0;
    for(int i = 0; i < 10; i++) 
     {
      average3 = average3 + analogRead(v_in2);
      delay(20);
     }                                     //reads the analog input
    val2 = average3/10;
    Vout2 = (val2 * 5.00) / 1023.00;  // formula for calculating voltage out i.e. V+, here 5.00
    Vin2 = Vout2 / (R22/(R11+R22));      // formula for calculating voltage in i.e. GND
   
    if(Vin2 < 0.10) { Vin2 = 0.00; }

    lcd.setCursor(9, 1);   lcd.print("WV:");
    lcd.print(Vin2,1);   lcd.print("  ");
    Blynk.virtualWrite(V2, Vin2);
    delay (100);
  }

BLYNK_WRITE(V3) 
 {
   int Sw1 = param.asInt();
   if(Sw1 == 1) { digitalWrite(L_rly,LOW); }
   else   { digitalWrite(L_rly,HIGH);  }
 } 
 
