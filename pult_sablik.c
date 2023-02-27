/*
 * BR101
 *
 * Created: 15.09.2022
 *  Author: Michal
 *
 * Sablik pult
 * 
 */

#include "inc/common_defs.h"
#include <avr/io.h>
#include "inc/defines.h"
#include <avr/interrupt.h>
#include "inc/timer.h"
#include "inc/inputs.h"
#include "inc/xpressnet.h"
#include "inc/logic.h"
#include "inc/xn_accessory.h"
#include "inc/xn_stack.h"

#include <util/delay.h>

// DCC_dec need Timer2 and one of INT

#include <util/delay.h>
volatile byte timer0_flag = 0; // T = 0.1ms
volatile byte timer1_flag = 0; // T = 10ms


//----------------------------------------------------------
ISR(INT0_vect) {
  //RS_ISR_int();
}

//----------------------------------------------------------
ISR(INT1_vect) {
  // sei();
  //DCC_ISR_int();
}

//----------------------------------------------------------
ISR(TIMER0_OVF_vect) {
  // T = 0.1ms
  timer0_flag = true;
}

//----------------------------------------------------------
ISR(TIMER1_CAPT_vect) {
  // T = 10ms
  timer1_flag = true;
}

//----------------------------------------------------------
ISR(TIMER2_COMP_vect)
{
  // sei();
  //DCC_ISR_timer2();
}

//----------------------------------------------------------
void process_timer_10Hz(void)
{
  static int postsc_2min = 1000;
  if (timer1_flag) { // T = 10ms
    timer1_flag = false;
    io_timer_10hz();
    logic_timer_10hz();
    uart_update();
    // refresh RS state after 2 minutes
    if (--postsc_2min == 0) {
      postsc_2min = 12000; // 100*60*2
      //RS_forceUpdate();
    }
  } // if timer1_flag
}

//----------------------------------------------------------
void process_timer_7kHz(void)
{
  if (timer0_flag) { // T = 10ms
    timer0_flag = false;
    //RS_ISR_timer_fast();
  }
}

//----------------------------------------------------------
void init(void)
{
  
  io_init();
  DDRF |= BV(PF0); // set XpressNet dir line to output
  PORTF &= ~BV(PF0); // set XPressNet to receiving
  timer_init();
  uart_init(26); // default address
  xns_init();
  xnacc_init();
  
  sei();
}

//----------------------------------------------------------
int main(void)
{ 
  XDIV = 0x80 | 0x77; // low frequency 10 MHz / 10 =  1.000 MHz
  //XDIV = 0x80 | 0x7C; // low frequency 10 MHz /  5 =  2.000 MHz
  
  init();

  while(1) { // mail loop
    process_timer_7kHz();
    process_timer_10Hz();
    io_loop();
    xnacc_loop();
    xns_loop(); // send messages from queue
    do_pult_logic();
    //dbg_show_byte(outp[1]);
  }
}
