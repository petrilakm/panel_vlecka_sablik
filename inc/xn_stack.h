#ifndef _XN_STACK_H_
#define _XN_STACK_H_

//include dependencies
#include "common_defs.h"
#include "xn_accessory.h"
#include "xpressnet.h"

// set constants
#define XNS_SIZE (16)

extern byte xnmsg[4];

// define queue of commands to send and their states
// states
enum xns_queue_state {
  xnqs_empty = 0, // empty slot
  xnqs_new = 1, // new data, waiting to transmit
  xnqs_send1 = 2, // data transmitted 1x
  xnqs_send2 = 3, // data transmitted 2x
  xnqs_send3 = 4, // data transmitted 3x
  xnqs_send4 = 5,  // data transmitted 4x
  xnqs_error = 6
};

// one item in queue
typedef struct {
  enum xns_queue_state status;
  byte data[3];
} xns_queue_item;

void xns_init(void);
void xns_send(void);
void xns_loop(void);

#endif
