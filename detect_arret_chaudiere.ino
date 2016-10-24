#include "Wire.h"
#include <EEPROM.h>
#include <Deuligne.h>
#include "temp_sensor.h"
#include "utils.h"
#include "lcd.h"
#include <Metro.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ROM ADDRESS
#define ADDRESS 1 //adresse d'écriture en ROM de la valeur de consigne température

// TEMPOS
#define IDLE 80000 // durée en ms avant mise en veille écran
#define REFRESH 1000 // durée en ms avant mise en veille écran

// PIN WIRING
#define BTN_PLUS 12 // pin digital 11 input button plus : need to set internal pullup resistor
#define BTN_MOINS 11 // pin digital 11 input button plus : need to set internal pullup resistor
#define BUZZER 10 // pin digital 10 Buzzer tone
#define ONE_WIRE_BUS 2 // One wire DS18B20 on pin2

// OTHER CONSTANTS

#define TEMP_TAB_LENGTH
// GLOBAL VARIABLES
int value_update_consigne = 0; // relative value to add to consigne
float temp = 0; // température read from adc
int consigne = 0; // consigne température
int mode = 1; // mode 0 = écran en veille, 1 = hors veille 
int btnp; //bouton plus
int btnm; //bouton moins

// Library instances
Deuligne lcd;
Metro go_idle = Metro(IDLE); //timer gestion mode veille
Metro check_temp = Metro(REFRESH); //timer gestion mode veille
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

void setup()
{
	pinMode(BTN_PLUS, INPUT);           // set BTN_PLUS to input
	digitalWrite(BTN_PLUS, HIGH);       // turn on pullup resistors	on BTN_PLUS
	pinMode(BTN_MOINS, INPUT);           // set BTN_MOINS to input
	digitalWrite(BTN_MOINS, HIGH);       // turn on pullup resistors on BTN_MOINS
	sensors.begin(); // Start up the library OneWire DS18B20
	lcd.init();
	lcd.setCursor(0, 0);
	lcd.print("init");
	Serial.begin(19200); 
	//EEPROM.write(ADDRESS, 15);
	consigne = EEPROM.read(ADDRESS);
	lcd.clear();
	temp = temp_as_c(); // get temp
	delay(500);
	go_idle.reset();
	check_temp.reset();
//	while (1);
  }

int getTempDallas (){
  //Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  return (Arrondi(sensors.getTempCByIndex(0)));
}

void save_consigne (){
	int mem = EEPROM.read(ADDRESS);
	if(mem!=consigne){
		EEPROM.write(ADDRESS, consigne);
	}
}

void print_consigne (int consigne){
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("consigne");  
	lcd.setCursor(0, 1);
	lcd.print(consigne);  
}
void print (){
	lcd.clear();
	char buffer1[16];
	sprintf(buffer1, "température %d", Arrondi(temp));
	lcd.setCursor(0, 0);
	lcd.print(buffer1);  

	char buffer2[16];
	lcd.setCursor(0, 1);
	sprintf(buffer2, "consigne %d", consigne);
	lcd.print(buffer2);  

}
void print_and_update_consigne(int value){
	consigne = consigne + value_update_consigne;
	print ();
	delay(300);	
}
void manage_button() {
	unsigned long time;
	time = millis();
	print();
	delay(1000);
	while(millis() < time + 4000) {
		btnp = digitalRead(BTN_PLUS);
		btnm = digitalRead(BTN_MOINS);
		delay(100); // for debounce
		if(btnp == LOW || btnm == LOW){
			value_update_consigne = 0;
			if(btnm == LOW) {value_update_consigne = -5; }
			if(btnp == LOW) {value_update_consigne = 5;}
		time = millis();
		lcd.clear();
		print_and_update_consigne(value_update_consigne);
		}
	}
	save_consigne ();
	lcd.clear();
	btnp = HIGH;
	btnm = HIGH;
	go_idle.reset();
}
void switch_mode(int value){
	mode = value;
	if(mode == 1) {lcd.backLight(1);}
	if(mode == 0) {lcd.backLight(0);}
}
void manage_alert(float temp, int consigne){
	if(mode == 0) {switch_mode(1);}
	go_idle.reset();
	lcd.clear();
	char buffer1[16];
	sprintf(buffer1, "température %d", Arrondi(temp));
	lcd.setCursor(0, 0);
	lcd.print(buffer1);  
	lcd.setCursor(0, 1);
	lcd.print("! ALERT ! ");  
	tone(BUZZER, 440, 1000);
	delay(2000);
}


void loop() {
	if (check_temp.check() == 1){
		//temp = temp_as_c(); // get temp
		temp = getTempDallas(); // get temp
		print ();
		check_temp.reset();
	}
	if(temp < consigne){
		manage_alert(temp, consigne);
	}
	btnp = digitalRead(BTN_PLUS);
	btnm = digitalRead(BTN_MOINS);
	delay(100); // for debounce
	if((btnp == LOW || btnm == LOW) && mode == 0) {
		go_idle.reset();
		switch_mode(1);
		btnp = HIGH;
		btnm = HIGH;
	}
	if((btnp == LOW || btnm == LOW) && mode == 1) {
		manage_button();
	}
	if ((go_idle.check() == 1) && mode == 1){switch_mode(0);}
	
}
