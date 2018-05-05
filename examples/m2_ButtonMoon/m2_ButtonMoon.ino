#include <m2_io.h>

//+============================================================================= =======================================
void  heartbeat (void)
{
  // 'timer' is the list of times for which the LED will be on/off
  //         assuming the LED is OFF before we call heartbeat() for the first time
  //           the list starts in the ON condition
  // 'pi'    is a pointer to the period in the timer list which is currently active
  // 'ms'    is the last timer reading we made
  int                  timer[] = {/*on*/100,/*off*/160,/*on*/100,/*off*/1400, 0};
  static int*          pi      = NULL;
  static unsigned int  ms      = millis();

  // This is only ever executed the FIRST time you call heartbeat
  if (!pi) {
    led_toggle(LED_RED2);
    pi = timer;
  }
  
  // When the current timer expires,  toggle the LED and move on to the next entry
  // ...If you find a timer of value 0, wrap back round to the start of the list
  if (millis() >= ms + *pi) {
    led_toggle(LED_RED2);
    if (!*(++pi))  pi = timer ;
    ms = millis();
  }
}

//+============================================================================= =======================================
// Similar to heartbeat(), a different timer list, no comments, and...
// This timer has a "stop" facility ... ie.  pulse(false);
// If you wanted lots of timers running, 
//   you might want to write a single (more complex) function which handles multiple timers
//   rather than accrue lots of copies of a function - all virtually identical to each other
//
void  pulse (bool run)
{
  int                  timer[] = {/*on*/100,/*off*/160+100+1400, 0};
  static int*          pi      = NULL;
  static unsigned int  ms      = millis();

  if (run) {
    if (!pi) {
      led_toggle(LED_GRN2);
      pi = timer;
    }
  
    if (millis() >= ms + *pi) {
      led_toggle(LED_GRN2);
      if (!*(++pi))  pi = timer ;
      ms = millis();
    }
    
  } else {
    led_off(LED_GRN2);
  }
}

//+============================================================================= =======================================
// If this function needs comments, you've just "bitten off more than you can chew"
//
void  ledTest (void)
{
  led_on(LED_GRN );  delay(150);  led_off(LED_GRN );  delay(150);
  led_on(LED_YEL3);  delay(150);  led_off(LED_YEL3);  delay(150);
  led_on(LED_YEL2);  delay(150);  led_off(LED_YEL2);  delay(150);
  led_on(LED_YEL1);  delay(150);  led_off(LED_YEL1);  delay(150);
  led_on(LED_RED );  delay(150);  led_off(LED_RED );  delay(150);
  led_on(LED_RED2);  delay(150);  led_off(LED_RED2);  delay(150);
  led_on(LED_GRN2);  delay(150);  led_off(LED_GRN2);  delay(150);
  led_on(LED_BLU );  delay(150);  led_off(LED_BLU );  delay(150);

  led_on(LED_GRN);
  led_on(LED_YEL3);  led_on(LED_YEL2);  led_on(LED_YEL1);
  led_on(LED_RED);
  led_on(LED_RED2);  led_on(LED_GRN2);  led_on(LED_BLU);

  delay(1000);
  
  led_off(LED_GRN);
  led_off(LED_YEL3);  led_off(LED_YEL2);  led_off(LED_YEL1);
  led_off(LED_RED);
  led_off(LED_RED2);  led_off(LED_GRN2);  led_off(LED_BLU);
}

//+============================================================================= =======================================
void  setup (void) 
{
  // Configure all the LEDs
  led_init_all(NULL);

  // Configure the Serial port
  //
  // LED_BLU says we are waiting for a serial port connection
  // IF you want console logging, you've got 10,000mS (10 seconds) to connect.
  //
  // I suggest 115200 for fast-vs-reliable serial comms
  // But if you are using Native USB over a Virtual COM Port, this number is meaningless
  //
  led_on(LED_BLU);
  SerialUSB.begin(115200);
  for (unsigned int ms = millis(); !SerialUSB && ((ms + 10000) > millis());  ) ;
  led_off(LED_BLU);
  if (SerialUSB)  SerialUSB.print("# Serial connection established") ;
  
  // Configure the buttons, with the default debounce period
  btn_init_all(BTN_TMR_DFLT);

  // Run an LED test
  SerialUSB.print("# LED Test running");
  ledTest();
  SerialUSB.println("- done");

  SerialUSB.println("# Button Test - ready...");
}

