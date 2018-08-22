// APPLICATION SETTINGS
#define DEFAULT_MOTOR_POWER 10   // default power for motor control (max 10)

#define IDLE_TIME_THRESHOLD 30    // time to wait (in seconds) before machine goes into idle state (for improving power consumption)

#define DEFAULT_T_ON_SECONDS 0    // default time to keep motors powered on
#define DEFAULT_T_ON_MINUTES 4
#define DEFAULT_T_ON_HOURS 0

#define DEFAULT_T_OFF_SECONDS 0   // default time to keep motors powered off
#define DEFAULT_T_OFF_MINUTES 15
#define DEFAULT_T_OFF_HOURS 2 

// MOTORS HARDWARE SETTINGS
#define MOTOR_1_PIN  3  // digital pin used to command driver for motor 1 (PD3)
#define MOTOR_2_PIN  5  // digital pin used to command driver for motor 2 (PD5)
#define MOTOR_3_PIN  6  // digital pin used to command driver for motor 3 (PD6)
#define MOTOR_4_PIN  9  // digital pin used to command driver for motor 4 (PB1)
#define MOTOR_5_PIN 10  // digital pin used to command driver for motor 5 (PB2)
#define MOTOR_6_PIN 11  // digital pin used to command driver for motor 6 (PB3)

// BUTTONS HARDWARE SETTINGS
#define BUTTON_1_PIN 4  // digital pin used to read button state (PD4) - used to change selected config value
#define BUTTON_2_PIN 7  // digital pin used to read button state (PD7) - used to select next config value
#define BUTTON_3_PIN 8  // digital pin used to read button state (PB0) - used to reset internal timer

// DISPLAY HARDWARE SETTINGS
#define DISPLAY_SWITCH_PIN 2  // digital pin used to control display's power switch

// GENERIC
#define TRUE 1
#define FALSE 0

#define INCREASE 1
#define DECREASE -1

#define LED LED_BUILTIN
#define TURN_ON_LED  (digitalWrite(LED, HIGH))
#define TURN_OFF_LED (digitalWrite(LED, LOW))

#define __100_MS__ 100

// STATE MACHINE
#define STATE_ERROR       99  // internal error
#define STATE_INIT         0  // uC initializing
#define STATE_MOTORS_ON    1  // motors are turned on
#define STATE_MOTORS_OFF   2  // motors are turned off
#define STATE_CONFIG       3  // modify settings / configurations
#define STATE_IDLE         4  // low power consumption mode

// EEPROM MAP
#define T_ON_HOURS_EEPROM_ADDRESS     0x00  // store & load EEPROM adresses
#define T_ON_MINUTES_EEPROM_ADDRESS   0x01  // store & load EEPROM adresses
#define T_ON_SECONDS_EEPROM_ADDRESS   0x02  // store & load EEPROM adresses

#define T_OFF_HOURS_EEPROM_ADDRESS    0x03  // store & load EEPROM adresses
#define T_OFF_MINUTES_EEPROM_ADDRESS  0x04  // store & load EEPROM adresses
#define T_OFF_SECONDS_EEPROM_ADDRESS  0x05  // store & load EEPROM adresses

#define MOTOR_1_POWER_EEPROM_ADDRESS  0x06  // store & load EEPROM adresses
#define MOTOR_2_POWER_EEPROM_ADDRESS  0x07  // store & load EEPROM adresses
#define MOTOR_3_POWER_EEPROM_ADDRESS  0x08  // store & load EEPROM adresses
#define MOTOR_4_POWER_EEPROM_ADDRESS  0x09  // store & load EEPROM adresses
#define MOTOR_5_POWER_EEPROM_ADDRESS  0x0A  // store & load EEPROM adresses
#define MOTOR_6_POWER_EEPROM_ADDRESS  0x0B  // store & load EEPROM adresses

#define MOTORS_ENABLED_EEPROM_ADDRESS 0x0C  // store & load EEPROM adresses

// APPLICATION INNER SETTING
#define CONFIG_INDEX_T_ON_H_MSD 0
#define CONFIG_INDEX_T_ON_H_LSD 1
#define CONFIG_INDEX_T_ON_M_MSD 2
#define CONFIG_INDEX_T_ON_M_LSD 3
#define CONFIG_INDEX_T_ON_S_MSD 4
#define CONFIG_INDEX_T_ON_S_LSD 5

#define CONFIG_INDEX_T_OFF_H_MSD 6
#define CONFIG_INDEX_T_OFF_H_LSD 7
#define CONFIG_INDEX_T_OFF_M_MSD 8
#define CONFIG_INDEX_T_OFF_M_LSD 9
#define CONFIG_INDEX_T_OFF_S_MSD 10
#define CONFIG_INDEX_T_OFF_S_LSD 11

#define CONFIG_INDEX_MOTOR_1_ENABLED 12
#define CONFIG_INDEX_MOTOR_2_ENABLED 13
#define CONFIG_INDEX_MOTOR_3_ENABLED 14
#define CONFIG_INDEX_MOTOR_4_ENABLED 15
#define CONFIG_INDEX_MOTOR_5_ENABLED 16
#define CONFIG_INDEX_MOTOR_6_ENABLED 17

#define CONFIG_INDEX_MOTOR_1_POWER 18
#define CONFIG_INDEX_MOTOR_2_POWER 19
#define CONFIG_INDEX_MOTOR_3_POWER 20
#define CONFIG_INDEX_MOTOR_4_POWER 21
#define CONFIG_INDEX_MOTOR_5_POWER 22
#define CONFIG_INDEX_MOTOR_6_POWER 23

