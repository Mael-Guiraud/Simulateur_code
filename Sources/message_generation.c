#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


#include "struct.h"
#include "bemodel.h"

int min(int a, int b)
{
	return (a<b)?a:b;
}


int* repart(int k, int n){
	int* res = calloc(k,sizeof(int));
	if(k<=1) return res;
	int q = n/k;
	int r = n%k;
	int* space = repart(r,k);
	for(int i= 0; i< r; i++){
		res[space[i]] = 1;
	}
	for(int i = 1; i < k; i++ ){
		res[i] += res[i-1] + q;
	}
	free(space);
	return res;
}

int * repart_inter(int nb_inters,int nb_antenas)
{
	int * rep = (int*)malloc(sizeof(int)*nb_inters);

	for(int i=0;i<nb_inters;i++)
	{
		rep[i] = nb_antenas/nb_inters;
	}
	for(int i=0;i<nb_antenas%nb_inters;i++)
	{
		rep[i]++;
	}

	return rep;
}
int inter_id(int * inters, int nb_inters,int antena_id)
{
	int cmpt = 0;
	for(int i=0;i<nb_inters;i++)
	{
		cmpt+= inters[i];
		
		if(cmpt>antena_id)
		{
			return i;
		}
	}
	return -1;
}
int nb_previous(int * inters, int nb_inters,int antena_id)
{
	int cmpt = 0;
	for(int i=0;i<nb_inters;i++)
	{
		
		cmpt = cmpt+inters[i];
		
		if(cmpt>antena_id)
		{
		
			return cmpt-inters[i];
		}
	}
	return -1;
}

