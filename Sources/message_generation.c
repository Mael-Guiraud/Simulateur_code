#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


#include "struct.h"

int poisson_distribution(float random_drawed, float lambda)
{
	double proba = exp (- lambda);
	int x = 0;
	double cumul = proba;
	while (random_drawed > cumul)
	{
		x++;
		proba *=lambda/x;
		cumul += proba;
	} 
	if(DEBUG)printf("Poisson distribution return value :%d\n",x);
	return x;
}
int hyper_expo(float burst_proba, float lambda_burst,float lambda_regular)
{
	float random = ((float)rand() / RAND_MAX );
	if(DEBUG)printf("Random for hyper_expo:%f\n",random);
	if (random < burst_proba)
	{
		random = ((float)rand() / RAND_MAX );
		if(DEBUG)printf("Burst generation.\n");
		return poisson_distribution(random,lambda_burst);
	}
	else											//lambda petit
	{
		random = ((float)rand() / RAND_MAX );
		if(DEBUG)printf("Normal flow generation.\n");
		return poisson_distribution(random,lambda_regular);
	}
}

//Init the sending slot of the antenas for a node
void init_CRAN(int* antenas,int period, int nb_antenas)
{
	for(int i=0;i<nb_antenas;i++)
	{
		antenas[i]= rand()%period;
		if(DEBUG)printf("Generation of a random offset(%d) for antena %d.\n",antenas[i],i);
	}
}

int ** init_nodes_antenas(int nb_nodes, int nb_antenas, int period, int nb_BBU)
{
	int ** nodes;
	assert(nodes = (int **)malloc(sizeof(int*)*nb_nodes));

	for(int i=nb_BBU;i<nb_nodes;i++)
	{
		if(DEBUG)printf("Antenas generation at node %d:\n",i);
		assert(nodes[i]=(int*)malloc(sizeof(int)*nb_antenas));
		init_CRAN(nodes[i],period,nb_antenas);
	}
	return nodes;
}

void free_nodes_antenas(int ** nodes, int nb_nodes,int nb_BBU)
{
	for(int i=nb_BBU;i<nb_nodes;i++)
		free(nodes[i]);
	free(nodes);
}

void generation_BE(int * BE_Q, int nb_nodes,float burst_proba, float lambda_burst, float lambda_regular,int size_BE)
{
	for(int i=0;i<nb_nodes;i++)
	{
		if(DEBUG)printf("BE messages generation at node %d.\n",i);
		BE_Q[i] += (hyper_expo(burst_proba,lambda_burst,lambda_regular)*size_BE); 
	}
}

void generation_CRAN(int* CRAN_Q,int** nodes_antenas, int nb_nodes, int nb_antenas, int current_slot, int size_CRAN,int nb_BBU, int period)
{
	for(int i=nb_BBU;i<nb_nodes;i++)
	{
		for(int j=0;j<nb_antenas;j++)
		{
			if(nodes_antenas[i][j] == current_slot%period)
			{
				if(DEBUG)printf("CRAN sending by antena %d(node %d).\n",j,i);
				CRAN_Q[i] += size_CRAN;
			}
		}
	}
}

void generation_answers(Packet* ring, int* nodes_positions, int* CRAN_Q, int nb_BBU, int ring_size)
{
	int reading_slot;
	for(int i=0;i<nb_BBU;i++)
	{
		reading_slot = (ring_size+nodes_positions[i]-1)%ring_size;
		if(ring[reading_slot].owner != i )
		{
			if(ring[reading_slot].nb_CRAN > 0)
			{
				if(DEBUG)printf("Answer generation to BBU %d.\n",i);
				CRAN_Q[i] += ring[reading_slot].nb_CRAN;
			}
		}
	}
}
void insert_packets(int* BE_Q, int * CRAN_Q, Packet* ring, int* nodes_positions,int packet_size, int minimal_buffer_size, Policy mode, int nb_nodes)
{
	int writing_Slot;
	for(int i=0;i<nb_nodes;i++)
	{
		writing_Slot = nodes_positions[i];
		if(ring[writing_Slot].owner == -1) //IF the slot is free
		{
			switch(mode)
			{
				case CRAN_FIRST:
					if(CRAN_Q[i]>0)//is there is some CRAN
					{
						if(DEBUG)printf("CRAN packet at node %d",i);
						ring[writing_Slot].owner = i;
						ring[writing_Slot].nb_CRAN = CRAN_Q[i];
						if(CRAN_Q[i]<= packet_size) //we empty all possible CRAN
						{
							if(BE_Q[i] > 0)
							{
								if(DEBUG)printf(" with best effort");
								BE_Q[i] -= (packet_size - CRAN_Q[i]);
							}
							CRAN_Q[i] = 0;
						}
						else
						{
							CRAN_Q[i] = packet_size - CRAN_Q[i];
						}
						if(DEBUG)printf(".\n");
					}
					if(BE_Q[i]>=minimal_buffer_size)
					{
						if(DEBUG)printf("BE packet at node %d.\n",i);
						ring[writing_Slot].owner = i;
						ring[writing_Slot].nb_CRAN = 0;
						if(CRAN_Q[i]<= packet_size)
						{
							BE_Q[i] = 0;
						}
						else
						{
							BE_Q[i] = packet_size - CRAN_Q[i];
						}
					}
				break;
				default:
					printf("This policy hasn't been implemented yet \n");
					exit(21);
				break;
			}
		}
	}
}

void remove_packets(int* nodes_positions, Packet* ring, int nb_nodes,int ring_size)
{
	int reading_slot;
	for(int i=0;i<nb_nodes;i++)
	{
		reading_slot = (ring_size+nodes_positions[i]-1)%ring_size;
		if(ring[reading_slot].owner == i)
		{
			if(DEBUG)printf("Nodes %d removes his packet\n",i);
			ring[reading_slot].owner = -1;
			ring[reading_slot].nb_CRAN = -1;
		}
	}
}
