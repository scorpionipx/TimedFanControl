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

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define STATE_ERROR 99  // internal error occurred
#define STATE_INIT 0  // uC initializing
#define STATE_FAN_ON 1  // fans are turned on
#define STATE_FAN_OFF 2  // fans are turned off
#define STATE_CONFIG 3  // configuring T_ON and T_OFF times

#define FAN_1_PIN 0  // PD0 
#define FAN_2_PIN 1  // PD1

#define BUTTON_1_PIN 6  // PD6 
#define BUTTON_2_PIN 7  // PD7 
#define BUTTON_3_PIN 8  // PB0 

#define DEFAULT_T_ON_SECONDS 0
#define DEFAULT_T_ON_MINUTES 4
#define DEFAULT_T_ON_HOURS 0

#define DEFAULT_T_OFF_SECONDS 0
#define DEFAULT_T_OFF_MINUTES 15
#define DEFAULT_T_OFF_HOURS 2 

#define T_ON_HOURS_EEPROM_ADDRESS 0x00  // store & load adresses
#define T_ON_MINUTES_EEPROM_ADDRESS 0x01  // store & load adresses
#define T_ON_SECONDS_EEPROM_ADDRESS 0x02  // store & load adresses

#define T_OFF_HOURS_EEPROM_ADDRESS 0x03  // store & load adresses
#define T_OFF_MINUTES_EEPROM_ADDRESS 0x04  // store & load adresses
#define T_OFF_SECONDS_EEPROM_ADDRESS 0x05  // store & load adresses

unsigned short STATE = STATE_INIT; // state machine
unsigned short DISPLAY_IS_ON = TRUE;  // display status

unsigned short T_ON_SECONDS = DEFAULT_T_ON_SECONDS;  // time amount to keep fans turned on
unsigned short T_ON_MINUTES = DEFAULT_T_ON_MINUTES;  // time amount to keep fans turned on
unsigned short T_ON_HOURS = DEFAULT_T_ON_HOURS;  // time amount to keep fans turned on

unsigned short T_OFF_SECONDS = DEFAULT_T_OFF_SECONDS;  // time amount to keep fans turned off
unsigned short T_OFF_MINUTES = DEFAULT_T_OFF_MINUTES;  // time amount to keep fans turned off
unsigned short T_OFF_HOURS = DEFAULT_T_OFF_HOURS;  // time amount to keep fans turned of

String INTERNAL_ERROR = "No error";

struct clock_structure
{
  unsigned short seconds;  // count hours
  unsigned short minutes;  // count_minutes
  unsigned short hours;  // count seconds
  unsigned short is_paused;  // specify if clock is paused
}CLOCK;

void setup()   
{            
  init_fan_control();
  init_buttons_control();
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
    delay(1500);
    go_to_state(STATE_FAN_OFF);
  }
}
 

void loop() 
{
  delay(10);
  switch(STATE)
  {
    case STATE_FAN_OFF:
    {
      decrease_clock_time();
      if (DISPLAY_IS_ON ==  TRUE)
      {
        display_clock();
      }

      if (CLOCK.hours == 0 && CLOCK.minutes == 0 && CLOCK.seconds == 0)
      {
        display.setTextSize(2);
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("ACTIVATINGFANS"); 
        display.display();
        delay(500);
        go_to_state(STATE_FAN_ON);
      }
      break;  
    }
    case STATE_FAN_ON:
    {
      decrease_clock_time();
      if (DISPLAY_IS_ON ==  TRUE)
      {
        display_clock();
      }

      if (CLOCK.hours == 0 && CLOCK.minutes == 0 && CLOCK.seconds == 0)
      {
        display.setTextSize(2);
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("STOPPING  FANS"); 
        display.display();
        go_to_state(STATE_FAN_OFF);
        delay(500);
      }
      break;  
    }
    default:
    {
      break;
    }
  }
}

void display_clock(void)
{
  display.setTextSize(2);
  display.clearDisplay();
  display.setCursor(0,0);
  if(STATE == STATE_FAN_OFF)
  {
    display.print("Fans OFF"); 
  }
  else if(STATE == STATE_FAN_ON)
  {
    display.print("Fans ON");
  }
  display.setCursor(0,16);
  display.print(CLOCK.hours / 10);
  display.print(CLOCK.hours % 10);
  display.print(":");
  display.print(CLOCK.minutes / 10);
  display.print(CLOCK.minutes % 10);
  display.print(":");
  display.print(CLOCK.seconds / 10);
  display.print(CLOCK.seconds % 10);
  display.display();
}

void display_times(void)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("TIME HH:MM:SS");  
  display.setCursor(0,8);
  display.print("ON:  ");  
  display.print(T_ON_HOURS / 10);
  display.print(T_ON_HOURS % 10);
  display.print(":");
  display.print(T_ON_MINUTES / 10);
  display.print(T_ON_MINUTES % 10);
  display.print(":");
  display.print(T_ON_SECONDS / 10);
  display.print(T_ON_SECONDS % 10);
  display.setCursor(0,16);
  display.print("OFF: "); 
  display.print(T_OFF_HOURS / 10);  
  display.print(T_OFF_HOURS % 10);  
  display.print(":");
  display.print(T_OFF_MINUTES / 10); 
  display.print(T_OFF_MINUTES % 10);
  display.print(":");
  display.print(T_OFF_SECONDS / 10);
  display.print(T_OFF_SECONDS % 10);
  display.display();
}

