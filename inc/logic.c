// Logic of control panel

#include "common_defs.h"
#include "defines.h"
#include "inputs.h"
#include "xn_accessory.h"
#include "logic.h"

/*
 * usable functions:
 * 
 * byte io_get_state(byte num);  // get single value from inp variable
 * void io_set_state(byte num, byte val);  // set single value to outp variable
 * byte io_blink_2Hz
 * 
 * 
 */

void do_pult_logic_internal(void);
byte logic_event_flag;

byte logic_nocom_cnt = 0;
#define logic_nocom_cnt_max (6+1)

byte logic_flag_10Hz = 0; // private
byte logic_state = 0; // private
#define LOGIC_INITTIMER_MAX (70)
byte logic_inittimer = LOGIC_INITTIMER_MAX; // private
byte logic_dcctimer = 60; // private, chack dcc status

byte logic_navstate = 0;
byte logic_osvstate[3] = {0,0,0};

// indikace AUS
byte logic_indstate;  // on
byte logic_indstate0; // off
/*
void logic_debug(byte num)
{

#define bB0 (8)
#define bB1 (9)
#define bB2 (0)
#define bB3 (1)
#define bB4 (3)
#define bB5 (2)
#define bB6 (6)
#define bB7 (7)

io_set_state(bB0,(num & 0x01) >> 0);
io_set_state(bB1,(num & 0x02) >> 1);
io_set_state(bB2,(num & 0x04) >> 2);
io_set_state(bB3,(num & 0x08) >> 3);
io_set_state(bB4,(num & 0x10) >> 4);
io_set_state(bB5,(num & 0x20) >> 5);
io_set_state(bB6,(num & 0x40) >> 6);
io_set_state(bB7,(num & 0x80) >> 7);
}
*/
void logic_turnout_indicate(byte num, byte out)
{
  num--;
  if (xnacc_turnout_state[num] == 0) {
    io_set_state(out  ,logic_indstate0);
    io_set_state(out+1,logic_indstate0);
  }
  if (xnacc_turnout_state[num] == 1) {
    io_set_state(out  ,logic_indstate);
    io_set_state(out+1,0);
  }
  if (xnacc_turnout_state[num] == 2) {
    io_set_state(out  ,0);
    io_set_state(out+1,logic_indstate);
  }
  if (xnacc_turnout_state[num] == 3) {
    io_set_state(out  ,3);
    io_set_state(out+1,3);
  }
}

