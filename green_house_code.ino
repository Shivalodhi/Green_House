#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const double VCC = 5.0;             // NodeMCU on board 3.3v vcc
const double R2 = 10000;            // 10k ohm series resistor
const double adc_resolution = 1023; // 10-bit adc

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741;

String iot_data[10];
String send_data_string;

bool dc_fan_status = 0;
bool bulb_status = 0;
bool pump_status = 0;
bool dc_fan_old_status = 0;
bool bulb_old_status = 0;
bool pump_old_status = 0;

SoftwareSerial esp8266(2, 3);

#define soil_moisture_sensor_pin A2
#define temperature_sensor_pin A3
#define light_sensor_pin A4

#define dc_fan 11
#define bulb 12
#define pump 13

unsigned int temp = 0;
int light_level = 0;
int moisture_level = 0;
int temperature;

long previous_time = 0;
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

void setup() {
  pinMode(dc_fan, OUTPUT);
  pinMode(bulb, OUTPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(dc_fan, LOW);
  digitalWrite(bulb, HIGH);
  digitalWrite(pump, HIGH);

  Serial.begin(9600);
  esp8266.begin(9600);
  lcd.begin(20, 4);

  lcd.clear();
  lcd.print("Greenhous Monitoring");// project");
  lcd.setCursor(0, 1);
  lcd.print("And Control System");
  lcd.setCursor(5, 3);
  lcd.print("Using IOT ");
  delay(2000); // delay 2 secs

  previous_time = millis();
}

void loop() {

  measure_temperature();
  measure_light_intensity();
  measure_soil_moisture();
  display_parameters();
  process_data();

  Serial.println("light_level = " + String(light_level));
  Serial.println("moisture_level = " + String(moisture_level));
  Serial.println("temperature = " + String(temperature));
  Serial.println(" ");
  Serial.println("pump_status = " + String(pump_status));
  Serial.println("bulb_status = " + String(bulb_status));
  Serial.println("dc_fan_status = " + String(dc_fan_status));


  send_data_string = (iot_data[0]) + "*" + String(iot_data[1]) + "*" + String(iot_data[2]) + "*" + String(iot_data[3]) + "*" + String(iot_data[4]) + "*" + String(iot_data[5]);
  Serial.println("send_data_string = " + String(send_data_string)); display_parameters();

  if ((millis() - previous_time >= 30000) or (dc_fan_old_status != dc_fan_status) or (bulb_old_status != bulb_status) or (pump_old_status != pump_status))
    //  if ((dc_fan_old_status != dc_fan_status) or (bulb_old_status != bulb_status) or (pump_old_status != pump_status))
  {
    send_data_string = (iot_data[0]) + "*" + String(iot_data[1]) + "*" + String(iot_data[2]) + "*" + String(iot_data[3]) + "*" + String(iot_data[4]) + "*" + String(iot_data[5]);
    Serial.println("send_data_string = " + String(send_data_string)); display_parameters();
    send_parameters();
    previous_time = millis();
  }
  dc_fan_old_status = dc_fan_status;
  bulb_old_status = bulb_status;
  pump_old_status = pump_status;
}

void send_parameters()
{
  esp8266.listen();
  lcd.setCursor(0, 3);
  //  lcd.print("Up");
  lcd.print("Uploading To IOT");
  esp8266.print(send_data_string);
  delay(10000);
}