#define MOTOR_MAX_POWER 10

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

String INTERNAL_ERROR = "OK";

struct clock_structure
{
  unsigned short seconds;    // count hours
  unsigned short minutes;    // count_minutes
  unsigned short hours;      // count seconds
  unsigned short is_paused;  // indicates that clock is paused
} CLOCK;                      // internal clock used to track time

struct boolean_struct
{
   unsigned short MOTOR_1_ENABLED_ : 1;  // motor 1 enabled status
   unsigned short MOTOR_2_ENABLED_ : 1;  // motor 2 enabled status
   unsigned short MOTOR_3_ENABLED_ : 1;  // motor 3 enabled status
   unsigned short MOTOR_4_ENABLED_ : 1;  // motor 4 enabled status
   unsigned short MOTOR_5_ENABLED_ : 1;  // motor 5 enabled status
   unsigned short MOTOR_6_ENABLED_ : 1;  // motor 6 enabled status
   unsigned short DISPLAY_IS_ON_ : 1;    // display power status
   unsigned short INVALID_EEPROM_DATA_ : 1;
} BOOLEAN;                                // bit field struct to store boolean values

#define MOTOR_1_ENABLED BOOLEAN.MOTOR_1_ENABLED_
#define MOTOR_2_ENABLED BOOLEAN.MOTOR_2_ENABLED_
#define MOTOR_3_ENABLED BOOLEAN.MOTOR_3_ENABLED_
#define MOTOR_4_ENABLED BOOLEAN.MOTOR_4_ENABLED_
#define MOTOR_5_ENABLED BOOLEAN.MOTOR_5_ENABLED_
#define MOTOR_6_ENABLED BOOLEAN.MOTOR_6_ENABLED_
#define DISPLAY_IS_ON BOOLEAN.DISPLAY_IS_ON_
#define INVALID_EEPROM_DATA BOOLEAN.INVALID_EEPROM_DATA_

#define ALL_MOTORS_ENABLED 0b00111111

int STATE = STATE_INIT;    // initial state at power up
int CONFIG_INDEX = -1;
unsigned short CONFIG_INDEX_SHOWED = TRUE;

unsigned short T_ON_SECONDS = DEFAULT_T_ON_SECONDS;    // time amount to keep fans turned on
unsigned short T_ON_MINUTES = DEFAULT_T_ON_MINUTES;    // time amount to keep fans turned on
unsigned short T_ON_HOURS = DEFAULT_T_ON_HOURS;        // time amount to keep fans turned on

unsigned short T_OFF_SECONDS = DEFAULT_T_OFF_SECONDS;  // time amount to keep fans turned off
unsigned short T_OFF_MINUTES = DEFAULT_T_OFF_MINUTES;  // time amount to keep fans turned off
unsigned short T_OFF_HOURS = DEFAULT_T_OFF_HOURS;      // time amount to keep fans turned off

unsigned short MOTOR_1_POWER = DEFAULT_MOTOR_POWER;
unsigned short MOTOR_2_POWER = DEFAULT_MOTOR_POWER;
unsigned short MOTOR_3_POWER = DEFAULT_MOTOR_POWER;
unsigned short MOTOR_4_POWER = DEFAULT_MOTOR_POWER;
unsigned short MOTOR_5_POWER = DEFAULT_MOTOR_POWER;
unsigned short MOTOR_6_POWER = DEFAULT_MOTOR_POWER;

unsigned short counter_100_ms = 0;
unsigned short idle_counter = 0;

void setup() 
{
//  upload_times_to_eeprom(30, 2, 0, 0, 30 ,1);
//  upload_motors_enabled_to_eeprom(ALL_MOTORS_ENABLED);
//  upload_motors_power_to_eeprom(DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER);
  
  init_uC();
  init_application();
  
  if(STATE == STATE_INIT)
  {
    go_to_state(STATE_MOTORS_OFF);
  }
  else
  {
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("STATE INIT ERROR");
    display.display();
    delay(500);
    go_to_state(STATE_ERROR);
  }
}

void loop() 
{
  delay(__100_MS__);
  count();
  display_info();
  read_buttons();
}

void display_settings(void)
{
  display_times();
  delay(2000);
  display_motors_enabled();
  delay(2000);
  display_motors_powers();
  delay(2000);
}

void count(void)
{
  if (CLOCK.is_paused)
  {
    return;
  }
  counter_100_ms ++;
  if(counter_100_ms >= 10)
  {
    if(DISPLAY_IS_ON)
    {
      idle_counter ++;
    }
    counter_100_ms = 0;
    decrease_clock_time();
    display_clock();
  }

  if(idle_counter >= IDLE_TIME_THRESHOLD)
  {
    go_to_sleep();
  }

  if (CLOCK.hours == 0 && CLOCK.minutes == 0 && CLOCK.seconds == 0)
  {
    toggle_motors_state();
  }
}

void go_to_sleep(void)
{
    display.setTextSize(2);
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Going to  sleep."); 
    display.display();
    delay(200);
    display.print(".");
    display.display(); 
    delay(200);
    display.print(".");
    display.display(); 
    delay(200);
      
    turn_off_display();
    idle_counter = 0;
}

void toggle_motors_state(void)
{
  switch(STATE)
  {
    case STATE_MOTORS_ON:
    {
      go_to_state(STATE_MOTORS_OFF);
      break;
    }
    case STATE_MOTORS_OFF:
    {
      go_to_state(STATE_MOTORS_ON);
      break;
    }
  }
}

