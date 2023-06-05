/*

  SE USA PULL DOWN

 * CODIGO CON :
 * - I2C
 * - 74hc595
 * - Bluetooth
 * - Servos
 * - Infras
 * 
 * - Cuenta regresiva
 *
 * 
 * CAMBIAR LOS PINES
 *  
 * 
 * NO SE PROBO
 * - cuenta general
 * - conteo de cada dedo
 * 
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <VarSpeedServo.h>


//queda libre el pin 2, 12, 13
//0 y 1 son Rx y tx son para el bluetooth

#define infra1 A0
#define infra2 A1
#define infra3 A2
#define infra4 A3
#define infra5 4

#define pinLatch 7  // es la patita SH_CP
#define clockPin 8 // es la patita DS
#define dataPin 9  // es la patita ST_CP

#define incremento 10
#define inicio 12 //revisar si esto coincide con la conexion de la plaqueta

VarSpeedServo miservo_1, miservo_2, miservo_3;

LiquidCrystal_I2C lcd(0x3F, 16, 2); //Va conectado al A5 y al A4

volatile int numViajes = 0;
volatile int aceptacion = 0;

volatile int flagRegresion = 0;
volatile int contadorViajes = 0;
volatile int numAnterior = 0;

volatile int activacionJuego = 0; // flag para iniciar el juego
volatile int aleatorio = 0;

volatile int regresion;
volatile int aux = 0;

volatile int grados1 = 0;
volatile int grados2 = 0;
volatile int grados3 = 0;
volatile int state = 0;
/*
volatile int seg = 0;
volatile int min = 0;
volatile int hs = 0;*/

volatile int flagFinal = 0;

// volatile int indice = 0;
// volatile int anular = 0;
// volatile int mayor = 0;
// volatile int menique = 0;

volatile int rapido = 200;
volatile int normal = 0;

void cantViajes();
void cuentaRegresiva();
void juego();
void finDelJuego();

void conteoGeneral();
void derecha(); 
void izquierda();
void adelante();
void atras();
void abajo();

