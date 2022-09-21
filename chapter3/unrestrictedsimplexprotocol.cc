#include "protocol.h"

/*
Protocol 1 (utopia) provides for data transmission in one direction only, from
sender to receiver. Communication channel is assumed to be error free,
and receiver is assumed to be able to process all input infinitely quickly.
Consequently, sender just sits in loop pumping data out onto line as
fast as it can
*/

typdef enum{frame arrival} event type;

void sender1(void) {
  frame s; // buffer for outbound frame
  packet buffer; // buffer for outbound packet

  while (true) {
    from_network_layer(&buffer); // go get something to send
    s.info = buffer; // copy it into s for transmission
    to_physical_layer(&s); // send it on its way
  }
}

/*
Tomorrow, and tomorrow, and tomorrow,
Creeps in this petty pace from day to day
To the last syllable of recorded time
- Macbeth, V, v */

void receiver1(void) {
  frame r;
  event_type event; // filled in by wait, but not used here

  while (true) {
    wait_for_event(&event); // only possibility is frame_arrival
    from_physical_layer(&r); // go get inbound frame
    to_network_layer(&r.info); // pass data to network layer
  }
}