void display_info(void)
{
  switch(STATE)
  {
    case STATE_ERROR:
    {
      break;
    }
    case STATE_MOTORS_ON:
    {
      break;
    }
    case STATE_MOTORS_OFF:
    {
      break;
    }
    case STATE_CONFIG:
    {
      if(CONFIG_INDEX == -1)
      {
        INTERNAL_ERROR = "CONFIG_STATE reached with CONFIG_INDEX -1 in display_info!";
      }

      if(CONFIG_INDEX <= CONFIG_INDEX_T_OFF_S_LSD)
      {
        display_times();
        return;
      }
      
      if(CONFIG_INDEX <= CONFIG_INDEX_MOTOR_6_ENABLED)
      {
        display_motors_enabled();
        return;
      }
      
      if(CONFIG_INDEX <= CONFIG_INDEX_MOTOR_6_POWER)
      {
        display_motors_powers();
        return;
      }
      
      break;
    }
    default:
    {
      INTERNAL_ERROR = "Invalid state specified in display_info!";
      go_to_state(STATE_ERROR);
      break;
    }
  }
}

void read_buttons(void)
{
  if(digitalRead(BUTTON_1_PIN) == LOW)
  {
    button_1_function();
    return;
  }
  if(digitalRead(BUTTON_2_PIN) == LOW)
  {
    button_2_function();
    return;
  }
  if(digitalRead(BUTTON_3_PIN) == LOW)
  {
    button_3_function();
    return;
  }
}

void __generic_button_pressed__(void)
{
  idle_counter = 0;
}

void button_1_function(void)
{
  __generic_button_pressed__();
  if(!DISPLAY_IS_ON)
  {
    turn_on_display();
    return;
  }
  
  switch(STATE)
  {
    case STATE_MOTORS_ON:
    case STATE_MOTORS_OFF:
    {
      display_settings();
      break;
    }
    
    case STATE_CONFIG:
    {
      modify_config(INCREASE);
      delay(50);
      break;
    }
  }
}

void button_2_function(void)
{
  __generic_button_pressed__();
  if(!DISPLAY_IS_ON)
  {
    turn_on_display();
    return;
  }

  switch(STATE)
  {
    case STATE_MOTORS_ON:
    case STATE_MOTORS_OFF:
    {
      delay(150);
      go_to_state(STATE_CONFIG);
      break;
    }
    
    case STATE_CONFIG:
    {
      CONFIG_INDEX ++;
      if(CONFIG_INDEX > CONFIG_INDEX_MOTOR_6_POWER)
      {
        save_configuration();
      }
      delay(50);
      break;
    }
  }
}

void button_3_function(void)
{
  __generic_button_pressed__();
  if(!DISPLAY_IS_ON)
  {
    turn_on_display();
    return;
  }

  switch(STATE)
  {
    case STATE_MOTORS_ON:
    case STATE_MOTORS_OFF:
    {
      toggle_motors_state();
      break;
    }
    case STATE_CONFIG:
    {
      save_configuration();
      break;
    }
    default:
    {
      INTERNAL_ERROR = "Invalid STATE in button_3_function!";
      go_to_state(STATE_ERROR);
    }
  }
}

