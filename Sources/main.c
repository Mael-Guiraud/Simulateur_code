
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

	int nb_antenas=75;
	int period=1000;

	int max_size = 1000000; // buffer max of the queues

	int minimal_buffer_size = 700;
	int size_CRAN = 500;
	int size_BE = 5;
	int packet_size = 1000;
	//Policy mode = NO_MANAGMENT;
	Policy mode = CRAN_FIRST;
	int simulation_lenght = 5000;

	int nb_simuls= 100;
	int time_before_measure = 2000;


	//Generation of best effort parameters
	int state = 0;
	int nb_states = 2 ; //depends of the SBBP model
	float *** vectors = init_vectors(nb_states);
	float ** chain = init_chain(nb_states);
	read_SBBP_file(chain,vectors,nb_states);

	
	int table_size = 500; //Upgrade this value if the programs answers that the table to save the datas is too short

	char name[64];
	sprintf(name,"../gnuplot/loaded_prio.pdf"); // name of the output pdf


	//Results storage
	float * tab_CRAN;
	assert(tab_CRAN = (float*)calloc(table_size,sizeof(float)));
	float	* tab_BE;
	assert(tab_BE = (float*)calloc(table_size,sizeof(float)));
	float * tab_ANSWERS;
	assert(tab_ANSWERS = (float*)calloc(table_size,sizeof(float)));


	for(int i=0;i<nb_simuls;i++	)
	{
		fprintf(stdout,"\r Step %d/%d",i+1,nb_simuls);fflush(stdout);
		simulate(ring_size,nb_nodes,nb_antenas,period,minimal_buffer_size,nb_BBU,size_CRAN,size_BE,packet_size,mode,simulation_lenght,time_before_measure, max_size,tab_BE,tab_CRAN,tab_ANSWERS,table_size,vectors,chain,&state);
	}

	printf("\n");

	
	write_files(tab_BE,tab_CRAN, tab_ANSWERS, table_size);

	print_gnuplot(name);

	free(tab_ANSWERS);
	free(tab_BE);
	free(tab_CRAN);
	
	return 0;
}