#include <stdint.h>

#include "m2_btn.h"
              
unsigned int  btnTimer[BTN_CNT];
uint8_t       btnState[BTN_CNT];
unsigned int  btnDebounce;

//+============================================================================= =======================================
int  btn_get (btn_t btn)
{
	unsigned int  tm      = millis();
	int           arr     = btn2arr(btn);
	int           swState = btn_read(btn);
	
	// No change  OR  Still inside the debounce timer
	if ( (swState == btnState[arr]) ||
	     (tm      <  btnTimer[arr])    )  return btnState[arr] ;  // Return state
	
	// State change
	btnState[arr] = swState;               // Note the new value
	btnTimer[arr] = tm + btnDebounce;      // Restart the debounce timer
	return BTN_EDGE | (int)btnState[arr];  // Return edge condition
}

//+============================================================================= =======================================
bool  btn_init (btn_t btn)
{
	int  arr = btn2arr(btn);
	
	pinMode(btn, INPUT_PULLUP);
	
	btnTimer[arr] = 0;
	btnState[arr] = btn_read(btn);
	
	return true;
}

//+============================================================================= =======================================
bool  btn_init_all (int mS)
{
	btnDebounce = mS;
	btn_init(BTN_SW1);
	btn_init(BTN_SW2);
	return true;
}
