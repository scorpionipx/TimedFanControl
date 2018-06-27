/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRUE 1
#define FALSE 0

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define STATE_ERROR 99  // internal error occurred
#define STATE_INIT 0  // uC initializing
#define STATE_FAN_ON 1  // fans are turned on
#define STATE_FAN_OFF 2  // fans are turned off
#define STATE_CONFIG 3  // configuring T_ON and T_OFF times

#define DEFAULT_T_ON_MINUTES 4
#define DEFAULT_T_ON_HOURS 0
#define DEFAULT_T_OFF_MINUTES 15
#define DEFAULT_T_OFF_HOURS 2 

#define T_ON_HOURS_EEPROM_ADDRESS 0x00  // store & load adresses
#define T_ON_MINUTES_EEPROM_ADDRESS 0x01  // store & load adresses
#define T_OFF_HOURS_EEPROM_ADDRESS 0x02  // store & load adresses
#define T_OFF_MINUTES_EEPROM_ADDRESS 0x03  // store & load adresses

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

unsigned short STATE = STATE_INIT; // state machine

unsigned short T_ON_MINUTES = DEFAULT_T_ON_MINUTES;  // time amount to keep fans turned on
unsigned short T_ON_HOURS = DEFAULT_T_ON_HOURS;  // time amount to keep fans turned on
unsigned short T_OFF_MINUTES = DEFAULT_T_OFF_MINUTES;  // time amount to keep fans turned off
unsigned short T_OFF_HOURS = DEFAULT_T_OFF_HOURS;  // time amount to keep fans turned of

String INTERNAL_ERROR = "No error";

void setup()   
{            

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done

  // Clear the buffer.
  display.clearDisplay();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
  display.println("Scorpion");  
  display.setCursor(50,16);
  display.println("IPX");  
  display.display();

  delay(500);
  load_times_from_eeprom();

  if(STATE == STATE_INIT)
  {
    display_times();
    delay(500);
  }
}


void loop() {
  
}

void display_times(void)
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("ON:  ");  
  display.print(T_ON_HOURS / 10);
  display.print(T_ON_HOURS % 10);
  display.print(":");
  display.print(T_ON_MINUTES / 10);
  display.print(T_ON_MINUTES % 10);
  display.setCursor(0,16);
  display.print("OFF: "); 
  display.print(T_OFF_HOURS / 10);  
  display.print(T_OFF_HOURS % 10);  
  display.print(":");
  display.print(T_OFF_MINUTES / 10); 
  display.print(T_OFF_MINUTES % 10); 
  display.display();
}

void load_times_from_eeprom(void)
{
  T_ON_MINUTES = (unsigned short)EEPROM.read(T_ON_MINUTES_EEPROM_ADDRESS);
  T_ON_HOURS = (unsigned short)EEPROM.read(T_ON_HOURS_EEPROM_ADDRESS);
  
  T_OFF_MINUTES = (unsigned short)EEPROM.read(T_OFF_MINUTES_EEPROM_ADDRESS);
  T_OFF_HOURS = (unsigned short)EEPROM.read(T_OFF_HOURS_EEPROM_ADDRESS);

  unsigned char error_loading_eeprom_data = 0;

  if(T_ON_MINUTES > 59)
  {
    INTERNAL_ERROR = "Invalid EEPROM data ~om! Default data values uploaded. Please reset device! ";
    error_loading_eeprom_data = TRUE;
    goto _error;
  }
  if(T_OFF_MINUTES > 59)
  {
    INTERNAL_ERROR = "Invalid EEPROM data ~fm! Default data values uploaded. Please reset device! ";
    error_loading_eeprom_data = TRUE;
    goto _error;
  }
  if(T_ON_HOURS > 99)
  {
    INTERNAL_ERROR = "Invalid EEPROM data ~oh! Default data values uploaded. Please reset device! ";
    error_loading_eeprom_data = TRUE;
    goto _error;
  }
  if(T_OFF_HOURS > 99)
  {
    INTERNAL_ERROR = "Invalid EEPROM data ~fh! Default data values uploaded. Please reset device! ";
    error_loading_eeprom_data = TRUE;
    goto _error;
  }

  _error:
  if (error_loading_eeprom_data == TRUE)
  { 
    upload_times_to_eeprom(DEFAULT_T_ON_MINUTES, DEFAULT_T_ON_HOURS, DEFAULT_T_OFF_MINUTES, DEFAULT_T_OFF_HOURS);
    go_to_state(STATE_ERROR);
  }
}

void load_default_times(void)
{
  T_ON_MINUTES = DEFAULT_T_ON_MINUTES;  // time amount to keep fans turned on
  T_ON_HOURS = DEFAULT_T_ON_HOURS;  // time amount to keep fans turned on
  T_OFF_MINUTES = DEFAULT_T_OFF_MINUTES;  // time amount to keep fans turned off
  T_OFF_HOURS = DEFAULT_T_OFF_HOURS;  // time amount to keep fans turned of
}

void upload_times_to_eeprom(unsigned short t_on_minutes, unsigned short t_on_hours, unsigned short t_off_minutes, unsigned short t_off_hours)
{
  EEPROM.write(T_ON_MINUTES_EEPROM_ADDRESS, t_on_minutes);
  EEPROM.write(T_ON_HOURS_EEPROM_ADDRESS, t_on_hours);
  EEPROM.write(T_OFF_MINUTES_EEPROM_ADDRESS, t_off_minutes);
  EEPROM.write(T_OFF_HOURS_EEPROM_ADDRESS, t_off_hours);
}

void go_to_state(unsigned short state)
{
  switch(state)
  {
    case STATE_ERROR:
    {
      STATE = STATE_ERROR;
      display.setTextSize(1);
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Error: "); 
      display.print(INTERNAL_ERROR); 
      display.display();
      break;
    }
    default:
    {
      STATE = STATE_ERROR;
      display.setTextSize(1);
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Error: undefined state " + state); 
      display.print(INTERNAL_ERROR); 
      display.display();
    }
  }
}



