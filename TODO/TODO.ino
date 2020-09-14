#include <DHT.h>
#include <DHT_U.h>
#include <Keypad.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Servo.h>

#define DHTPIN 48//Pin 
#define DHTTYPE DHT22//Sensor
DHT dht(DHTPIN, DHTTYPE);

float temperatura;//Var para almacenar la lectura



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
int buzzerP = 22; //buzzer de cerrado
int WIFI = 53; //Señal de la app
// END SERVO MOTOR -------------------

/* PINES PARA LAS LUCES */
int LL1 = 26;
int LL2 = 27;
int LE = 28;
int LS = 29;
String texto = "";

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
byte candadoAbierto[8] = {
  B00111,
  B00001,
  B00001,
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
byte checkE[8] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};
byte ampersand[8] = {
  B01100,
  B10010,
  B10010,
  B10100,
  B01000,
  B10101,
  B10010,
  B01101
};
// END EMOJIS ----------

// LOGIN ----------------
int IO52LOG = 52;
boolean logueado = false;
int contadorIntentos = 0;
int registro = 0;
long randomNumber;
// END LOGIN ------------

// Usuario
struct Usuario {
  char identificacion[10];
  char contrasenia[8];
};

void setup() {
  Serial.begin(9600);

  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();

  // Declaracion de emojis
  lcd.createChar(0, candado);
  lcd.createChar(1, candadoAbierto);
  lcd.createChar(2, smiley);
  lcd.createChar(3, caritaCalavera);
  lcd.createChar(4, checkE);
  lcd.createChar(5, ampersand);

  //LUCES
  pinMode(LL1, OUTPUT);
  pinMode(LL2, OUTPUT);
  pinMode(LE, OUTPUT);
  pinMode(LS, OUTPUT);

  //Servo
  pinMode(PORTON, OUTPUT);
  pinMode(portonAbierto, OUTPUT);
  pinMode(portonCerrado, OUTPUT);
  // pinMode(buzzerP, OUTPUT);
  pinMode(WIFI, INPUT);

  // Barra T
  pinMode(BLAB1, INPUT);  // Señal de Bluetooth (Quitar DESPUES)
  pinMode(BLAB2, INPUT);  // Señal de Bluetooth (Quitar DESPUES)
  pinMode(LAB1, INPUT);
  pinMode(LAB2, INPUT);

  pinMode(IO1, OUTPUT);
  pinMode(IO2, OUTPUT);
  pinMode(IO3, OUTPUT);
  pinMode(IO4, OUTPUT);

  // LOGIN
  pinMode(IO52LOG, OUTPUT);
  // crearAdministrador();
  EEPROM.get(0, registro);
  randomSeed(registro);

  mensajePrincipal();
}

void loop() {

  temperatura = dht.readTemperature();
  Serial.print(temperatura);
  Serial.print(" C");
  Serial.print("|");
  Serial.print("ALGO");
  delay(2000);
  
  if (!logueado) {
    login();
  } else {
    controlarLuces();
    controlPorton();
    barraTransportadora();
  }
}

void limpiarEEPROM() {
  EEPROM.get(0, registro);
  for (int i = 0; i < registro; i++) {
    EEPROM.write(i, 0);
    delay(50);
  }
  Serial.println(">> MEMORIA LIMPIA");
}

bool buscarEEPROM(String usr, String pwd) {
  EEPROM.get(0, registro);
  Usuario temp;
  String rUSR;
  String rPWD;

  for (int i = sizeof(registro); i < registro; i += sizeof(Usuario)) {
    EEPROM.get(i, temp);
    rUSR = temp.identificacion;
    rPWD = temp.contrasenia;

    if (rUSR.equals(usr) && rPWD.equals(pwd)) {
      return true;
    }
    delay(100);
  }

  return false;
}

void crearAdministrador() {
  limpiarEEPROM();

  registro = 0;
  Usuario administrador = {
    "20180",
    "0106"
  };

  registro += sizeof(registro);
  EEPROM.put(registro, administrador);
  delay(50);

  registro += sizeof(Usuario);
  EEPROM.update(0, registro);
  delay(50);

  Serial.println(">> ADMINISTRADOR AGREGADO");
}

