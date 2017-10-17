
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "struct.h"
#include "data_treatment.h"
#include "simulator.h"
#include "sbbp.h"

int main()
{
	srand(time(NULL));
	int ring_size = 100;
	int nb_nodes=5;
	int nb_BBU=1;

	int nb_antenas=2;
	int period=1000;

	int emission_time = 500;
	int emission_gap = 10;

	int max_size = 1000000; // buffer max of the queues

	int minimal_buffer_size = 700;
	int size_CRAN = 1000;
	int size_BE =0 ;
	int packet_size = 1000;

	Policy mode = NO_MANAGMENT;
	//Policy mode = CRAN_FIRST;
	int simulation_lenght = 5000;

	int nb_simuls= 100;
	int time_before_measure = 2000;


	//Generation of best effort parameters
	int state = 0;
	int nb_states = 2 ; //depends of the SBBP model
	int nb_elems[nb_states];
	float *** vectors = init_vectors(nb_states);
	float ** chain = init_chain(nb_states);
	read_SBBP_file(chain,vectors,nb_states,nb_elems);

	
	int table_size = 50000; //Upgrade this value if the programs answers that the table to save the datas is too short

	char name[64];
	sprintf(name,"../gnuplot/1_a_no_BE.pdf"); // name of the output pdf

	float Average_load = 0.0;
	float max_load = 0.0;
	float min_load = 1.0;
	float load;
	//Results storage
	float * tab_CRAN;
	assert(tab_CRAN = (float*)calloc(table_size,sizeof(float)));
	float	* tab_BE;
	assert(tab_BE = (float*)calloc(table_size,sizeof(float)));
	float * tab_ANSWERS;
	assert(tab_ANSWERS = (float*)calloc(table_size,sizeof(float)));
	float * tab_BE_BBU;
	assert(tab_BE_BBU = (float*)calloc(table_size,sizeof(float)));


	for(int i=0;i<nb_simuls;i++	)
	{
		fprintf(stdout,"\r Step %d/%d",i+1,nb_simuls);fflush(stdout);
		load = simulate(ring_size,nb_nodes,nb_antenas,period,minimal_buffer_size,nb_BBU,size_CRAN,size_BE,packet_size,emission_time,emission_gap, mode,simulation_lenght,time_before_measure, max_size,tab_BE,tab_CRAN,tab_ANSWERS,tab_BE_BBU,table_size,vectors,chain,&state);
		Average_load += load;
		max_load = f_max(max_load,load);
		min_load = f_min(min_load,load);
	}

	printf("\n End of the simulation : \n Min load = %f \n Average load = %f \n Max load = %f \n",min_load,Average_load/nb_simuls,max_load);

	
	write_files(tab_BE,tab_CRAN, tab_ANSWERS,tab_BE_BBU, table_size);

	print_gnuplot(name);

	free_chain(chain,nb_states);
	free_vectors(vectors,nb_states, nb_elems);
	free(tab_ANSWERS);
	free(tab_BE);
	free(tab_BE_BBU);
	free(tab_CRAN);
	
	return 0;
}