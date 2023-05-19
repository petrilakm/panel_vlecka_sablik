#ifndef _XN_STACK_H_
#define _XN_STACK_H_

//include dependencies
#include "common_defs.h"
#include "xn_accessory.h"
#include "xpressnet.h"

// set constants
#define XNS_SIZE (16)
// delay betweeb two messages in queue 1 = receiver one inquiry msg, 10 = received 10x inquiry messages
#define XNS_MESSAGES_TIMESTEP (15)

extern byte xnmsg[4];

extern byte dbg;

// define queue of commands to send and their states
// states
enum xns_queue_state {
  xnqs_empty = 0, // empty slot
  xnqs_new = 1, // new data, waiting to transmit
  xnqs_send = 2, // data transmitted
  xnqs_error = 3
};

// one item in queue
typedef struct {
  enum xns_queue_state status;
  byte data[3];
} xns_queue_item;

void xns_init(void); // init output stack
void xns_send(void); // send data from xnbuf
void xns_loop(void); // loop in main
void xns_ack(void); // ack reception by command station (simulated)
void xns_busy(void); // received busy

byte xns_empty_queue(void); // test if whole queue is empty

#endif
