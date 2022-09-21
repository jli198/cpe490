// Protocol 3 (par) allows unidirectional data flow over unreliable channel
// positive ackowledgment with retransmission protocol

#define MAX_SEQ 1 // must be 1 for protocol 3
typedef enum {frame_arrival, cksum_err, timeout} event_type;
#include "protocol.h"

void sender3(void) {
  seq_nr next_frame_to_send; // seq number of next outgoing frame
  frame s; // scratch variable
  packet buffer; // buffer for outbound packet
  event_type event;

  next_frame_to_send = 0; // initialize outbound sequence numbers
  from_network_layer(&buffer); // fetch first packet
  while (true) {
    s.info = buffer; // construct frame for transmission
    s.seq = next_frame_to_send; // insert sequence number in frame
    to_physical_layer(&s); // sends it on its way
    start_timer(s.seq); // if answer takes too long, time out
    wait_for_event(&event); // frame_arrival, cksum_err, timeout
    if (event == frame_arrival) {
      from_physical_layer(&s); // get acknowledgement
      if(s.ack == next_frame_to_send) {
        stop_timer(s.ack); // turn timer off
        from_network_layer(&buffer); // give next one to send
        inc(next_frame_to_send); // invert next frame to send
      }
    }
  }
}

void receiver3(void) {
  seq_nr frame_expected;
  frame r, s;
  event_type event;

  frame_expected = 0;
  while (true) {
    wait_for_event(&event); // possibilities: frame_arrival, cksum_err
    if(event == frame_arrival) { // valid frame has arrived
      from_physical_layer(&r); // go get newly arrived frame
      if (r.seq == frame expected) { // this is what have been waiting for
        to_network_layer(&r.info); // pass data to network layer
        inc(frame_expected); // next time expect other sequence nr
      }
      s.ack = 1 - frame_expected; // tell which frame is being acked
      to_physical_layer(&s); // send acknowledgement
    }
  }
}
// positive acknowledge with retransmission protocol