void registrarEEPROM(String user, String password) {
  Usuario nuevoUsuario;

  for (int i = 0; i < user.length(); i++) {
    nuevoUsuario.identificacion[i] = user.charAt(i);
    delay(75);
  }
  for (int i = 0; i < password.length(); i++) {
    nuevoUsuario.contrasenia[i] = password.charAt(i);
    delay(75);
  }

  EEPROM.get(0, registro);
  EEPROM.put(registro, nuevoUsuario);
  delay(75);

  registro += sizeof(Usuario);
  EEPROM.update(0, registro);
  delay(75);
}

void mensajePrincipal() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.write(byte(0));
  lcd.print(" BIENVENIDO ");
  lcd.write(byte(0));
  delay(5000);
}

void login() {
  lcd.clear();

  if (contadorIntentos < 4) {
    String pass = "";
    char key;

    do {
      lcd.setCursor(0, 0);
      lcd.print("DIGITE SU CLAVE");
      lcd.setCursor(0, 1);
      lcd.print(pass);

      key = keypad.getKey();
      if (key && key != '#') {
        pass += key;
      }

      if (pass.length() > 8) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MAX 8 CARACTERES");
        delay(2000);
        pass = "";
        lcd.clear();
      }
    } while (key != '#');

    if (pass == "0000") {
      registrarUsuario();
    } else {
      iniciarSesion(pass);
    }
  } else {
    tone(buzzerP, 600); // digitalWrite(buzzerP, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("SIST.  BLOQUEADO");
    lcd.setCursor(0, 1);
    lcd.print("AVISE AL GERENTE");
    delay(5000);
    noTone(buzzerP); // digitalWrite(buzzerP, LOW);
    avisoGerente();
  }
}

void avisoGerente() {
  String pass = "";
  String user = "";
  char key;

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("CLAVE GERENTE");
    lcd.setCursor(0, 1);
    lcd.print(pass);

    key = keypad.getKey();
    if (key && key != '#') {
      pass += key;
    }

    if (pass.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      pass = "";
      lcd.clear();
    }
  } while (key != '#');

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("ID GERENTE");
    lcd.setCursor(0, 1);
    lcd.print(user);
    key = keypad.getKey();

    if (key && key != '#') {
      user += key;
    }

    if (user.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      user = "";
      lcd.clear();
    }
  } while (key != '#');

  Usuario temp;
  EEPROM.get(sizeof(registro), temp); long randomNumber;
  String USR = temp.identificacion;
  String PWD = temp.contrasenia;

  if (USR.equals(user) && PWD.equals(pass)) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("SISTEMA");
    lcd.setCursor(2, 1);
    lcd.print("DESBLOQUEADO");

    contadorIntentos = 0;
  }
  delay(2000);
}

void iniciarSesion(String pass) {
  String user = "";
  char key;

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("DIGITE SU ID");
    lcd.setCursor(0, 1);
    lcd.print(user);
    key = keypad.getKey();

    if (key && key != '#') {
      user += key;
    }

    if (user.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      user = "";
      lcd.clear();
    }
  } while (key != '#');

  if (buscarEEPROM(user, pass)) {
    logueado = true;
    digitalWrite(IO52LOG, HIGH);

    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("ACCESO");
    lcd.setCursor(4, 1);
    lcd.print("PERMITIDO");

    contadorIntentos = 0;
    luces();
  } else {
    contadorIntentos++;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("ERROR  ");
    lcd.setCursor(11, 0);
    lcd.print(contadorIntentos);
    lcd.setCursor(0, 1);
    lcd.print("CLAVE INCORRECTA");
  }

  delay(2000);
}

