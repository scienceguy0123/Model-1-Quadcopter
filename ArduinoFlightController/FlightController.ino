#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService maneuverService(deviceServiceUuid);
BLEUnsignedShortCharacteristic maneuverCharacteristic(deviceServiceCharacteristicUuid,  BLERead | BLEWrite | BLEWriteWithoutResponse| BLENotify );

int ipadValue, previousIpadValue;
int motorValue, frontCCWMotorValue, frontCWMotorValue, backCCWMotorValue, backCWMotorValue;


#include <Servo.h>
byte frontCWMotorPin = 2;
byte frontCCWMotorPin = 3;
byte backCWMotorPin = 4;
byte backCCWMotorPin = 5;

Servo frontCCWMotor;
Servo frontCWMotor;
Servo backCCWMotor;
Servo backCWMotor;

#include <Wire.h>
float RateRoll, RatePitch, RateYaw;
float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw, RateCalibrationAccX, RateCalibrationAccY, RateCalibrationAccZ;
int RateCalibrationNumber;
bool gyroCaliberated = false, AccCaliberated = false;

float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;

// Ultrasonic Sensor variables
const int trigPin = 1;
const int echoPin = 0;
float height;

// LED matrix variable
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;
byte setupComplete[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1 , 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte calibrationComplete[8][12] = {
  { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0 , 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

//PID control varibales
float AccYSetPoint = 0.00, AccXSetPoint = 0.00, AltSetPoint = 0.00;
float YTime, YTimePrev, YElapsedTime, XTime, XTimePrev, XElapsedTime, AltTime, AltTimePrev, AltElapsedTime;
//PID Constants
double YAxisKp = 15;
double YAxisKi = 0.5;
double YAxiskd = 7;

double XAxisKp = 15;
double XAxisKi = 0.2;
double XAxisKd = 7;

double AltKp = 10;
double AltKi = 0.5;
double AltKd = 7;

float YPID, YError, YPreviousError, XPID, XError, XPreviousError, AltPID, AltError, AltPreviousError;
float YPID_P=0, YPID_I=0, YPID_D=0, XPID_P=0, XPID_I=0, XPID_D=0, AltPID_P=0, AltPID_I=0, AltPID_D=0;

bool YAxisPIDEnabled = true, XAxisPIDEnabled = true, AltPIDEnabled = false;
float frontCCWPIDValue=0, frontCWPIDValue=0, backCCWPIDValue=0, backCWPIDValue=0;

bool ShutDown = false, SafetyOn = false, Hover = false, Calibrated = false, Calibrating = false, ESCSetUp = false, CharacteristicSubscribed=false;


void setup() {
  Serial.begin(57600);
  while (!Serial); 
  delay(1000);

  Serial.println("Setting up ble");
  setupBLE();
 
  while(!BLE.connected() || !maneuverCharacteristic.subscribed()){
    Serial.println("Trying to connect BLE");
  };

  Serial.println("BLE connected");
  delay(5000);
  // setupESC();
  setupGyroAcc();
  setupUltraSonic();

  delay(10);
  //Setup completes, light up LED matrix
  matrix.begin();
  matrix.renderBitmap(setupComplete, 8, 12);
}

void loop() {
  if(!BLE.connected() || ShutDown){
    gradualStopMotor();  
  }

  // LEDMatrixs();

  if (!Calibrated) {
    setupESC();
  //   while(!Calibrated){
  //     Serial.println("Inside calibration loop");
  //     readIpadValue();
  //     writeESC();
    Calibrated=true; 
    };

  
  
  readIpadValue();
  
  
  if(YAxisPIDEnabled) {
    pidControlYAxis() ;
  }

  if(XAxisPIDEnabled){
    pidControlXAxis();
  }

  writeESC();
  

  if(ipadValue == 0){
    gradualStopPIDValue();
  }
  // readAccSignal();
  calculateAngle();
  readUltraSonic();
  serialPrintData();
}

void resetPreviousPIDError() {
  YPreviousError = 0;
  XPreviousError = 0;
  AltPreviousError = 0;
  
}

void gradualStopPIDValue() {

  if(abs(frontCCWPIDValue - 0) > 5){
    frontCCWPIDValue *= 0.7;
  }else{
    frontCCWPIDValue = 0;
  }  

  if(abs(frontCWPIDValue - 0) > 5){
    frontCWPIDValue *= 0.7;
  }else{
    frontCWPIDValue = 0;
  }  

  if(abs(backCCWPIDValue - 0) > 5){
    backCCWPIDValue *= 0.7;
  }else{
    backCCWPIDValue = 0;
  }  

  if(abs(backCWPIDValue - 0) > 5){
    backCWPIDValue *= 0.7;
  }else{
    backCWPIDValue = 0;
  }  
}

void readIpadValue(){
  BLEDevice central = BLE.central();
  if (central && central.connected() && maneuverCharacteristic.subscribed()) {
    if (maneuverCharacteristic.written()) {
      int characteristicValue = maneuverCharacteristic.value();
        if(characteristicValue >= 0 && characteristicValue<= 1023 && !ShutDown){
          if(SafetyOn && characteristicValue > 500){
            ipadValue = 500;
            return;
          }
          ipadValue = maneuverCharacteristic.value();
        }
        else{
          specialCommand(maneuverCharacteristic.value());
        }
      }
  }
} 

void specialCommand(int command){
  switch (command){
      case 2000 :
        ShutDown = true;
        gradualStopMotor();
        break;
      
      case 2001 : {
        ShutDown = false;
        break;
      
      case 2002 :
        SafetyOn = true;
        break;
      
      case 2003 :
        SafetyOn = false;
        break;

      case 2004:
        Calibrated = !Calibrated;
        Calibrating = !Calibrating;
        break;
      }
      case 2005:
        if (!Calibrated) {
          maxThrottle();
          Calibrating = true;
        }
        break;
      case 2006:
        if (!Calibrated) {
          minThrottle();
          Calibrating = true;
        }
        break;
    }
}

void maxThrottle() {
  //for calibration use only
  ipadValue = 1023;
}

void minThrottle() {
  //for calibration use only
  ipadValue = 0;
  
}

void pidControlAlt() 
{
  AltTimePrev = AltTime;
  AltTime = millis();
  AltElapsedTime = (AltTime - AltTimePrev) / 1000;
  
  AltError = height - AltSetPoint;

  AltPID_P = AltKp * AltError;

  if(-1 < AltError < 1){
    AltPID_I =  AltPID_I + (AltKi * AltError);
  }


  AltPID_D = AltKd * ((AltError - AltPreviousError)/ AltElapsedTime);

  AltPID = AltPID_P + AltPID_I + AltPID_D;


  AltPreviousError = AltError;

  frontCCWPIDValue += AltPID;
  frontCWPIDValue -= AltPID;
  backCCWPIDValue -= AltPID;
  backCWPIDValue += AltPID; 
}

void pidControlXAxis() 
{
  XTimePrev = XTime;
  XTime = millis();
  XElapsedTime = (XTime - XTimePrev) / 1000;
  XError = AccX - AccXSetPoint;

  XPID_P = XAxisKp * XError;

  if(-0.5 < XError < 0.5){
    XPID_I =  XPID_I + (XAxisKi * XError);
  }


  XPID_D = XAxisKd * ((XError - XPreviousError)/ XElapsedTime);
  XPID = XPID_P + XPID_I + XPID_D;

  if (ipadValue != 0 && abs(AccX) > 0.05 ) {
    XPreviousError = XError;
    frontCWPIDValue += XPID;
    frontCCWPIDValue -= XPID;
    backCWPIDValue -= XPID; 
    backCCWPIDValue += XPID;
  }

}

void pidControlYAxis() 
{
  YTimePrev = YTime;
  YTime = millis();
  YElapsedTime = (YTime - YTimePrev) / 1000;
  YError = AccY - AccYSetPoint;

  YPID_P = YAxisKp * YError;

  if(-0.5 < YError < 0.5){
    YPID_I =  YPID_I + (YAxisKi * YError);
  }


  YPID_D = YAxiskd * ((YError - YPreviousError)/ YElapsedTime);
  YPID = YPID_P + YPID_I + YPID_D;


  

  if (ipadValue != 0 && abs(AccY) >= 0.05 ){
    YPreviousError = YError;
    frontCWPIDValue -= YPID;
    frontCCWPIDValue -= YPID;
    backCWPIDValue += YPID; 
    backCCWPIDValue += YPID;
  }

  
}

void resetPIDValue() {

  frontCCWPIDValue = 0;
  frontCWPIDValue = 0;
  backCCWPIDValue = 0;
  backCWPIDValue = 0; 
}

void setupBLE() {
  Serial.println("Start setting bluetooth connection");
  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("FLight Controller");  
  BLE.setAdvertisedService(maneuverService);
  maneuverService.addCharacteristic(maneuverCharacteristic);
  BLE.addService(maneuverService);
  maneuverCharacteristic.writeValue(-1);
  // BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  // BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // maneuverCharacteristic.setEventHandler(BLEWritten, maneuverCharacteristicWritten);
  // maneuverCharacteristic.setEventHandler(BLESubscribed, maneuverCharacteristicSubscribed);
  
  BLE.advertise();
  Serial.println("Finish setting bluetooth connection");
}



void setupESC() {
  Serial.println("Start setting ESC");
  frontCWMotor.attach(frontCWMotorPin, 1000 ,2000);
  frontCCWMotor.attach(frontCCWMotorPin, 1000, 2000);
  backCWMotor.attach(backCWMotorPin, 1000 ,2000);
  backCCWMotor.attach(backCCWMotorPin, 1000 ,2000);
  Serial.println("Finish setting ESC");
}

void writeESC(){
  
  motorValue= map(ipadValue, 0, 1023, 0, 180);

  frontCWMotorValue = noLessZeroOrGreatOneEighty(motorValue  + round(frontCWPIDValue));
  frontCCWMotorValue = noLessZeroOrGreatOneEighty(motorValue + round(frontCCWPIDValue));
  backCWMotorValue = noLessZeroOrGreatOneEighty(motorValue + round(backCWPIDValue));
  backCCWMotorValue = noLessZeroOrGreatOneEighty(motorValue + round(backCCWPIDValue));

  
  frontCWMotor.write(frontCWMotorValue);
  frontCCWMotor.write(frontCCWMotorValue);
  backCWMotor.write(backCWMotorValue);
  backCCWMotor.write(backCCWMotorValue);

}

int noLessZeroOrGreatOneEighty(int value) {
  if(value < 0 ){
    return 0;
  }
  else if ( value > 180) {
    return 180;
  }
  return value;
}

void setupGyroAcc() {
  Wire.setClock(400000);

  Wire.begin();
  delay(250);

  // 0x68 is the address of MPU6050
  Wire.beginTransmission(0x68); 
  // Reset power management
  Wire.write(0x6B);
  Wire.write(0x00);

  Wire.endTransmission();

  caliberateGyro();
}

void caliberateGyro() {
  for (RateCalibrationNumber=0;
        RateCalibrationNumber<2000; 
        RateCalibrationNumber ++) {
  readGyroSignal();
  RateCalibrationRoll+=RateRoll;
  RateCalibrationPitch+=RatePitch;
  RateCalibrationYaw+=RateYaw;
  delay(1);
  }
  RateCalibrationRoll/=2000;
  RateCalibrationPitch/=2000;
  RateCalibrationYaw/=2000; 
  gyroCaliberated =true;
  RateCalibrationNumber = 0;
  Serial.println("Finish Setting Up Gyro");

  for (RateCalibrationNumber=0;
        RateCalibrationNumber<2000; 
        RateCalibrationNumber ++) {
  readAccSignal();
  RateCalibrationAccX+=AccX;
  RateCalibrationAccY+=AccY;
  RateCalibrationAccZ+=AccZ;
  delay(1);
  }
  RateCalibrationAccX/=2000;
  RateCalibrationAccY/=2000;
  RateCalibrationAccZ= 1 - RateCalibrationAccZ/2000;
  AccCaliberated = true;
  Serial.println("Finish Setting Up ACC");
}

void readGyroSignal(){
  Wire.beginTransmission(0x68);
  // Setting low pass filter
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission(); 

  Wire.beginTransmission(0x68);
  // Setting gyro sensitivity, 65.5 LSB degree per second
  Wire.write(0x1B); 
  Wire.write(0x8); 
  Wire.endTransmission(); 

  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t GyroX=Wire.read()<<8 | Wire.read();
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();

  // 65.5 is the LSB sensitivity
  RateRoll=(float)GyroX/65.5;
  RatePitch=(float)GyroY/65.5;
  RateYaw=(float)GyroZ/65.5;

  if(gyroCaliberated) {
    RateRoll-=RateCalibrationRoll;
    RatePitch-=RateCalibrationPitch;
    RateYaw-=RateCalibrationYaw;
  }

}

void readAccSignal() {
  
  Wire.beginTransmission(0x68);
  // Setting low pass filter
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  // Setting acc full scale range, choose +- 8g here
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t AccXLSB = Wire.read() << 8 | Wire.read();
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();
  // the sensitivity is 4096 LSB/g for +- 8g full scale range
  //0.02 and 0.01 are error found by looking at logs
    AccX=(float)AccXLSB/4096;
    AccY=(float)AccYLSB/4096;
    AccZ=(float)AccZLSB/4096;

  if(AccCaliberated) {
    AccX=(float)AccX - RateCalibrationAccX;
    AccY=(float)AccY - RateCalibrationAccY;
    AccZ=(float)AccZ + RateCalibrationAccZ;
  }
}


void calculateAngle() {
  readGyroSignal();
  readAccSignal();
  AngleRoll=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
  AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
}

void setupUltraSonic() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Finish Setting Up Ultra Sonic");
}

void readUltraSonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  height = (duration*.0343)/2;
}

void serialPrintData(){
  Serial.print("value from IPad: ");
  Serial.print(ipadValue);
  Serial.print(" motorValue: ");
  Serial.print(motorValue);
    Serial.print(" frontCWMotorValue: ");
  Serial.print(frontCWMotorValue);
  Serial.print(" frontCCWMotorValue: ");
  Serial.print(frontCCWMotorValue);
  Serial.print(" backCWMotorValue: ");
  Serial.print(backCWMotorValue);
  Serial.print(" backCCWMotorValue: ");
  Serial.print(backCCWMotorValue);

  Serial.print(" frontCCWPIDValue: ");
  Serial.print(round(frontCCWPIDValue));
  Serial.print(" frontCWPIDValue: ");
  Serial.print(round(frontCWPIDValue));
  Serial.print(" backCCWPIDValue: ");
  Serial.print(round(backCCWPIDValue));
   Serial.print(" backCWPIDValue: ");
  Serial.print(round(backCWPIDValue));
  // Serial.print(" Roll rate [°/s]= ");
  // Serial.print(RateRoll);
  // Serial.print(" Pitch Rate [°/s]= ");
  // Serial.print(RatePitch);
  // Serial.print(" AccX= ");
  // Serial.print(AccX);
  // Serial.print(" AccY= ");
  // Serial.print(AccY);
  // Serial.print(" AccZ= ");
  // Serial.print(AccZ);
  // Serial.print("BLE connected : ");
  // Serial.print(BLE.connected());
  // Serial.print("AngleRoll:");
  // Serial.print(AngleRoll);
  // Serial.print(",");
  // Serial.print("AnglePitch:");
  // Serial.println(AnglePitch);
  // Serial.print(" CharacteristicSubscribed= ");
  // Serial.print(CharacteristicSubscribed);
  Serial.print( "height:");
  Serial.println(height);
  
}

void gradualStopMotor() {
  ShutDown = true;
  gradualStopPIDValue();
  if (ipadValue > 10){
    ipadValue *= 0.98;
  }
  else{
    ipadValue = 0;
    // resetPIDValue();
  }

  if (frontCCWMotorValue > 10){
    frontCCWMotorValue *= 0.98;
  }
  else{
    frontCCWMotorValue = 0;
    // resetPIDValue();
  }

  if (frontCWMotorValue > 10){
    frontCWMotorValue *= 0.98;
  }
  else{
    frontCWMotorValue = 0;
    // resetPIDValue();
  }

  if (backCCWMotorValue > 10){
    backCCWMotorValue *= 0.98;
  }
  else{
    backCCWMotorValue = 0;
    // resetPIDValue();
  }

  if (backCWMotorValue > 10){
    backCWMotorValue *= 0.98;
  }
  else{
    backCWMotorValue = 0;
    // resetPIDValue();
  }
}

void LEDMatrixs() {
  if (Calibrated) {
      matrix.renderBitmap(calibrationComplete, 8, 12);
  }
}

// void blePeripheralConnectHandler(BLEDevice central) {
//   Serial.print("Connected event, central: ");
//   Serial.println(central.address());
// }

// void blePeripheralDisconnectHandler(BLEDevice central) {
//   Serial.print("Disconnected event, central: ");
//   Serial.println(central.address());
// }

// void maneuverCharacteristicSubscribed(BLEDevice central, BLECharacteristic characteristic) {
//   CharacteristicSubscribed = true;
// }

// void maneuverCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

//     switch (maneuverCharacteristic.value()){
//       case 2000 :
//         ShutDown = true;
//         break;
      
//       case 2001 : {
//         ShutDown = false;
//         break;
      
//       case 2002 :
//         SafetyOn = true;
//         break;
      
//       case 2003 :
//         SafetyOn = false;
//         break;

//       case 2004:
//         Calibrated = true;
//         Calibrating = false;
//         break;
//       }
//       case 2005:
//         if (!Calibrated) {
//           maxThrottle();
//           Calibrating = true;
//         }
//         break;
//       case 2006:
//         if (!Calibrated) {
//           minThrottle();
//           Calibrating = true;
//         }
//         break;
//     }

//     // if(Calibrating){
//     //   //Do not listen to ipad
//     //   return;
//     // }

//     if (! ShutDown) {
//       if(SafetyOn){

//         if(maneuverCharacteristic.value() > 500 && maneuverCharacteristic.value() < 2000) {
//           ipadValue = 500;
//           return;
//         }
//         ipadValue = maneuverCharacteristic.value();

//       }else{
//           ipadValue = maneuverCharacteristic.value();
//       }

//       if (ipadValue == previousIpadValue) {
//         Hover = true;
//         AltSetPoint = height; 
//       }
//       else{
//         Hover = false;
//       }
//       previousIpadValue = ipadValue;
//     }
// }