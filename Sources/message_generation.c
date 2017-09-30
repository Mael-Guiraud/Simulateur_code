#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


#include "struct.h"
#include "sbbp.h"

int min(int a, int b)
{
	return (a<b)?a:b;
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

void generation_BE(Queue * BE_Q, int nb_nodes,int size_BE, int current_slot, int max_size, float *** vectors,float ** chain, int* state)
{
	int number_generated;

	*state= change_state(chain, *state);
	for(int i=0;i<nb_nodes;i++)
	{
		if(DEBUG)printf("BE messages generation at node %d.\n",i);
		number_generated = sbbp_generation(vectors, *state);
		BE_Q[i].size += (number_generated*size_BE); 
		if(size_BE)
		{
			for(int j=0;j<number_generated;j++)
			{	
				BE_Q[i].queue[BE_Q[i].max_id] = current_slot; 
				BE_Q[i].kind[BE_Q[i].max_id] = 1; 
				BE_Q[i].max_id= (BE_Q[i].max_id+1)%max_size;
			}
		}

	}

}

void generation_CRAN(Queue* CRAN_Q,int** nodes_antenas, int nb_nodes, int nb_antenas, int current_slot, int size_CRAN,int nb_BBU, int period, int max_size, int emission_time, int emission_gap)
{
	for(int i=nb_BBU;i<nb_nodes;i++)
	{
		for(int j=0;j<nb_antenas;j++)
		{
			for(int k=0;k<emission_time;k+=emission_gap)
			{
				if( (nodes_antenas[i][j]+k)%period == current_slot%period)
				{
					CRAN_Q[i].size += size_CRAN;
					CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 	
					CRAN_Q[i].kind[CRAN_Q[i].max_id] = 2; 	
					CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
					if(DEBUG)printf("Creating C_RAN on queue %d at date %d (max id = %d)\n ",i,current_slot,CRAN_Q[i].max_id);
				}
			}
		}
	}
}

void generation_answers(Packet* ring, int* nodes_positions, Queue* CRAN_Q, int nb_BBU, int ring_size, int current_slot, int size_CRAN,int max_size)
{
	int reading_slot;
	for(int i=0;i<nb_BBU;i++)
	{
		reading_slot = (ring_size+nodes_positions[i]-1)%ring_size;
		if(ring[reading_slot].owner >= nb_BBU )
		{
			if(ring[reading_slot].nb_CRAN > 0)
			{
				CRAN_Q[i].size += ring[reading_slot].nb_CRAN;
				for(int j=0;j<ring[reading_slot].nb_CRAN/size_CRAN;j++)
				{
					CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 
					CRAN_Q[i].kind[CRAN_Q[i].max_id] = 3; 

					CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
					if(DEBUG)printf("Creating Answer to %d on queue %d at date %d  (max id = %d)\n ",ring[reading_slot].owner,i,current_slot,CRAN_Q[i].max_id);
				}
			}
		}
	}
}
int insert_packets(Queue* BE_Q, Queue * CRAN_Q, Packet* ring, int* nodes_positions,int packet_size, int minimal_buffer_size, Policy mode, int nb_nodes, int size_CRAN, int size_BE, int max_size,int current_slot,int nb_BBU, float* tab_BE,float * tab_CRAN, float* tab_ANSWERS,float * tab_BE_BBU,int time_before_measure,int table_Size)
{
	int writing_Slot;
	int gap;
	int inserted = 0;
	int packet_created_size;
	int cran_to_write;
	int be_to_write;

	for(int i=0;i<nb_nodes;i++)
	{
		writing_Slot = nodes_positions[i];
		packet_created_size = 0;
		if(ring[writing_Slot].owner == -1) //IF the slot is free
		{
			switch(mode)
			{
				case CRAN_FIRST:
					if(CRAN_Q[i].size>0)//if there is some CRAN
					{
						inserted++;
						if(DEBUG)printf("CRAN packet at node %d",i);
						ring[writing_Slot].owner = i;
						cran_to_write = min(packet_size,CRAN_Q[i].size);
						for(int j=0;j<cran_to_write/size_CRAN;j++)
						{
							if(current_slot>time_before_measure)
							{
								if(current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] < table_Size)
								{
									if(i<nb_BBU)
									{
										tab_ANSWERS[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;
										
									}
									else
									{
										tab_CRAN[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;
										
									}
								}
								else
								{
									printf("Warning : The table to save the datas is too short\n");
								}									
							}
							CRAN_Q[i].queue[CRAN_Q[i].min_id] = -1;
							CRAN_Q[i].min_id= (CRAN_Q[i].min_id+1)%max_size;
						}
						ring[writing_Slot].nb_CRAN = cran_to_write;
						if(BE_Q[i].size > 0)
						{
							if(DEBUG)printf(" with best effort");
							if( BE_Q[i].size >  (packet_size - cran_to_write) )
							{
								gap = packet_size - CRAN_Q[i].size;
								BE_Q[i].size -= (packet_size - cran_to_write);
							}
							else
							{
								gap = BE_Q[i].size;
								BE_Q[i].size = 0;
							}
							for(int j=0;j<gap/size_BE;j++)
							{
								if(current_slot>time_before_measure)
								{
									if(current_slot-BE_Q[i].queue[BE_Q[i].min_id] < table_Size)
									{
										if(i<nb_BBU)
										{
											tab_BE_BBU[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;

										}
										else
										{
											tab_BE[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;

										}
									}
									else
									{
										printf("Warning : The table to save the datas is too short\n");
									}
									//fprintf(f_BE,"%d\n",current_slot-BE_Q[i].queue[BE_Q[i].min_id]);
								}
								BE_Q[i].queue[BE_Q[i].min_id] = -1;
								BE_Q[i].min_id= (BE_Q[i].min_id+1)%max_size;
							}
						}
						CRAN_Q[i].size -= cran_to_write;
						if(DEBUG)printf(".\n");
					}
					else // No CRAN
					{
						if(BE_Q[i].size>=minimal_buffer_size)
						{
							inserted++;
							if(DEBUG)printf("BE packet at node %d.\n",i);
							ring[writing_Slot].owner = i;
							ring[writing_Slot].nb_CRAN = 0;
							be_to_write = min (BE_Q[i].size,packet_size);
		
							for(int j=0;j<be_to_write/size_BE;j++)
							{	

								if(current_slot>time_before_measure)	
								{
									//fprintf(f_BE,"%d\n",current_slot-BE_Q[i].queue[BE_Q[i].min_id]);
									if(current_slot-BE_Q[i].queue[BE_Q[i].min_id] < table_Size)
									{
										if(i<nb_BBU)
											tab_BE_BBU[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
										else
											tab_BE[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
									}
									else
									{
										printf("Warning : The table to save the datas is too short\n");
									}
								}
								BE_Q[i].queue[BE_Q[i].min_id] = -1;
								BE_Q[i].min_id= (BE_Q[i].min_id+1)%max_size;	
							}
							BE_Q[i].size -= be_to_write;
							
							
						}
					}
				break;
				default:
					if(CRAN_Q[i].size>=minimal_buffer_size)
					{
						inserted++;
						ring[writing_Slot].owner = i;
						packet_created_size = 0;

						while((CRAN_Q[i].size>0) && (packet_created_size <= packet_size))
						{
							//printf("***********%d-%d\n",CRAN_Q[i].kind[CRAN_Q[i].min_id],CRAN_Q[i].min_id);
							if(CRAN_Q[i].kind[CRAN_Q[i].min_id] == 1)
							{
								if(packet_created_size <= packet_size- size_BE)
								{
									if(current_slot>time_before_measure)	
									{
										//fprintf(f_BE,"%d\n",current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]);
										if(current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] < table_Size)
										{
											if(i<nb_BBU)
												tab_BE_BBU[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
											else
												tab_BE[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
										}
										else
										{
											printf("Warning : The table to save the datas is too short\n");
										}
									}
									CRAN_Q[i].queue[CRAN_Q[i].min_id] = -1;
									CRAN_Q[i].kind[CRAN_Q[i].min_id] = -1;
									CRAN_Q[i].min_id= (CRAN_Q[i].min_id+1)%max_size;
									CRAN_Q[i].size -= size_BE;
									packet_created_size += size_BE;
								}
								else
								{
									//we dont have enough place for a BE 
									break;
								}
							}
							else
							{
								if(packet_created_size <= packet_size- size_CRAN)
								{
									if(current_slot>time_before_measure)
									{
										if(CRAN_Q[i].kind[CRAN_Q[i].min_id] == 2)
										{
										//	fprintf(f_CRAN,"%d\n",current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]);
											if(current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] < table_Size)
											{
												tab_CRAN[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;
											}
											else
											{
												printf("Warning : The table to save the datas is too short\n");
											}
										}
										else
										{
											//fprintf(f_ANSWERS,"%d\n",current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]);
											if(current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] < table_Size)
											{
												tab_ANSWERS[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;

											}
											else
											{
												printf("Warning : The table to save the datas is too short\n");
											}
										}
									}	

									CRAN_Q[i].queue[CRAN_Q[i].min_id] = -1;
									CRAN_Q[i].kind[CRAN_Q[i].min_id] = -1;
									CRAN_Q[i].min_id= (CRAN_Q[i].min_id+1)%max_size;
									CRAN_Q[i].size -= size_CRAN;
									packet_created_size += size_CRAN;
									ring[writing_Slot].nb_CRAN += size_CRAN;
								}
								else
								{

									//not enough place for a CRAN
									//printf("Sending of a packet of size : %d\n", packet_created_size);
									break;
								}
							}
						}
					}
				break;
			}
		}
	}
	return inserted;
}

void remove_packets(int* nodes_positions, Packet* ring, int nb_nodes,int ring_size)
{
	int reading_slot;
	for(int i=0;i<nb_nodes;i++)
	{
		reading_slot = (ring_size+nodes_positions[i]-1)%ring_size;
		if(ring[reading_slot].owner == i)
		{
			if(DEBUG)printf("Nodes %d removes its packet\n",i);
			ring[reading_slot].owner = -1;
			ring[reading_slot].nb_CRAN = 0;
		}
	}
}
