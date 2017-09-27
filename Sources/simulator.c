#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "struct.h"

#include "message_generation.h"



void rotate_ring(Packet * ring, int size)
{
	if(DEBUG)printf("Rotation of the ring .\n");
	Packet temp;
	temp.owner = ring[size-1].owner;
	temp.nb_CRAN = ring[size-1].nb_CRAN;
	for(int i=size-1;i>0;i--)
	{
		ring[i].owner = ring[i-1].owner;
		ring[i].nb_CRAN = ring[i-1].nb_CRAN;
	}
	ring[0].owner = temp.owner;
	ring[0].nb_CRAN = temp.nb_CRAN;
}

int* init_nodes_positions(int nb_nodes,int size)
{
	
	int* nodes_positions ;
	assert(nodes_positions = (int*)malloc(sizeof(int)*nb_nodes));
	for(int i=0;i<nb_nodes;i++)
	{
		nodes_positions[i] = i*size/nb_nodes;
	}
	return nodes_positions;
}

Packet* init_ring(int size)
{
	Packet* ring ;
	assert(ring = (Packet*)malloc(sizeof(Packet)*size));
	for(int i=0;i<size;i++)
	{
		ring[i].owner = -1;
		ring[i].nb_CRAN = -1;
	}
	return ring;
}

Queue * init_nodes_queues(int nb_nodes, int max_size)
{
	Queue* nodes_queues ;
	assert(nodes_queues = (Queue*)malloc(sizeof(Queue)*nb_nodes));
	for(int i=0;i<nb_nodes;i++)
	{
		nodes_queues[i].size = 0;
		nodes_queues[i].queue = (int*)malloc(sizeof(int)*max_size);
		nodes_queues[i].kind = (int*)malloc(sizeof(int)*max_size);
		nodes_queues[i].min_id = 0;
		nodes_queues[i].max_id = 0;
	}
	return nodes_queues;
}
void free_nodes_queues(Queue * nodes_queues, int nb_nodes)
{
	for(int i=0;i<nb_nodes;i++)
	{
		free(nodes_queues[i].queue);
		free(nodes_queues[i].kind);
	}
	free(nodes_queues);
}
void aff_queues(Queue* BE_Q,Queue* CRAN_Q, int nb_nodes)
{
	for(int i=0;i<nb_nodes;i++)
	{
		printf("Node %d : BE[%d],CRAN[%d]\n",i,BE_Q[i].size,CRAN_Q[i].size);
	}
}

void simulate(int ring_size, int nb_nodes,int nb_antenas, int period,int minimal_buffer_size,int nb_BBU,int size_CRAN,int size_BE,int packet_size, int emission_time, int emission_gap,Policy mode, int simulation_lenght,int time_before_measure, int max_size,float * tab_BE,float* tab_CRAN,float* tab_ANSWERS,float * tab_BE_BBU, int tab_size , float *** vectors,float ** chain, int* state)
{
	Packet* ring = init_ring(ring_size);
	int * nodes_positions = init_nodes_positions(nb_nodes,ring_size);
	int** nodes_antenas = init_nodes_antenas(nb_nodes,nb_antenas,period,nb_BBU);
	Queue * BE_Q = init_nodes_queues(nb_nodes, max_size);
	Queue * CRAN_Q = init_nodes_queues(nb_nodes, max_size);


	float load =0.0;
	for(int current_slot=0;current_slot<simulation_lenght;current_slot++)
	{
		
		if(DEBUG)aff_queues(BE_Q, CRAN_Q,nb_nodes);

		generation_BE(BE_Q,nb_nodes,size_BE,current_slot,max_size,vectors,chain,state);
		switch(mode)
		{
			case CRAN_FIRST:
				generation_CRAN(CRAN_Q,nodes_antenas,nb_nodes,nb_antenas,current_slot,size_CRAN,nb_BBU,period,max_size,emission_time,emission_time);
				generation_answers(ring,nodes_positions,CRAN_Q,nb_BBU,ring_size,current_slot, size_CRAN,max_size);
				load += (float)insert_packets(BE_Q,CRAN_Q,ring,nodes_positions,packet_size,minimal_buffer_size,mode,nb_nodes,size_CRAN,size_BE,max_size,current_slot,nb_BBU,tab_BE,tab_CRAN,tab_ANSWERS,tab_BE_BBU ,time_before_measure,tab_size);
			break;
			default:
				generation_CRAN(BE_Q,nodes_antenas,nb_nodes,nb_antenas,current_slot,size_CRAN,nb_BBU,period,max_size,emission_time,emission_gap);
				generation_answers(ring,nodes_positions,BE_Q,nb_BBU,ring_size,current_slot, size_CRAN,max_size);
				load += (float)insert_packets(BE_Q,BE_Q,ring,nodes_positions,packet_size,minimal_buffer_size,mode,nb_nodes,size_CRAN,size_BE,max_size,current_slot,nb_BBU,tab_BE,tab_CRAN,tab_ANSWERS,tab_BE_BBU, time_before_measure,tab_size);
				
			break;
		}
		
		remove_packets(nodes_positions,ring,nb_nodes,ring_size);
		rotate_ring(ring,ring_size);
	}
	if(DEBUG)printf("\n Real Load = %f\n",load/simulation_lenght);

	free(ring);
	free(nodes_positions);
	free_nodes_antenas(nodes_antenas,nb_nodes,nb_BBU);
	free_nodes_queues(BE_Q, nb_nodes);
	free_nodes_queues(CRAN_Q, nb_nodes);
}