//Init the sending slot of the antenas for a node
void init_CRAN(int* antenas,int period, int nb_antenas, Policy mode,int ** nodes_positions, int node_id, int * antenas_distrib, int nb_nodes,int res_kind,int emission_gap, int emission_time,int ring_size, int ** shift)
{
	int antena_id;
	int cumulantennas[5];
	int* repartition, * repartition_inter ;
	int nb_macro_inters;
	int inter_idt;
	int nb_antenas_previous;
	int freq;
	int delay;
	int shifted_freq;
	int nb_reserved_freqs;

	int nb_antenas_per_freq ;
	int nb_freq_needed ;
	int freq_id ;
	int pos_in_freq;
	int gap_in_period;
	int offset ;
	int current_freq,current_node;
	int nb_CRAN;
	int prov;
	for(int i=0;i<5;i++)cumulantennas[i]=0;
	cumulantennas[0] = antenas_distrib[0];
	for(int i=1;i<5;i++)
	{
		cumulantennas[i] = cumulantennas[i-1] +antenas_distrib[i];
		
	}
	for(int i=0;i<antenas_distrib[node_id];i++)
	{
		/*if(i==0)
			antena_id = node_id-1;
		else
			antena_id = (i)*nb_nodes + node_id-i-1;
		*/
		antena_id = 0;
		for(int j=0;j<node_id;j++)
			antena_id += antenas_distrib[j];
		printf("%d %d %d\n",antena_id,i,node_id);
		antena_id += i;
		switch(mode){
			case RESERVATION:
				switch(res_kind)
				{
					case 1://TOUT COLLE avec ET>EP/2 donc - de 5 antennes
				 		antenas[i] = ( ((antena_id)*2+1) - (nodes_positions[node_id][0] -1) +period)%period; // (i*2-1) is the sequence of messages in the BBU [_1_2_3_4....]
				 	break;
				 	case 2:// moins de 5 antennes, réparties
				 		repartition = repart(nb_antenas,emission_gap/2);
				 		antenas[i] = ( ((repartition[antena_id])*2+1) - (nodes_positions[node_id][0] -1) +period)%period; // (i*2-1) is the sequence of messages in the BBU [_1_2_3_4....]
				 		free(repartition);
				 		repartition = repart(nb_antenas,period);
				 		antenas[i] += repartition [antena_id] - (repartition [antena_id] % emission_gap);
				 		//printf("%d [%d %d] \n",period,antena_id,repartition[antena_id]);
				 		free(repartition);
				 	break;
				 	case 3://Tout collé dans des intervalles
				 		nb_antenas_per_freq = (period- ring_size) / emission_time;

				 		
				 		antenas[i] = (  (antena_id/(nb_antenas_per_freq)*2+1) - (nodes_positions[node_id][0] -1) +  (antena_id%nb_antenas_per_freq)*(emission_time) +period)%period; 
				 	break;

				 	case 4://Réaprtis dans des intervalles,0 balanced in slot
				 		nb_macro_inters = period/(emission_time+ring_size);
				 		//printf("\n\n %d --------\nNB nb_macro_inters = %d \n",antena_id,nb_macro_inters);
				 		repartition_inter = repart_inter(nb_macro_inters,nb_antenas);
				 		//for(int z=0;z<nb_macro_inters;z++)printf("%d,",repartition_inter[z]);printf("\n");
				 		inter_idt = inter_id(repartition_inter,nb_macro_inters,antena_id);
				 		//printf("inter_idt = %d\n",inter_idt);
				 		nb_antenas_previous = nb_previous(repartition_inter,nb_macro_inters,antena_id);
				 		//printf("nb_antenas_previous = %d \n",nb_antenas_previous);
				 		repartition = repart(repartition_inter[inter_idt],emission_gap/2);
				 		//for(int z=0;z<repartition_inter[inter_idt];z++)printf("%d,",repartition[z]);printf("\n");
		
				 		antenas[i] = ( ((repartition[antena_id-nb_antenas_previous])*2+1) - (nodes_positions[node_id][0] -1) + (inter_idt*(emission_time+ring_size) ) +period)%period; 
				 		//printf("%d \n",antenas[i]);
				 		free(repartition);

	
				 		free(repartition_inter);
				 		
				 	break;		
				 	case 5: //balanced in period + slot
					 	nb_antenas_per_freq = (period- ring_size) / emission_time;
					 	nb_freq_needed = nb_antenas / nb_antenas_per_freq ;
					 	if(nb_antenas % nb_antenas_per_freq )
					 		nb_freq_needed++;
					 	repartition = repart(nb_freq_needed,emission_gap/2);
					 	repartition_inter = repart(nb_freq_needed,period);
					 	freq_id = antena_id / nb_antenas_per_freq;
					 	pos_in_freq = antena_id % nb_antenas_per_freq;
					 	antenas[i] = (repartition[freq_id]*2+1 + pos_in_freq*emission_time - (nodes_positions[node_id][0] -1)+repartition_inter[freq_id]  +period)%period;
					 	free(repartition);
					 	free(repartition_inter);
					 	//printf("%d %d %d %d %d %d %d\n",antena_id,nb_antenas_per_freq,nb_freq_needed,freq_id,antenas[i],pos_in_freq,nodes_positions[node_id][0]);
				 	break;
				 	case 6://only balancing inside the period
				 		nb_antenas_per_freq = (period- ring_size) / emission_time;
				 		nb_freq_needed = nb_antenas / nb_antenas_per_freq ;
					 	if(nb_antenas % nb_antenas_per_freq )
					 		nb_freq_needed++;
					 	gap_in_period = period/nb_freq_needed - ( (period/nb_freq_needed) % (emission_gap/2)  );
				 		freq_id = antena_id / nb_antenas_per_freq;
				 		antenas[i] = (  (antena_id/(nb_antenas_per_freq)*2+1) - (nodes_positions[node_id][0] -1) +  (antena_id%nb_antenas_per_freq)*(emission_time) + (freq_id*gap_in_period) +period)%period; 
								// 					FREQ                             decalage nodes                   //decalage in the freq                            decalage in the period
					 	
				 	break;

		
				}

			break;
			case SPLIT_FREQ:
			
				 offset =0;
				 current_freq = 0;
				 nb_CRAN = 0;
				for(int j =0;j<=antena_id;j++)
				{
					if((nb_CRAN+emission_time) <= period)
					{

						nb_CRAN +=  emission_time;
					}
					else
					{
						prov = (nb_CRAN - (period-ring_size));
						nb_CRAN +=  prov;
						current_freq = current_freq+2;

					}
				}
			
				antenas[i] = (current_freq +  nb_CRAN +period)%period; 
				if( (offset+ emission_time) <= period)
					shift[node_id][i] = emission_time;
				else
					shift[node_id][i] = prov;
				printf("Antenna %d offset %d node[i] %d current_freq %d shift %d nodeid %d distance %d \n",antena_id,offset,antenas[i],current_freq,shift[node_id][i],node_id, nodes_positions[node_id][0]);
			

				
			break;
			default:
				antenas[i]= rand()%period;
			break;
		}	
	}
	



}

