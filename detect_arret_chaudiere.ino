#include "Wire.h"
#include <EEPROM.h>
#include <Deuligne.h>
#include "temp_sensor.h"
#include "utils.h"
#include "lcd.h"
#include <Metro.h>

#define ADDRESS 1 //adresse d'écriture en ROM de la valeur de consigne température
#define IDLE 8000 // durée en ms avant mise en veille écran
#define REFRESH 1000 // durée en ms avant mise en veille écran
int value = 0;

float temp = 0; //température lue via adc
int consigne = 0; //consigne de température
int button = -1; // boutton joystick lcd deuligne
int mode = 1; // mode 0 = écran en veille, 1 = hors veille 
Deuligne lcd;
Metro go_idle = Metro(IDLE); //timer gestion mode veille
Metro check_temp = Metro(REFRESH); //timer gestion mode veille

void setup()
{
	lcd.init();
	lcd.setCursor(0, 0);
	lcd.print("init");
	Serial.begin(19200); 
	//EEPROM.write(ADDRESS, 15);
	consigne = EEPROM.read(ADDRESS);
	delay(500);
	lcd.clear();
	go_idle.reset();
	check_temp.reset();

//	while (1);
  }

void save_consigne (){
	int mem = EEPROM.read(ADDRESS);
	if(mem!=consigne){
		EEPROM.write(ADDRESS, consigne);
	}
}
void print_custom (char*){
	lcd.setCursor(0, 0);
	lcd.print("custom");  
	lcd.setCursor(0, 1);
	lcd.print(value);  
}
void print_consigne (int consigne){
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("consigne");  
	lcd.setCursor(0, 1);
	lcd.print(consigne);  
}
void print (){
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
	consigne = consigne + value;
	print ();
	delay(300);	
}

void manage_button() {
	unsigned long time;
	time = millis();
	print();
	delay(1000);
	while(millis() < time + 4000) {		
		button=lcd.get_key();
		delay(100); // for debounce
		if(button == LEFT || button == RIGHT || button == UP || button == DOWN){
			value = 0;
			if(button == LEFT) {value = -1; }
			if(button == RIGHT) {value = 1;}
			if(button == DOWN) {value = -5;}
			if(button == UP) {value = 5;}
		time = millis();
		print_and_update_consigne(value);
		}
	}
	save_consigne ();
	lcd.clear();
	button = -1;
	go_idle.reset();
}

void switch_mode(int value){
	mode = value;
	if(mode == 1) {lcd.backLight(1);}
	if(mode == 0) {lcd.backLight(0);}
}

void check_consigne(float temp, int consigne){
	if(temp < consigne){
		if(mode == 0) {switch_mode(1);}
		go_idle.reset();
		lcd.clear();
		char buffer1[16];
		sprintf(buffer1, "température %d", Arrondi(temp));
		lcd.setCursor(0, 0);
		lcd.print(buffer1);  
		lcd.setCursor(0, 1);
		lcd.print("! ALERT ! ");  
		delay(2000);
	}
}

void loop() {
	
	check_consigne(temp, consigne);

	if (check_temp.check() == 1){
		temp = temp_as_c(); // get temp value
		print ();
		check_temp.reset();
	}
	
	button=lcd.get_key();
	delay(100); // for debounce
	if(button > -1 && mode == 0) {
		go_idle.reset();
		switch_mode(1);
		button = -1;
	}
	if(button > -1 && mode == 1) {
		manage_button();
	}
	if ((go_idle.check() == 1) && mode == 1){switch_mode(0);}
//	if((millis() > (sleep + IDLE)) && mode == 1){switch_mode(0);}	
}
