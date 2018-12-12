//DAN Coleman Beacon for Assignment 2
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include <stdio.h>
#include "stdbool.h"

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Beacon");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
}

static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
static struct broadcast_conn broadcast;

//Struct that holds the node info
struct node_info{
  int sequence_number;
  int hop;
  bool wipe_node; //If you need to wipe the tree on this cycle
};

//Runs a thread that loops a broadcast to the other nodes
PROCESS_THREAD(example_broadcast_process, ev, data)
{
   //Creates ETimer
  static struct etimer et;
   PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
   PROCESS_BEGIN();

   //Opens Broad on port 146
   broadcast_open(&broadcast, 146, &broadcast_call);

  //Creates instance of node info called packet
   struct node_info packet; 

   //Instansiates counters
   static int sequence_counter = 0;

   while(1) {
     // Delay for 4 seconds
     etimer_set(&et, CLOCK_SECOND * 4); //+ random_rand() % (CLOCK_SECOND * 4));
     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    //Sets the hop to zero to count how many hops to each node
     packet.hop = 0;
     //Sets sequence number to which sequence its on (resets after 15)
     packet.sequence_number = sequence_counter;
     //Indicates it doesn't need to wipe the system yet
     packet.wipe_node = false;

    //Once wipe limit has been reached
    if(packet.sequence_number > 15){
      //Indicates the system is to be wiped
       packet.wipe_node = true;
       printf("Wiping"); //For testing
     }

    else //Shows that broadcast has been sent out, expect the nodes to start returning info
      printf("Broadcast message sent\n"); 

    /*copies data from the packet struct
    the data copied is the size of node_info */    
    packetbuf_copyfrom(&packet, sizeof(struct node_info));
    //Sends the broadcast specified above
    broadcast_send(&broadcast);

    //If it goes past the limit (15) it wipes everything
    if(sequence_counter > 15){
      sequence_counter = 0;
      packet.wipe_node = false;
     }

    //Otherwise it increments the sequence number and starts again
    else{ 
      sequence_counter += 1;
     }

   }

   PROCESS_END();

}

/*---------------------------------------------------------------------------*/