void registrarUsuario() {
  String pass = "";
  String confPass = "";
  char key;

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("REGISTRO");
    lcd.setCursor(0, 1);
    lcd.print("CLAVE: ");
    lcd.setCursor(7, 1);
    lcd.print(pass);

    key = keypad.getKey();
    if (key && key != '#') {
      pass += key;
    }

    if (pass.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      pass = "";
      lcd.clear();
    }
  } while (key != '#');

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("CONFIRMAR");
    lcd.setCursor(0, 1);
    lcd.print("CLAVE: ");
    lcd.setCursor(7, 1);
    lcd.print(confPass);

    key = keypad.getKey();
    if (key && key != '#') {
      confPass += key;
    }

    if (confPass.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      confPass = "";
      lcd.clear();
    }
  } while (key != '#');

  if (pass.equals(confPass)) {
    confirmarRegistro(pass);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LAS  CONTRASENAS");
    lcd.setCursor(2, 1);
    lcd.print("NO COINCIDEN");
    delay(2000);
    confPass = "";
    pass = "";
    lcd.clear();
  }
}

void confirmarRegistro(String usrPass) {
  String pass = "";
  String user = "";
  char key;

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("CLAVE GERENTE");
    lcd.setCursor(0, 1);
    lcd.print(pass);

    key = keypad.getKey();
    if (key && key != '#') {
      pass += key;
    }

    if (pass.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      pass = "";
      lcd.clear();
    }
  } while (key != '#');

  lcd.clear();
  do {
    lcd.setCursor(0, 0);
    lcd.print("ID GERENTE");
    lcd.setCursor(0, 1);
    lcd.print(user);
    key = keypad.getKey();

    if (key && key != '#') {
      user += key;
    }

    if (user.length() > 8) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAX 8 CARACTERES");
      delay(2000);
      user = "";
      lcd.clear();
    }
  } while (key != '#');

  Usuario temp;
  EEPROM.get(sizeof(registro), temp);
  String USR = temp.identificacion;
  String PWD = temp.contrasenia;

  if (USR.equals(user) && PWD.equals(pass)) {
    logueado = true;
    digitalWrite(IO52LOG, HIGH);

    randomNumber = random(10000000, 99999999);
    String id = String(randomNumber);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ACCESO PERMITIDO");
    lcd.setCursor(0, 1);
    lcd.print("ID: ");
    lcd.setCursor(3, 1);
    lcd.print(id);

    registrarEEPROM(id, usrPass);
    contadorIntentos = 0;

    luces();
  } else {
    contadorIntentos++;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("ERROR  ");
    lcd.setCursor(11, 0);
    lcd.print(contadorIntentos);
    lcd.setCursor(0, 1);
    lcd.print("USUARIO  GERENTE");
  }
  delay(2000);
}

void luces() {
  tone(buzzerP, 400); // digitalWrite(buzzerP, HIGH);
  delay(2000);

  noTone(buzzerP); // digitalWrite(buzzerP, LOW);
  digitalWrite(IO52LOG, LOW);
  digitalWrite(LL1, HIGH);
  digitalWrite(LL2, HIGH);
  digitalWrite(LE, HIGH);
  digitalWrite(LS, HIGH);
  lcd.clear();
}


void controlPorton() {
  delay(10);
  char c = Serial.read();
  texto += c;
  if (texto == "3") {
    posicion = 1;
  } else {
    posicion = 0;
  }

  if (posicion == 1) { //encender led por 6 seg al terminar se cierra el porton
    abrirPorton();
    digitalWrite(portonAbierto, HIGH);
    for (int i = 0; i <= 6000; i++) {
      while (Serial.available()) {
        delay(10);
        char c = Serial.read();
        texto += c;
      }
      if (texto == "3") {
        posicion = 1;
      } else {
        posicion = 0;
      }
      if (posicion != 1) {
        break;
      }
      delay(1);
    }
    digitalWrite(portonAbierto, LOW);

    posicion = 0;
    cerrarPorton();
  }
}

void abrirPorton() {            // 2 vueltas derecha
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.print("CONTROLPORTON");
  lcd.write(byte(5));

  lcd.setCursor(2, 1);
  lcd.write(byte(1));
  lcd.write(byte(3));
  lcd.print("ABRIENDO");
  lcd.write(byte(3));

  motor.attach(PORTON);
  for (int i = 0; i <= 180; i++) {
    motor.write(i);
    delay(25);
  }
}