void modify_config(short unit)
{
//
//#define CONFIG_INDEX_T_ON_H_MSD 0
//#define CONFIG_INDEX_T_ON_H_LSD 1
//#define CONFIG_INDEX_T_ON_M_MSD 2
//#define CONFIG_INDEX_T_ON_M_LSD 3
//#define CONFIG_INDEX_T_ON_S_MSD 4
//#define CONFIG_INDEX_T_ON_S_LSD 5
//
//#define CONFIG_INDEX_T_OFF_H_MSD 6
//#define CONFIG_INDEX_T_OFF_H_LSD 7
//#define CONFIG_INDEX_T_OFF_M_MSD 8
//#define CONFIG_INDEX_T_OFF_M_LSD 9
//#define CONFIG_INDEX_T_OFF_S_MSD 10
//#define CONFIG_INDEX_T_OFF_S_LSD 11
//
//#define CONFIG_INDEX_MOTOR_1_ENABLED 12
//#define CONFIG_INDEX_MOTOR_2_ENABLED 13
//#define CONFIG_INDEX_MOTOR_3_ENABLED 14
//#define CONFIG_INDEX_MOTOR_4_ENABLED 15
//#define CONFIG_INDEX_MOTOR_5_ENABLED 16
//#define CONFIG_INDEX_MOTOR_6_ENABLED 17
//
//#define CONFIG_INDEX_MOTOR_1_POWER 18
//#define CONFIG_INDEX_MOTOR_2_POWER 19
//#define CONFIG_INDEX_MOTOR_3_POWER 20
//#define CONFIG_INDEX_MOTOR_4_POWER 21
//#define CONFIG_INDEX_MOTOR_5_POWER 22
//#define CONFIG_INDEX_MOTOR_6_POWER 23
  
  switch(CONFIG_INDEX)
  {
    case -1:
    {
      INTERNAL_ERROR = "modify_config called with CONFIG_INDEX -1!";
      go_to_state(STATE_ERROR);
      break;
    }

    case CONFIG_INDEX_T_ON_H_MSD:
    {
      T_ON_HOURS += 10 * unit;
      T_ON_HOURS %= 60;
      break;
    }

    case CONFIG_INDEX_T_ON_H_LSD:
    {
      T_ON_HOURS += unit;
      if(T_ON_HOURS % 10 == 0)
      {
        T_ON_HOURS -= 10;
      }
      break;
    }

    case CONFIG_INDEX_T_ON_M_MSD:
    {
      T_ON_MINUTES += 10 * unit;
      T_ON_MINUTES %= 60;
      break;
    }

    case CONFIG_INDEX_T_ON_M_LSD:
    {
      T_ON_MINUTES += unit;
      if(T_ON_MINUTES % 10 == 0)
      {
        T_ON_MINUTES -= 10;
      }
      break;
    }

    case CONFIG_INDEX_T_ON_S_MSD:
    {
      T_ON_SECONDS += 10 * unit;
      T_ON_SECONDS %= 60;
      break;
    }

    case CONFIG_INDEX_T_ON_S_LSD:
    {
      T_ON_SECONDS += unit;
      if(T_ON_SECONDS % 10 == 0)
      {
        T_ON_SECONDS -= 10;
      }
      break;
    }

    case CONFIG_INDEX_T_OFF_H_MSD:
    {
      T_OFF_HOURS += 10 * unit;
      T_OFF_HOURS %= 60;
      break;
    }

    case CONFIG_INDEX_T_OFF_H_LSD:
    {
      T_OFF_HOURS += unit;
      if(T_OFF_HOURS % 10 == 0)
      {
        T_OFF_HOURS -= 10;
      }
      break;
    }

    case CONFIG_INDEX_T_OFF_M_MSD:
    {
      T_OFF_MINUTES += 10 * unit;
      T_OFF_MINUTES %= 60;
      break;
    }

    case CONFIG_INDEX_T_OFF_M_LSD:
    {
      T_OFF_MINUTES += unit;
      if(T_OFF_MINUTES % 10 == 0)
      {
        T_OFF_MINUTES -= 10;
      }
      break;
    }

    case CONFIG_INDEX_T_OFF_S_MSD:
    {
      T_OFF_SECONDS += 10 * unit;
      T_OFF_SECONDS %= 60;
      break;
    }

    case CONFIG_INDEX_T_OFF_S_LSD:
    {
      T_OFF_SECONDS += unit;
      if(T_OFF_SECONDS % 10 == 0)
      {
        T_OFF_SECONDS -= 10;
      }
      break;
    }

    case CONFIG_INDEX_MOTOR_1_ENABLED:
    {
      MOTOR_1_ENABLED ++;
      MOTOR_1_ENABLED &= 1;
      break;
    }

    case CONFIG_INDEX_MOTOR_2_ENABLED:
    {
      MOTOR_2_ENABLED ++;
      MOTOR_2_ENABLED &= 1;
      break;
    }

    case CONFIG_INDEX_MOTOR_3_ENABLED:
    {
      MOTOR_3_ENABLED ++;
      MOTOR_3_ENABLED &= 1;
      break;
    }

    case CONFIG_INDEX_MOTOR_4_ENABLED:
    {
      MOTOR_4_ENABLED ++;
      MOTOR_4_ENABLED &= 1;
      break;
    }

    case CONFIG_INDEX_MOTOR_5_ENABLED:
    {
      MOTOR_5_ENABLED ++;
      MOTOR_5_ENABLED &= 1;
      break;
    }

    case CONFIG_INDEX_MOTOR_6_ENABLED:
    {
      MOTOR_6_ENABLED ++;
      MOTOR_6_ENABLED &= 1;
      break;
    }

    case CONFIG_INDEX_MOTOR_1_POWER:
    {
      MOTOR_1_POWER %= MOTOR_MAX_POWER;
      MOTOR_1_POWER ++;
      break;
    }

    case CONFIG_INDEX_MOTOR_2_POWER:
    {
      MOTOR_2_POWER %= MOTOR_MAX_POWER;
      MOTOR_2_POWER ++;
      break;
    }

    case CONFIG_INDEX_MOTOR_3_POWER:
    {
      MOTOR_3_POWER %= MOTOR_MAX_POWER;
      MOTOR_3_POWER ++;
      break;
    }

    case CONFIG_INDEX_MOTOR_4_POWER:
    {
      MOTOR_4_POWER %= MOTOR_MAX_POWER;
      MOTOR_4_POWER ++;
      break;
    }

    case CONFIG_INDEX_MOTOR_5_POWER:
    {
      MOTOR_5_POWER %= MOTOR_MAX_POWER;
      MOTOR_5_POWER ++;
      break;
    }

    case CONFIG_INDEX_MOTOR_6_POWER:
    {
      MOTOR_6_POWER %= MOTOR_MAX_POWER;
      MOTOR_6_POWER ++;
      break;
    }
    
    default:
    {
      INTERNAL_ERROR = "unknown CONFIG_INDEX value in modify_config!";
      go_to_state(STATE_ERROR);
      break;
    }
  }
}

void save_configuration(void)
{
  CONFIG_INDEX = -1;
  
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Saving");
    display.setCursor(0, 8);
    display.print("configuration..."); 
    display.display();
    delay(1000);
    upload_motors_power_to_eeprom(MOTOR_1_POWER, MOTOR_2_POWER, MOTOR_3_POWER, MOTOR_4_POWER, MOTOR_5_POWER, MOTOR_6_POWER);
    upload_motors_enabled_to_eeprom(MOTOR_1_ENABLED +  2 * MOTOR_2_ENABLED + 4 * MOTOR_3_ENABLED + 8 * MOTOR_4_ENABLED + 16 * MOTOR_5_ENABLED + 32 * MOTOR_6_ENABLED);
    upload_times_to_eeprom(T_ON_SECONDS, T_ON_MINUTES, T_ON_HOURS, T_OFF_SECONDS, T_OFF_MINUTES, T_OFF_HOURS);

    load_times_from_eeprom();
    load_motors_enabled_from_eeprom();
    load_motors_power_from_eeprom();
    
    go_to_state(STATE_MOTORS_OFF);
    
}

