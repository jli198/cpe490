#define MAX_PKT 1024 // determines packet size in bytes

typedef enum {false, true} boolean; // boolean type
typedef unsigned int seq_nr; // sequence or ack numbers
typedef struct {unsigned char data[MAX_PKT];} packet; // packet definition
typedef enum {data, ack, nak} frame_kind; // frame_kind definition

typedef struc { // frames are transported in this layer
  frame_kind kind; // what kind of frame is it?
  seq_nr seq; // sequence number
  seq_nr ack; // acknowledgement number
  packet info; // network layer packet
} frame;

/* 
Some definitions needed in protocols to follow.
These are located in file protocol.h 
*/

// Wait for event to happen; return its type in event
void wait_for_event(event_type *event);

// Fetch packet from network layer for transmission on channel
void from_network_layer(packet *p);

// Deliver information from inbound frame to network layer
void to_network_layer(packet *p);

// Go get inbound frame from physical layer and copy it to r
void from_physical_layer(frame *r);

// Pass frame to physical layer for transmission
void to_physical_layer(frame *s);

// Start clock running and enable timeout event
void start_timer(seq_nr k);

// Stop clock and disable timeout event
void stop_timer(seq_nr k);

// Start auxilary timer and enable ack_timeout event
void start_ack_timer(void);

// Stop auxiliary timer and disable ack_timeout event
void stop_ack_timer(void);

// Allow network layer to cause network_layer_ready event
void enable_network_layer(void);

// Forbid network layer from causing network_layer_ready event
void disable_network_layer(void);

// Macro inc is expanded in-line: Increment k circularly
#define inc(k) 
if (k < MAX_SEQ)
  k = k + 1;
else
  k = 0;