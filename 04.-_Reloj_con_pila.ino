#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd (0x27, 20, 4);
char texto[20];
int botonModo = 8;
int botonUp = 10;
int botonDown = 11;
int botonLuz = 12;
int horasAlarma = 0;
int minutosAlarma = 0;
int modo = 0;
boolean estadoLuz = true;
const int N = 3;
int BUZZER = 2;
String estadoAlarma = "OFF";
boolean alarma = false;
DateTime fecha;



String diasDeLaSemana[7] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

void setup() {
  Wire.begin();
  lcd.begin(20, 4);
  lcd.clear();
  lcd.backlight();
  if (! rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("Modulo no");
    while(1);
  }
  pinMode(botonModo, INPUT);
  pinMode(botonUp, INPUT);
  pinMode(botonDown, INPUT);
  pinMode(botonLuz, INPUT);
  pinMode(BUZZER, OUTPUT);
  /*Ejecutar esta orden solo una vez para introducir la fecha en el modulo.
    Una vez introducida, comentarla.*/
  rtc.adjust(DateTime (__DATE__, __TIME__));
}

void fijarCursorModo(){
  switch(modo){
    case 0:
      lcd.noBlink();
      break;
    case 1:
      lcd.setCursor(12, 1);
      lcd.blink();
      delay(100);
      break;
    case 2:
      lcd.setCursor(15, 1);
      lcd.blink();
      delay(100);
      break;
  }
}

void incrementarHorasAlarma(){
  horasAlarma ++;
  horasAlarma = horasAlarma % 24;
}

void decrementarHorasAlarma(){
  horasAlarma --;
  if (horasAlarma < 0) {
    horasAlarma = 23;
  }
}

void incrementarMinutosAlarma(){
  minutosAlarma ++;
  minutosAlarma = minutosAlarma % 60;
}

void decrementarMinutosAlarma(){
  minutosAlarma --;
  if (minutosAlarma < 0) {
    minutosAlarma = 59;
  }
}

void ponerFecha(){
  fecha = rtc.now();
  lcd.setCursor(0, 0);
  sprintf(texto, "%02d/%02d/", fecha.day(), fecha.month());
  lcd.print(texto);
  lcd.print(fecha.year());
  lcd.print(" ");
  sprintf(texto, "%02d:%02d:%02d", fecha.hour(), fecha.minute(), fecha.second());
  lcd.print(texto);
  lcd.setCursor(0, 1);
  lcd.print(diasDeLaSemana[fecha.dayOfTheWeek()]);
}

void ponerAlarma() {
  lcd.setCursor(11, 1);
  sprintf(texto, "%02d:%02d ", horasAlarma, minutosAlarma);
  lcd.print(texto);
  lcd.print(estadoAlarma);
}

void comprobarAlarma(){
  if (fecha.hour() == horasAlarma && fecha.minute() == minutosAlarma && fecha.second() == 0){
    if (estadoAlarma == "ON "){
      alarma = true;
    }
  }
}

void alarmaActiva() {
  if (alarma){
    tone(BUZZER, 440);
    delay(500);
    if(digitalRead(botonDown) == HIGH){
      noTone(BUZZER);
      alarma = false;
    }
    noTone(BUZZER);
    delay(500);
    if(digitalRead(botonDown) == HIGH){
      noTone(BUZZER);
      alarma = false;
    }
  }
}

void loop() {
  digitalWrite(BUZZER, HIGH);
  if (digitalRead(botonModo) == HIGH){
    modo++;
    if (modo > 2) {
      modo = 0;
    }
    while(digitalRead(botonModo) == HIGH);
  }
  switch(modo){
    case 0:
      if (digitalRead(botonLuz) == HIGH) {
        if (estadoLuz) {
          lcd.backlight();
          estadoLuz = false;
          delay(50);
        } else {
          lcd.noBacklight();
          estadoLuz = true;
          delay(50);
        }
      }
      if(digitalRead(botonDown) == HIGH){
        if (estadoAlarma == "ON "){
          estadoAlarma = "OFF";
        } else {
          estadoAlarma = "ON ";
        }
        while(digitalRead(botonDown) == HIGH);
      }
      break;
    case 1:
      if(digitalRead(botonUp) == HIGH){
        incrementarHorasAlarma();
        while(digitalRead(botonUp) == HIGH);
      }
      if(digitalRead(botonDown) == HIGH){
        decrementarHorasAlarma();
        while(digitalRead(botonDown) == HIGH);
      }
      break;
    case 2:
      if(digitalRead(botonUp) == HIGH){
        incrementarMinutosAlarma();
        while(digitalRead(botonUp) == HIGH);
      }
      if(digitalRead(botonDown) == HIGH){
        decrementarMinutosAlarma();
        while(digitalRead(botonDown) == HIGH);
      }
      break;
     }
  ponerFecha();
  ponerAlarma();
  comprobarAlarma();
  alarmaActiva();
  fijarCursorModo();
}
