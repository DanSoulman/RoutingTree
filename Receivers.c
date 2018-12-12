//DAN Coleman Reciever Assignment 2
#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include "stdbool.h"

//---------------------------------------------
PROCESS(example_unicast_process, "Reciever");
AUTOSTART_PROCESSES(&example_unicast_process);
//---------------------------------------------

//Struct that holds the node info
struct node_info{
  int sequence_number;
  int hop;
  bool wipe_node;
};

//Static sequence number for working out parents 
static int sequence_number = -1; 

static struct broadcast_conn broadcast;

//address of the parent node 
static linkaddr_t sender;

//Boolean to show if node has a parent 
bool parentSet = false;

//Initializes Struct with the node information named packet
static struct node_info packet;

//What to do when node recieves a broadcast
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){

  //Gets a pointer (store) to the data from the packet
  struct node_info *store = packetbuf_dataptr();
  //Set the node info (packets) information to the information from the packet (store)
  packet.sequence_number = store->sequence_number;
  packet.hop = store->hop;
  packet.wipe_node = store->wipe_node;
 //Increment hop count by one since we hopped to this node
  packet.hop +=1 ;

  //If the packet was to wipe (16th in sequence)
  if(packet.wipe_node == true){
    //Set sequence number to -1 to start again
    sequence_number = -1;
    //No Parent set for each node
    parentSet = false;
    //The address of the parent is null
    sender.u8[0] = NULL;
    sender.u8[1] = NULL;

    //Tells us this is a wiping node 
    //(known issue: The wipe doesn't seem to reach all nodes)
    printf("Wiping node");

    /*copies data from the packet struct
    the data copied is the size of node_info and sends broadcast */    
    packetbuf_copyfrom(&packet, sizeof(struct node_info));
    broadcast_send(&broadcast);
  }
  //If not wiping
  else{ 
    //And doesn't have a parent
    if(parentSet == false){
      //Sets the parents info 
      sender.u8[0] = from->u8[0];
      sender.u8[1] = from->u8[1];
      //Sequence number is packet sequence no. 
      sequence_number = packet.sequence_number;

      //Prints the information
      printf("Seq: %d, Hop: %d\n", packet.sequence_number, packet.hop);
      //Parent now set, won't enter this loop again until boolean set to false 
      parentSet = true;

      /*copies data from the packet struct
    the data copied is the size of node_info and sends broadcast */
      packetbuf_copyfrom(&packet, sizeof(struct node_info));
      broadcast_send(&broadcast);

    }
    //If not wiping anf it has a parent
    else if(packet.sequence_number > sequence_number && ((from->u8[0] == sender.u8[0]) && (from->u8[1] == sender.u8[1]))){
      //Sequence no. is number of packt sent 
      sequence_number = packet.sequence_number;
      //Prints info on packet 
      printf("Seq: %d, Hop: %d\n", packet.sequence_number, packet.hop);

    /*copies data from the packet struct
    the data copied is the size of node_info and sends broadcast */
      packetbuf_copyfrom(&packet, sizeof(struct node_info));
      broadcast_send(&broadcast);
    }
  }

}

 //lets node know which function to do when broadcast is received
 static const struct broadcast_callbacks broadcast_call = { broadcast_recv };


PROCESS_THREAD(example_unicast_process, ev, data){
  //Runs thread
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();


  //Opens Broad port 146 
  broadcast_open(&broadcast, 146, &broadcast_call);

  //Ends Process Thread
  PROCESS_END();
}