int ** init_nodes_antenas(int nb_nodes, int nb_antenas, int period, int nb_BBU,Policy mode, int ** nodes_positions, int * antenas_distrib, int res_kind, int emission_gap, int emission_time,int ring_size, int ** shift)
{
	int ** nodes;
	assert(nodes = (int **)malloc(sizeof(int*)*nb_nodes));

	for(int i=nb_BBU;i<nb_nodes;i++)
	{
		if(DEBUG)printf("Antenas generation at node %d:\n",i);
		assert(nodes[i]=(int*)malloc(sizeof(int)*antenas_distrib[i]));

		init_CRAN(nodes[i],period,nb_antenas,mode,nodes_positions,i,antenas_distrib,nb_nodes,res_kind,emission_gap, emission_time,ring_size,shift);
		/*printf("\n");
		for(int j=0;j<antenas_distrib[i];j++)
		{
			printf("%d %d \n",i,nodes[i][j]);
		}*/
	}



	
	return nodes;
}

void free_nodes_antenas(int ** nodes, int nb_nodes,int nb_BBU)
{
	for(int i=nb_BBU;i<nb_nodes;i++)
		free(nodes[i]);
	free(nodes);
}
/*
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

}*/

void generation_BE(Queue* BE_Q, int nb_nodes, int size_BE, int current_slot, int max_size,float * distrib_be_generation, int deadline,int * be_offset_generation)
{
	for(int i=0;i<nb_nodes;i++)
	{
		if(be_offset_generation[i]==0)
		{
			BE_Q[i].size += size_BE;
			BE_Q[i].queue[BE_Q[i].max_id] = current_slot; 
			BE_Q[i].kind[BE_Q[i].max_id] = 1; 
			BE_Q[i].max_id= (BE_Q[i].max_id+1)%max_size;
			be_offset_generation[i] = inverse_transform(distrib_be_generation,deadline);
		}
		else
		{
			be_offset_generation[i]--;
		}
	}
	
}

void generation_CRAN(Queue* CRAN_Q,int** nodes_antenas, int nb_nodes, int nb_antenas, int current_slot, int size_CRAN,int size_BE,int nb_BBU, int period, int max_size, int emission_time, int emission_gap,int * antenas_distrib,Policy mode,int ** nodes_positions,int ring_size,int ** shift)
{
	for(int i=nb_BBU;i<nb_nodes;i++)
	{
		for(int j=0;j<antenas_distrib[i];j++)
		{
			switch(mode)
			{
				case SPLIT_FREQ:
					;	
					int k;
					for(k=0;k<shift[i][j];k+=emission_gap)
					{
						
						if( (nodes_antenas[i][j]+k)%period == current_slot%period)
						{
							
							
							CRAN_Q[i].size += size_CRAN;
							CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 	
							CRAN_Q[i].kind[CRAN_Q[i].max_id] = 2; 	
							CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
							
							
							
						}
					}
					for(;k<emission_time;k+= emission_gap)
					{
						
						if( (nodes_antenas[i][j]+k+2)%period == current_slot%period)
						{
							
							CRAN_Q[i].size += size_CRAN;
							CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 	
							CRAN_Q[i].kind[CRAN_Q[i].max_id] = 2; 	
							CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
							//printf("Geneartion du cran au noeaud %d a la date %d \n",i,current_slot);
						}
					}
				break;
				default:
					for(int k=0;k<emission_time;k+=emission_gap)
					{
						if( (nodes_antenas[i][j]+k)%period == current_slot%period)
						{
							if(mode == NO_MANAGMENT)
							{
								for(int l=0;l<size_CRAN/size_BE;l++)
								{
									CRAN_Q[i].size += size_BE;
									CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 	
									CRAN_Q[i].kind[CRAN_Q[i].max_id] = 2; 	
									CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
								}
							}
							else
							{
								CRAN_Q[i].size += size_CRAN;
								CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 	
								CRAN_Q[i].kind[CRAN_Q[i].max_id] = 2; 	
								CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
							}
						}
					}
				break;
			}
		}
	}
}

