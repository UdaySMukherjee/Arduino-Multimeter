
#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
const int select_button = 2; 
const int right_button = 3; 
const int OhmMeter = A0; 
const int CapacitancMeter = A1; 
const int VoltMeter = A2; 

const int R3 = 6; 
const int R2 = 5; 
const int R1 = 4; 
const int ChargePin = 13; 
const int DischargePin = 11; 
boolean is_select = false;
int navigator = 0; 
int flag = 0; 
float R = 0.00; 
float V = 0.00; 

float C = 0.00; 
boolean nano = false; 
boolean kilo = false; 
boolean mili = false; 
void OLED_init() { 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed")); 
    for (;;); 
  } 
  display.clearDisplay(); 
  display.println("DIY Multimeter"); 
  display.display(); 
  delay(2000); 
  display_clear(); 
} 
void display_clear() { 
  display.clearDisplay(); 
  display.display(); 
} 
 
void display_text(int sz, int x, int y, String str) { 
  display.setTextSize(sz);
  display.setTextColor(WHITE); 
  display.setCursor(x, y);
  display.println(str); 
} 
void display_number(int sz, int x, int y, double num) { 
  display.setTextSize(sz);
  display.setTextColor(WHITE); 
  display.setCursor(x, y);
  display.println(num); 
} 
void calculate_resistor() { 
  float v_ref = 4.94; 
  float r1 = 0.00; 
  float r_ref1 = 1000.00; 
  float adc_value1 = 0.00;
  float voltage1 = 0.00; 
  float r2 = 0.00; 
  float r_ref2 = 10000.00;
  float adc_value2 = 0.00;
  float voltage2 = 0.00; 
  float r3 = 0.00; 
  float r_ref3 = 100000.00; 
  float adc_value3 = 0.00;
  float voltage3 = 0.00; 
  pinMode(R1, OUTPUT); 
  pinMode(R2, INPUT); 
  pinMode(R3, INPUT); 
  digitalWrite(R1, HIGH); 
  for (int i = 0; i < 20 ; i++) 
  { 
    adc_value1 = adc_value1 + analogRead(OhmMeter); 
    delay(3); 
  } 
  adc_value1 = adc_value1 / 20; 
  if (adc_value1 < 1022.90) 
  { 
    voltage1 = ((adc_value1 * v_ref) / 1024); 
    r1 = (voltage1 * r_ref1) / (v_ref - voltage1); 
  } 
  pinMode(R1, INPUT); 
  pinMode(R2, OUTPUT); 
  pinMode(R3, INPUT); 
  digitalWrite(R2, HIGH); 
  for (int i = 0; i < 20 ; i++) 
  { 
    adc_value2 = adc_value2 + analogRead(OhmMeter); 
    delay(3); 
  } 
  adc_value2 = adc_value2 / 20; 
  if (adc_value2 < 1022.90) 
  { 
    voltage2 = ((adc_value2 * v_ref) / 1024); 
    r2 = (voltage2 * r_ref2) / (v_ref - voltage2); 
  } 
  pinMode(R1, INPUT); 
  pinMode(R2, INPUT); 
  pinMode(R3, OUTPUT); 
  digitalWrite(R3, HIGH); 
  for (int i = 0; i < 20 ; i++) 
  { 
    adc_value3 = adc_value3 + analogRead(OhmMeter); 
    delay(3); 
  } 
  adc_value3 = adc_value3 / 20; 
  if (adc_value3 < 1022.90) 
  { 
    voltage3 = ((adc_value3 * v_ref) / 1024); 
    r3 = (voltage3 * r_ref3) / (v_ref - voltage2); 
  } 
  r1 = r1 / 1000; 
  r2 = r2 / 1000; 
  r3 = r3 / 1000; 
  if (r1 < 2 && r2 < 101 && r3 < 1001) R = r1 * 1000; 
  else if (r1 > 2 && r2 < 101 && r3 < 1001) R = r2; 
  else if (r1 > 2 && r2 > 101 && r3 < 2000) R = r3; 
  else R = 0.00; 
  if (R < 1) 
  { 
    R = R * 1000; 
    kilo = false; 
  } 
  else 
  { 
    kilo = true; 
  } 
} 
void calculate_capacitance() { 
  unsigned long start_time; 
  unsigned long elapsed_time; 
  float microFarads; 
  float nanoFarads; 
  float r_ref = 10000.00; 
  digitalWrite(ChargePin, HIGH); 
  start_time = millis(); 
  while (analogRead(CapacitancMeter) < 648) {} 
  elapsed_time = millis() - start_time; 
  microFarads = ((float)elapsed_time / r_ref) * 1000; 
  if (microFarads > 1) 
  { 
    C = microFarads; 
    nano = false; 
  } 
  else 
  { 
    nanoFarads = microFarads * 1000.0; 
    C = nanoFarads; 
    nano = true; 
  } 
  digitalWrite(ChargePin, LOW); 
  pinMode(DischargePin, OUTPUT); 
  digitalWrite(DischargePin, LOW); 
  while (analogRead(CapacitancMeter) > 0) {} 
  pinMode(DischargePin, INPUT); 
} 
void calculate_voltage() { 
  float R1 = 10000.00; 
  float R2 = 4700.00; 
  float v_ref = 5.00; 
  float resistor_ratio = 0.00; 
  float adc_value = 0.00; 
  float voltage = 0.00; 
  resistor_ratio = (R2 / (R1 + R2)); 
  for (int i = 0; i < 20 ; i++) 
  { 
    adc_value = adc_value + analogRead(VoltMeter); 
    delay(3); 
  } 
  adc_value = adc_value / 20; 
  voltage = ((adc_value * v_ref) / 1024); 
  V = voltage / resistor_ratio; 
} 

