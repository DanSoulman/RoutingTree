//DAN Coleman unicast Sample 
//This is for the different versions of contiki
#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include "stdbool.h"

PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);

struct node_info{
  int sequence_number;
  int hop;
  bool wipe_node;
};

static int sequence_number = -1;

static struct broadcast_conn broadcast;

static linkaddr_t sender = NULL;

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
  printf("broadcast message received from %d.%d:\n", from->u8[0], from->u8[1]);

  struct node_info *store = packetbuf_dataptr();

  struct node_info packet;

  packet.sequence_number = store->sequence_number;
  packet.hop = store->hop;
  packet.wipe_node = store->wipe_node;
  packet.hop +=1 ;

  if(sender == NULL){
    sender.u8[0] = from->u8[0];
    sender.u8[1] = from->u8[1];

    sequence_number = packet.sequence_number;

    printf("Seq: %d, Hop: %d\n", packet.sequence_number, packet.hop);

    packetbuf_copyfrom(&packet, sizeof(struct node_info));
    broadcast_send(&broadcast);

  }
  else if(packet.sequence_number > sequence_number &&
   ((from->u8[0] == sender.u8[0]) && (from->u8[1] == sender.u8[1])){
     
    sequence_number = packet.sequence_number;

    printf("Seq: %d, Hop: %d\n", packet.sequence_number, packet.hop);

    packetbuf_copyfrom(&packet, sizeof(struct node_info));
    broadcast_send(&broadcast);
  }

}

 //lets collector know which function to do when broadcast is received
 static const struct broadcast_callbacks broadcast_call = { broadcast_recv };


PROCESS_THREAD(example_unicast_process, ev, data){

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

  //Opens Broad and Unicast on ports 146 and 140
  broadcast_open(&broadcast, 146, &broadcast_call);


  //Ends Process Thread
  PROCESS_END();
}