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

  mensajePrincipal();
}

void loop() {
  if (!loggeado) {
    // login();
    controlPorton();
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
