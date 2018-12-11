//DAN Coleman Broadcast Sample 
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include <stdio.h>
#include "stdbool.h"

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

//executes when the collector gets broadcast						           
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
}

//Tells collector which function to call when it gets a broadcast
static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
static struct broadcast_conn broadcast;

struct node_info{
  int sequence_number;
  int hop;
  bool wipe_node;
};

//Runs a thread that loops a broadcast to the other nodes
PROCESS_THREAD(example_broadcast_process, ev, data)
{
   //Creates ETimer
  static struct etimer et;
   PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
   PROCESS_BEGIN();

   //Opens Broad and Unicast on ports 146 and 140
   broadcast_open(&broadcast, 146, &broadcast_call);

   struct node_info packet;
   static int sequence_counter = 0;
   int wipe_counter = 0;

   while(1) {
     // Delay for 4 seconds
     etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND*4)); //+ random_rand() % (CLOCK_SECOND * 4));

     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

     packet.hop = 0;
     packet.sequence_number = sequence_counter;

     packet.wipe_node = false;
     if(wipe_counter > 15){
       packet.wipe_node = true;
     }
          printf("Broadcast message sent\n");
     packetbuf_copyfrom(&packet, sizeof(struct node_info));
     broadcast_send(&broadcast);

     if(wipe_counter > 15){
       wipe_counter = 0;
       sequence_counter = 0;
       packet.wipe_node = false;
     }
     else{
       wipe_counter += 1;
       sequence_counter += 1;
     }

   }

   PROCESS_END();

}

/*---------------------------------------------------------------------------*/