void turn_on_display(void)
{
  digitalWrite(DISPLAY_SWITCH_PIN, HIGH);
  delay(10);  // wait for display to power up
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_logo();
  delay(600);
  DISPLAY_IS_ON = TRUE;
}

void turn_off_display(void)
{
  digitalWrite(DISPLAY_SWITCH_PIN, LOW);
  DISPLAY_IS_ON = FALSE;
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

void enable_all_motors(void)
{
  MOTOR_1_ENABLED = TRUE;
  MOTOR_2_ENABLED = TRUE;
  MOTOR_3_ENABLED = TRUE;
  MOTOR_4_ENABLED = TRUE;
  MOTOR_5_ENABLED = TRUE;
  MOTOR_6_ENABLED = TRUE;
}

void init_uC(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  __init_uC_motor_control__();
  __init_uC_buttons_control__();
}

void init_application(void)
{
  INVALID_EEPROM_DATA = FALSE;
  reset_clock();
  CLOCK.is_paused = FALSE;
  turn_on_display();
  load_application_data_from_eeprom();
}

void __init_uC_motor_control__(void)
{
  // set required pins as output, on low level (0V)
  pinMode(MOTOR_1_PIN, OUTPUT); 
  digitalWrite(MOTOR_1_PIN, LOW);
  pinMode(MOTOR_2_PIN, OUTPUT); 
  digitalWrite(MOTOR_2_PIN, LOW);
  pinMode(MOTOR_3_PIN, OUTPUT); 
  digitalWrite(MOTOR_3_PIN, LOW);
  pinMode(MOTOR_4_PIN, OUTPUT); 
  digitalWrite(MOTOR_4_PIN, LOW);
  pinMode(MOTOR_5_PIN, OUTPUT); 
  digitalWrite(MOTOR_5_PIN, LOW);
  pinMode(MOTOR_6_PIN, OUTPUT); 
  digitalWrite(MOTOR_6_PIN, LOW);
}

void __init_uC_buttons_control__(void)
{
  pinMode(BUTTON_1_PIN, INPUT); 
  pinMode(BUTTON_2_PIN, INPUT); 
  pinMode(BUTTON_3_PIN, INPUT); 
}

void load_application_data_from_eeprom(void)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Loading EEPROM.");
  display.display();
  delay(150);
  display.print(".");
  display.display();
  delay(150);
  display.print(".");
  display.display();
  load_times_from_eeprom();
  load_motors_enabled_from_eeprom();
  load_motors_power_from_eeprom();
  delay(150);
}

void load_motors_power_from_eeprom(void)
{
  MOTOR_1_POWER = (unsigned short)EEPROM.read(MOTOR_1_POWER_EEPROM_ADDRESS);
  MOTOR_2_POWER = (unsigned short)EEPROM.read(MOTOR_2_POWER_EEPROM_ADDRESS);
  MOTOR_3_POWER = (unsigned short)EEPROM.read(MOTOR_3_POWER_EEPROM_ADDRESS);
  MOTOR_4_POWER = (unsigned short)EEPROM.read(MOTOR_4_POWER_EEPROM_ADDRESS);
  MOTOR_5_POWER = (unsigned short)EEPROM.read(MOTOR_5_POWER_EEPROM_ADDRESS);
  MOTOR_6_POWER = (unsigned short)EEPROM.read(MOTOR_6_POWER_EEPROM_ADDRESS);

  if(MOTOR_1_POWER > MOTOR_MAX_POWER || MOTOR_2_POWER > MOTOR_MAX_POWER || MOTOR_3_POWER > MOTOR_MAX_POWER || 
  MOTOR_4_POWER > MOTOR_MAX_POWER || MOTOR_5_POWER > MOTOR_MAX_POWER || MOTOR_6_POWER > MOTOR_MAX_POWER)
  {
    INVALID_EEPROM_DATA = TRUE;
    upload_motors_power_to_eeprom(DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER, DEFAULT_MOTOR_POWER);
    go_to_state(STATE_ERROR);
  }
  
  _error:
  if (INVALID_EEPROM_DATA == TRUE)
  { 
    INTERNAL_ERROR = "Invalid motors power EEPROM data!";
    upload_motors_enabled_to_eeprom(ALL_MOTORS_ENABLED);
    go_to_state(STATE_ERROR);
  }
}

void load_motors_enabled_from_eeprom(void)
{
  unsigned short motors_enabled = (unsigned short)EEPROM.read(MOTORS_ENABLED_EEPROM_ADDRESS);

  if(motors_enabled > 0xb00111111)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }
  
  MOTOR_1_ENABLED = motors_enabled & 1;
  MOTOR_2_ENABLED = (motors_enabled >> 1) & 1;
  MOTOR_3_ENABLED = (motors_enabled >> 2) & 1;
  MOTOR_4_ENABLED = (motors_enabled >> 3) & 1;
  MOTOR_5_ENABLED = (motors_enabled >> 4) & 1;
  MOTOR_6_ENABLED = (motors_enabled >> 5) & 1;
  
  _error:
  if (INVALID_EEPROM_DATA == TRUE)
  { 
    INTERNAL_ERROR = "Invalid motors enabled EEPROM data!";
    upload_motors_enabled_to_eeprom(ALL_MOTORS_ENABLED);
    go_to_state(STATE_ERROR);
  }
}

