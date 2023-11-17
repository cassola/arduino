// US barra DigiGlaze
/*Caja para US solo con ida. El retorno se hace a mano
La tracción es un hilo que se enrrolla en el eje del motor, y se une al carro
con una pinza para que se pueda solar en caso de se enganche con algo*/
#include <EEPROM.h>
#include <AccelStepper.h>

#define SPEAKER_PIN 10
#define NOTA 196
#define PIN_START 2
#define PIN_STOP 3
#define LED_RUN 13
#define LED_CERO 12
#define LED_FIN 11
#define PIN_HALFT 7
#define VELOCIDAD_MAX 5000
#define VELOCIDAD_SET 11 //11 es una hora. (38,216voltes x 1000 imp) / 3600 seg =~ 11 step/s
#define LONGUITUD 38216 //38216 toda la barra (1200mm / 31,4 mm/volta) x 1000imp/volta
#define BUZZER false //true o false

const int startPin = PIN_START;
const int stopPin = PIN_STOP;
const int ledRun = LED_RUN;   //roig
const int ledCero = LED_CERO; //groc
const int ledFin = LED_FIN;   //verd
const int halfPin = PIN_HALFT;


volatile bool startStado = false;
volatile bool stopStado = false;

bool run = false;
bool runCero = false;
bool runPausa = false;
bool buzzer = BUZZER;

unsigned long tope = LONGUITUD;
unsigned long posicion = 0;
unsigned long retardo = 3000;
unsigned long lapso = 0;
unsigned long miliseg = 0;
unsigned long topeEEPROM = LONGUITUD;
bool halfState = HIGH;

AccelStepper stepper(1, 5, 6); // (Typeof driver: con 2 pins, STEP, DIR)

void setup()
 {
  Serial.begin(9600);
  //Modo medio paso:
  pinMode(halfPin, OUTPUT);
  pinMode(startPin, INPUT);
  pinMode(stopPin, INPUT);
  pinMode(ledRun, OUTPUT);
  pinMode(ledFin, OUTPUT);
  pinMode(ledCero, OUTPUT);

  pinMode(SPEAKER_PIN, OUTPUT);
  
  EEPROM.get(0, topeEEPROM);
  Serial.print("Tope EEPROM: ");Serial.println(topeEEPROM);
  
  if (topeEEPROM == 0 || topeEEPROM > 40000){
   	EEPROM.put(0, tope);
  }else{
  	tope = topeEEPROM;
  }
  
  
  String teststr;
  int topeNuevo = 0;    
  Serial.print("Tope guardado es: ");Serial.println(tope);
  Serial.println("Enter nuevo tope:");
  Serial.setTimeout(3000);

  //while (Serial.available() == 0) {};     
  teststr = Serial.readString();
  teststr.trim();                       
  topeNuevo = teststr.toInt();
  if(topeNuevo!=0)
  {
    tope = topeNuevo;
    EEPROM.put(0, tope);
    Serial.print("Tope actual es: ");Serial.println(tope);
    delay(2000);
  }
  Serial.print("Tope guardado continua: ");Serial.println(tope);

  //Les interrupcions:
  
  attachInterrupt( digitalPinToInterrupt(stopPin), ServicioStopFall, FALLING);
  attachInterrupt( digitalPinToInterrupt(startPin), ServicioStartFall, FALLING);

  stepper.setCurrentPosition(posicion);
 
  digitalWrite(halfPin, halfState);

 }

void loop()
 {

  miliseg = millis();
  
  bool valStart = digitalRead(startPin);
  bool valStop = digitalRead(startPin);
  
  posicion = stepper.currentPosition();
  //Enciende los LEDs como toca 
  if(posicion == 0 && (valStart != 0)){
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
    digitalWrite(ledRun, ((miliseg & 512) == 0) );
  }
  
 
  // Continua Start
  if (startStado && (posicion < tope) && (posicion != 0))
  { 
   startStado = false;
   if(run){
    Serial.println("En pausa");
    run = false;
    }else{
    Serial.println("Continua Start");
    runPausa = true;
    lapso = miliseg;
    noTone(SPEAKER_PIN);
   }
  }
  
  //Start de cero
  if (startStado && (posicion == 0))
  { Serial.println("Start de cero");
   startStado = false;
   runCero = true;
   lapso = millis();
   run = false;
  }
  
  if ((runCero || runPausa) && (valStart == 0) ) {
   	
    if((miliseg & 512) == 0){
      if(buzzer){tone(SPEAKER_PIN, NOTA);
      }else{digitalWrite(ledCero, HIGH);}
    }else{
      if(buzzer){noTone(SPEAKER_PIN);
      }else{digitalWrite(ledCero, LOW);}
    }
    
    if ((miliseg - lapso) > retardo){
      run = true; 
      runCero = false;
      runPausa = false;
      if(buzzer){noTone(SPEAKER_PIN);
      }else{digitalWrite(ledCero, LOW);}
    }
   
  }else{noTone(SPEAKER_PIN);}
  
  //Stop
  if (stopStado)
  { 
   Serial.println("Stop");
   stopStado = false;
   run = false;
   posicion = 0;
   stepper.setCurrentPosition(posicion);
   
  }
  
  if (run)
  {
       
    posicion++;  
    stepper.setMaxSpeed(VELOCIDAD_MAX); 
    stepper.setSpeed(VELOCIDAD_SET);
    stepper.run();
  
    if (posicion >= tope)
    {
      run = false;
      stepper.setCurrentPosition(posicion); 
    }
    
  }
   
 } // loop

//Les interrupcions
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