void cerrarPorton() {     // 2 vueltas izquierda
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.print("CONTROLPORTON");
  lcd.write(byte(5));

  lcd.setCursor(2, 1);
  lcd.write(byte(0));
  lcd.write(byte(3));
  lcd.print("CERRANDO");
  lcd.write(byte(3));

  motor.attach(PORTON);
  for (int i = 180; i >= 0; i--) {
    motor.write(i);
    delay(25);
  }

  Serial.println(">> LED AMARILLA");
  tone(buzzerP, 500); // digitalWrite(buzzerP, HIGH);
  digitalWrite(portonCerrado, HIGH);
  delay(3000);
  noTone(buzzerP); // digitalWrite(buzzerP, LOW);
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
    tone(buzzerP, 700); // digitalWrite(buzzerP, HIGH);
    delay(3000);
    noTone(buzzerP); // digitalWrite(buzzerP, LOW);
    
    while (digitalRead(LAB1) == LOW) {
      secuenciaUnPaso();
      mensajeLab("Corriendo hacia", "la Derecha");
    }
    mensajeLab("La muestra llego", "al LAB 1");
    
    tone(buzzerP, 800); // digitalWrite(buzzerP, HIGH);
    delay(1000);
    noTone(buzzerP); // digitalWrite(buzzerP, LOW);
    
    Lab2 = false;
  } else {
    mensajeLab("Poner Muestra", "en la banda LAB2");
  }
}

void haciaLab2() { // HACIA LA IZQUIERDA SENTIDO ANTIHORARIO
  delay(100);
  horario = 0;
  Cpasos = paso;
  if (digitalRead(LAB1) == HIGH) {
    tone(buzzerP, 700); // digitalWrite(buzzerP, HIGH);
    delay(3000);
    noTone(buzzerP); // digitalWrite(buzzerP, LOW);
    
    while (digitalRead(LAB2) == LOW) {
      secuenciaUnPaso();
      mensajeLab("Corriendo hacia", "la Izquierda");
    }
    mensajeLab("La muestra llego", "al LAB 2");
    
    tone(buzzerP, 900); // digitalWrite(buzzerP, HIGH);
    delay(1000);
    noTone(buzzerP); // digitalWrite(buzzerP, LOW);
    
    Lab1 = false;
  } else {
    mensajeLab("Poner Muestra", "en la banda LAB1");
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

void mensajeLab(String mensaje1, String mensaje2) {
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(mensaje1);
  lcd.setCursor(0, 1);
  lcd.print(mensaje2);
  delay(1500);
  lcd.clear();
}

void controlarLuces() {
  while (Serial.available()) {
    delay(10);
    char c = Serial.read();
    texto += c;
  }
  if (texto.length() > 0) {
    Serial.println(texto);
    if (texto == "gon") {
      digitalWrite(LL1, HIGH);
      digitalWrite(LL2, HIGH);
      digitalWrite(LE, HIGH);
      digitalWrite(LS, HIGH);
    }
    if (texto == "goff") {
      digitalWrite(LL1, LOW);
      digitalWrite(LL2, LOW);
      digitalWrite(LE, LOW);
      digitalWrite(LS, LOW);
    }
    if (texto == "eon") {
      digitalWrite(LE, HIGH);
    }
    if (texto == "eoff") {
      digitalWrite(LE, LOW);
    }
    if (texto == "l1on") {
      digitalWrite(LL1, HIGH);
    }
    if (texto == "l1off") {
      digitalWrite(LL1, LOW);
    }
    if (texto == "l2on") {
      digitalWrite(LL2, HIGH);
    }
    if (texto == "l2off") {
      digitalWrite(LL2, LOW);
    }
    if (texto == "son") {
      digitalWrite(LS, HIGH);
    }
    if (texto == "soff") {
      digitalWrite(LS, LOW);
    }
    texto = "";
  }
}
