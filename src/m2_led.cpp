#include <stdlib.h>  // abs()

#include "m2_led.h"

//------------------------------------------------------------------------------ ---------------------------------------
const  m2led_t  led_list_all[LED_CNT + 1] = {
  LED_RED, 
  LED_YEL1, LED_YEL2, LED_YEL3, 
  LED_GRN, 
  LED_RGB_RED, LED_RGB_GRN, LED_RGB_BLU, 
  LED_EOL
};

//------------------------------------------------------------------------------ ---------------------------------------
// We use Triangular numbers to generate smooth fading...
// The triangular root of DAC_MAX will give us the number of shades available
// https://en.wikipedia.org/wiki/Triangular_number#Triangular_roots_and_tests_for_triangular_numbers
// Triangle of value            :     (val * (val+1))       /2
// Triangular root (of DAC_MAX) : (sqrt((8*DAC_MAX) +1) -1) /2
// As this contains a SQRT, it cannot be in a preprocessor directive!
// ---------------------------------
// BITS : MAX    ->  Troot (actual)
// ---------------------------------
//  15  : 32640  ->   255  (32640)
// ...  : 
//  12  :  4095  ->    90  (4095) [unique case]
//  11  :  2047  ->    63  (2016)
//  10  :  1023  ->    44  (990)
//   9  :   511  ->    31  (496)
//   8  :   255  ->    22  (253)
// ---------------------------------
// So you would need a 15bit DAC to offer all 0xFF shades of colour
//
unsigned int  _dacBits;   // Number of bits for the DAC
unsigned int  _dacMax;    // Maximum value for DAC {0..DacMax)
unsigned int  _dacTroot;  // Triangular root of DacMax

//+============================================================================= =======================================
void  led_flash_die (m2led_t led,  int speed)
{
	for (;;) {  
		led_toggle(led);
		delay(speed);
	}
}

//+============================================================================= =======================================
// If you wish to set the DAC to something other than 12bits in your sketch
//   you can call this version of init_all and specify the resolution
//
void  _led_init_all (unsigned int res,  m2led_t* list)
{
	_dacBits  = res;
	_dacMax   = (1 << _dacBits) - 1;
	_dacTroot = (unsigned int)((sqrt((8 * _dacMax) +1) -1) /2);  // <= troot(dacMax)
	
	// Yes, I am casting off the const ...to satiate the compiler
	for (m2led_t* led_list = list ? list : (m2led_t*)led_list_all;  *led_list;  led_list++)  led_init(*led_list) ; 
}

//+============================================================================= 
// This configures the DAC read resolution to 12 bits
// If you don't want this, call _led_init_all(resolution, list) yourself
//
void  led_init_all (m2led_t* list)
{  
	_dacBits  = 12;  // Maximum resolution of Sam3x8e/Due {0..4095}
	analogWriteResolution(_dacBits);
	_led_init_all(_dacBits, list);
}

//+============================================================================= =======================================
// This will only work on the RGB LED(s) as it/these are the only one(s) attached to PWM pins on the uC
// 
bool  led_dim_lvl (m2led_t led,  unsigned int lvl)  
{
	if (lvl > _dacTroot)  lvl = _dacTroot ;
	// Convert brightness level to dac value (inversed because of the common-anode wiring on the M2)
	analogWrite(abs(led), _dacMax - ((lvl * (lvl + 1)) / 2));
	return true;
}

//+============================================================================= 
// Dim the LED to an arbitrary scale
//
bool  led_dim_scale (m2led_t led,  unsigned int val,  unsigned int scale)  
{
	if (led >= 0)  return false ;
	
	if      (val ==     0)  return led_off(led) ;
	else if (val >= scale)  return led_on(led) ;
	else                    return led_dim_lvl(led, (val * _dacTroot) / scale);  // Convert to triangular brightness 'level'
}

//+=============================================================================
// Dim the LED as a percentage
//
bool  led_dim_pc (m2led_t led,  unsigned int pc)  
{  
	return led_dim_scale(led, pc, 100);
}

//+============================================================================= 
// Dim the LED on a scale of 0..255
//
bool  led_dim_hex (m2led_t led,  uint8_t hex)  
{  
	return led_dim_scale(led, hex, 0xFF);
}

//+============================================================================= 
// Set TriColour LED to 0xRRGGBB
//
bool  led_tri_hex (uint32_t hex)  
{  
	if   (!led_dim_scale(LED_RGB_RED, (hex >> 16) & 0xFF, 0xFF))  return false ;
	if   (!led_dim_scale(LED_RGB_GRN, (hex >>  8) & 0xFF, 0xFF))  return false ;
	return led_dim_scale(LED_RGB_BLU, (hex      ) & 0xFF, 0xFF);
}

//+============================================================================= =======================================
// Fade in one LED (must be on a PWM pin)
//
bool  led_fade_in (m2tri_t tri,  unsigned int ms)
{
	int  dly = ms / _dacTroot; 
	
	if (tri > LED_TRI_MAX)  return false ;
	
	for (int level = _dacMax, step = 0;  level > 0;  level -= step++) {
		if (tri & LED_TRI_RED)  analogWrite(abs(LED_RGB_RED), level) ;
		if (tri & LED_TRI_GRN)  analogWrite(abs(LED_RGB_GRN), level) ;
		if (tri & LED_TRI_BLU)  analogWrite(abs(LED_RGB_BLU), level) ;
		delay(dly);
	} 
	if (tri & LED_TRI_RED)  if (!led_on(LED_RGB_RED)) return false ;
	if (tri & LED_TRI_GRN)  if (!led_on(LED_RGB_GRN)) return false ;
	if (tri & LED_TRI_BLU)  if (!led_on(LED_RGB_BLU)) return false ;
	
	return true;
}

//+============================================================================= =======================================
// Fade out one LED (must be on a PWM pin)
//
bool  led_fade_out (m2tri_t tri,  unsigned int ms)
{
	int  dly = ms / _dacTroot; 
	
	if (tri > LED_TRI_MAX)  return false ;
	
	for (unsigned int level = 0, step = _dacTroot;  (level < _dacMax) && step;  level += step--) {
		if (tri & LED_TRI_RED)  analogWrite(abs(LED_RGB_RED), level) ;
		if (tri & LED_TRI_GRN)  analogWrite(abs(LED_RGB_GRN), level) ;
		if (tri & LED_TRI_BLU)  analogWrite(abs(LED_RGB_BLU), level) ;
		delay(dly);
	} 
	if (tri & LED_TRI_RED)  if (!led_off(LED_RGB_RED)) return false ;
	if (tri & LED_TRI_GRN)  if (!led_off(LED_RGB_GRN)) return false ;
	if (tri & LED_TRI_BLU)  if (!led_off(LED_RGB_BLU)) return false ;
	
	return true;
}
