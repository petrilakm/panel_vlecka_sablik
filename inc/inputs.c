/* ========================================================================== */
/*                                                                            */
/*   DCC.h                                                                    */
/*   (c) 2015 Michal Petrilak                                                 */
/*                                                                            */
/*   DCC parser - user functions                                              */
/*                                                                            */
/*   libraly for inputs debounce and hold for specific time                   */
/*                                                                            */
/* ========================================================================== */

#include <avr/io.h>
#include "common_defs.h"
#include "defines.h"
#include "inputs.h"

byte inp         [3] = {0,0,0}; // input states for program
byte inp_real    [3] = {0,0,0}; // real states, for debounce
byte inp_shadow  [3] = {0,0,0};                               // shadow of inp, for calculation
byte outp        [32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // output states for program
byte outp_shadow [3] = {0,0,0}; // internal for sequential output propagation
byte inp_last    [3] = {0,0,0}; // last button state, for event generation
byte inp_event_1 [3] = {0,0,0}; // only changed from last time to active state (button pressed)
byte inp_event_0 [3] = {0,0,0}; // only changed from last time to default state
byte inp_cnt [20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // for debounce - all inputs
//byte inp_hold[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // for hold - only normal inputs
void (*pgenerate_events)(void) = 0; // function pointer for callback function GenerateEvents
byte io_flag_10Hz = 0; // private
byte io_blink_10Hz = 1; // public
byte io_blink_2Hz = 1; // public
byte io_blink_10Hz_cnt = 0; // private
byte io_blink_2Hz_cnt = 0; // private

/*
void dbg_show_byte(byte num)
{
if (num & 0x01) PORTA |= BV(2); else PORTA &= ~BV(2);
if (num & 0x02) PORTF |= BV(6); else PORTF &= ~BV(6);
if (num & 0x04) PORTC |= BV(7); else PORTC &= ~BV(7);
if (num & 0x08) PORTA |= BV(7); else PORTA &= ~BV(7);
if (num & 0x10) PORTC |= BV(5); else PORTC &= ~BV(5);
if (num & 0x20) PORTC |= BV(4); else PORTC &= ~BV(4);
if (num & 0x40) PORTG |= BV(1); else PORTG &= ~BV(1);
if (num & 0x80) PORTC |= BV(0); else PORTC &= ~BV(0);
}
*/

//----------------------------------------------------
void io_init(void)
{
  // pullups
  PORTA = 0b01111000;
  PORTB = 0b01100000;
  PORTC = 0b01001110;
  PORTD = 0b00110000;
  PORTE = 0b11000000;
  PORTF = 0b10011110;
  PORTG = 0b00000001;
  
  // směry pinů
  DDRA = 0b10000100;
  DDRB = 0b10010001;
  DDRC = 0b10110001; // prog
  DDRD = 0b11001010; // serial
  DDRE = 0b00110000; // doubleline, prog
  DDRF = 0b01100001;
  DDRG = 0b00000010;
}

//----------------------------------------------------
byte io_get_state(byte num)
{
  byte pt_seg, pt_b;

  pt_seg = num >> 3;   // byte pointer
  pt_b   = num & 0x07; // bit  pointer

  return ((inp[pt_seg]      >> pt_b) & 1);
}

//----------------------------------------------------
byte io_get_change_on(word num)
{
  byte pt_seg, pt_b;
  byte ret;

  pt_seg = num >> 3;   // byte pointer
  pt_b   = num & 0x07; // bit  pointer
  ret = 0;
  if ((inp_event_1[pt_seg]  >> pt_b) & 1) {
    inp_event_1[pt_seg] &= ~(1 << pt_b); // clear flag
    ret = 1;
  }
  return (ret);
}

//----------------------------------------------------
byte io_get_change_off(word num)
{
  byte pt_seg, pt_b;
  byte ret;

  pt_seg = num >> 3;   // byte pointer
  pt_b   = num & 0x07; // bit  pointer
  ret = 0;
  if ((inp_event_0[pt_seg]  >> pt_b) & 1) {
    inp_event_0[pt_seg] &= ~(1 << pt_b); // clear flag
    ret = 1;
  }
  return (ret);
}

//----------------------------------------------------
void io_set_state(byte num, byte val)
{
  outp[num] = val;
}

//----------------------------------------------------
void io_get_real_state(byte num)
{
  //read all inputs
  
//#define ifsetbit(cond, seg, bit) if (cond) inp_real[seg] |= (1 << bit); else inp_real[seg] &= ~(1 << bit)
#define ifsetbit(port, pin, seg, bit) if ((~PIN##port) & (1 << pin)) inp_real[seg] |= (1 << bit); else inp_real[seg] &= ~(1 << bit)

  switch(num) {
    case 1:
      ifsetbit(A,6,0,0); // 1+          B05         0
      break;
    case 2:
      ifsetbit(A,5,0,1); // 1-          B06         1
      break;
    case 3:
      ifsetbit(C,2,0,2); // 2/3b+       A19         2
      break;
    case 4:
      ifsetbit(C,3,0,3); // 2/3b-       A20         3
      break;
      
    case 5:
      ifsetbit(D,4,0,4); // 3a/5+       A11         4
      break;
    case 6:
      ifsetbit(D,5,0,5); // 3a/5-       A12         5
      break;
    case 7:
      ifsetbit(F,7,0,6); // 4+          B10         6
      break;
    case 8:
      ifsetbit(A,3,0,7); // 4-          B08         7
      break;
      
    case 9:
      ifsetbit(E,7,1,0); // 6/7+        A03         8
      break;
    case 10:
      ifsetbit(E,6,1,1); // 6/7-        A04         9
      break;
    case 11:
      ifsetbit(F,4,1,2); // Se          B13         10
      break;
    case 12:
      ifsetbit(F,3,1,3); // O1          B14         11
      break;
      
    case 13:
      ifsetbit(F,2,1,4); // O2          B15         12
      break;
    case 14:
      ifsetbit(F,1,1,5); // O3          B16         13
      break;
    case 15:
      ifsetbit(A,4,1,6); // R1          B07         14
      break;
    case 16:
      ifsetbit(C,1,1,7); // R2          A18         15
      break;
      
    case 17:
      ifsetbit(B,6,2,0); // R3          B02
      break;
    case 18:
      ifsetbit(C,6,2,1); // R4          A07
      break;
    case 19:
      ifsetbit(B,5,2,2); // R5          A15
      break;
    case 20:
      ifsetbit(G,0,2,3); // R6          B13
      break;
    default:
      break;
  }

  /*
      ifsetbit(A,6,0,0); // 1+          B05         0
      ifsetbit(A,5,0,1); // 1-          B06         1
      ifsetbit(C,2,0,2); // 2/3b+       A19         2
      ifsetbit(C,3,0,3); // 2/3b-       A20         3
      ifsetbit(D,4,0,4); // 3a/5+       A11         4
      ifsetbit(D,5,0,5); // 3a/5-       A12         5
      ifsetbit(F,7,0,6); // 4+          B10         6
      ifsetbit(A,3,0,7); // 4-          B08         7
      ifsetbit(E,6,1,0); // 6/7+        A03         8
      ifsetbit(E,7,1,1); // 6/7-        A04         9
      ifsetbit(F,4,1,2); // Se          B13         10
      ifsetbit(F,3,1,3); // O1          B14         11
      ifsetbit(F,2,1,4); // O2          B15         12
      ifsetbit(F,1,1,5); // O3          B16         13
      ifsetbit(A,4,1,6); // R1          B07         14
      ifsetbit(C,1,1,7); // R2          A18         15
      ifsetbit(B,6,2,0); // R3          B02         16
      ifsetbit(C,6,2,1); // R4          A07         17
      ifsetbit(B,5,2,2); // R5          A15         18
      ifsetbit(G,0,2,3); // R6          B13         19
  */
}

//----------------------------------------------------
void io_set_shadow_output(void)
{
  // set all outputs
  // seg+bit points to outp array, port+pin point to physical pin
#define setoutputbit(seg, bit, port, pin) if ((outp_shadow[seg] >> bit) & 1) PORT##port |= (1 << pin); else PORT##port &= ~(1 << pin)
  
  //if ((outp_shadow[0] >> 0) & 1) PORTA |= (1 << 7); else PORTA &= ~(1 << 7);
  setoutputbit(0,0,A,7); // L1+     B04         0
  setoutputbit(0,1,C,7); // L1-     B03         1
  setoutputbit(0,2,C,4); // L2+     A21         2
  setoutputbit(0,3,C,5); // L2-     B01         3
  setoutputbit(0,4,D,6); // L3a+    A13         4
  setoutputbit(0,5,D,7); // L3a-    A14         5
  setoutputbit(0,6,C,0); // L3b+    A17         6
  setoutputbit(0,7,G,1); // L3b-    A16         7
  
  setoutputbit(1,0,F,6); // L4+     B11         8
  setoutputbit(1,1,A,2); // L4-     B09         9
  setoutputbit(1,2,B,7); // L5+     A09         10
  setoutputbit(1,3,D,1); // L5-     A10         11
  setoutputbit(1,4,B,0); // L6+     A05         12
  setoutputbit(1,5,B,4); // L6-     A06         13
  setoutputbit(1,6,E,4); // L7+     A01         14
  setoutputbit(1,7,E,5); // L7-     A02         15
  
  setoutputbit(2,0,F,5); // LSe     B12         16
}

//----------------------------------------------------
void io_debounce_single(byte num, byte debounce_num)
{
  byte but_re, but; // temporary variables
  byte pt_seg, pt_b;

  pt_seg = num >> 3;   // byte pointer
  pt_b   = num & 0x07; // bit  pointer
  but_re = (inp_real[pt_seg] >> pt_b) & 1;
  but    = (inp[pt_seg]      >> pt_b) & 1;

  if (but != but_re) {
    // input is in different state - wait some time
    inp_cnt[num]++;
//    if (!but_re) {
//      debounce_num = 5;
//    }

    if (inp_cnt[num] >= debounce_num) {
      // reflect real state to shadow registers
      if (but_re) {
        inp_shadow[pt_seg] |=  (1 << pt_b);
       } else {
        inp_shadow[pt_seg] &= ~(1 << pt_b);
      }
    }
   } else {
    // input is in same state
    inp_cnt[num] = 0;
  }
}

//----------------------------------------------------
void io_calculate_output(byte num)
{
  byte val;
  byte pt_seg, pt_b;

  pt_seg = num >> 3;   // byte pointer
  pt_b   = num & 0x07; // bit  pointer
  
  val = outp[num] & 3;

  switch(val) {
    case 0: // off
      outp_shadow[pt_seg] &= ~(1 << pt_b);
      break;
    case 1: // on
      outp_shadow[pt_seg] |= (1 << pt_b);
      break;
      
    case 2: // blink 2 Hz
      if (io_blink_2Hz) {
        outp_shadow[pt_seg] |= (1 << pt_b);
       } else {
        outp_shadow[pt_seg] &= ~(1 << pt_b);
      }
      break;
    case 3: // blink 10 Hz
      if (io_blink_10Hz) {
        outp_shadow[pt_seg] |= (1 << pt_b);
       } else {
        outp_shadow[pt_seg] &= ~(1 << pt_b);
      }
      break;
      
    default:
      break;
  }
}

//----------------------------------------------------
void io_loop(void)
{
  static byte io_loop_state = 0;
  byte num;
  byte seg;
  byte subseg;
  byte i;

  byte changes;
  // state machine state
  // 0 T T C  C C C C
  // 0 T T s  s i i i
  // C = counter in current task (0..31)
  // T = task
  // 0x00 - wait
  // 0x20 - get real state
  // 0x40 - debounce
  // 0x60 - propagate results
  if (io_loop_state == 0) io_loop_state = 1;
  if (io_flag_10Hz) {
    io_flag_10Hz = false;
    // start io sequence
    if (io_loop_state == 0) io_loop_state = 1; // start if stopped

    // generate 2Hz signal
    if (++io_blink_2Hz_cnt > 5) {
      io_blink_2Hz_cnt = 0;
      if (io_blink_2Hz) io_blink_2Hz=0; else io_blink_2Hz=1;
    }
    
    // generate 10Hz signal
    if (io_blink_10Hz) io_blink_10Hz=0; else io_blink_10Hz=1;
    
  }

  num    = (io_loop_state & 0x1F); // 0 - 32
  seg    = (io_loop_state & 0x60) >> 5; // 0 - 2
  subseg = (io_loop_state & 0x18) >> 3; // 0 - 2
  i      = (io_loop_state & 0x07); // 0 - 7

  // Task - real_state & prepare
  if (seg == 1) {
    // get real state
    io_get_real_state(num);    // capture input state
    //io_loop_state |= 0x1F;  // end of phase
  }


  // Task - debounce single
  if (seg == 2) {
    switch (num) {
      case  0 ... 16:
        // set shadow outputs
        io_calculate_output(num);
        // test for locked inputs
        io_debounce_single(num, DEBOUNCE_NUM);
      case 17 ... 19:
        // test for locked inputs
        io_debounce_single(num, DEBOUNCE_NUM);        
        break;
      default:
        io_loop_state |= 0x1F;  // end of phase
        break;
    }
  }

  if ((seg == 3) && (subseg == 0)) {
    inp[i] = inp_shadow[i];
    if (i==2) io_loop_state |= 0x07;
  }
  if ((seg == 3) && (subseg == 1)) {
    io_set_shadow_output(); // flush output to pins
    if (i==0) io_loop_state |= 0x07;
  }
  if ((seg == 3) && (subseg == 2)) {
    changes = (inp[i] ^ inp_last[i]); // detect changes
    inp_event_1[i] |= (( inp[i]) & changes); // catch transitions 0->1
    inp_event_0[i] |= ((~inp[i]) & changes); // catch transitions 1->0
    inp_last[i] = inp[i];
    
    if (i==2) io_loop_state = 0; // stop state machine - all is done
  }

  // if state machine is running move to next stage
  if (io_loop_state != 0) { io_loop_state++; }
}

//----------------------------------------------------
void io_timer_10hz(void)
{
  io_flag_10Hz = true;
}