void do_pult_logic_internal(void)
{ 
  if (xnacc_poweron) {
    logic_indstate = 1; // power on -> steady LED
    logic_indstate0= 0; // power on -> steady off
   } else {
    logic_indstate = 2; // power off -> blink LED
    logic_indstate0= 2; // power on  -> blink LED
  }

  switch(logic_state) {
    case 1:
      logic_turnout_indicate(4,0);  // 1 logic_turnout_indicate(turnout address, first LED number)
      break;
    case 2:
      logic_turnout_indicate(7,2);  // 2
      break;
    case 3:
      logic_turnout_indicate(5,4);  // 3a
      break;
    case 4:
      logic_turnout_indicate(7,6);  // 3b
      break;
    case 5:
      logic_turnout_indicate(11,8);  // 4
      break;
    case 6:
      logic_turnout_indicate(5,10);  // 5
      break;
    case 7:
      logic_turnout_indicate(9,12); // 6
      break;
    case 8:
      logic_turnout_indicate(9,14); // 7
      break;

    case 9:
      if (logic_navstate) {  // Se
        io_set_state(16, 1); // povolen
       } else {
        io_set_state(16, 0); // zakázán
      } 
      break;
    case 12:
      if (io_get_change_on(0)) xnacc_turnout_action(4, 1); // button 0 operate turnout 4 to position +
      break;
    case 13:
      if (io_get_change_on(1)) xnacc_turnout_action(4, 0); // button 1 operate turnout 4 to position -
      break;
    case 14:
      if (io_get_change_on(2)) xnacc_turnout_action(7, 1); // button 2 operate turnout 7 to position +
      break;
    case 15:
      if (io_get_change_on(3)) xnacc_turnout_action(7, 0); // button 3 operate turnout 7 to position -
      break;
    case 16:
      if (io_get_change_on(4)) xnacc_turnout_action(5, 1); // button 4 operate turnout 5 to position +
      break;
    case 17:
      if (io_get_change_on(5)) xnacc_turnout_action(5, 0); // button 5 operate turnout 5 to position -
      break;
    case 18:
      if (io_get_change_on(6)) xnacc_turnout_action(11, 1);// button 6 operate turnout 11 to position +
      break;
    case 19:
      if (io_get_change_on(7)) xnacc_turnout_action(11, 0);// button 7 operate turnout 11 to position -
      break;
    case 20:
      if (io_get_change_on(8)) xnacc_turnout_action(9, 0); // button 8 operate turnout 9 to position 0
      break;
    case 21:
      if (io_get_change_on(9)) xnacc_turnout_action(9, 1); // button 9 operate turnout 9 to position 1
      break;

    case 22:
      if (io_get_change_on(10)) {
        if (xnacc_poweron) logic_navstate ^= 1;
        xnacc_turnout_action(63, logic_navstate);   // operate Se signal
      }
      break;
    case 23:
      if (io_get_change_on(11)) {
        if (xnacc_poweron) logic_osvstate[0] ^= 1;
        xnacc_turnout_action(137, logic_osvstate[0]);   // operate osvìtlení 1
      }
      break;
    case 24:
      if (io_get_change_on(12)) {
        if (xnacc_poweron) logic_osvstate[1] ^= 1;
        xnacc_turnout_action(138, logic_osvstate[1]);   // operate osvìtlení 2
      }
      break;
    case 25:
      if (io_get_change_on(13)) {
        if (xnacc_poweron) logic_osvstate[2] ^= 1;
        xnacc_turnout_action(139, logic_osvstate[2]);   // operate osvìtlení 3
      }
      break;
    case 26:
      if (io_get_change_on(14)) {
        xnacc_turnout_action(129, 1);   // operate rozpojovaè 1
      }
      if (io_get_change_off(14)) {
        xnacc_turnout_action(129, 2);   // operate rozpojovaè 1
      }
      break;
    case 27:
      if (io_get_change_on(15)) {
        xnacc_turnout_action(130, 1);   // operate rozpojovaè 2
      }
      if (io_get_change_off(15)) {
        xnacc_turnout_action(130, 2);   // operate rozpojovaè 2
      }
      break;
    case 28:
      if (io_get_change_on(16)) {
        xnacc_turnout_action(131, 1);   // operate rozpojovaè 3
      }
      if (io_get_change_off(16)) {
        xnacc_turnout_action(131, 2);   // operate rozpojovaè 3
      }
      break;
    case 29:
      if (io_get_change_on(17)) {
        xnacc_turnout_action(132, 1);   // operate rozpojovaè 4
      }
      if (io_get_change_off(17)) {
        xnacc_turnout_action(132, 2);   // operate rozpojovaè 4
      }
      break;
    case 30:
      if (io_get_change_on(18)) {
        xnacc_turnout_action(133, 1);   // operate rozpojovaè 5
      }
      if (io_get_change_off(18)) {
        xnacc_turnout_action(133, 2);   // operate rozpojovaè 5
      }
      break;
    case 31:
      if (io_get_change_on(19)) {
        xnacc_turnout_action(134, 1);   // operate rozpojovaè 6
      }
      if (io_get_change_off(19)) {
        xnacc_turnout_action(134, 2);   // operate rozpojovaè 6
      }
      break;

  }

  if (logic_state > 0) logic_state++;
  if (logic_state > 35) logic_state = 0;
}

