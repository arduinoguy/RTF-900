/*
 * Nabard.cpp
 *
 * Created: 11-Aug-15 10:39:32 AM
 *  Author: SDK
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "libs/Arduino.h"
#include "libs/LiquidCrystal.h"

#define pb0 0
#define pb1 1
#define pb2 2 
#define pb3 3 
#define pb4 4
#define Pb5 5 
#define pb6 6
#define pb7 7

#define pc0 16
#define pc1 17
#define pc2 18
#define pc3 19
#define pc4 20 
#define pc5 21
#define pc6 22
#define pc7 23

#define pd0 8
#define pd1 9
#define pd2 10
#define pd3 11
#define pd4 12
#define pd5 13
#define pd6 14
#define pd7 15

#define pa0 A0
#define pa1 A1
#define pa2 A2
#define pa3 A3
#define pa4 A4
#define pa5 A5
#define pa6 A6
#define pa7 A7

#define auto_manaul		    pb0   
#define start_button		pb1  
#define weight_sensor_l     pa6		   
#define weight_sensor_r		pa5 
#define shutter_sensor_l	pa2 
#define shutter_sensor_r	pa1 
#define presence_sensor_r	pb2  //interrupt2
#define presence_sensor_l	pd3 //interrupt1
#define cup_change_sensor_l pa4 
#define cup_change_sensor_r pa3
#define stop_button			pd2  //interrupt0

#define start  true
#define stop   false
#define left   0
#define right  1
#define auto   true
#define manual false
#define manual_on 0
#define conveyor_led pc6

#define flap_piston			pc0 
#define shutter_piston_l	pc1		
#define shutter_piston_r    pc2
#define change_piston       pc3		
#define conveyor			pc4
#define manaul_out          pc7


bool cup=false;
bool new_cup_flag_l=true;
bool new_cup_flag_r=true;
bool start_stop_status=false;
bool wait_check=false;
bool p_flag=false;

void conveyor_stop();
void halt();
void home();
bool cup_position_check();
bool cup_presence_check();
void shutter_open();
void manual_check();
void check_weight_empty();
void var_reset();
void check_new_cup();
void stop_check();

LiquidCrystal lcd(8,9,12,15,24,13,14);


void setup()
{
	
	
	MCUCSR|= (1<<JTD);
	MCUCSR|= (1<<JTD); //JTAG disable
	
	DDRA=0;
	DDRB=0;
	DDRD=0;
	DDRC=0xff;
	
	PORTA=0xff;
	PORTB=0xff;
	PORTD=0xff;
	PORTC=0;
	
	home();
    LiquidCrystal lcd1(8,9,12,15,24,13,14);	
	lcd.begin(16,2);
	lcd.clear(); 
	lcd.print("Resnova Pvt. Ltd");
	lcd.setCursor(4,1);
	lcd.print("RTF-900");
	_delay_ms(3000);
	
	if(digitalRead(auto_manaul))
	{
		
		 while(digitalRead(start_button))
		 manual_check();
		 
		 lcd.setCursor(0,1);
		 lcd.print("                ");
	}
}

 
 void loop()
{
   	while(1)
	check_weight_empty();
	
}
  
   
void home()
{ 
	//positions to fill the left container
	
	PORTC=0; //cup under right shutter
	cup=left;
}







void shutter_open() //opens the shutter, activates the pressing piston
{
	if(cup) 
	{
		cup=!cup;	
		
		if(!digitalRead(shutter_sensor_l))
		digitalWrite(flap_piston,left);
		
		else {digitalWrite(conveyor,0); digitalWrite(conveyor_led,0);}
		
		if(!digitalRead(presence_sensor_r))
		{
			digitalWrite(shutter_piston_r,1);
		
			for(uint8_t x=0;x<=100;x++)
			{ 
				check_new_cup();
				conveyor_stop();
				manual_check();
				stop_check();
			
				if((!p_flag)&&!new_cup_flag_l)
				{
					lcd.setCursor(0,1);
					lcd.print("                ");
					lcd.setCursor(1,1);
					lcd.print("Place new L Cup");
					p_flag=true;
			    }
			
				_delay_ms(10);
			}
		
		
		digitalWrite(shutter_piston_r,0);
		
		new_cup_flag_r=false;
		
		for(int x=0;x<=300;x++)
		{ 
		  check_new_cup();
		  conveyor_stop();
		  manual_check();
		  stop_check();
		  
		  
		  if(!digitalRead(weight_sensor_l))
		  digitalWrite(flap_piston,right);
		  
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
		
		digitalWrite(change_piston,0);
		
	
	}
	
	else if(!cup)
	{
		cup=!cup;
		
		if(!digitalRead(shutter_sensor_r))
		digitalWrite(flap_piston,right);
		
		else { 
			   digitalWrite(conveyor,0);
			   digitalWrite(conveyor_led,0);
		     }
		
		if(!digitalRead(presence_sensor_l))
	    {
			digitalWrite(shutter_piston_l,1);
			
				for(uint8_t x=0;x<=100;x++)
				{   
					check_new_cup();
					conveyor_stop();
					manual_check();
					stop_check();
					
					if((!p_flag)&&!new_cup_flag_r)
					{
						lcd.setCursor(0,1);
						lcd.print("                ");
						lcd.setCursor(1,1);
						lcd.print("Place new R Cup");
						p_flag=true;
					}
					_delay_ms(10);
				}
				
				digitalWrite(shutter_piston_l,0);
		    
			    new_cup_flag_l=false;
			
				for(int x=0;x<=300;x++)
				{
					check_new_cup();
					conveyor_stop();
					manual_check();
					stop_check();
					
					if(!digitalRead(weight_sensor_r))
					 digitalWrite(flap_piston,left);
					 
					 if(new_cup_flag_r)
					 {
						 lcd.setCursor(0,1);
						 lcd.print("                ");
					 }
					_delay_ms(10);
				}
				
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

				digitalWrite(change_piston,1);
		}
	}
	}
				
bool cup_presence_check()  //checks if a cup is present and positions it under the shutter
{  
	if (cup)
	{
		if(!digitalRead(presence_sensor_r))
		return true;
			
		
		
		else return false;
	}
	
 else 
	{
		if(!digitalRead(presence_sensor_l)) 
	    return true;
		
		else return false;
	}
	
}

bool cup_position_check() //checks if the cup is under the shutter
{
	if (cup)
	{   
		if(!digitalRead(cup_change_sensor_r)) 
		return true;
		
		else return false;
	}
	
	else
	{  
		if (!digitalRead(cup_change_sensor_l))
		return true;
		
		else return false;
	}
}
		



void halt()
{   
	static bool conveyor_flag=false;
	
	if(digitalRead(conveyor))
	{
		digitalWrite(conveyor,0);
		digitalWrite(conveyor_led,0);
		conveyor_flag=true;
	}
	
	lcd.clear();
	lcd.setCursor(5,0);
	lcd.print("Halted");
	lcd.setCursor(4,1);
	lcd.print("Press ON");
	
	
	while(digitalRead(start_button))
	{
	      
		   manual_check();
		   lcd.clear();
		   lcd.print("ERROR");
		   _delay_ms(2000);
	}
	
	lcd.clear();
	lcd.setCursor(2,0);
	lcd.print("Auto mode ON");
	
	if(conveyor_flag)
	{
		digitalWrite(conveyor,1);
		digitalWrite(conveyor_led,1);
		conveyor_flag=false;
	}
	
	
}

void manual_check()
{
    static bool hi= true;
	
	MANUAL:
	if(!digitalRead(auto_manaul))
	{   
		hi=false;
		start_stop_status=false;
		new_cup_flag_l=true;
		new_cup_flag_r=true;
		
		home();
		digitalWrite(manaul_out,1);
		
		if(!hi)
		{
	    lcd.clear();
		lcd.setCursor(1,0);
	    lcd.print("Manual mode ON");
		hi=true;
		}
		
		while(!digitalRead(auto_manaul));
		
		digitalWrite(manaul_out,0);
		
		home();
		
	    _delay_ms(30);
	  
		while(digitalRead(auto_manaul)&&(!start_stop_status))
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
			
			if(!digitalRead(start_button))
			{   
				lcd.setCursor(0,1);
				lcd.print("                ");
				start_stop_status=true;
				home();
				var_reset();
				loop();
				break;
			}
		}
		
	
	
	if(!digitalRead(auto_manaul))
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


void check_weight_empty()
{   
	_delay_ms(4);
	manual_check();
	if(digitalRead(weight_sensor_l)&&!digitalRead(shutter_sensor_l))
	{   
		
		digitalWrite(flap_piston,left);
		digitalWrite(conveyor,1);
		digitalWrite(conveyor_led,1);
		
		while(digitalRead(weight_sensor_l))
		{
			
			cup=left;
			manual_check();
			conveyor_stop();
			cup=!cup;
			check_new_cup();
			cup=!cup;
			
			stop_check();
		}
	}
	
	 if(digitalRead(weight_sensor_r)&&!digitalRead(shutter_sensor_r))
	{   
	    digitalWrite(flap_piston,right);
		digitalWrite(conveyor,1);
		digitalWrite(conveyor_led,1);
	}
	
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
	cup=right;
		
//starting of right cup

while(digitalRead(weight_sensor_r))
{   
	manual_check();
	cup=right;
	
	conveyor_stop();
	cup=!cup;
	check_new_cup();
	cup=!cup;
	stop_check();
}

if(!digitalRead(weight_sensor_r))
{
	digitalWrite(flap_piston,left);
}

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
	cup=left;
}

void conveyor_stop()
{
	
	if(digitalRead(shutter_sensor_l)&&digitalRead(shutter_sensor_r))
	{
		digitalWrite(conveyor,0);
		digitalWrite(conveyor_led,0);
	}
	
	
	else if((!digitalRead(weight_sensor_l))&&(!digitalRead(weight_sensor_r)))
	{
		digitalWrite(conveyor,0);
		digitalWrite(conveyor_led,0);
	}
	
	
	else {
		   digitalWrite(conveyor,1); 
		   digitalWrite(conveyor_led,1);
	}
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
	if(!digitalRead(stop_button))
	halt();
}