//+============================================================================= =======================================
void  loop (void) 
{
    static unsigned int  longClick   = 500;    // mS (1S == 1000mS)
    static bool          autorelease = false;  // Autorelease off/on for longClick on SW1

    // The heartbeat
    heartbeat();

    // --------------------------------------------------------------------------------------------------------
    // Handle presses of the Button labelled "SW1"
    // --> NOTICE HOW THE HEARTBEAT  !STOPS!  WHILE THE BUTTON IS HELD
    //
    // This model allows for (the simple implementation of) features such as auto-release
    // But the rest of the system will hang while the button is pressed
    //
    // onDown   : LED_GRN  = !LED_GRN (toggle)
    // isDown   : LED_YEL3 = ON
    // onUp     : LED_YEL3 = OFF
    // fastClick: LED_YEL2 = ON  ;  LED_GRN  = !LED_GRN (un'toggle)
    // longClick: Do NOT "un'toggle" LED_GRN (do nothing)
    //
    if (btn_onDown(BTN_SW1)) {
      unsigned int  ms = millis();  // Capture the time at which it was pressed
      
      // ** What to do on the "falling edge condition" [button pressed]
      led_toggle(LED_GRN);
      
      // Wait for Button to be released
      while (btn_isDown(BTN_SW1)) {
        // If 'autorelease' is enabled, release the button when we reach the longClick timeout
        if (autorelease && (millis() >= ms + longClick))  break ;
        // ** What to do while the button is being held down [button down]
        led_toggle(LED_YEL3);
        delay(100);
      }
      led_off(LED_YEL3);  // Make sure we end up in an off state

      // ** What to do on the "rising edge condition" [button released]
      led_toggle(LED_GRN);   // Leave LED_GRN as we found it
      
      if (millis() < ms + longClick) {
        // ** What to do if the "rising edge condition" occurred WITHIN the longClick period [fastClick]
        led_toggle(LED_YEL2);  // Toggle LED_YEL2
        
      } else {
        // ** What to do if the "rising edge condition" occurred ON/AFTER the longClick period [longClick]
        led_toggle(LED_YEL1);  // Toggle LED_RED
        // We leave LED_GRN in the new state
      }
    }    
    
    // --------------------------------------------------------------------------------------------------------
    // Handle presses of the Button labelled "SW2"
    //
    // This model allows the system to keep running while the button is held down
    // But it is considerably more difficult to implement features akin to auto-release
    //
    // Any press of the SW2 will toggle the autorelease feature
    //
    // --> NOTICE HOW THE HEARTBEAT  !CONTINUES!  WHILE THE BUTTON IS HELD
    switch (btn_get(BTN_SW2)) {
      case ONDOWN :  
        led_on(LED_BLU);
        // Fallthrough : ISDOWN
      case ISDOWN :
        pulse(true);
        break;
      
      case ONUP :
        led_off(LED_BLU);
        autorelease = !autorelease;
        led_set(LED_RED, autorelease ? LED_ON : LED_OFF) ;
        pulse(false);
        // Fallthrough : ISUP
      case ISUP :  
        // This section will commonly be empty
        break;
        
      default :  break ;  // Good coding practice
    }

}


//============================================================================== =======================================
// Some cut'n'paste code for you...
//
#if 0

    // --------------------------------------------------------------------------------------------------------
    // This model allows for features such as auto-release
    // But the rest of the system will hang while the button is pressed
    {
      static unsigned int  longClick   = 500;    // mS (1S == 1000mS)
      static bool          autorelease = false;  // Autorelease off/on for longClick on SW1
      static bool          autorepeat  = 50;     // Autorepeat delay
      
      if (btn_onDown(BTN_SW1)) {
        unsigned int  ms = millis();
        
        // ** falling edge
        SerialUSB.print("SW1 : Pressed");
        
        while (btn_isDown(BTN_SW1)) {  // Non-Autorelease version
          // If 'autorelease' is enabled, release the button when we reach the longClick timeout
          if (autorelease && (millis() >= ms + longClick))  break ;
          
          // ** button held down
          SerialUSB.print(".");
          delay(autorepeat);
        }
        
        // ** rising edge
        SerialUSB.print("released - ");
        
        if (millis() >= ms + longClick) {
          // ** Long press
          SerialUSB.println("longClick");
          
        } else {
          // ** Short press
          SerialUSB.println("fastClick");
        }
      }
    }
    
    // --------------------------------------------------------------------------------------------------------
    // This model allows the system to keep running while the button is pressed
    // But it is considerably more difficult to implement features akin to auto-release
    {
      static unsigned int  lClk = 500;  // mS (1S == 1000mS)
      static unsigned int  ms   = 0;    // mS timer while button is pressed (ISDOWN)
      static unsigned int  ts   = 0;    // Timestamp of when button was pressed (ONDOWN)
      static unsigned int  rpt  = 50;   // Repeat delay
      
      switch (btn_get(BTN_SW2)) {
        case ONDOWN :  
          ts = millis();
          SerialUSB.print("SW2 : Pressed");
          // Fallthrough : ISDOWN
        case ISDOWN :
          if (millis() > ms + rpt) {
            SerialUSB.print(".");
            ms = millis();
          }
          break;
        
        case ONUP :
          SerialUSB.print("released - ");
          if (millis() >= ts + lClk) {
            SerialUSB.println("longClick");
          } else {
            SerialUSB.println("fastClick");
          }
          // Fallthrough : ISUP
        case ISUP :  
          // This section will commonly be empty
          break;
          
        default :  break ;  // Good coding practice
      }
    }    
#endif  

