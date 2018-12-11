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

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
  printf("broadcast message received from %d.%d:\n", from->u8[0], from->u8[1]);

  struct node_info *store = packetbuf_dataptr();

  struct node_info packet;

  packet.sequence_number = store->sequence_number;
  packet.hop = store->hop;
  packet.wipe_node = store->wipe_node;

  printf("Seq: %d, Hop: %d", packet.sequence_number, packet.hop);

  linkaddr_t destination;

}

 //lets collector know which function to do when broadcast is received
 static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
 static struct broadcast_conn broadcast;

PROCESS_THREAD(example_unicast_process, ev, data){

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

  //Opens Broad and Unicast on ports 146 and 140
  broadcast_open(&broadcast, 146, &broadcast_call);

  while(1) {

  }

  //Ends Process Thread
  PROCESS_END();
}