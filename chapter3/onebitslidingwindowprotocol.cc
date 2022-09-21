// Protocol 4 (sliding winter) is bidirectional
#define MAX_SEQ 1 // must be 1 for protocol 4
typedef enum{frame_arrival, cksum_err, timeout} event_type;
#include "protocol.h"

void protocol4 (void) {
  seq_nr next_frame_to_send; // 0 or 1 only
  seq_nr frame_expected; // 0 or 1 only
  frame r, s; // scratch variables
  packet buffer; // current packet being sent
  event_type event;

  next_frame_to_send = 0; // next frame on outbound stream
  frame_expected = 0; // frame expected next
  from_network_layer(&buffer); // fetch packet from network layer
  s.info = buffer; // prepare to send initial frame
  s.seq = next_frame_to_send; // insert sequence number into frame
  s.ack = 1 - frame_expected; // piggybacked ack
  to_physical_layer(&s); // transmit frame
  start_timer(s.seq); // start timer running
  while (true) {
    wait_for_event(&event); // frame_arrival, cksum_err, or timeout
    if (event == frame_arrival) { // frame has arrived undamaged
      from_physical_layer(&r); // go get it

      if (r.seq == frame_expected) { // handle inbound frame stream
        to_network_layer(&r.info); // pass packet to network layer
        inc(frame_expected); // invert seq number expected next
      }
      if (r.ack == next_frame_to_send) { // handle outbound frame stream
        stop_timer(r.ack); // turn timer off
        from_network_layer(&buffer); // fetch new pkt from network layer
        inc(next_frame_to_send); // invert senderis sequence number
      }
    }
    s.info = buffer; // construct outbound frame
    s.seq = next_frame_to_send; // insert sequence number into it
    s.ack = 1 - frame_expected; // seq number of last received frame
    to_physical_layer(&s); // transmit frame
    start_timer(s.seq); // start timer running
  }
}