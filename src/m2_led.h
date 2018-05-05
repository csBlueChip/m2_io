#ifndef M2_LED_H_
#define M2_LED_H_

#include <Arduino.h>

//------------------------------------------------------------------------------ ---------------------------------------
typedef 
	enum m2led {
		LED_RED     = DS2,
		LED_YEL1    = DS3,
		LED_YEL2    = DS4,
		LED_YEL3    = DS5,
		LED_GRN     = DS6,
		LED_RGB_RED = -DS7_RED,    // We use negative pin numbers to indicate 
		LED_RGB_GRN = -DS7_GREEN,  //   the LED is connected to a PWM enabled pin
		LED_RGB_BLU = -DS7_BLUE,   //     and is, therefore, dimmable.
	}
m2led_t;

#define LED_BLU   LED_RGB_BLU
#define LED_RED2  LED_RGB_RED
#define LED_GRN2  LED_RGB_GRN

// The fade_in() and fade_out() functions allow fading of tricolour LEDs
// This 
typedef 
	enum m2tri {
		LED_TRI_BLK = 0,
		LED_TRI_RED = 0x01,
		LED_TRI_GRN = 0x02,
		LED_TRI_BLU = 0x04,
		LED_TRI_YEL = LED_TRI_RED | LED_TRI_GRN,
		LED_TRI_MAG = LED_TRI_RED | LED_TRI_BLU,
		LED_TRI_CYN = LED_TRI_GRN | LED_TRI_BLU,
		LED_TRI_WHT = LED_TRI_RED | LED_TRI_GRN | LED_TRI_BLU,
	}
m2tri_t;
#define LED_TRI_MAX  (LED_TRI_WHT)

#define LED_EOL  ((m2led_t)0)
#define LED_CNT  (8)

// LED_Anode   -> Arduino  =>  LED_ON == HIGH 
// LED_Cathode -> Arduino  =>  LED_ON == LOW 
typedef 
	enum m2ledState {
		LED_ON  = LOW,
		LED_OFF = HIGH,
	}
m2ledState_t;

//------------------------------------------------------------------------------ ---------------------------------------
extern  const  m2led_t  led_list_all[] ;

extern  unsigned int  _dacBits;   // Number of bits for the DAC
extern  unsigned int  _dacMax;    // Maximum value for DAC {0..DacMax)
extern  unsigned int  _dacTroot;  // Triangular root of DacMax

//@============================================================================= =======================================
static inline  bool  led_set    (m2led_t led,  m2ledState_t state)  {  return digitalWrite(abs(led), state), true; }

//@============================================================================= 
static inline  bool  led_on     (m2led_t led)  {  return led_set(led, LED_ON);   }
static inline  bool  led_off    (m2led_t led)  {  return led_set(led, LED_OFF);  }

//@============================================================================= 
static inline  bool  led_toggle (m2led_t led)  {  return led_set(led, (digitalRead(abs(led)) == LED_ON) ? LED_OFF : LED_ON);  }

//@============================================================================= =======================================
static inline  bool  led_init   (m2led_t led)  {  pinMode(abs(led), OUTPUT);  return led_off(led);  }

//============================================================================== =======================================
void  led_flash_die (m2led_t led,  int speed) ;
void  _led_init_all (unsigned int res,  m2led_t* list) ;
void  led_init_all  (m2led_t* list) ;

bool  led_dim_lvl   (m2led_t led,  unsigned int lvl) ;
bool  led_dim_scale (m2led_t led,  unsigned int val,  unsigned int scale) ;
bool  led_dim_pc    (m2led_t led,  unsigned int pc) ;
bool  led_dim_hex   (m2led_t led,  uint8_t hex) ;
bool  led_tri_hex   (uint32_t hex) ;

bool  led_fade_in   (m2tri_t tri,  unsigned int ms) ;
bool  led_fade_out  (m2tri_t tri,  unsigned int ms) ;


#endif // M2_LED_H
