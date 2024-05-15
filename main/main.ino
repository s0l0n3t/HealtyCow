#include <Wire.h>
#include <OneWire.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <DallasTemperature.h>
#include <PulseSensorPlayground.h>
#include <ESP8266TimerInterrupt.h>
#include <ESP8266WiFi.h>
#define BOT_TOKEN "7081993631:AAFrY-gCUMeXY5e5lhq5kIfXD9eBzmOKW4g"
#define CHAT_ID "338442260"
#define USE_ARDUINO_INTERRUPTS true

int HWID = 1;

int cow_status = 0;
const char* ssid     = "TurkTelekom_TPD7F0_2.4GHz";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "chsnth4uCAu7";     // The password of the Wi-Fi network
// token 7081993631:AAFrY-gCUMeXY5e5lhq5kIfXD9eBzmOKW4g
int nabiz; //Pulse defined.
const int PulseWire = A0;
int Threshold = 550; //pulse threshold
int THI=0;
PulseSensorPlayground pulseSensor;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);//Telegram bot authentication.
// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;    

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  
  Serial.begin(9600);
  initWiFi();//TEST
  delay(100);
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);
  Wire.begin(sda, scl);
  sensors.begin();// Start the DS18B20 sensor
  MPU6050_Init();
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);  
 // pulseSensor.begin();
  
  delay(500);
  
}

void loop() {
  WiFiClient client;//added.
  client.connect("192.168.1.123",8000);
  nabiz = analogRead(PulseWire) - 550;
  double Ax, Ay, Az, T, Gx, Gy, Gz;
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0); 
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  //divide each with their sensitivity scale factor
  Ax = (double)AccelX/AccelScaleFactor;
  Ay = (double)AccelY/AccelScaleFactor;
  Az = (double)AccelZ/AccelScaleFactor;
  T = (double)Temperature/340+36.53; //temperature formula
/*
  Serial.print(Ax);
  Serial.print(" ");
  Serial.print(Ay);
  Serial.print(" ");
  Serial.print(Az);//needs to be 0.5 or higher
  Serial.print(" ");*/
  // total acceleration = sqrt(ay^2+ax^2+az^2) so...
  if(sqrt(pow(Ay,2)+pow(Ax,2)+pow(Az,2)) >= 1.10 && cow_status == 0){
    cow_status = 1; //standing
  }
  else if (sqrt(pow(Ay,2)+pow(Ax,2)+pow(Az,2)) >= 1.10 && cow_status == 1){
    cow_status = 0;//sitting
  } 
  else{}
  
  if(nabiz < 40 || nabiz > 120){
    nabiz = 0;
  }
  else{

  }
  client.println(String(HWID) + " " + String(cow_status) + " " +String(temperatureC)+ " " +String(nabiz));
  //client.println("1 0 32 102");
  client.stop();
  Serial.print(HWID);
  Serial.print(" ");
  Serial.print(cow_status);// 1.00 is steable.
  Serial.print(" ");
  Serial.print(temperatureC);
  Serial.print("ºC");
  Serial.print(" ");
  Serial.println(nabiz);
  Serial.println();
  //bot.sendMessage(CHAT_ID, "sa", "");
  //Control tab
  

  if(temperatureC <= 38.00 && temperatureC >= 30.00){
    bot.sendMessage(CHAT_ID, "1 numaralı inek düşük sıcaklık !", "");
    delay(100);
  //  38.62- 39.76 °C body term
  }
  else if(temperatureC >= 40.00){
    bot.sendMessage(CHAT_ID, "1 numaralı inek yüksek sıcaklık !", "");
    delay(100);
  }
  
  
  else if (nabiz != 0 && nabiz >= 90 && nabiz <= 120){
    bot.sendMessage(CHAT_ID, "1 numaralı inek nabız yüksek !", "");
    
    delay(100);
  }
  else if(nabiz > 40 && nabiz < 90){
    bot.sendMessage(CHAT_ID, "1 numaralı inek nabız düşük !", "");
    
    delay(100);
  }
  else{}


  delay(2000);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

// d6 sda d7 scl
//https://www.esp8266.com/viewtopic.php?p=84363