void generation_answers(Packet* ring, int** nodes_positions, Queue* CRAN_Q, int nb_BBU, int ring_size, int current_slot, int size_CRAN,int size_BE,Policy mode, int max_size)
{
	int reading_slot;
	for(int i=0;i<nb_BBU;i++)
	{
		reading_slot = (ring_size+nodes_positions[0][i]-1)%ring_size;
		if(ring[reading_slot].owner >= nb_BBU )
		{
			if(ring[reading_slot].nb_CRAN > 0)
			{
				CRAN_Q[i].size += ring[reading_slot].nb_CRAN;
				if(mode == NO_MANAGMENT)
				{
					for(int j=0;j<ring[reading_slot].nb_CRAN/size_BE;j++)
					{
						CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 
						CRAN_Q[i].kind[CRAN_Q[i].max_id] = 3; 

						CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
						//printf("Creating Answer to %d on queue %d at date %d  (max id = %d)\n ",ring[reading_slot].owner,i,current_slot,CRAN_Q[i].max_id);
					}
				}
				else
				{
					for(int j=0;j<ring[reading_slot].nb_CRAN/size_CRAN;j++)
					{
						CRAN_Q[i].queue[CRAN_Q[i].max_id] = current_slot; 
						CRAN_Q[i].kind[CRAN_Q[i].max_id] = 3; 

						CRAN_Q[i].max_id= (CRAN_Q[i].max_id+1)%max_size;
						//printf("Creating Answer to %d on queue %d at date %d  (max id = %d)\n ",ring[reading_slot].owner,i,current_slot,CRAN_Q[i].max_id);
					}
				}
			}
		}
	}
}

