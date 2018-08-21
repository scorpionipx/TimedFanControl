// APPLICATION SETTINGS
#define DEFAULT_MOTOR_POWER 100   // default duty cycle for motor control

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

#define BUTTON_1_PIN 4  // digital pin used to read button state (PD4) - used to select value
#define BUTTON_2_PIN 7  // digital pin used to read button state (PD7) - used to change value
#define BUTTON_3_PIN 8  // digital pin used to read button state (PB0) - used to reset internal timer

// GENERIC
#define TRUE 1
#define FALSE 0

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

#define MOTOR_1_PIN_POWER_EEPROM_ADDRESS  0x06  // store & load EEPROM adresses
#define MOTOR_2_PIN_POWER_EEPROM_ADDRESS  0x07  // store & load EEPROM adresses
#define MOTOR_3_PIN_POWER_EEPROM_ADDRESS  0x08  // store & load EEPROM adresses
#define MOTOR_4_PIN_POWER_EEPROM_ADDRESS  0x09  // store & load EEPROM adresses
#define MOTOR_5_PIN_POWER_EEPROM_ADDRESS  0x0A  // store & load EEPROM adresses
#define MOTOR_6_PIN_POWER_EEPROM_ADDRESS  0x0B  // store & load EEPROM adresses

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

struct clock_structure
{
  unsigned short seconds;    // count hours
  unsigned short minutes;    // count_minutes
  unsigned short hours;      // count seconds
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
   unsigned short BOOOOOOOOOOOOOL_ : 1;
} BOOLEAN;                                // bit field struct to store boolean values

#define MOTOR_1_ENABLED BOOLEAN.MOTOR_1_ENABLED_
#define MOTOR_2_ENABLED BOOLEAN.MOTOR_2_ENABLED_
#define MOTOR_3_ENABLED BOOLEAN.MOTOR_3_ENABLED_
#define MOTOR_4_ENABLED BOOLEAN.MOTOR_4_ENABLED_
#define MOTOR_5_ENABLED BOOLEAN.MOTOR_5_ENABLED_
#define MOTOR_6_ENABLED BOOLEAN.MOTOR_6_ENABLED_
#define DISPLAY_IS_ON BOOLEAN.DISPLAY_IS_ON_

int STATE = STATE_INIT;    // initial state at power up

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

void setup() 
{
  init_uC();
  init_application();
}

void loop() 
{

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
  __init_uC_motor_control__();
  __init_uC_buttons_control__();
}

void init_application(void)
{
  reset_clock();
  enable_all_motors();
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

