#ifndef _XN_ACC_H_
#define _XN_ACC_H_

#include "common_defs.h"

// main queue in .c file !

void xnacc_uart_on_receive(uint8_t recipient, uint8_t data[], uint8_t size); // ISR
void xnacc_uart_on_sniff(uint8_t recipient, uint8_t data[], uint8_t size); // ISR
void xnacc_uart_parse_buffer(void);
void xnacc_uart_on_addressed(void);
void xnacc_uart_on_addressed_stopped(void);
void xnacc_init(void);
void xnacc_loop(void);

void xnacc_turnout_action(word num, byte state);
byte xnacc_trackpower_request(void);
byte xnacc_feedback_request(word num);

extern byte xnacc_ccavail;
extern byte xnacc_turnout_state[1024];
extern byte xnacc_poweron;



#endif