void upload_motors_enabled_to_eeprom(unsigned short motors_enabled)
{
  EEPROM.write(MOTORS_ENABLED_EEPROM_ADDRESS, motors_enabled);
}

void load_times_from_eeprom(void)
{
  T_ON_SECONDS = (unsigned short)EEPROM.read(T_ON_SECONDS_EEPROM_ADDRESS);
  T_ON_MINUTES = (unsigned short)EEPROM.read(T_ON_MINUTES_EEPROM_ADDRESS);
  T_ON_HOURS = (unsigned short)EEPROM.read(T_ON_HOURS_EEPROM_ADDRESS);
  
  T_OFF_SECONDS = (unsigned short)EEPROM.read(T_OFF_SECONDS_EEPROM_ADDRESS);
  T_OFF_MINUTES = (unsigned short)EEPROM.read(T_OFF_MINUTES_EEPROM_ADDRESS);
  T_OFF_HOURS = (unsigned short)EEPROM.read(T_OFF_HOURS_EEPROM_ADDRESS);

  if(T_ON_SECONDS > 59)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }
  if(T_OFF_SECONDS > 59)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }
  if(T_ON_MINUTES > 59)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }
  if(T_OFF_MINUTES > 59)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }
  if(T_ON_HOURS > 99)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }
  if(T_OFF_HOURS > 99)
  {
    INVALID_EEPROM_DATA = TRUE;
    goto _error;
  }

  _error:
  if (INVALID_EEPROM_DATA == TRUE)
  { 
    INTERNAL_ERROR = "Invalid time EEPROM data!";
    upload_times_to_eeprom(DEFAULT_T_ON_SECONDS, DEFAULT_T_ON_MINUTES, DEFAULT_T_ON_HOURS, DEFAULT_T_OFF_SECONDS, DEFAULT_T_OFF_MINUTES, DEFAULT_T_OFF_HOURS);
    go_to_state(STATE_ERROR);
  }
}
void upload_motors_power_to_eeprom(unsigned short m1_p, unsigned short m2_p, unsigned short m3_p, unsigned short m4_p, unsigned short m5_p, unsigned short m6_p)
{
  EEPROM.write(MOTOR_1_POWER_EEPROM_ADDRESS, m1_p);
  EEPROM.write(MOTOR_2_POWER_EEPROM_ADDRESS, m2_p);
  EEPROM.write(MOTOR_3_POWER_EEPROM_ADDRESS, m3_p);
  EEPROM.write(MOTOR_4_POWER_EEPROM_ADDRESS, m4_p);
  EEPROM.write(MOTOR_5_POWER_EEPROM_ADDRESS, m5_p);
  EEPROM.write(MOTOR_6_POWER_EEPROM_ADDRESS, m6_p);
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
      CLOCK.is_paused = TRUE;
      display.setTextSize(1);
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Error: "); 
      display.print(INTERNAL_ERROR); 
      display.display();
      break;
    }
    case STATE_MOTORS_ON:
    {
      STATE = STATE_MOTORS_ON;
      CLOCK.is_paused = FALSE;
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("Activatingfans"); 
      display.display();
      delay(150);
      display.print("."); 
      display.display();
      delay(150);
      display.print("."); 
      display.display();
      delay(150);
      display.print("."); 
      display.display();
      delay(150);
      turn_on_motors();
      set_clock(T_ON_HOURS, T_ON_MINUTES, T_ON_SECONDS);
      break;
    }
    case STATE_MOTORS_OFF:
    {
      STATE = STATE_MOTORS_OFF;
      CLOCK.is_paused = FALSE;
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("Stopping  fans"); 
      display.display();
      delay(150);
      display.print("."); 
      display.display();
      delay(150);
      display.print("."); 
      display.display();
      delay(150);
      display.print("."); 
      display.display();
      delay(150);
      turn_off_motors();
      set_clock(T_OFF_HOURS, T_OFF_MINUTES, T_OFF_SECONDS);
      break;
    }
    case STATE_CONFIG:
    {
      STATE = STATE_CONFIG;
      turn_off_motors();
      reset_clock();
      CLOCK.is_paused = TRUE;
      CONFIG_INDEX = CONFIG_INDEX_T_ON_H_MSD;
      break;
    }
    default:
    {
      STATE = STATE_ERROR;
      INTERNAL_ERROR = "Invalid state specified!";
      display.setTextSize(1);
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Error: "); 
      display.print(INTERNAL_ERROR); 
      display.display();
    }
  }
}

void turn_on_motor(unsigned char motor, unsigned char power)
{
  analogWrite(motor, (unsigned long)((power * 255) / 10));
}

void turn_on_motors(void)
{
  if(MOTOR_1_ENABLED)
  {
    turn_on_motor(MOTOR_1_PIN, MOTOR_1_POWER);
  }
  if(MOTOR_2_ENABLED)
  {
    turn_on_motor(MOTOR_2_PIN, MOTOR_2_POWER);
  }
  if(MOTOR_3_ENABLED)
  {
    turn_on_motor(MOTOR_3_PIN, MOTOR_3_POWER);
  }
  if(MOTOR_4_ENABLED)
  {
    turn_on_motor(MOTOR_4_PIN, MOTOR_4_POWER);
  }
  if(MOTOR_5_ENABLED)
  {
    turn_on_motor(MOTOR_5_PIN, MOTOR_5_POWER);
  }
  if(MOTOR_6_ENABLED)
  {
    turn_on_motor(MOTOR_6_PIN, MOTOR_6_POWER);
  }
}

