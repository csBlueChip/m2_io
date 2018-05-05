#ifndef M2_BTN_H_
#define M2_BTN_H_

#include <Arduino.h>
#include <stdint.h>

//------------------------------------------------------------------------------ ---------------------------------------
#define BTN_CNT       (2)     // Number of buttons

#define BTN_EDGE      (0x02)  // 00x0 : Set -> Edge condition
#define BTN_UP        (0x01)  // 000x : Set -> Button up
#define BTN_DOWN      (0x00)  // 000x : Clr -> Button down

// Note that ISDOWN is *not* the same as isDown
//   ie.  isDown == (ONDOWN || ISDOWN)
#define ISDOWN        (BTN_DOWN)
#define ISUP          (BTN_UP)

#define ONDOWN        (BTN_DOWN | BTN_EDGE)
#define ONUP          (BTN_UP   | BTN_EDGE)

#define BTN_TMR_DFLT  (5)     // Default debounce timer (in mS)

//------------------------------------------------------------------------------ ---------------------------------------
extern  unsigned int  btnTimer[BTN_CNT];  // Debounce timers
extern  uint8_t       btnState[BTN_CNT];  // State memory
extern  unsigned int  btnDebounce;        // Debounce period

//@============================================================================= =======================================
// Pin number -> array entry
//
// "Button1/2" is how the M2 refer to its buttons - which are labelled "SW1/2"
// If you're porting this library, this will more classcially be {1..n} and/or {A1..An}
//
typedef
	enum btn {
		BTN_SW1 = Button1,    // Pin names for buttons
		BTN_SW2 = Button2,
	}
btn_t;

//------------------------------------------------------------------------------ 
// We need a function to map the BTN_??? names to array entries
//
static inline
int  btn2arr (btn_t btn)
{
	switch (btn) {
		case BTN_SW1 :  return 0 ;
		case BTN_SW2 :  return 1 ;
		default      :  return 0 ;  //! You may want to handle this error condition
	}
}

//============================================================================== =======================================
int   btn_get      (btn_t btn) ;
bool  btn_init     (btn_t btn) ;
bool  btn_init_all (int mS) ;

//@============================================================================= =======================================
// Set/get debounce period
static inline  void  btn_debounce_set (int mS)  {  btnDebounce = mS;    }
static inline  int   btn_debounce_get (void)    {  return btnDebounce;  }

// Read WITHOUT debounce
static inline  int   btn_read    (btn_t btn)    {  return (digitalRead(btn) == LOW) ? BTN_DOWN : BTN_UP ;  }

// Check for EDGE condition
static inline  bool  btn_onDown  (btn_t btn)    {  return  (btn_get(btn) == ONDOWN) ? true : false ;  }
static inline  bool  btn_onUp    (btn_t btn)    {  return  (btn_get(btn) == ONUP  ) ? true : false ;  }
                                              
// Check for EDGE *OR* STATE condition
static inline  bool  btn_isDown  (btn_t btn)    {  return ((btn_get(btn) & ~BTN_EDGE) == BTN_DOWN) ? true : false ;  }
static inline  bool  btn_isUp    (btn_t btn)    {  return ((btn_get(btn) & ~BTN_EDGE) == BTN_UP  ) ? true : false ;  }
                                              
#endif // M2_BTN_H_
