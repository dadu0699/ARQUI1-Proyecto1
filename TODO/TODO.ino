#include <Keypad.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Servo.h>

// LiquidCrystal ------------------------
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7
// END LiquidCrystal --------------------

// Keypad ------------------------
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
  {'7', '8', '9', 'A'},
  {'4', '5', '6', 'B'},
  {'1', '2', '3', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {2, 3, 4, 5};
byte colPins[cols] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
// END Keypad --------------------

// SERVO MOTOR -----------------------
Servo motor;
int PORTON = 25;
int posicion = 0;
int portonAbierto = 24; //led porton abierto ROJO
int portonCerrado = 23; //led porton cerrado AMARRILLO
//int buzzerP = 22; //buzzer de cerrado
// END SERVO MOTOR -------------------


/* PINES PARA LA BARRA TRANSPORTADORA */
int IO1 = 10;
int IO2 = 11;
int IO3 = 12;
int IO4 = 13;

int BLAB1 = 43; // Señal de Bluetooth (Quitar DESPUES)
int BLAB2 = 44; // Señal de Bluetooth (Quitar DESPUES)
int LAB1 = 41;
int LAB2 = 42;

/* VARIABLES PARA LA BARRA TRANSPORTADORA */
int paso = 4;
int Cpasos = 0;
int horario = 1;
int velocidad = 1000;
boolean Lab1 = false;
boolean Lab2 = false;

const int unPaso[4] = {
  B1000,
  B0100,
  B0010,
  B0001
};

// EMOJIS --------------
byte candado[8] = {
  B11111,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111
};
byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};
byte caritaCalavera[8] = {
  B11111,
  B10101,
  B10101,
  B11111,
  B01110,
  B10001,
  B11111,
  B11111
};
// END EMOJIS ----------

// LOGIN ----------------
boolean loggeado = false;
int contadorIntentos = 0;
String pass = "";
// END LOGIN ------------

void setup() {
  Serial.begin(9600);

  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();

  // Declaracion de emojis
  lcd.createChar(0, candado);
  lcd.createChar(1, smiley);
  lcd.createChar(2, caritaCalavera);

  //Servo
  pinMode(PORTON, OUTPUT);
  pinMode(portonAbierto, OUTPUT);
  pinMode(portonCerrado, OUTPUT);


  // Barra T 
  pinMode(BLAB1, INPUT);  // Señal de Bluetooth (Quitar DESPUES)
  pinMode(BLAB2, INPUT);  // Señal de Bluetooth (Quitar DESPUES)
  pinMode(LAB1, INPUT);
  pinMode(LAB2, INPUT);

  pinMode(IO1, OUTPUT);
  pinMode(IO2, OUTPUT);
  pinMode(IO3, OUTPUT);
  pinMode(IO4, OUTPUT);


  mensajePrincipal();
}

void loop() {
  if (!loggeado) {
    // login();
    //controlPorton();
    barraTransportadora(); 
  } else {
    controlPorton();
  }
}

void mensajePrincipal() {
  lcd.home();
  lcd.setCursor(1, 0);
  lcd.write(byte(0));
  lcd.print(" BIENVENIDO ");
  lcd.write(byte(0));
  delay(5000);
  lcd.clear();
}

void login() {
  lcd.setCursor(0, 0);
  lcd.print("DIGITE SU CLAVE");

  if (pass.length() <= 8) {
    lcd.setCursor(0, 1);
    lcd.print(pass);

    char key = keypad.getKey();
    if (key) {
      pass += key;
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MAX 8 CARACTERES");
    delay(2000);
    pass = "";
  }
}


void controlPorton() {
  posicion = 1;

  if (posicion == 1) { //encender led por 6 seg al terminar se cierra el porton
    Serial.println(">> HIGH");

    abrirPorton();

    Serial.println(">> LED ROJA");
    digitalWrite(portonAbierto, HIGH); //No enciende la led Roja
    delay(6000);
    digitalWrite(portonAbierto, LOW);

    posicion = 0;
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


// BARRA TRANSPORTADORA 
void barraTransportadora() {
  if (digitalRead(BLAB1)) {
    Lab1 = true;
  }

  if ( digitalRead(BLAB2)) {
    Lab2 = true;
  }

  if (Lab1) {
    haciaLab2();
  }
  if (Lab2) {
    haciaLab1();
  }

}

void haciaLab1() { // HACIA LA DERECHA SENTIDO HORARIO
  delay(100);
  horario = 1;
  Cpasos = -1;

  if (digitalRead(LAB2) == HIGH) {
    while (digitalRead(LAB1) == LOW) {
      secuenciaUnPaso();
      
    }
    Serial.println("Llego el paquete al Laboratorio 1"); 
    Lab2 = false;
  } else {
    Serial.println("No se ha cargado ningun paquete en el Laboratorio 2");
  }


}

void haciaLab2() { // HACIA LA IZQUIERDA SENTIDO ANTIHORARIO
  delay(100);
  horario = 0;
  Cpasos = paso;
  if (digitalRead(LAB1) == HIGH) {
    while (digitalRead(LAB2) == LOW) {
      secuenciaUnPaso();
    }
    Serial.println("Llego el paquete al Laboratorio 2");
    Lab1 = false;
  } else {
    Serial.println("No se ha cargado ningun paquete en Laboratorio 1");
  }
}

void secuenciaUnPaso() {
  if (horario == 1) {
    Cpasos++;
    if (Cpasos >= paso) {
      Cpasos = 0;
    }
  } else {
    Cpasos--;
    if (Cpasos < 0) {
      Cpasos = paso - 1;
    }
  }

  puerto(unPaso[Cpasos], IO1, IO4);
  paso = 4 ;
  delay(velocidad);
}

void puerto(int bits, int inicio, int fin) {
  for (int i = inicio; i <= fin; i++) {
    digitalWrite(i, bitRead(bits, i - inicio));
  }
}