void turn_off_motors(void)
{
  digitalWrite(MOTOR_1_PIN, LOW);
  digitalWrite(MOTOR_2_PIN, LOW);
  digitalWrite(MOTOR_3_PIN, LOW);
  digitalWrite(MOTOR_4_PIN, LOW);
  digitalWrite(MOTOR_5_PIN, LOW);
  digitalWrite(MOTOR_6_PIN, LOW);
}

void display_logo(void)
{
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
  display.println("Scorpion");  
  display.setCursor(50,16);
  display.println("IPX");  
  display.display();
  
  DISPLAY_IS_ON = TRUE;
}

void decrease_clock_time(void)
{
  if(CLOCK.seconds == 0)
  {
    if(CLOCK.minutes == 0)
    {
      if(CLOCK.hours == 0)
      {
        // nothing
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
      // CLOCK.seconds = 59;
      CLOCK.seconds = 58;  // compensate program's logic time loss
    }
  }
  else
  {
    CLOCK.seconds --;
  }
}

void display_clock(void)
{
  if(DISPLAY_IS_ON)
  {
    display.setTextSize(2);
    display.clearDisplay();
    display.setCursor(0,0);
    if(STATE == STATE_MOTORS_OFF)
    {
      display.print("Fans OFF"); 
    }
    else if(STATE == STATE_MOTORS_ON)
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
}

void display_motors_powers(void)
{
    display.setTextSize(1);
    display.clearDisplay();

    // draw table
    display.drawFastHLine(9, 9, 108, 1);
    display.drawFastHLine(9, 19, 108, 1);
    display.drawFastHLine(9, 29, 108, 1);
    display.drawFastVLine(9, 9, 21, 1);
    display.drawFastVLine(117, 9, 21, 1);
    display.drawFastVLine(27, 9, 21, 1);
    display.drawFastVLine(45, 9, 21, 1);
    display.drawFastVLine(63, 9, 21, 1);
    display.drawFastVLine(80, 9, 21, 1);
    display.drawFastVLine(98, 9, 21, 1);
    
    display.setCursor(0, 0);
    display.print("Motors power:");
    display.setCursor(0, 11);
    display.print("M ");
    display.setCursor(16, 11);
    display.print("1  2  3  4  5  6");
    display.setCursor(0, 21);
    display.print("P ");
    
    
    if(CONFIG_INDEX == -1)
    {
      display.print(MOTOR_1_POWER / 10);
      display.print(MOTOR_1_POWER % 10);
      display.print(' ');
      display.print(MOTOR_2_POWER / 10);
      display.print(MOTOR_2_POWER % 10);
      display.print(' ');
      display.print(MOTOR_3_POWER / 10);
      display.print(MOTOR_3_POWER % 10);
      display.print(' ');
      display.print(MOTOR_4_POWER / 10);
      display.print(MOTOR_4_POWER % 10);
      display.print(' ');
      display.print(MOTOR_5_POWER / 10);
      display.print(MOTOR_5_POWER % 10);
      display.print(' ');
      display.print(MOTOR_6_POWER / 10);
      display.print(MOTOR_6_POWER % 10);
      display.print(' ');
      display.display();
      return;
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_1_POWER)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_1_POWER / 10);
        display.print(MOTOR_1_POWER % 10);
        display.print(" ");
      }
    }
    else
    {
      display.print(MOTOR_1_POWER / 10);
      display.print(MOTOR_1_POWER % 10);
      display.print(" ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_2_POWER)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_2_POWER / 10);
        display.print(MOTOR_2_POWER % 10);
        display.print(" ");
      }
    }
    else
    {
      display.print(MOTOR_2_POWER / 10);
      display.print(MOTOR_2_POWER % 10);
      display.print(" ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_3_POWER)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_3_POWER / 10);
        display.print(MOTOR_3_POWER % 10);
        display.print(" ");
      }
    }
    else
    {
      display.print(MOTOR_3_POWER / 10);
      display.print(MOTOR_3_POWER % 10);
      display.print(" ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_4_POWER)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_4_POWER / 10);
        display.print(MOTOR_4_POWER % 10);
        display.print(" ");
      }
    }
    else
    {
      display.print(MOTOR_4_POWER / 10);
      display.print(MOTOR_4_POWER % 10);
      display.print(" ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_5_POWER)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_5_POWER / 10);
        display.print(MOTOR_5_POWER % 10);
        display.print(" ");
      }
    }
    else
    {
      display.print(MOTOR_5_POWER / 10);
      display.print(MOTOR_5_POWER % 10);
      display.print(" ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_6_POWER)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_6_POWER / 10);
        display.print(MOTOR_6_POWER % 10);
        display.print(" ");
      }
    }
    else
    {
      display.print(MOTOR_6_POWER / 10);
      display.print(MOTOR_6_POWER % 10);
      display.print(" ");
    }
    
  display.display();
}

