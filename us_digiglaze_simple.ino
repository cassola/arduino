#include <Stepper.h>

const int startPin = 3;
const int stopPin = 2;
const int ledRun = 13;
const int ledCero = 12;
const int ledFin = 11;
bool startStado = false;
bool stopStado = false;

bool run = false;
bool runCero = false;
int tope = 100;
int posicion = 0;
int retardo = 1000;
double lapso = 0;



Stepper motor = Stepper(500, 5, 6);


void setup()
 { Serial.begin(9600);
 	
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
