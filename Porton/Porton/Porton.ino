#include <pitches.h>
#include <Servo.h>

// SERVO MOTOR
int PORTON = 2;
Servo motor;
int posicion = 0;
int portonAbierto = 4;//led porton abierto ROJO
int portonCerrado = 7;//led porton cerrado AMARRILLO
int WIFI = 12; //Señal de la app
int buzzerP = 8; //buzzer de cerrado

int estrellita[] = {
    NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, 
    NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
   
};
int estrellitaN[] = {
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4
};


void setup() {
  Serial.begin(9600);

  //Servo
  pinMode(PORTON, OUTPUT);
  pinMode(portonAbierto, OUTPUT);
  pinMode(portonCerrado, OUTPUT);
  pinMode(WIFI, INPUT);
  pinMode(buzzerP, OUTPUT);

   cancionEstrellita();
}

void loop() {
  //controlPorton();
 
}

void controlPorton() {
  posicion = digitalRead(WIFI);

  if (posicion == 1) { //encender led por 6 seg al terminar se cierra el porton
    Serial.println(">> HIGH");

    abrirPorton();

    Serial.println(">> LED ROJA");
    digitalWrite(portonAbierto, HIGH); 
    for (int i = 0; i <= 6000; i++) {
      posicion = digitalRead(WIFI);
      if (posicion != 1) {
        break;
      }
      delay(1);
    }
    digitalWrite(portonAbierto, LOW);

    posicion = LOW;
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
  digitalWrite(buzzerP, HIGH);
  digitalWrite(portonCerrado, HIGH);
  delay(3000);
  digitalWrite(buzzerP, LOW);
  digitalWrite(portonCerrado, LOW);
  
}

void cancionEstrellita(){
  for (int Nota = 0; Nota < 14; Nota++) {    
    int Duracion = 1000 / estrellitaN[Nota];
    tone(8, estrellita[Nota], Duracion);
int pausa = Duracion * 1.40;
       delay(pausa);
    noTone(8);
  }
}
