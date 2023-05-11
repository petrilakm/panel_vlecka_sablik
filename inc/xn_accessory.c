// Read & Send XpressNET accessory messages

#include "common_defs.h"
#include "defines.h"
#include "xn_stack.h"

byte xnacc_ccavail = false; // command center available
byte xnacc_turnout_state[1024];
byte xnacc_poweron = 0;

byte xnacc_datatoparse    = false;

// uart_send(data*, len)
// V5+ 52 01 89 (42 01 06)
// V5- 52 01 88 (42 01 05)

byte xnacc_buffer[64];
byte xnacc_buffer_size;

byte xnacc_req_ok = true;
byte xnacc_req_addr = 0;

void xnacc_uart_parse_acessoryinputs(uint8_t data1, uint8_t data2);

byte xnacc_feedback_request(word num)
{
  byte addr_hi;
  byte addr_lo;

  num--;
  addr_hi = (num) >> 2;
  addr_lo = ((num) >> 1) & 1;
  xnmsg[0] = 0x42;
  xnmsg[1] = addr_hi;
  xnmsg[2] = addr_lo | 0x80;
  xnacc_req_ok = false;
  xnacc_req_addr = num;
  xns_send();
  return 0;
  //return uart_send(xnmsg, 4);
}

byte xnacc_trackpower_request(void)
{
  xnmsg[0] = 0x21;
  xnmsg[1] = 0x24;
  xnacc_req_ok = false;
  xnacc_req_addr = 255;
  xns_send();
  return 0;
}


void xnacc_turnout_action(word num, byte state)
{
  byte addr_hi;
  byte addr_lo;
  
  num--;
  
  // if track power on, send request to turnout action
  if (xnacc_poweron) {
    addr_hi = (num) >> 2;
    addr_lo = ((num) & 0x03) << 1;
    xnmsg[0] = 0x52;
    xnmsg[1] = addr_hi;
    xnmsg[2] = addr_lo | ((state & 1)) | 0x88; // press
    xns_send();
    xnmsg[2] = addr_lo | ((state & 1)) | 0x80; // release
    xns_send();
  }
}

void xnacc_init(void)
{
  uart_on_receive = &xnacc_uart_on_receive;
  uart_on_sniff = &xnacc_uart_on_sniff;
  uart_on_addressed = &xnacc_uart_on_addressed;
  uart_on_addressed_stopped = &xnacc_uart_on_addressed_stopped;
}

void xnacc_uart_on_sniff(uint8_t recipient, uint8_t data[], uint8_t size)
{
  //if (recipient == 0) { // general anouncment
}

void xnacc_uart_on_receive(uint8_t recipient, uint8_t data[], uint8_t size)
{
  byte i;
  for(i=0; i<size; i++) {
    xnacc_buffer[i] = data[i];
  }
  xnacc_buffer_size = size;
  xnacc_datatoparse = true;
}

void xnacc_uart_parse_buffer(void)
{
  // power status announcement
  if (xnacc_buffer_size == 3) {
    if ((xnacc_buffer[0] == 0x61) && (xnacc_buffer[1] == 0x00)) { // aus
      xnacc_poweron = 0;
    }
    if ((xnacc_buffer[0] == 0x61) && (xnacc_buffer[1] == 0x01)) { // go
      xnacc_poweron = 1;
    }
    if ((xnacc_buffer[0] == 0x81) && (xnacc_buffer[1] == 0x00)) { // estop
      xnacc_poweron = 0;
    }
    if ((xnacc_buffer[0] == 0x61) && (xnacc_buffer[1] == 0x02)) { // service mode
      xnacc_poweron = 0;
    }
    if ((xnacc_buffer[0] == 0x61) && (xnacc_buffer[1] == 0x81)) { // busy
      xns_busy();
    }
  }

  // read track power status (prog read as off state)
  byte bi;
  if (xnacc_buffer_size == 4) {
    if (xnacc_buffer[0] == 0x62) {
      if (xnacc_buffer[1] == 0x22) {
        // indicate successful reception if requested
        if (!xnacc_req_ok) {
          if (xnacc_req_addr == 255) {
            xnacc_req_ok = true;
          }
        }
        // read track status
        bi = xnacc_buffer[2] & 0x4B;
        if (bi == 0) {
          xnacc_poweron = 1;
         } else {
          xnacc_poweron = 0;
        }
      }
    }
  }

  word num;
  // catch acc data
  if (xnacc_buffer[0] == 0x42) {
    //acessory information
    // parse all posible data
    num = 1;
    xnacc_buffer_size -= 2; // korekce pro while podmínku
    while (num < xnacc_buffer_size) {
        xnacc_uart_parse_acessoryinputs(xnacc_buffer[num], xnacc_buffer[num+1]);
        num += 2; 
    }
  }
}

void xnacc_uart_parse_acessoryinputs(uint8_t data1, uint8_t data2)
{
  word num; // decoded number (external address)
  // load actual state from RS packet
  num = (data1 << 1) | ((data2 & 0b00010000) >> 4); // get module address
  // indicate successful reception if requested
  if (!xnacc_req_ok) {
    if (xnacc_req_addr == num) {
      xnacc_req_ok = true;
    }
  }
  switch (data2 & 0x03) { // low nibble
    case 1: // -
      xnacc_turnout_state[num*2  ] = 2;
      break;
    case 2: // +
      xnacc_turnout_state[num*2  ] = 1;
      break;
    default:
      xnacc_turnout_state[num*2  ] = 0;
  }
  switch ((data2 & 0x0C) >> 2) { // high nibble
    case 1: // -
      xnacc_turnout_state[num*2+1] = 2;
      break;
    case 2: // +
      xnacc_turnout_state[num*2+1] = 1;
      break;
    default:
      xnacc_turnout_state[num*2+1] = 0;
  }
}

void xnacc_uart_on_addressed(void)
{
  // command center is available
  xnacc_trackpower_request();
  xnacc_ccavail = true;
}

void xnacc_uart_on_addressed_stopped(void)
{
  // command center is not available
  xnacc_ccavail = false;
}

void xnacc_loop(void)
{
  if (xnacc_datatoparse) {
    xnacc_datatoparse = false;
    xnacc_uart_parse_buffer();
  }
}
