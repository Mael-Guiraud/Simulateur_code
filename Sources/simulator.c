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

int * init_nodes_queues(int nb_nodes)
{
	int* nodes_queues ;
	assert(nodes_queues = (int*)malloc(sizeof(int)*nb_nodes));
	for(int i=0;i<nb_nodes;i++)
	{
		nodes_queues[i] = 0;
	}
	return nodes_queues;
}
void aff_queues(int* BE_Q,int* CRAN_Q, int nb_nodes)
{
	for(int i=0;i<nb_nodes;i++)
	{
		printf("Node %d : BE[%d],CRAN[%d]\n",i,BE_Q[i],CRAN_Q[i]);
	}
}

void simulate(int ring_size, int nb_nodes,int nb_antenas, int period, float burst_proba,float lambda_burst,float lambda_regular,int minimal_buffer_size,int nb_BBU,int size_CRAN,int size_BE,int packet_size, Policy mode, int simulation_lenght)
{
	Packet* ring = init_ring(ring_size);
	int * nodes_positions = init_nodes_positions(nb_nodes,ring_size);
	int** nodes_antenas = init_nodes_antenas(nb_nodes,nb_antenas,period,nb_BBU);
	int * BE_Q = init_nodes_queues(nb_nodes);
	int * CRAN_Q = init_nodes_queues(nb_nodes);

	for(int i=0;i<simulation_lenght;i++)
	{
		if(DEBUG)aff_queues(BE_Q, CRAN_Q,nb_nodes);
		generation_BE(BE_Q,nb_nodes,burst_proba,lambda_burst,lambda_regular,size_BE);
		generation_CRAN(CRAN_Q,nodes_antenas,nb_nodes,nb_antenas,i,size_CRAN,nb_BBU,period);
		generation_answers(ring,nodes_positions,CRAN_Q,nb_BBU,ring_size);
		insert_packets(BE_Q,CRAN_Q,ring,nodes_positions,packet_size,minimal_buffer_size,mode,nb_nodes);
		remove_packets(nodes_positions,ring,nb_nodes,ring_size);
		rotate_ring(ring,ring_size);
	}

	free(ring);
	free(nodes_positions);
	free_nodes_antenas(nodes_antenas,nb_nodes,nb_BBU);
	free(BE_Q);
	free(CRAN_Q);
}