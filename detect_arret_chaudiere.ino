#include "Wire.h"
#include <EEPROM.h>
#include <Deuligne.h>

#include "temp_sensor.h"
#include "utils.h"
#include "lcd.h"

#define ADDRESS 1

enum running_mode {
  up,
  idle
};
running_mode GO_IDLE = idle;
running_mode GO_UP = up;

int value = 0;

int consigne = 0;
int consigne_init = 0;
int button = -1;
int sleep = millis();
int mode = 1;
Deuligne lcd;
void setup()
{
	Serial.begin(19200); 
	lcd.init();
	lcd.setCursor(0, 0);
	lcd.print("init");  
	delay(700);
	
//	EEPROM.write(ADDRESS, 15);
	consigne = EEPROM.read(ADDRESS);
	char buffer[16];
	sprintf(buffer, "consigne = %d", consigne);
	lcd.setCursor(0, 1);
	lcd.print(buffer);  
	delay(2000);
	lcd.clear();
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

void print_temp (float temp){
	lcd.setCursor(0, 0);
	lcd.print("température");   
	lcd.setCursor(0, 1);
	lcd.print(Arrondi(temp));  
//	Serial.print("temp: ");
//	Serial.println(temp);
}

void print_and_update_consigne(int value){
	consigne = consigne + value;
	print_consigne(consigne);
	delay(300);	
}

void manage_button() {
	unsigned long time;
	time = millis();
	print_consigne(consigne);
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
	sleep = millis();
}

void switch_mode(int value){
	mode = value;
	if(mode == up) {lcd.display();}
	if(mode == idle) {lcd.noDisplay();}
}

void check_consigne(float temp, int consigne){
	if(temp < consigne){
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("ALERT");  
		lcd.setCursor(0, 0);
		lcd.print("ALERT !!!");
		delay(1000);
	}
}

void loop() {
	
	float temp = temp_as_c(); // get temp value
	print_temp (temp);
	delay(1000);
	check_consigne(temp, consigne);

	button=lcd.get_key();
	delay(100); // for debounce
	if(button > -1 && mode == up) {
		manage_button();
	}
	if(button > -1 && mode == idle) {
		sleep = millis();
		switch_mode(1);
	}
	if(millis() > sleep + 6000){switch_mode(0);}	
}
