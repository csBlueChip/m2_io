#include <m2_io.h>

// The EIGHT LEDs are:
//   LED_RED, 
//   LED_YEL1, LED_YEL2, LED_YEL3, 
//   LED_GRN, 
//   LED_RGB_RED, LED_RGB_GRN, LED_RGB_BLU, 

void setup (void) 
{
	// We can initialise SOME of the LEDs by making a list of the ones we are interested in
	// This may be useful if the pins are functionally (not electrically) overloaded
	// The last entry MUST be LED_EOL [End Of List]
	m2led_t  myLeds[] = {LED_RGB_RED,  LED_RGB_GRN,  LED_RGB_BLU,  LED_EOL} ;
	led_init_all(myLeds);
	
  // The standard init_all() function sets the DAC *WRITE* resolution to 12-bit (maximum)
  // If you wish to run the DAC at a lower resolution you can use this
  _led_init_all(8, NULL);
  
  // Or we can initialise ALL the LEDs AND set the resolution to the optimum 12-bit
  led_init_all(NULL);

	// Turning LEDs on and off  
	led_set(LED_RED, LED_ON);
	led_set(LED_GRN, LED_OFF);
	
	led_on(LED_YEL1);
	led_off(LED_YEL2);
	
	// Flash an LED in a loop
	for (int i = 0;  i < 9;  i++) {
		led_toggle(LED_YEL3);
		delay(300);
	}

  // The RGB LED(s) are connected to PWM pins, so we can fade them
  // At the core of everything is dim_lvl() which will dim on a scale of 0.._dacTroot
  // You will probably never need/want to use this function
  led_dim_lvl(LED_RGB_BLU, _dacTroot / 3);  // Set to 1/3rd
  delay(1000);

  // You can DIM the LED as a percentage
  led_dim_pc(LED_RGB_BLU, 80);  // Set to 80%
  delay(1000);

  // You can DIM on a hex {0x00..0xFF) scale
  led_dim_pc(LED_RGB_BLU, 0x40);  // Set to 25%
  delay(1000);

  // You can DIM on an arbitrary scale
  led_dim_scale(LED_RGB_BLU, 136,145);  // Set to 136 on a scale of {0..145}
  delay(1000);

  // You can set the RGB LED(s) with a standard (web style) hex value
  led_tri_hex(0xFF8040);
  delay(1000);

  // You can set the RGB LED(s) with a standard (web style) hex value
  led_tri_hex(0x000000);
  delay(1000);

  // Fade the RGB LED(s)
  // Available colours: 0:BLK, 1:RED, 2:GRN, 4:BLU, 6:CYN, 5:MAG, 3:YEL, 7:WHT
  for (int colour = 1;  colour <= LED_TRI_MAX;  colour++) {
    led_fade_in((m2tri_t)colour, 1000);   // Fade in 1.5 seconds
    led_fade_out((m2tri_t)colour, 1000);  // Fade out 1.5 seconds
  }
  delay(1000);

  // Turn everything on to finish
  led_on(LED_YEL2);
  led_on(LED_RGB_RED);
  led_on(LED_RGB_GRN);
  led_on(LED_RGB_BLU);
  
	// For critical errors
	// This will enter a busy-loop flashing the LED ...and NEVER return
	// This example changes the LED state every 300mS (0.3S)
	led_flash_die(LED_GRN, 300);
}

void loop (void) { }

