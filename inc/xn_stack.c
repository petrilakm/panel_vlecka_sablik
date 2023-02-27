// stack (queue) for XPressNET library

// stack can hold multiple messages to send
// if command station is busy, messege is repeated

#include "xn_stack.h"

xns_queue_item xns_queue[XNS_SIZE]; // main queue to send
byte xns_queue_begin;

byte xnmsg[4] = {0,0,0,0};


byte _xns_queue_plusone(byte i)
{
  return ((i+1) % XNS_SIZE);
}

byte _xns_queue_offset(byte i)
{
  return ((i+xns_queue_begin) % XNS_SIZE);
}

void xns_init(void)
{
  byte i;
  for(i=0; i<XNS_SIZE; i++) {
    xns_queue[i].status = xnqs_empty;
  }
  xns_queue_begin = 0;
}
// add item to queue from xn_buf
void xns_send(void)
{
  byte i, pos,pos_w;
  pos_w = 255;
  // find first empty slot
  for(i=0; i<XNS_SIZE; i++) {
    pos = _xns_queue_offset(i);
    if (xns_queue[pos].status == xnqs_empty) {
      pos_w = pos;
      break;
    };
  };
  if (pos_w == 255) {
    // no space in queue
    return;
  }
  // empty slot number in pos_w
  for(i=0; i<3; i++) {
    // copy data to queue
    xns_queue[pos_w].data[i] = xnmsg[i];
  }
  // mark message to send
  xns_queue[pos_w].status = xnqs_new; 
}

void xns_loop(void)
{
  //
  byte pos, len;
  pos = xns_queue_begin;
  if (xns_queue[pos].status > 0) {
    // data in queue
    if (uart_can_fill_output_buf()) {
      // outgoing channel is ready
      len = (xns_queue[pos].data[0] & 0x0F) + 2;
      xns_queue[pos].status++;
      if (xns_queue[pos].status == xnqs_error) {
        // so many tries, give up sending
        xns_queue[pos].status = xnqs_empty;
        return;
      }
      // try to send
      uart_send(xns_queue[pos].data, len);
      // ToDo: parse ack message 
    }
   } else {
    // begin points to empty slot, advance to next 
    xns_queue_begin = _xns_queue_plusone(xns_queue_begin);
  }

}

