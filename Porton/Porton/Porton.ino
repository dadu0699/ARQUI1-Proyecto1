#include <Servo.h>

// SERVO MOTOR
int PORTON = 2;
Servo motor;
int posicion = 0;
int portonAbierto = 4;//led porton abierto ROJO
int portonCerrado = 7;//led porton cerrado AMARRILLO
int WIFI = 12; //Señal de la app
void setup() {
//Servo
  pinMode(PORTON, OUTPUT);
  pinMode(portonAbierto, OUTPUT);
  pinMode(portonCerrado, OUTPUT);
  pinMode(WIFI, INPUT);
}

void loop() {

 controlPorton();

}

void controlPorton() {
if (WIFI == HIGH){ //encender led por 6 seg al terminar se cierra el porton
  abrirPorton();
  digitalWrite(portonAbierto, HIGH); //No enciende la led Roja
  delay(6000);
  digitalWrite(portonAbierto, LOW);
  WIFI = LOW;
} else if (WIFI == LOW){        //Controlar que cuando se apague si se apague y no esperar a los 6 seg
  cerrarPorton();
  digitalWrite(portonCerrado, HIGH);
  delay(3000);
  digitalWrite(portonCerrado, LOW);
} else { // apagar led roja, cerrar porton. led amarilla y buzer encendido 3 seg
   cerrarPorton();
  digitalWrite(portonCerrado, HIGH);
  delay(3000);
  digitalWrite(portonCerrado, LOW);
  }
}

void abrirPorton(){             // 2 vueltas derecha
   motor.attach(PORTON);
  for (int i = 0; i <= 720; i++) {
    motor.write(i);
    delay(25);
  }
}

void cerrarPorton(){      // 2 vueltas izquierda
 motor.attach(PORTON); 
  for (int i = 719; i>0; i--){
    motor.write(i);
    delay(25);
  }
}
