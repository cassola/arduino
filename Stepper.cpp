#include <EEPROM.h>

#include <Stepper.h>



const int startPin = 3;
const int stopPin = 2;
const int ledRun = 13;
const int ledCero = 12;
const int ledFin = 11;
volatile bool startStado = false;
volatile bool stopStado = false;

bool run = false;
bool runCero = false;

unsigned int tope = 100;
unsigned int posicion = 0;
unsigned long retardo = 1000;
unsigned long lapso = 0;
unsigned int topeEEPROM = 0;



Stepper motor = Stepper(500, 5, 6);


void setup()
 {
  
  EEPROM.get(0, topeEEPROM);
  
  if (topeEEPROM == 0){
   	EEPROM.put(0, tope);
  }else{
  	tope = topeEEPROM;
  }
  
  Serial.begin(9600);
  String teststr;
  int topeNuevo = 0;    
  Serial.print("Tope guardado es: ");Serial.println(tope);
  Serial.println("Enter nuevo tope:");
  Serial.setTimeout(2000);
  //while (Serial.available() == 0) {};     
  teststr = Serial.readString();
  teststr.trim();                       
  topeNuevo = teststr.toInt();
  if(topeNuevo!=0)
  {
    tope = topeNuevo;
    Serial.print("Tope actual es: ");Serial.println(tope);
    delay(1000);
  }
  Serial.print("Tope guardado continua: ");Serial.println(tope);

  attachInterrupt( digitalPinToInterrupt(startPin), ServicioStartFall, FALLING);
  attachInterrupt( digitalPinToInterrupt(stopPin), ServicioStopFall, FALLING);

  pinMode(startPin, INPUT);
  pinMode(stopPin, INPUT);
  pinMode(ledRun, OUTPUT);
  pinMode(ledFin, OUTPUT);
  pinMode(ledCero, OUTPUT);
 }

void loop()
 {
  
  int valStart = digitalRead(startPin);
  int valStop = digitalRead(startPin);
  
  //Enciende los LEDs como toca 
  if(posicion == 0){
    digitalWrite(ledCero,HIGH);
    digitalWrite(ledFin, LOW);
    digitalWrite(ledRun, LOW);
  }
  if(posicion >= tope){
    digitalWrite(ledFin,HIGH);
    digitalWrite(ledCero, LOW);
    digitalWrite(ledRun,LOW);
  }
  if(run){
    digitalWrite(ledRun,HIGH);
    digitalWrite(ledCero,LOW);
  }
  if((posicion > 0) && (run == false) && (posicion < tope)){
    digitalWrite(ledRun, ((millis() & 512) > 0) );
  }
  
 
  // Continua Start
  if (startStado && (posicion < tope) && (posicion != 0))
  { Serial.println("Continua Start");
   startStado = false;
   run = !run;
  }
  
  //Start de cero
  if (startStado && (posicion == 0))
  { Serial.println("Start de cero");
   startStado = false;
   runCero = true;
   lapso = millis();
   run = false;
  }
  
  if (runCero && (valStart == 0) ) {
   	digitalWrite(ledCero, ((millis() & 256) > 0) );
    if ((millis() - lapso) > retardo){
      run = true;
      runCero = false;
      digitalWrite(ledCero,HIGH);
    }
      
    
  }
  
  //Stop
  if (stopStado)
  { Serial.println("Stop");
   stopStado = false;
   run = false;
   posicion = 0;
   
  }
  
  if (run)
  {
       
    posicion++;  
    motor.setSpeed(2);
    motor.step(1);
  
  
    if (posicion >= tope)
    {
      run = false;
    }
    
    Serial.print("posicion ");
    Serial.println(posicion);
  }
    
    
  
 } // loop


void ServicioStartFall()
 { 
  
  startStado = true;
  stopStado = false;
 } 
void ServicioStopFall()
 { 
 
  startStado = false;
  stopStado = true;
 } 