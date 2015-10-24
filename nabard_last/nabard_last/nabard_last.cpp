/*
 * nabard_last.cpp
 *
 * Created: 10/19/2015 7:55:20 PM
 *  Author: HELLO
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "LiquidCrystal.h"


#define AUTO_MANUAL			(1<<PINB0)
#define START_BUTTON		(1<<PINB1)
#define WEIGHT_SENSOR_L		(1<<PINA6)
#define WEIGHT_SENSOR_R		(1<<PINA5)
#define SHUTTER_SENSOR_L	(1<<PINA2)
#define SHUTTER_SENSOR_R	(1<<PINA1)
#define PRESENCE_SENSOR_R	(1<<PINB2)
#define PRESENCE_SENSOR_L	(1<<PIND3)
#define CUP_CHANGE_SENSOR_L (1<<PINA4)
#define CUP_CHANGE_SENSOR_R (1<<PINA3)
#define STOP_BUTTON			(1<<PIND2)

#define START				true
#define STOP				false
#define LEFT				0
#define RIGHT				1

#define CONVEYOR_LED	(1<<PORTC6)
#define FLAP			(1<<PORTC0)
#define L_SHUTTER		(1<<PORTC1)
#define R_SHUTTER		(1<<PORTC2)
#define CHANGE_PISTON	(1<<PORTC3)
#define CONVEYOR		(1<<PORTC4)
#define MANUAL_OUT		(1<<PORTC7)

void conveyor_stop();
void halt();
void home();
bool cup_position_check();
bool cup_presence_check();
void shutter_open();
void manual_check();
void run();
void var_reset();
void check_new_cup();
void stop_check();
void runss();

bool cup=false;
bool new_cup_flag_l=true;
bool new_cup_flag_r=true;
bool start_stop_status=false;
bool wait_check=false;
bool p_flag=false;

LiquidCrystal lcd(8,9,12,15,24,13,14);

int main(void)
{
	DDRA=0;
	DDRB=0;
	DDRD=0;
	DDRC=0xff;
	
	PORTA=0xff;
	PORTB=0xff;
	PORTD=0xff;
	PORTC=0;
	
	LiquidCrystal lcd1(8,9,12,15,24,13,14);

	lcd.begin(16,2);
	lcd.clear();
	lcd.print("Resnova Pvt. Ltd");
	lcd.setCursor(4,1);
	lcd.print("RTF-900");
	_delay_ms(3000);
	
	home();
	
	
	
	if((PINB&AUTO_MANUAL))
	{
		while(PINB&START_BUTTON)
		manual_check();
		
		lcd.setCursor(0,1);
		lcd.print("                ");
	}
	

	
	
    while(1)
    {
	
      runss();    
    }
}


void home()
{
	PORTC=0;
	cup=LEFT;
}


void shutter_open()
{
	
	
	if(cup)
	{
		cup=!cup;
		
		if(!(PINA&SHUTTER_SENSOR_L))
		PORTC&=~FLAP;
		
		else PORTC&=~((CONVEYOR)|(CONVEYOR_LED));
		
		if(!(PINB&PRESENCE_SENSOR_R))
		{
			PORTC|=R_SHUTTER;
			
			for(uint8_t x=0;x<=100;x++)
			{
				check_new_cup();
				conveyor_stop();
				manual_check();
				stop_check();
				
				if((!p_flag)&&(!new_cup_flag_l))
				{
					p_flag=true;
					lcd.setCursor(0,1);
					lcd.print("                ");
					lcd.setCursor(1,1);
					lcd.print("Place new L Cup");
					
				}
				
				_delay_ms(10);
			}
			
			PORTC&=~R_SHUTTER;
			new_cup_flag_r=false;
			
			for(int x=0;x<=300;x++)
			{
				check_new_cup();
				conveyor_stop();
				manual_check();
				stop_check();
				
				if(!(PINA&WEIGHT_SENSOR_L))
				PORTC|=FLAP;
				
				if(new_cup_flag_l)
				{
					lcd.setCursor(0,1);
					lcd.print("                ");
				}
				
				_delay_ms(10);
			}
			
			while(!new_cup_flag_l)
			{
				check_new_cup();
				manual_check();
				conveyor_stop();
				stop_check();
				
				wait_check=true;
			}
			p_flag=false;
			lcd.setCursor(0,1);
			lcd.print("                ");
			
		}
		
		if(wait_check)
		{
			for(uint8_t x=0;x<=200;x++)
			{
				conveyor_stop();
				manual_check();
				stop_check();
				_delay_ms(10);
			}
			
			wait_check=false;
		}
		
		PORTC&=~CHANGE_PISTON;
}

else if(!cup)
{
  cup=!cup;
  
  if(!(PINA&SHUTTER_SENSOR_R))
  PORTC|=FLAP;
  
  else
  {
	  PORTC&=~(CONVEYOR|CONVEYOR_LED);
  }
  
  if(!(PIND&PRESENCE_SENSOR_L))
  {
	  PORTC|=L_SHUTTER;
	  
	  for(uint8_t x=0;x<=100;x++)
	  {
		  check_new_cup();
		  conveyor_stop();
		  manual_check();
		  stop_check();
	  
	  
	  if((!p_flag)&&(!new_cup_flag_r))
	  {
		  lcd.setCursor(0,1);
		  lcd.print("                ");
		  lcd.setCursor(1,1);
		  lcd.print("Place new R Cup");
		  p_flag=true;
	  }
	  _delay_ms(10);
	  }
	  
  
      PORTC&=~L_SHUTTER;
  
      new_cup_flag_l=false;
  
  for(int x=0;x<=300;x++)
  {   
	  //lcd.clear();
	  //lcd.print("DELAY");
	  
	  check_new_cup();
	  conveyor_stop();
	  manual_check();
	  stop_check();
	  
	  if(!(PINA&WEIGHT_SENSOR_R))
	   PORTC&=~FLAP;
	   
	   if(new_cup_flag_r)
	   {
		   lcd.setCursor(0,1);
		   lcd.print("                ");
	   }
	   _delay_ms(10);
	   
  }
  //lcd.clear();
  //lcd.print("OUT");
  
  
  while(!new_cup_flag_r)
  {
	  check_new_cup();
	  
	  if(!p_flag)
	  {   
		  lcd.setCursor(0,1);
		  lcd.print("                ");
		  lcd.setCursor(1,1);
		  lcd.print("Place new R Cup");
		  p_flag=true;
	  }
	  
	  manual_check();
	  conveyor_stop();
	  stop_check();
	  
	  wait_check=true;
  }
  
    p_flag=false;
  	lcd.setCursor(0,1);
  	lcd.print("                ");
  
  
  if(wait_check)
  {
	  for(uint8_t x=0;x<=200;x++)
	  {
		  conveyor_stop();
		  manual_check();
		  stop_check();
		  _delay_ms(10);
	  }
	  
	  wait_check=false;
	  
  }
  
  PORTC|=CHANGE_PISTON;
  
  
}
}

}

bool cup_presence_check()
{
	if(cup)
	{
		if(!(PINB&PRESENCE_SENSOR_R))
		return true;
		
		else return false;
	}
	
	else
	{
		if(!(PIND&PRESENCE_SENSOR_L))
		return true;
		
		else return false;
	}
}

bool cup_position_check()
{
	if(cup)
	{
		if(!(PINA&CUP_CHANGE_SENSOR_R))
		return true;
		
		else return false;
	}
	
	else 
	{
		if(!(PINA&CUP_CHANGE_SENSOR_L))
		return true;
		
		else return false;
	}
}


void halt()
{
	static bool conveyor_flag=false;
	
	if(PINC&CONVEYOR)
	{
		PORTC&=~(CONVEYOR|CONVEYOR_LED);
		conveyor_flag=true;
	}
	
	lcd.clear();
	lcd.setCursor(5,0);
	lcd.print("Halted");
	lcd.setCursor(4,1);
	lcd.print("Press ON");
	
	while(PINB&START_BUTTON)
	{
		manual_check();
	}
	
	lcd.clear();
	lcd.setCursor(2,0);
	lcd.print("Auto mode ON");
	
	if(conveyor_flag)
	{
		PORTC|=(CONVEYOR|CONVEYOR_LED);
		conveyor_flag=false;
	}
}


void manual_check()
{
	static bool hi= true;
	
	MANUAL:
	if(!(PINB&AUTO_MANUAL))
	{
		hi=false;
		start_stop_status=false;
		var_reset();
		home();
		PORTC|=MANUAL_OUT;
		
		if(!hi)
		{
			lcd.clear();
			lcd.setCursor(1,0);
			lcd.print("Manual mode ON");
			hi=true;
		}
		
		while(!(PINB&AUTO_MANUAL));
		
	    PORTC&=~MANUAL_OUT;
		home();
		_delay_ms(30);
		
		while((PINB&AUTO_MANUAL)&&(!start_stop_status))
		{
			if(hi)
			{
				lcd.clear();
				lcd.setCursor(2,0);
				lcd.print("Auto mode ON");
				lcd.setCursor(4,1);
				lcd.print("Press ON");
				hi=false;
			}
			
			if(!(PINB&START_BUTTON))
			{   
				lcd.setCursor(0,1);
				lcd.print("                ");
				
				start_stop_status=true;
				home();
				var_reset();
				runss();
				break;
				
			}
		}
		
		if(!(PINB&AUTO_MANUAL))
		goto MANUAL;
	}
	
	else
	{
		if(hi)
		{
			lcd.clear();
			lcd.setCursor(2,0);
			lcd.print("Auto mode ON");
			lcd.setCursor(4,1);
			lcd.print("Press ON");
			hi=false;
		}
	}
	
	
}


void run()
{
	manual_check();
	
	
	if((PINA&WEIGHT_SENSOR_L)&&(!(PINA&SHUTTER_SENSOR_L)))
	{
		PORTC&=~FLAP;
		PORTC|=(CONVEYOR|CONVEYOR_LED);
		
		
		
		while(PINA&WEIGHT_SENSOR_L)
		{   
		  
			manual_check();
			conveyor_stop();
			cup=!cup;
			check_new_cup();
			cup=!cup;
			stop_check();
		}
	}
	
	else if(PINA&SHUTTER_SENSOR_L)
	{   
		lcd.setCursor(0,1);
		lcd.print("                ");
		lcd.setCursor(1,1);
		lcd.print("L Shutter Open");
		
		while(PINA&SHUTTER_SENSOR_L);
		lcd.setCursor(0,1);
		lcd.print("                ");
		
		
			while(PINA&WEIGHT_SENSOR_L)
			{
				
				manual_check();
				conveyor_stop();
				cup=!cup;
				check_new_cup();
				cup=!cup;
				stop_check();
			}
		
	}
	
	if((PINA&WEIGHT_SENSOR_R)&&!(PINA&SHUTTER_SENSOR_R))
	 
	 PORTC|=(FLAP|CONVEYOR|CONVEYOR_LED);
	 
	 while(!cup_presence_check())
	 {
		 manual_check();
		 conveyor_stop();
		 cup=!cup;
		 check_new_cup();
		 cup=!cup;
		 stop_check();
	 }
	 
	 while(!cup_position_check())
	 {
		 manual_check();
		 conveyor_stop();
	 }
	 
	 shutter_open();
	 cup=RIGHT;
	 
	 //starting of right cup
	 if(PINA&SHUTTER_SENSOR_R)
	 {
		 lcd.setCursor(0,1);
		 lcd.print("                ");
		 lcd.setCursor(1,1);
		 lcd.print("R Shutter Open");
	 }
	 while(PINA&SHUTTER_SENSOR_R);
	 
	 while(PINA&WEIGHT_SENSOR_R)
	 {
		 manual_check();
		 cup=RIGHT;
		 conveyor_stop();
		 cup=!cup;
		 check_new_cup();
		 cup=!cup;
		 stop_check();
	 }
	 
	 PORTC&=~FLAP;
	 
	 while(!cup_presence_check())
	 {
		 manual_check();
		 conveyor_stop();
		 cup=!cup;
		 check_new_cup();
		 cup=!cup;
		 stop_check();
	 }
	 
	 while(!cup_position_check())
	 {
		 manual_check();
		 conveyor_stop();
		 cup=!cup;
		 check_new_cup();
		 cup=!cup;
		 stop_check();
	 }
	 
	 shutter_open();
	 cup=LEFT;
	
}


void conveyor_stop()
{
	if((PINA&SHUTTER_SENSOR_L)&&(PINA&SHUTTER_SENSOR_R))
	PORTC&=~(CONVEYOR|CONVEYOR_LED);
	
	else if(!(PINA&WEIGHT_SENSOR_L)&&(!(PINA&WEIGHT_SENSOR_R)))
	     PORTC&=~(CONVEYOR|CONVEYOR_LED);
    
	else PORTC|=(CONVEYOR|CONVEYOR_LED);
}

void var_reset()
{
	cup=false;
	new_cup_flag_l=true;
	new_cup_flag_r=true;
	start_stop_status=false;
	wait_check=false;
}

void check_new_cup()
{
	static bool cup_missing_l=false;
	static bool cup_missing_r=false;
	
	if(!cup_presence_check())
	{
		if(cup)
		cup_missing_r=true;
		else
		cup_missing_l=true;
	}
	
	else
	{
		if(cup)
		{
			if(cup_missing_r)
			new_cup_flag_r=true;
			
			cup_missing_r=false;
		}
		
		else
		{
			
			if(cup_missing_l)
			new_cup_flag_l=true;
			cup_missing_l=false;
			
		}
	}
	
	
}

void stop_check()
{
	if(!(PIND&STOP_BUTTON))
	halt();
}

void runss()
{
	while(1)
	{
		run();
	}
}