void do_pult_logic(void)
{
  byte ret;
  
  if (xnacc_poweron) {
    logic_indstate = 1; // power on -> steady LED
    logic_indstate0= 0; 
   } else {
    logic_indstate = 2; // power off -> blink LED
    logic_indstate0= 2;
  }

  
  if (logic_flag_10Hz) {
    logic_flag_10Hz = false;
    // start logic state machine
    if (logic_state == 0) {
      logic_state = 1;
    }
    
    logic_nocom_cnt++;
    
    if (xnacc_ccavail) {
      if ((logic_inittimer == 0) || (logic_inittimer == LOGIC_INITTIMER_MAX)) { // after init or no init
        if (logic_dcctimer > 0) logic_dcctimer--;
      }
      
      if (logic_dcctimer == 0) {
        // periodic
        logic_dcctimer=120;
        xnacc_trackpower_request(); // dotaz sa stav DCC
      }
      
      
      // count inittimer
      if (logic_inittimer > 0) {
       logic_inittimer--;
      }
      
      //timer po zapnutí  // 78
      ret = 0;
//      if (logic_inittimer == 70) ret=xnacc_trackpower_request(); // dotaz sa stav DCC
      if (logic_inittimer == 68) ret=xnacc_feedback_request(4);  // dotaz na pøestávník s adresou 4
      if (logic_inittimer == 66) ret=xnacc_feedback_request(5);
      if (logic_inittimer == 64) ret=xnacc_feedback_request(7);
      if (logic_inittimer == 62) ret=xnacc_feedback_request(9);
      if (logic_inittimer == 60) ret=xnacc_feedback_request(11);
      if (ret != 0) {
        logic_inittimer++;
      }
      if (logic_inittimer == 58) {                           // pokud máme stavy, tak neprovádìj sekvenci dále 
        byte sum = 0;
        byte i;
        for(i = 0; i < 20; i++) {
          sum += xnacc_turnout_state[i];
        }
        if (sum > 3) logic_inittimer = 0; // pokud máme nìjaký stav, tak pøestaneme s inicializací (centrála není po restartu)
      }
      if (logic_inittimer == 56) xnacc_turnout_action(4, 1); // nastavit pøestavník s adresou 4 do +
      if (logic_inittimer == 54) xnacc_turnout_action(5, 1);
      if (logic_inittimer == 52) xnacc_turnout_action(7, 1);
      if (logic_inittimer == 50) xnacc_turnout_action(9, 1);
      if (logic_inittimer == 48) xnacc_turnout_action(11, 1);
      if (logic_inittimer == 46) xnacc_turnout_action(63, 1);
    }
  }
  
  if (xnacc_ccavail) {
    do_pult_logic_internal();
  }
  
  if ((!xnacc_poweron) && (logic_inittimer > 0)) {
    // není DCC a neprobìhla celá inicializace
    logic_inittimer = LOGIC_INITTIMER_MAX; // inicializace znova
  } 
    
  if (!xnacc_ccavail) {
    logic_inittimer = LOGIC_INITTIMER_MAX;
    // command center is not available (no communication)
    if (logic_nocom_cnt>logic_nocom_cnt_max) {
      logic_nocom_cnt = 0;
    }
    //logic_nocom_cnt=6;
    if (logic_nocom_cnt == 0) {
      io_set_state(logic_nocom_cnt_max*2, 0);
      io_set_state(logic_nocom_cnt_max*2+1, 0);
     } else {
      io_set_state((logic_nocom_cnt-1)*2  , 0);
      io_set_state((logic_nocom_cnt-1)*2+1, 0);
    
    }
    io_set_state(logic_nocom_cnt*2, 3);
    io_set_state(logic_nocom_cnt*2+1, 3);
    
  }
}

//----------------------------------------------------
void logic_timer_10hz(void)
{
  logic_flag_10Hz = true;
}