void load_times_from_eeprom(void)
{
  T_ON_SECONDS = (unsigned short)EEPROM.read(T_ON_SECONDS_EEPROM_ADDRESS);
  T_ON_MINUTES = (unsigned short)EEPROM.read(T_ON_MINUTES_EEPROM_ADDRESS);
  T_ON_HOURS = (unsigned short)EEPROM.read(T_ON_HOURS_EEPROM_ADDRESS);
  
  T_OFF_SECONDS = (unsigned short)EEPROM.read(T_OFF_SECONDS_EEPROM_ADDRESS);
  T_OFF_MINUTES = (unsigned short)EEPROM.read(T_OFF_MINUTES_EEPROM_ADDRESS);
  T_OFF_HOURS = (unsigned short)EEPROM.read(T_OFF_HOURS_EEPROM_ADDRESS);

  unsigned char error_loading_eeprom_data = 0;

  if(T_ON_SECONDS > 59)
  {
    INTERNAL_ERROR = "Invalid EEPROM data ~os! Default data values uploaded. Please reset device! ";
    error_loading_eeprom_data = TRUE;
    goto _error;
  }
  if(T_OFF_SECONDS > 59)
  {
    INTERNAL_ERROR = "Invalid EEPROM data ~fs! Default data values uploaded. Please reset device! ";
    error_loading_eeprom_data = TRUE;
    goto _error;
  }
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
    upload_times_to_eeprom(DEFAULT_T_ON_SECONDS, DEFAULT_T_ON_MINUTES, DEFAULT_T_ON_HOURS, DEFAULT_T_OFF_SECONDS, DEFAULT_T_OFF_MINUTES, DEFAULT_T_OFF_HOURS);
    go_to_state(STATE_ERROR);
  }
}

void load_default_times(void)
{
  T_ON_SECONDS = DEFAULT_T_ON_SECONDS;  // time amount to keep fans turned on
  T_ON_MINUTES = DEFAULT_T_ON_MINUTES;  // time amount to keep fans turned on
  T_ON_HOURS = DEFAULT_T_ON_HOURS;  // time amount to keep fans turned on
  
  T_OFF_SECONDS = DEFAULT_T_OFF_SECONDS;  // time amount to keep fans turned off
  T_OFF_MINUTES = DEFAULT_T_OFF_MINUTES;  // time amount to keep fans turned off
  T_OFF_HOURS = DEFAULT_T_OFF_HOURS;  // time amount to keep fans turned of
}

void upload_times_to_eeprom(unsigned short t_on_seconds, unsigned short t_on_minutes, unsigned short t_on_hours, 
unsigned short t_off_seconds, unsigned short t_off_minutes, unsigned short t_off_hours)
{
  EEPROM.write(T_ON_SECONDS_EEPROM_ADDRESS, t_on_seconds);
  EEPROM.write(T_ON_MINUTES_EEPROM_ADDRESS, t_on_minutes);
  EEPROM.write(T_ON_HOURS_EEPROM_ADDRESS, t_on_hours);
  
  EEPROM.write(T_OFF_SECONDS_EEPROM_ADDRESS, t_off_seconds);
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
    case STATE_FAN_ON:
    {
      STATE = STATE_FAN_ON;
      turn_on_fans();
      set_clock(T_ON_HOURS, T_ON_MINUTES, T_ON_SECONDS);
      CLOCK.is_paused = FALSE;
      break;
    }
    case STATE_FAN_OFF:
    {
      STATE = STATE_FAN_OFF;
      turn_off_fans();
      set_clock(T_OFF_HOURS, T_OFF_MINUTES, T_OFF_SECONDS);
      CLOCK.is_paused = FALSE;
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

void set_clock(unsigned short hours, unsigned short minutes, unsigned short seconds)
{
  CLOCK.hours = hours;
  CLOCK.minutes = minutes;
  CLOCK.seconds = seconds;
}

void reset_clock(void)
{
  set_clock(0, 0, 0);
}

void decrease_clock_time(void)
{
  if(CLOCK.seconds == 0)
  {
    if(CLOCK.minutes == 0)
    {
      if(CLOCK.hours == 0)
      {
        
      }
      else
      {
        CLOCK.hours --;
        CLOCK.minutes = 59;
        CLOCK.seconds = 59;
      }
    }
    else
    {
      CLOCK.minutes --;
      CLOCK.seconds = 59;
    }
  }
  else
  {
    CLOCK.seconds --;
  }
}

void init_fan_control(void)
{
  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  
  digitalWrite(FAN_1_PIN, LOW);
  digitalWrite(FAN_2_PIN, LOW);
}

void init_buttons_control(void)
{
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);
}

void turn_on_fans(void)
{
  turn_on_fan_1();
  turn_on_fan_2();
}

void turn_off_fans(void)
{
  turn_off_fan_1();
  turn_off_fan_2();
}

void turn_on_fan_1(void)
{
  digitalWrite(FAN_1_PIN, HIGH);
}
void turn_off_fan_1(void)
{
  digitalWrite(FAN_1_PIN, LOW);
}
void turn_on_fan_2(void)
{
  digitalWrite(FAN_2_PIN, HIGH);
}
void turn_off_fan_2(void)
{
  digitalWrite(FAN_2_PIN, LOW);
}