void setup() { 
  Serial.begin(9600); 
  OLED_init(); 
  pinMode(right_button, INPUT_PULLUP); 
  pinMode(select_button, INPUT_PULLUP); 
  pinMode(ChargePin, OUTPUT); 
  digitalWrite(ChargePin, LOW); 
} 
 int cnt=0;
void loop() { 
    if(cnt==0)
    {
      display.clearDisplay();  
    display_text(1, 20, 9, "DIY Multimeter"); 
    display.display();
    delay(2000);
    cnt=1;
      }
  if (digitalRead(right_button) == 0) 
  { 
    navigator++; 
    while (digitalRead(right_button) == 0); 
    delay(5); 
    if (navigator > 3) navigator = 0; 
    Serial.println(navigator); 
  } 
  if ( digitalRead(select_button) == 0) 
  { 
    is_select = true; 
    while ( digitalRead(select_button) == 0); 
  } 
  if (navigator == 0) 
  { 
    display.clearDisplay();  
    display_text(2, 17, 8, "Resistor"); 
    display.display(); 
    while (is_select) 
    { 
      display.clearDisplay(); 
      display_text(1, 0, 0, "Resistor"); 
      display_text(2, 12, 8, "R="); 
      display_number(2, 42, 8, R); 
      if (kilo) display_text(1, 115, 15, "k"); 
      display.display(); 
      calculate_resistor(); 
      if ( digitalRead(select_button) == 0) 
      { 
        is_select = false; 
        while ( digitalRead(select_button) == 0); 
      } 
    } 
  } 
  if (navigator == 1) 
  { 
    display.clearDisplay(); 
    display_text(2, 17, 8, "Voltage"); 
    display.display(); 
    while (is_select) 
    { 
      display.clearDisplay(); 
      display_text(1, 0, 0, "Voltage"); 
      display_text(2, 12, 8, "V="); 
      display_number(2, 42, 8, V); 
      display_text(1, 115, 15, "v"); 
      display.display(); 
      calculate_voltage(); 
      if ( digitalRead(select_button) == 0) 
      { 
        is_select = false; 
        while ( digitalRead(select_button) == 0); 
      } 
    } 
  } 
 
  if (navigator == 3) 
  {
    display.clearDisplay();  
    display_text(2, 12, 8, "Capacitor"); 
    display.display(); 
    while (is_select) 
    { 
      display.clearDisplay(); 
      display_text(1, 0, 0, "Capacitor"); 
      display_text(2, 12, 8, "C="); 
      display_number(2, 42, 8, C); 
      if (nano) display_text(1, 115, 22, "nF"); 
      if (!nano) display_text(1, 115, 22, "uF"); 
      display.display(); 
      calculate_capacitance(); 
      if ( digitalRead(select_button) == 0) 
      { 
        is_select = false; 
        while ( digitalRead(select_button) == 0); 
      } 
    } 
  } 
} 
