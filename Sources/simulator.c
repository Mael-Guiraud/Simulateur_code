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
	temp.reserved_for = ring[size-1].reserved_for;
	temp.pre_reserved = ring[size-1].pre_reserved;
	for(int i=size-1;i>0;i--)
	{
		ring[i].owner = ring[i-1].owner;
		ring[i].nb_CRAN = ring[i-1].nb_CRAN;
		ring[i].reserved_for = ring[i-1].reserved_for;
		ring[i].pre_reserved = ring[i-1].pre_reserved;
	}
	ring[0].owner = temp.owner;
	ring[0].nb_CRAN = temp.nb_CRAN;
	ring[0].reserved_for = temp.reserved_for;
	ring[0].pre_reserved = temp.pre_reserved;
}

int** init_nodes_positions(int nb_nodes,int size)
{
	
	int** nodes_positions ;
	assert(nodes_positions = (int**)malloc(sizeof(int*)*nb_nodes));
	int link_size = (size/nb_nodes);
	//Allocation
	for(int i=0;i<nb_nodes;i++)
	{
		assert(nodes_positions[i] = (int*)malloc(sizeof(int)*nb_nodes));	
	}	
	//Init the values
	for(int i=0;i<nb_nodes-1;i++)
	{
		nodes_positions[i][i] = 0;	
		nodes_positions[i][0] = size-( i * link_size);
		nodes_positions[0][i] = i * link_size;
	}
	//The last arc is longer or equal
	nodes_positions[nb_nodes-1][0] = size-( (nb_nodes-1) * link_size);
	nodes_positions[0][nb_nodes-1] = (nb_nodes-1) * link_size;
	return nodes_positions;
}

