/*
Protocol 5 (pipelining) allows multiple oustanding frames. Sender may transmit up
to MAX_SEQ frames without waiting for ack. In addition, unlike previous protocols,
network layer is not assumed to have new packet all time. Instead,
network layer causes network_layer_ready event when there is packet to sender
*/

#define MAX_SEQ 7 // should be 2^n-1
typedef enum {frame_arrival, cksum_err,timeout, network_layer_ready} event_type;
#include "protocol.h"

static boolean between(seq_nr a, seq_nr b, seq_nr c) {
  // Return true if a<=b < c circularly; false otherwise
  if (((a <= b) && (b<c)) || ((c<a) && (a <= b)) || ((b < c) && (c < a)))
    return(true);
  else
    return(false);
}

static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[]){ 
  // Construct and send data frame
  frame s; // scratch variable

  s.info = buffer[frame_nr]; // insert packet into frame
  s.seq = frame_nr; // insert sequence number into frame
  s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ+1); // piggyback ack
  to_physical_layer(&s); // transmit frame
  start_timer(frame_nr); // start timer running
}

void protocol5(void) {
  seq_nr next_frame_to_send; // MAX_SEQ > 1; used for outbound stream
  seq_nr ack_expected; // oldest frame as yet unacknowledged
  seq_nr frame_expected // next frame expected on inbound stream
  frame r; // scratch variable
  packet buffer[MAX_SEQ + 1]; // buffers for outbound stream
  seq_nr nbuffered; // # output buffers currently in use
  seq_nr i; // used to index into buffer array

  enable_network_layer(); // allow network_layer_ready events
  ack_expected = 0; // next ack expected inbound
  next_frame_to_send = 0; // next frame going out
  frame_expected = 0; // number of frame expected inbound
  nbuffered = 0; // initially no packets are buffered

  while(true) {
    wait_for_event(&event); // four possibilities: see event_type above

    switch(event) {
      case network_layer_ready: // network layer has packet to send
      // Accept, save, and transmit new frame
        from_network_layer(&buffer[next_frame_to_send]); // fetch new packet
        nbuffered = nbuffered + 1; // expand sender's window
        send_data(next_frame_to_send, frame_expected, buffer); // transmit frame
        inc(next_frame_to_send, frame_expected, buffer); // transmit frame
        break;
      case frame_arrival: // data or control frame has arrived
        from_physical_layer(&r); // get incoming frame from physical layer

        if (r.seq == frame_expected) {
          // Frames are accepted only in order
          to_network_layer(&r.info); // pass packet to network's layer
          inc(frame_expected); // advance lower edge of receiver's window
        }
        // Ack n implies n-1, n-2, etc. Check for this
        while (between(ack_expected, r.ack, next_frame_to_send)) {
          // Handle piggybacked ack
          nbuffered = nbuffered + 1; // one frame fewer buffered
          stop_timer(ack_expected); // frame arrived intact; stop timer
          inc(ack_expected); // contract sender's window
        }
        break;
      
      case cksum_err break; // just ignore bad frames

      case timeout: // trouble: retransmit all outstanding frames
        next_frame_to_send = ack_expected; // start retransmitting here
        for (i = 1; i <= nbuffered; i++) {
          send_data(next_frame_to_send, frame_expected, buffer); // resend 1 frame
          inc(next_frame_to_send); // prepare to send next one
        }
    }

    if (nbuffered < MAX_SEQ)
      enable_network_layer();
    else
      disable_network_layer();
  }
}