void reservation_management(Packet* ring, int ring_size, int** nodes_antenas, int** nodes_positions, int nb_nodes, int current_slot,int nb_BBU, int period, int emission_time, int emission_gap,int * antenas_distrib,Policy mode, int ** shift)
{
	int writing_Slot ;
	int k;
	for(int i=nb_BBU;i<nb_nodes;i++)
	{
		writing_Slot  = nodes_positions[0][i];
		for(int j=0;j<antenas_distrib[i];j++)
		{
			for(k=0;k<ring_size;k+=emission_gap)
			{
				/*if( (nodes_antenas[i][j] + k - ring_size + period)%period == current_slot%period )
				{
					//printf("We reserve (%d %d) on a slot reserved for (%d %d)\n",0,i,ring[writing_Slot+1].reserved_for,ring[writing_Slot].reserved_for);
					ring[writing_Slot+1].reserved_for = 0;
					ring[writing_Slot].reserved_for = i;
				} */
				if( (nodes_antenas[i][j] - ring_size + k+nodes_positions[i][0]-1)%period == current_slot%period)
				{
					ring[0].reserved_for = 0;
					
					printf("%d reserve for 0 at date %d (k = %d)\n",i,current_slot,k);
				}
				if( (nodes_antenas[i][j]+k - ring_size + period)%period == current_slot%period)
				{
					ring[writing_Slot].reserved_for = i;
					
					printf("%d reserve for itself at date %d (k = %d)\n",i,current_slot,k);
				}
			}
			switch(mode)
			{
				case SPLIT_FREQ: 

					if(shift[i][j] != emission_time)
						for(k=0;k<ring_size;k+=emission_gap)
						{
							if( (nodes_antenas[i][j]+shift[i][j]- ring_size+k+2+nodes_positions[i][0]-1)%period == current_slot%period)
							{
								ring[0].reserved_for = 0;
								printf("%d reserve for 0 at date %d (k = %d, split freq)\n",i,current_slot,k);
							}
							if( (nodes_antenas[i][j]+shift[i][j]- ring_size+k+2)%period == current_slot%period)
							{

								ring[writing_Slot].reserved_for = i;
								printf("%d reserve for itself at date %d (k = %d, splitfreq)\n",i,current_slot,k);
							}
						}
					/*
					if(shift[i][j] == emission_time) // pas de split, comportement normal
					{
						//printf("node %d comportement normal \n",i);
						for(k=0;k<emission_time-ring_size;k+=emission_gap)
						{
							if( (nodes_antenas[i][j]+k+nodes_positions[i][0]-1)%period == current_slot%period)
							{
								ring[0].reserved_for = 0;
							}
							if( (nodes_antenas[i][j]+k)%period == current_slot%period)
							{
								ring[writing_Slot].reserved_for = i;
							}
						}
	
					}
					else
					{
					
						//printf("Antenne %d du noeud %d on a des slots rervés pendant %d \n",j,i,shift[i][j]);
						for(k=0;k<shift[i][j]-ring_size;k+=emission_gap) // comportement normal jusqu'a shift
						{
							if( (nodes_antenas[i][j]+k+nodes_positions[i][0]-1)%period == current_slot%period)
							{
								ring[0].reserved_for = 0;
							}
							if( (nodes_antenas[i][j]+k)%period == current_slot%period)
							{
								ring[writing_Slot].reserved_for = i;
							}
						}

						
						for(;k<shift[i][j];k+=emission_gap)// on libere sur les derniers slots mais on reserve les autres
						{
						

							if( (nodes_antenas[i][j]+k+2+nodes_positions[i][0]-1)%period == current_slot%period)
							{
								ring[0].reserved_for = 0;
							}
							if( (nodes_antenas[i][j]+k+2)%period == current_slot%period)
							{

								ring[writing_Slot].reserved_for = i;
							}
						}
					
						
					}
					*/
				break;
				/*
				default:
					for(k=0;k<emission_time-ring_size;k+=emission_gap)
					{
						if( (nodes_antenas[i][j]+k+nodes_positions[i][0]-1)%period == current_slot%period)
						{
							ring[0].reserved_for = 0;
						}
						if( (nodes_antenas[i][j]+k)%period == current_slot%period)
						{
							ring[writing_Slot].reserved_for = i;
						}
					}
					
				break;
				*/
			}
		}
	}
}
int insert_packets(Queue* BE_Q, Queue * CRAN_Q, Packet* ring, int** nodes_positions,int packet_size, int minimal_buffer_size,int deadline, Policy mode, int nb_nodes, int size_CRAN, int size_BE, int max_size,int current_slot,int nb_BBU, float* tab_BE,float * tab_CRAN, float* tab_ANSWERS,float * tab_BE_BBU,int time_before_measure,int table_Size)
{
	int writing_Slot;
	int gap;
	int inserted = 0;
	int packet_created_size;
	int cran_to_write;
	int be_to_write;

	for(int i=0;i<nb_nodes;i++)
	{
		writing_Slot = nodes_positions[0][i];
		packet_created_size = 0;
		if(i==0)
		{
			printf("%d [%d %d %d] [%d %d %d] %d \n",current_slot,ring[99].reserved_for,ring[99].owner,ring[99].nb_CRAN,ring[writing_Slot].reserved_for,ring[writing_Slot].owner,ring[0].nb_CRAN,CRAN_Q[i].size);
			
		}

		//printf(" We are at node %d, the slot is reserved for %d, used by %d and i have %d CRAN in queu\n",i,ring[writing_Slot].reserved_for,ring[writing_Slot].owner,CRAN_Q[i].size);
		/*if(ring[writing_Slot].reserved_for == i && CRAN_Q[i].size == 0 )
			printf("PB mon gars");*/
		if( (ring[writing_Slot].owner == -1) && ((ring[writing_Slot].reserved_for == -1)  || (ring[writing_Slot].reserved_for == i)   )   )//IF the slot is free and not reserved for another
		{
			switch(mode)
			{
				
				case NO_MANAGMENT:
					if((CRAN_Q[i].size>=minimal_buffer_size)|| ( (CRAN_Q[i].size > 0)&&( current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] >= deadline  ) ) )
					{
						inserted++;
						ring[writing_Slot].owner = i;
						packet_created_size = 0;

						while((CRAN_Q[i].size>0) && (packet_created_size <= packet_size))
						{
							//printf("***********%d-%d\n",CRAN_Q[i].kind[CRAN_Q[i].min_id],CRAN_Q[i].min_id);
							
							if(packet_created_size <= packet_size- size_BE)
							{
								if(current_slot>time_before_measure)	
								{
									//fprintf(f_BE,"%d\n",current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]);
									if(CRAN_Q[i].kind[CRAN_Q[i].min_id] == 1)
									{
										if(current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] < table_Size)
										{
											if(i<nb_BBU)
												tab_BE[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
											else
												tab_BE[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
										}
										else
										{
											printf("Warning : The table to save the datas is too short\n");
										}
									}
									else
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
												tab_CRAN[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;

											}
											else
											{
												printf("Warning : The table to save the datas is too short\n");
											}
										}
									}
								}
								if(CRAN_Q[i].kind[CRAN_Q[i].min_id] == 2)
									ring[writing_Slot].nb_CRAN += size_BE;

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
					}
				break;
				default:
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
										tab_CRAN[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;
										printf("BBU %d \n",current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]);
									}
									else
									{
										tab_CRAN[current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]]++;
										printf("RRH[%d] %d (date %d)\n",i,current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id],current_slot);
										if(current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id] > 0)
										{
											printf("%d \n",current_slot-CRAN_Q[i].queue[CRAN_Q[i].min_id]);
											exit(45);
										}
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
			
						CRAN_Q[i].size -= cran_to_write;
						ring[writing_Slot].reserved_for = -1;

						
					}
					else // No CRAN
					{
						
						if(BE_Q[i].size>0)
						{
							inserted++;
							if(DEBUG)printf("BE packet at node %d.\n",i);
							ring[writing_Slot].owner = i;
							ring[writing_Slot].nb_CRAN = 0;
	

							if(current_slot>time_before_measure)	
							{
								//fprintf(f_BE,"%d\n",current_slot-BE_Q[i].queue[BE_Q[i].min_id]);
								if(current_slot-BE_Q[i].queue[BE_Q[i].min_id] < table_Size)
								{
									if(i<nb_BBU)
										tab_BE[current_slot-BE_Q[i].queue[BE_Q[i].min_id]]++;
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
							
							BE_Q[i].size -= size_BE;
							
							
						}
						else
						{
							if (ring[writing_Slot].reserved_for == i)
							{
								inserted++;

							}
						}

					}
				break;
			}
		}
		else
		{

			if( CRAN_Q[i].size >0)
			{
				printf("%d essaye d'inserer mais il ne peut pas a la date %d (%d %d) (%d) \n",i,current_slot,ring[writing_Slot].owner,ring[writing_Slot].reserved_for,ring[writing_Slot].nb_CRAN);
				//exit(55);
			}
			
		}
	}
	return inserted;
}

void remove_packets(int** nodes_positions, Packet* ring, int nb_nodes,int ring_size)
{
	int reading_slot;
	for(int i=0;i<nb_nodes;i++)
	{
		reading_slot = (ring_size+nodes_positions[0][i]-1)%ring_size;
		if(ring[reading_slot].owner == i)
		{
			if(DEBUG)printf("Nodes %d removes its packet\n",i);
			ring[reading_slot].owner = -1;
			ring[reading_slot].nb_CRAN = 0;

		}
	}
}