Packet* init_ring(int size)
{
	Packet* ring ;
	assert(ring = (Packet*)malloc(sizeof(Packet)*size));
	for(int i=0;i<size;i++)
	{
		ring[i].owner = -1;
		ring[i].nb_CRAN = 0;
		ring[i].reserved_for = -1;
		ring[i].pre_reserved = -1;
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
void free_nodes_positions(int ** nodes, int nb_nodes)
{
	for(int i=0;i<nb_nodes;i++)
	{
		free(nodes[i]);
		
	}
	free(nodes);
}
void aff_queues(Queue* BE_Q,Queue* CRAN_Q, int nb_nodes)
{
	for(int i=0;i<nb_nodes;i++)
	{
		printf("Node %d : BE[%d],CRAN[%d]\n",i,BE_Q[i].size,CRAN_Q[i].size);
	}
}
int* init_nb_antenas(int nb_nodes,int nb_antenas)
{
	int* antenas_distrib ;
	assert(antenas_distrib=(int*)malloc(sizeof(int)*nb_nodes));
	for(int i=0;i<nb_nodes;i++)antenas_distrib[i]=0;
	for(int i=1;i<nb_nodes;i++)
	{
		for(int j=0;j<(nb_antenas/(nb_nodes-1));j++)
		{
			antenas_distrib[i]++;
		}
	}
	for(int i=1;i<(nb_antenas%(nb_nodes-1) +1 );i++)
	{
		antenas_distrib[i]++;
	}
	return antenas_distrib;

}

int max_chain_length(Packet* ring, int ring_size)
{
	int cmpt=0;
	int max = 0;
	int i;
	for(i=0;i<ring_size;i++)
	{
		if((ring[i].owner == -1) && (ring[i].reserved_for == -1))
			cmpt = 0;
		else
			cmpt++;
		if(cmpt > max)
			max = cmpt;
	}
	i=0;
	if(cmpt > 0) //if the last slot is used
	{

		while((ring[i].owner != -1) || (ring[i].reserved_for != -1))
		{
			//printf("%d %d %d\n",i,ring[i].owner,ring[i].reserved_for);
			cmpt++;
			i++;
			if(cmpt > max)
				max = cmpt;
			if(i==ring_size)
				return ring_size;
		}
	}
	return max;
}

int average_chain_length(Packet* ring, int ring_size)
{
	int begin = -1;
	int cmpt = 0;
	int average = 0;
	int nb_chains = 0;
	for(int i=0;i<ring_size;i++)
	{
		if((ring[i].owner == -1) && (ring[i].reserved_for == -1))
		{
			begin = i;
			break;
		}
	}
	if(begin == -1)
		return ring_size;
	for(int i=begin;i<ring_size+begin;i++)
	{
		//printf("%d,%d[%d][%d] %d %d\n",i,begin,ring[i%ring_size].owner,ring[i%ring_size].reserved_for,cmpt,nb_chains);
		if((ring[i%ring_size].owner == -1) && (ring[i%ring_size].reserved_for == -1))
		{
			average += cmpt;
			cmpt = 0;
		}
		else
 		{

 			if(cmpt == 0)
 			{
 				nb_chains++;
 			}
			cmpt++;
		}
	}
	if((ring[ring_size+begin-1].owner != -1) || (ring[ring_size+begin-1].reserved_for != -1))
	{
		average+=cmpt; 
		nb_chains++;
	}
	if (nb_chains == 0)return 0;
	return average / nb_chains;
}
void print_ring(Packet* ring, int ring_size)
{
	for(int i=0;i<ring_size;i++)
	{
		printf("[%d,%d,%d] ",i,ring[i].owner,ring[i].reserved_for);
	}
	printf("\n");
}

float simulate(int ring_size, int nb_nodes,int nb_antenas, int period,int minimal_buffer_size,int deadline,int nb_BBU,int size_CRAN,int size_BE,int packet_size, int emission_time, int emission_gap,Policy mode, int simulation_lenght,int time_before_measure, int max_size,float * tab_BE,float* tab_CRAN,float* tab_ANSWERS,float * tab_BE_BBU, int tab_size , float *** vectors,float ** chain, int* state, int res_kind)
{

	Packet* ring = init_ring(ring_size);
	int ** nodes_positions = init_nodes_positions(nb_nodes,ring_size);
	int* antenas_distrib = init_nb_antenas(nb_nodes,nb_antenas);

	FILE* train_long_file = fopen("../datas/train.data","w");
	if(!train_long_file){perror("Opening \"../datas/train.data\" failure\n");exit(2);}

	int ** shift = NULL;
	if(mode == SPLIT_FREQ)
	{
		assert(shift = (int**)malloc(sizeof(int*)*nb_nodes));		
		for(int i=0;i<nb_nodes;i++)
			assert(shift[i] = (int*)malloc(sizeof(int)*antenas_distrib[i]));		
	}
	int** nodes_antenas = init_nodes_antenas(nb_nodes,nb_antenas,period,nb_BBU,mode,nodes_positions,antenas_distrib,res_kind,emission_gap,emission_time,ring_size,shift);
	Queue * BE_Q = init_nodes_queues(nb_nodes, max_size);
	Queue * CRAN_Q = init_nodes_queues(nb_nodes, max_size);


	float load =0.0;
	for(int current_slot=0;current_slot<simulation_lenght;current_slot++)
	{
		
		//aff_queues(BE_Q, CRAN_Q,nb_nodes);
		//printf("\n");
		if(current_slot > 10*ring_size)
		{
			generation_BE(BE_Q,nb_nodes,size_BE,current_slot,max_size,vectors,chain,state);
		}
		switch(mode)
		{
			case NO_MANAGMENT:
				generation_CRAN(BE_Q,nodes_antenas,nb_nodes,nb_antenas,current_slot,size_CRAN,size_BE,nb_BBU,period,max_size,emission_time,emission_gap,antenas_distrib,mode,nodes_positions,ring_size,shift);
				generation_answers(ring,nodes_positions,BE_Q,nb_BBU,ring_size,current_slot, size_CRAN,size_BE,mode,max_size);
				load += (float)insert_packets(BE_Q,BE_Q,ring,nodes_positions,packet_size,minimal_buffer_size,deadline,mode,nb_nodes,size_CRAN,size_BE,max_size,current_slot,nb_BBU,tab_BE,tab_CRAN,tab_ANSWERS,tab_BE_BBU, time_before_measure,tab_size);
				
			break;
			default:
				generation_CRAN(CRAN_Q,nodes_antenas,nb_nodes,nb_antenas,current_slot,size_CRAN,size_BE,nb_BBU,period,max_size,emission_time,emission_gap,antenas_distrib,mode,nodes_positions,ring_size,shift);
				generation_answers(ring,nodes_positions,CRAN_Q,nb_BBU,ring_size,current_slot, size_CRAN,size_BE,mode,max_size);
				if(mode != CRAN_FIRST)
					reservation_management(ring, ring_size, nodes_antenas, nodes_positions,nb_nodes, current_slot,nb_BBU, period, emission_time, emission_gap, antenas_distrib,mode,shift);
				load += (float)insert_packets(BE_Q,CRAN_Q,ring,nodes_positions,packet_size,minimal_buffer_size,deadline,mode,nb_nodes,size_CRAN,size_BE,max_size,current_slot,nb_BBU,tab_BE,tab_CRAN,tab_ANSWERS,tab_BE_BBU ,time_before_measure,tab_size);
			break;

			
		}
		
		remove_packets(nodes_positions,ring,nb_nodes,ring_size);
		fprintf(train_long_file,"%d %d %d\n",current_slot,max_chain_length(ring,ring_size),average_chain_length(ring,ring_size));
		//fprintf(stdout,"%d %d %d\n",current_slot,max_chain_length(ring,ring_size),average_chain_length(ring,ring_size));
		//print_ring(ring, ring_size);
		rotate_ring(ring,ring_size);
		//if(current_slot == 200)exit(40);


	}
	if(DEBUG)printf("\n Real Load = %f\n",load/simulation_lenght);

	fclose(train_long_file);
	if(shift)free(shift);
	free(ring);
	free(antenas_distrib);
	free_nodes_positions(nodes_positions,nb_nodes);
	free_nodes_antenas(nodes_antenas,nb_nodes,nb_BBU);
	free_nodes_queues(BE_Q, nb_nodes);
	free_nodes_queues(CRAN_Q, nb_nodes);

	return load/simulation_lenght;
}