void setup()
{
  Serial.begin(57600); 

  lcd.init();
  lcd.backlight();
  //Mensaje de bienvenida
  lcd.setCursor(0, 0);
  lcd.print("  Bienvenido a  ");
  lcd.setCursor(0, 1);
  lcd.print("  Super Guanti  ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Una creacion de:");
  lcd.setCursor(0, 1);
  lcd.print("     M.A.L.     ");
  delay(1000);

  pinMode(incremento, INPUT);
  pinMode(inicio, INPUT);

  pinMode(infra3, INPUT);
  pinMode(infra2, INPUT);
  pinMode(infra3, INPUT);
  pinMode(infra4, INPUT);
  pinMode(infra5, INPUT);

  pinMode(pinLatch, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  miservo_1.attach(9, 350, 2900); //servo base, derecha-izquierda
  miservo_1.write(grados1); 

  miservo_2.attach(6, 1000, 2000); //servo de la derecha, adelante-atras
  miservo_2.write(grados2); 

  miservo_3.attach(11, 1000, 2000); //servo de la izqueirda, abajo
  miservo_3.write(grados3);
}

void loop(){

  if(aceptacion == 0) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cant de viajes:");
    cantViajes();
  }
  else 
  {
    switch (flagRegresion){
      /*
      * flagRegresion se utiliza para ir avanzando en los pasos del programa 
      */
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("El juego inicia");
        lcd.setCursor(0, 1);
        lcd.print("     en: ");
        flagRegresion = 1;
      break;
      case 1:
        /*
        * La funcion cuentaRegresiva empieza a interrumpirse con el timer a partir de ahora
        * 
        * tambien se muestra en pantalla
        */
        Timer1.initialize(1000000); // 1s
        Timer1.attachInterrupt(cuentaRegresiva);

        lcd.setCursor(10, 1);
        lcd.print(regresion);
      break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("    A JUGAR!    ");
        lcd.setCursor(0, 1);
        lcd.print("                ");

        // Timer1.initialize(1000000); // 1s
        // Timer1.attachInterrupt(conteoGeneral);

        while(activacionJuego == 0){
          juego();
        }
        if (digitalRead(infra1) == HIGH || digitalRead(infra2) == HIGH || digitalRead(infra3) == HIGH || digitalRead(infra4) == HIGH || digitalRead(infra5) == HIGH)
        {
          delay(500); // retencion del pulsador
          contadorViajes++;

          if (contadorViajes < numViajes)
          {
            activacionJuego = 0;
          }
          if (contadorViajes >= numViajes)
          {
            flagFinal = 1;
            lcd.clear();
            finDelJuego();
          }
        }

        /* Mientras el juego esta funcionando, se llama a las funciones
         * que manejan la grua dependiendo de los datos que se reciben del bluetooth
         *
         * Despues se podria probar con las funciones que estan declaradas abajo, (derecha, izquierda, atras, adelante, abajo)
        */

        if (Serial.available()){

          state = Serial.read(); 

          ///SERVO 1 -- DERECHA IZQUIERDA -- 9///
          if(state == '1'){
            grados1++;
            if(grados1 >= 180){
              grados1 = 180;
            }
            
            miservo_1.write(grados1, normal);
            delay(10);
          }
          if(state == '2'){
            grados1--; 
            if(grados1 <= 0){
              grados1 = 0;
            }
            
            miservo_1.write(grados1, normal);
            delay(10);
          }

          ///SERVO 1 -- DERECHA IZQUIERDA -- 9///
          if(state == '3'){
            grados2++;
            if(grados2 >= 180){
              grados2 == 180;
            }
            
            miservo_2.write(grados2, normal);
            delay(10);
          }
          if(state == '4'){
            grados2--;
            if(grados2 >= 0){
              grados2 == 0;
            }
            
            miservo_2.write(grados2, normal);
            delay(10);
          }

          ///SERVO 3 -- ABAJO -- 11///
          if(state == '5'){    
            grados3--;        
            if(grados3<=0){
              grados3 = 90;
            }
              
            miservo_3.write(grados3, rapido);
            delay(15);
          }  
        }
      break;
    }
  }
}
void cantViajes(){
  /* Al pulsar el boton de incremento se aumenta la cantidad de viajes
   *
   * Si se pulsa el boton inicio se termina la configuracion de cantidad de viajes e inicia la cuenta regresiva
  */
  do{
    if(digitalRead(incremento) == LOW){
      delay(300);

      numViajes++;
      lcd.setCursor(0,1);
      lcd.print(numViajes);
    } 
  }while(digitalRead(inicio) == HIGH);

  if (digitalRead(inicio) == LOW) {
    delay(300);
    aceptacion = 1;
    lcd.clear(); //el clear esta aca para que se ejecute solo una vez
  }
}
void juego() {
  /* 
   * Genera un numero aleatorio, el cual no puede ser igual al anterior
   * 
   * Dependiendo de ese numero se marca que salida debe tener el 74hc595
   * 
   * Los numeros en binario tienen un unico cero dentro de los primeros 5 digitos
   * 
   * Se ulizan las salidas QA, QB, QC, QD, QE del 74hc595
   * 
   * activacionJuego es una flag que se modifica para salir de la funcion juego y volver al codigo principal
  */
  do{
    aleatorio = random(0, 5);
  }while(aleatorio == numAnterior);
  
  switch (aleatorio)
  {
    case 0:
      digitalWrite(pinLatch, LOW);              
      shiftOut(dataPin, clockPin, MSBFIRST, 1); 
      digitalWrite(pinLatch, HIGH);
      numAnterior = 0;
      activacionJuego = 1;
    break;
    case 1:
      digitalWrite(pinLatch, LOW);              
      shiftOut(dataPin, clockPin, MSBFIRST, 2); 
      digitalWrite(pinLatch, HIGH);
      numAnterior = 1;
      activacionJuego = 1;
    break;
    case 2:
      digitalWrite(pinLatch, LOW);              
      shiftOut(dataPin, clockPin, MSBFIRST, 4); 
      digitalWrite(pinLatch, HIGH);
      numAnterior = 2;
      activacionJuego = 1;
    break;
    case 3:
      digitalWrite(pinLatch, LOW);              
      shiftOut(dataPin, clockPin, MSBFIRST, 8);
      digitalWrite(pinLatch, HIGH);
      numAnterior = 3;
      activacionJuego = 1;
    break;
    case 4:
      digitalWrite(pinLatch, LOW);               
      shiftOut(dataPin, clockPin, MSBFIRST, 16); 
      digitalWrite(pinLatch, HIGH);
      numAnterior = 4;
      activacionJuego = 1;
    break;
  }
}
void cuentaRegresiva(){
  /* Esto funciona de forma interrumpida por el Timer1 cada 1seg
   *
   * regresion sera la cuenta propiamente dicho
   * cuando llegue a 0 se cambiara flagRegresion y ya no se volvera a esta funcion (se utiliza <=0 porque sino empieza a mostrar numeros negativos)
  */
  regresion = 5 - (aux++);

  if (regresion <= 0){
    
    flagRegresion = 2;
  }
}
void finDelJuego(){

/*Es el mensaje que se mostrara al finalizar el juego
  
  Se muestra un mensaje de felicitaciones
  Se muestra el tiempo general desde que se inicio el juego hasta que se finaliza
  Se muestra la cantidad de veces que se pulso con cada dedo
*/
  lcd.setCursor(0, 0);
  lcd.print("  Felicidades!  ");
  lcd.setCursor(0, 1);
  lcd.print(" Fin del juego! ");
  delay(8000);
/*
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Su tiempo fue: ");
  lcd.setCursor(0, 1);
  lcd.print(hs":"min":"seg);
  delay(8000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Indice:");
  lcd.print(indice);
  lcd.setCursor(0,1);
  lcd.print("Anular:");
  lcd.print(anular);
  delay(8000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mayor:");
  lcd.print(mayor);
  lcd.setCursor(0,1);
  lcd.print("Meñique:");
  lcd.print(menique);
  delay(8000);*/
}
/*
void derecha(){
  grados1++;

  if(grados1 >= 180){  
    grados1 = 180;
  }
  
  miservo_1.write(grados1);
  delay(10);
}
void izquierda(){
  grados1--;
  
  if(grados1 <= 0){ 
    grados1 = 0;
  }
  
  miservo_1.write(grados1);
  delay(10);
}
void adelante(){
  grados2++;
  
  if(grados2 >= 180){ 
    grados2 = 180;
  }
  
  miservo_2.write(grados2);
  delay(10);
}
void atras(){
  grados2--;
  
  if(grados2 <= 0){ 
    grados2 = 0;
  }
  
  miservo_2.write(grados2);
  delay(10);
}
void abajo(){
   grados3--;        
      if(grados3<=0){
        grados3 = 90;
      }
        
      miservo_3.write(grados3, rapido);
      delay(15);
}
*/
/*
void conteoGeneral(){
  if(flagFinal == 0){
    seg++;
    if(seg == 60){
      min++;
      seg = 0;
      if(min == 60){
        hs++;
        min = 0;
      }
    }
  }
}
*/