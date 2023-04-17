#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <Servo.h>
/* 
 *  CODIGO CON LOS SERVOS, BLUETOOTH, INFRAS, 74HC595, LCD CON I2C Y CUENTA REGRRESIVA
 * 
 *  Se esta probando en fisico (omitiendo la parte de los servos)
 *  no funciona la parte del encendido de leds
*/


#define infra1 A0
#define infra2 A1 
#define infra3 A2 
#define infra4 A3 
#define infra5 0

#define lachtPin 8 // es la patita SH_CP
#define clockPin 13 // es la patita DS 
#define dataPin 12 //es la patita ST_CP

#define incremento 5
#define inicio 4
/*
Servo miservo_1; // servo 1 derecha izquierda
Servo miservo_2; //Servo 2 y 3 hacen lo msimo por que es para estabilizacion
Servo miservo_3;//servo 2 y 3 arriab y abajo
*/
LiquidCrystal_I2C lcd (0x3F,16,2);

volatile int numViajes = 0;
volatile int aceptacion = 0;

volatile int flagRegresion = 0;
volatile int contadorViajes = 0;

volatile int activacionJuego = 0; //flag para iniciar el juego
volatile int aleatorio = 0;
volatile byte myByte = 0;

volatile int regresion;
volatile int aux = 0;

int grados = 90;
int state = 0;

void cantViajes();
void cuentaRegresiva();
void juego();
void finDelJuego();

void setup(){

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print ("  Bienvenido a  ");
  lcd.setCursor(0, 1);
  lcd.print ("  Super Guanti  ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print ("Una creacion de:");
  lcd.setCursor(0, 1);
  lcd.print ("     M.A.L.     ");
  delay(2000);
  
  pinMode(incremento, INPUT);
  pinMode(inicio, INPUT);

  pinMode(infra1, INPUT);
  pinMode(infra2, INPUT);
  pinMode(infra3, INPUT);
  pinMode(infra4, INPUT);
  pinMode(infra5, INPUT);


  pinMode(lachtPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  /*miservo_1.attach(3,750,1800); // EL 2 ES EL PIN DONDE ESTA CONECTADO EL 750 ES EL 0 Y EL 1800 POR LOS 180°
  miservo_1.write(grados);

  miservo_2.attach(5,750,1800); // EL 2 ES EL PIN DONDE ESTA CONECTADO EL 750 ES EL 0 Y EL 1800 POR LOS 180°
  miservo_2.write(grados);

  miservo_3.attach(6,750,1800); // EL 2 ES EL PIN DONDE ESTA CONECTADO EL 750 ES EL 0 Y EL 1800 POR LOS 180°
  miservo_3.write(grados);*/
}

void loop(){

  if(aceptacion == 0){ 

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cant de viajes:");
    cantViajes();
  }
  else{
    
    switch(flagRegresion){  
      case 0:
        lcd.setCursor(0,0);
        lcd.print("El juego inicia");
        lcd.setCursor(0,1);
        lcd.print("     en: ");
        flagRegresion = 1;
      break;
      case 1:
        Timer1.initialize(1000000);//1s
        Timer1.attachInterrupt(cuentaRegresiva);
        lcd.setCursor(10,1);
        lcd.print(regresion);
      break;
      case 2:
        lcd.setCursor(0,0);
        lcd.print("    A JUGAR!    ");
        lcd.setCursor(0,1);
        lcd.print("                ");


  /*      if(Serial.available() > 0){
    
          state = Serial.read();
          Serial.write(state);   
          }

        if(state == '1'){

           grados++;

            if(grados >= 180){ // Protege el sero de no sobreexigirlos 

               grados = 180;  
         }
           miservo_1.write(grados);
           delay(10);
           state = 0;
           }
  
        if(state == '2'){

          grados--;
          if(grados <= 0){ //Protege el servo

              grados = 0;
          }
       miservo_1.write(grados);
       delay(10);
       }
      if(state == '3'){

        grados++;

      if(grados >= 180){ // Protege el sero de no sobreexigirlos 

        grados = 180;
      }
       miservo_2.write(grados);
       delay(10);
  
      miservo_3.write(grados);
      delay(10);
      }
      if(state == '4'){

        grados--;
        if(grados <= 0){ //Protege el servo

          grados = 0;
        }
        miservo_2.write(grados);
        delay(10);
  
        miservo_3.write(grados);
        delay(10);
        }*/        
        
        if(activacionJuego == 0){
          juego();
        }
        if(digitalRead(infra1) == LOW){ 
          delay(300);//retencion del pulsador
          contadorViajes++;
        
          if(contadorViajes < numViajes){
            activacionJuego = 0;
            juego();
          }
          if(contadorViajes >= numViajes){
            lcd.clear();
            finDelJuego();
          }
        }
      break;
     }
  }
}


void cantViajes() { 

  do{

    if(digitalRead(incremento) == LOW){

      numViajes++; //Esto se podria hacer de 5 en 5
      delay(300); //Este delay hay que sacarlo en un futuro, es para la retencion del pulsador
      lcd.setCursor(0,1);
      lcd.print(numViajes);
    }

  }while(digitalRead(inicio) == HIGH);
  
  if(digitalRead(inicio) == LOW){
    delay(300);
    aceptacion = 1;
    lcd.clear(); //el clear esta aca para que se ejecute solo una vez
  }
}
void juego(){
/* En aleatorio se guarda el numero que genera la funcion random
 * 
 * myByte esta declarado como byte porque tiene 8 posiciones 
 * 
 * en myByte se guarda un 1 desplazado la cantidad de veces que indique el numero guardado en aleatorio
 * 
 * myByte se manda a la salida y listo
 * 
 * se tiene que usar MSBFIRST para que funcione
 * 
 * activacionJuego es una flag que se implementa para entrar y salir de la funcion juego y que no se quede el programa trabado en el encendido de los leds
 */
 

 
  aleatorio = random(0, 5);
  myByte = 0;
  myByte = 1<<aleatorio;
  
  digitalWrite(lachtPin, LOW); //sube cada 8 ciclos
  shiftOut(dataPin, clockPin, MSBFIRST, myByte);//le mandamos cuatro datos primero la data despues el pin del reloj , el bit menos significativo
  digitalWrite(lachtPin, HIGH);

  activacionJuego = 1;
}
void cuentaRegresiva(){
  /* Esto funciona de forma interrumpida por el Timer1 cada 1seg
   *
   * regresion sera la cuenta propiamente dicho 
   * cuando llegue a 0 se cambiara flagRegresion y ya no se volvera a esta funcion (se utiliza <=0 porque sino empieza a mostrar numeros negativos)
  */
  regresion = 5 - (aux++);
  
  if(regresion <= 0){
    flagRegresion = 2;
  }
}
void finDelJuego(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Felicidades!  ");
  lcd.setCursor(0,1);
  lcd.print(" Fin del juego! ");
  delay(2000); //hay que sacar estos delay
}