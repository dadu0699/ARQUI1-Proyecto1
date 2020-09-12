#include <Servo.h>

// SERVO MOTOR
int PORTON = 2;
Servo motor;
int posicion = 0;
int portonAbierto = 4;//led porton abierto ROJO
int portonCerrado = 7;//led porton cerrado AMARRILLO
int WIFI = 12; //Señal de la app
//int buzzerP = 8; //buzzer de cerrado

void setup() {
  Serial.begin(9600);

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
  posicion = digitalRead(WIFI);

  if (posicion == 1) { //encender led por 6 seg al terminar se cierra el porton
    Serial.println(">> HIGH");

    abrirPorton();

    Serial.println(">> LED ROJA");
    digitalWrite(portonAbierto, HIGH); //No enciende la led Roja
    delay(6000);
    digitalWrite(portonAbierto, LOW);

    posicion = LOW;
    cerrarPorton();

  } else {       //Controlar que cuando se apague si se apague y no esperar a los 6 seg
    Serial.println(">> LOW");
    cerrarPorton();
  }
}

void abrirPorton() {            // 2 vueltas derecha
  motor.attach(PORTON);
  for (int i = 0; i <= 180; i++) {
    motor.write(i);
    delay(25);
  }
}

void cerrarPorton() {     // 2 vueltas izquierda
  motor.attach(PORTON);
  for (int i = 180; i >= 0; i--) {
    motor.write(i);
    delay(25);
  }

  Serial.println(">> LED AMARILLA");
  digitalWrite(portonCerrado, HIGH);
  delay(3000);
  digitalWrite(portonCerrado, LOW);
}
