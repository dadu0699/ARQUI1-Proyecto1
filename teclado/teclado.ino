#include <Keypad.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

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
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16,2);
  lcd.clear();

  // Declaracion de emojis
  lcd.createChar(0, candado);
  lcd.createChar(1, smiley);
  lcd.createChar(2, caritaCalavera);

  mensajePrincipal();
}

void loop() {
  if (!loggeado) {
    login();
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
