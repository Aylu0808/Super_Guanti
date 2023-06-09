/*
 * Programa para el atmega esclavo 
 * 
 * Falta agregarle una cuenta que muestre el tiempo total que se tardo en terminar el juego
 * Tambien los contadores que muestren la cantidad de veces que se pulso cada dedo 
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <VarSpeedServo.h>


#define infra1 A0
#define infra2 A1
#define infra3 A2
#define infra4 A3
#define infra5 4

#define pinLatch 7  // 74hc595
#define clockPin 8 
#define dataPin 5  

#define incremento 10
#define inicio 12 //revisar si esto coincide con la conexion de la plaqueta

VarSpeedServo miservo_1, miservo_2, miservo_3;

LiquidCrystal_I2C lcd(0x3F, 16, 2); //Va conectado al A5 y al A4

volatile int t = 0;
volatile int estadoRetencionIncremento = 0; 
volatile int estadoRetencionInicio = 0; 

volatile bool flagRetencionIncremento = FALSE;
volatile bool flagPulsoIncremento = FALSE;

volatile bool flagRetencionInicio = FALSE;
volatile bool flagPulsoInicio = FALSE;



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

volatile int flagFinal = 0;

void cantViajes();
void cuentaRegresiva();
void juego();
void finDelJuego();

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

  digitalWrite(pinLatch, LOW);              
  shiftOut(dataPin, clockPin, MSBFIRST, 0); 
  digitalWrite(pinLatch, HIGH);
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
        flagRegresion = 2; //me salteo el caso 1
      break;
      case 1:
        /*
        * La funcion cuentaRegresiva empieza a interrumpirse con el timer a partir de ahora
        * 
        * tambien se muestra en pantalla
        */
        // Timer1.initialize(1000000); // 1s
        // Timer1.attachInterrupt(cuentaRegresiva);

        // lcd.setCursor(10, 1);
        // lcd.print(regresion);
      break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("    A JUGAR!    ");
        lcd.setCursor(0, 1);
        lcd.print("                ");

        if(activacionJuego == 0){
          juego();
        }
        if (digitalRead(infra1) == HIGH || digitalRead(infra2) == HIGH || digitalRead(infra3) == HIGH || digitalRead(infra4) == HIGH || digitalRead(infra5) == HIGH)
        {
          // delay(500); // retencion del pulsador
          contadorViajes=7;

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

        /* Mientras el juego esta funcionando, se mandan instrucciones a la grua 
         * dependiendo de los datos que se reciben del bluetooth

         * El funcionamiento de los servos tambien se probo con un switch y la variable state (funciona)

         * El servo 3
        */
        if (Serial.available()){

          state = Serial.read(); 

          ///SERVO 1 -- DERECHA IZQUIERDA -- 9///
          if(state == '1'){
            grados1++;
            if(grados1 >= 180){
              grados1 = 180;
            }
            
            miservo_1.write(grados1, 0);
            
          }
          if(state == '2'){
            grados1--; 
            if(grados1 <= 0){
              grados1 = 0;
            }
            
            miservo_1.write(grados1, 0);
            
          }

          ///SERVO 2 -- ADELANTE ATRAS -- 6///
          if(state == '3'){
            grados2++;
            if(grados2 >= 180){
              grados2 == 180;
            }
            
            miservo_2.write(grados2, 200);
            
          }
          if(state == '4'){
            grados2--;
            if(grados2 >= 0){
              grados2 == 0;
            }
            
            miservo_2.write(grados2, 200);
            
          }

          ///SERVO 3 -- ABAJO -- 11///
          if(state == '5'){    
            grados3--;        
            if(grados3<=0){
              grados3 = 90;
            }
              
            miservo_3.write(grados3, 0);
            
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
 estadoRetencionIncremento = 1;
 estadoRetencionInicio = 1;

 switch(estadoRetencionIncremento){
    case 1:
      flagPulsoIncremento = FALSE;

      if(digitalRead(incremento) == HIGH)
        estadoRetencionIncremento = 1;

      if(digitalRead(incremento) == LOW){
        t = 0;
        estadoRetencionIncremento = 2;
      }
    break;
    case 2:
      if(flagRetencionIncremento == FALSE)
        estadoRetencionIncremento = 2;
      if(flagRetencionIncremento == TRUE)
        estadoRetencionIncremento = 3;
    break;
    case 3: 
      if(digitalRead(incremento) == LOW){
        flagPulsoIncremento = TRUE;
        estadoRetencionIncremento = 1;
      }
      else{
        flagPulsoIncremento = FALSE;
        estadoRetencionIncremento = 1;
      }
    break;
  }
  switch(estadoRetencionInicio){
    case 1:
      flagPulsoInicio = FALSE;

      if(digitalRead(inicio) == HIGH)
        estadoRetencionInicio = 1;

      if(digitalRead(inicio) == LOW){
        t = 0;
        estadoRetencionInicio = 2;
      }
    break;
    case 2:
      if(flagRetencionInicio == FALSE)
        estadoRetencionInicio = 2;
      if(flagRetencionInicio == TRUE)
        estadoRetencionInicio = 3;
    break;
    case 3: 
      if(digitalRead(inicio) == LOW){
        flagPulsoInicio = TRUE;
        estadoRetencionInicio = 1;
      }
      else{
        flagPulsoInicio = FALSE;
        estadoRetencionInicio = 1;
      }
    break;
  }
  
  if(flagPulsoIncremento == TRUE && flagPulsoInicio == FALSE){
    numViajes++;
    lcd.setCursor(0,0);
    lcd.print("Cant:");
    lcd.print(numViajes);
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("Inicio:");
  }


  // if(digitalRead(inicio) = LOW) {
  //   // delay(300);
  //   aceptacion = 1;
  //   lcd.clear(); //el clear esta aca para que se ejecute solo una vez
  // }
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
// void cuentaRegresiva(){
//   /* Esto funciona de forma interrumpida por el Timer1 cada 1seg
//    *
//    * regresion sera la cuenta propiamente dicho
//    * cuando llegue a 0 se cambiara flagRegresion y ya no se volvera a esta funcion (se utiliza <=0 porque sino empieza a mostrar numeros negativos)
//   */
//   regresion = 5 - (aux++);

//   if (regresion <= 0){
    
//     flagRegresion = 2;
//   }
// }
void finDelJuego(){
/*Es el mensaje que se mostrara al finalizar el juego
 *
 * Se muestra un mensaje de felicitaciones
*/
  lcd.setCursor(0, 0);
  lcd.print("  Felicidades!  ");
  lcd.setCursor(0, 1);
  lcd.print(" Fin del juego! ");
  lcd.clear();
}
void tiempo(){
  t++;
  
  if(t >= 100)
    flagRetencion = TRUE;
  else
    flagRetencion = FALSE;
}