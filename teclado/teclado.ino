#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// LiquidCrystal ------------------------
const int rs = 13;
const int en = 12;
const int d4 = 11;
const int d5 = 10;
const int d6 = 1;
const int d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
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
int i = 0;
char arr[10];
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
// END EMOJIS ----------

// LOGGIN ----------------
boolean loggeado = false;
int contadorIntentos = 0;
String pass = "";
// END LOGGIN ------------

void setup() {
  // Declaracion de emojis
  lcd.createChar(0, candado);
  lcd.createChar(1, smiley);

  lcd.begin(16, 2); // Se indica dimensiones de pantalla

  // MENSAJE PRINCIPAL
  lcd.home();
  lcd.setCursor(1, 0);
  lcd.write(byte(0));
  lcd.print(" BIENVENIDO ");
  lcd.write(byte(0));
  delay(5000);
  lcd.clear();
  // TERMINA MENSAJE PRINCIPAL
}

void loop() {
  if (!loggeado) {
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
      delay(2500);
      pass = "";
    }
  }
}