void display_motors_enabled(void)
{
    display.clearDisplay();
    display.setTextSize(1);

    // draw table
    display.drawFastHLine(9, 9, 108, 1);
    display.drawFastHLine(9, 19, 108, 1);
    display.drawFastHLine(9, 29, 108, 1);
    display.drawFastVLine(9, 9, 21, 1);
    display.drawFastVLine(117, 9, 21, 1);
    display.drawFastVLine(27, 9, 21, 1);
    display.drawFastVLine(45, 9, 21, 1);
    display.drawFastVLine(63, 9, 21, 1);
    display.drawFastVLine(80, 9, 21, 1);
    display.drawFastVLine(98, 9, 21, 1);
    
    display.setCursor(0, 0);
    display.print("Motors enabled:");
    display.setCursor(0, 11);
    display.print("M ");
    display.setCursor(16, 11);
    display.print("1  2  3  4  5  6");
    display.setCursor(0, 21);
    display.print("E ");
    display.setCursor(16, 21);
    
    if(CONFIG_INDEX == -1)
    {
      display.print(MOTOR_1_ENABLED);
      display.print("  ");
      display.print(MOTOR_2_ENABLED);
      display.print("  ");
      display.print(MOTOR_3_ENABLED);
      display.print("  ");
      display.print(MOTOR_4_ENABLED);
      display.print("  ");
      display.print(MOTOR_5_ENABLED);
      display.print("  ");
      display.print(MOTOR_6_ENABLED);
      display.print("  ");
      display.display();
      return;
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_1_ENABLED)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_1_ENABLED);
        display.print("  ");
      }
    }
    else
    {
      display.print(MOTOR_1_ENABLED);
      display.print("  ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_2_ENABLED)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_2_ENABLED);
        display.print("  ");
      }
    }
    else
    {
      display.print(MOTOR_2_ENABLED);
      display.print("  ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_3_ENABLED)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_3_ENABLED);
        display.print("  ");
      }
    }
    else
    {
      display.print(MOTOR_3_ENABLED);
      display.print("  ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_4_ENABLED)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_4_ENABLED);
        display.print("  ");
      }
    }
    else
    {
      display.print(MOTOR_4_ENABLED);
      display.print("  ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_5_ENABLED)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_5_ENABLED);
        display.print("  ");
      }
    }
    else
    {
      display.print(MOTOR_5_ENABLED);
      display.print("  ");
    }
    
  if(CONFIG_INDEX == CONFIG_INDEX_MOTOR_6_ENABLED)
    {
      if(CONFIG_INDEX_SHOWED)
      {
        CONFIG_INDEX_SHOWED = FALSE;
        display.print("   ");
      }
      else
      {
        CONFIG_INDEX_SHOWED = TRUE;
        display.print(MOTOR_6_ENABLED);
        display.print("  ");
      }
    }
    else
    {
      display.print(MOTOR_6_ENABLED);
      display.print("  ");
    }
    
  display.display();
}

void display_times(void)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("TIME HH:MM:SS");  
  display.setCursor(0,12);
  display.print("ON:  ");  
  
  if(CONFIG_INDEX == -1)
  {
    display.print(T_ON_HOURS / 10);
    display.print(T_ON_HOURS % 10);
    display.print(":");
    display.print(T_ON_MINUTES / 10);
    display.print(T_ON_MINUTES % 10);
    display.print(":");
    display.print(T_ON_SECONDS / 10);
    display.print(T_ON_SECONDS % 10);
    display.setCursor(0,24);
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
    return;
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_ON_H_MSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_ON_HOURS / 10);
    }
  }
  else
  {
    display.print(T_ON_HOURS / 10);
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_ON_H_LSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_ON_HOURS % 10);
    }
  }
  else
  {
    display.print(T_ON_HOURS % 10);
  }

  display.print(":");
    
  if(CONFIG_INDEX == CONFIG_INDEX_T_ON_M_MSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_ON_MINUTES / 10);
    }
  }
  else
  {
    display.print(T_ON_MINUTES / 10);
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_ON_M_LSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_ON_MINUTES % 10);
    }
  }
  else
  {
    display.print(T_ON_MINUTES % 10);
  }

  display.print(":");
    
  if(CONFIG_INDEX == CONFIG_INDEX_T_ON_S_MSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_ON_SECONDS / 10);
    }
  }
  else
  {
    display.print(T_ON_SECONDS / 10);
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_ON_S_LSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_ON_SECONDS % 10);
    }
  }
  else
  {
    display.print(T_ON_SECONDS % 10);
  }

  display.setCursor(0,24);
  display.print("OFF: "); 
 
  if(CONFIG_INDEX == CONFIG_INDEX_T_OFF_H_MSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_OFF_HOURS / 10);
    }
  }
  else
  {
    display.print(T_OFF_HOURS / 10);
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_OFF_H_LSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_OFF_HOURS % 10);
    }
  }
  else
  {
    display.print(T_OFF_HOURS % 10);
  }

  display.print(":");
    
  if(CONFIG_INDEX == CONFIG_INDEX_T_OFF_M_MSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_OFF_MINUTES / 10);
    }
  }
  else
  {
    display.print(T_OFF_MINUTES / 10);
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_OFF_M_LSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_OFF_MINUTES % 10);
    }
  }
  else
  {
    display.print(T_OFF_MINUTES % 10);
  }

  display.print(":");
    
  if(CONFIG_INDEX == CONFIG_INDEX_T_OFF_S_MSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_OFF_SECONDS / 10);
    }
  }
  else
  {
    display.print(T_OFF_SECONDS / 10);
  }

  if(CONFIG_INDEX == CONFIG_INDEX_T_OFF_S_LSD)
  {
    if(CONFIG_INDEX_SHOWED)
    {
      CONFIG_INDEX_SHOWED = FALSE;
      display.print(" ");
    }
    else
    {
      CONFIG_INDEX_SHOWED = TRUE;
      display.print(T_OFF_SECONDS % 10);
    }
  }
  else
  {
    display.print(T_OFF_SECONDS % 10);
  }
  
  display.display();
}


