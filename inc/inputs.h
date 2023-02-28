#ifndef _INPUTS_H_
#define _INPUTS_H_

//----------------------------------------------------

#define DEBOUNCE_NUM    (3)

//----------------------------------------------------
extern byte inp[3];      // buttons states for program
extern byte outp[32];      // leds states for program (blinking codes)
extern byte outp_shadow[3];      // leds states for pins
//extern byte inp_event_1[18]; // only changed from last time to active state
//extern byte inp_event_0[18]; // only changed from last time to default state
byte io_get_state(byte num);  // get single value from inp variable
byte io_get_change_on(word num); // get cached transition 0->1 and clear cache
byte io_get_change_off(word num);
void io_set_state(byte num, byte val);  // set single value to outp variable
extern void (*pgenerate_events)(void); 
extern byte io_blink_2Hz;
extern byte io_blink_10Hz;

//----------------------------------------------------
void io_init(void);
void io_loop(void);
void io_timer_10hz(void);
void io_generate_events(void);
void io_detect_events(void);
void io_get_real_state(byte num);
void io_set_shadow_output(void);

void io_debounce_single(byte num, byte debounce_num);

void dbg_show_byte(byte num);

#endif
