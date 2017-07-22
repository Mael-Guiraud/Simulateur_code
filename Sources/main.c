
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "struct.h"
#include "simulator.h"

int main()
{
	srand(time(NULL));
	int ring_size = 100;
	int nb_nodes=5;
	int nb_BBU=1;

	int nb_antenas=10;
	int period=1000;

	float burst_proba=0.05;
	float lambda_burst=100;
	float lambda_regular=30;
	int max_size = 1000000;

	int minimal_buffer_size = 700;
	int size_CRAN = 500;
	int size_BE = 5;
	int packet_size = 1000;
	Policy mode = NO_MANAGMENT;
	int simulation_lenght = 3000;

	int nb_simuls= 100;
	int time_before_measure = 900;

	FILE* f_CRAN = fopen("../datas/cran_distrib.data","w");
	if(!f_CRAN){perror("Opening \"../datas/cran_distrib.data\" failure\n");exit(2);}
	FILE* f_BE = fopen("../datas/be_distrib.data","w");
	if(!f_BE){perror("Opening \"../datas/be_distrib.data\" failure\n");exit(2);}
	FILE* f_ANSWERS = fopen("../datas/answers_distrib.data","w");		
	if(!f_ANSWERS){perror("Opening \"../datas/answers_distrib.data\" failure\n");exit(2);}

	for(int i=0;i<nb_simuls;i++	)
	{
		fprintf(stdout,"\r Step %d/%d",i+1,nb_simuls);fflush(stdout);
		simulate(ring_size,nb_nodes,nb_antenas,period,burst_proba,lambda_burst,lambda_regular,minimal_buffer_size,nb_BBU,size_CRAN,size_BE,packet_size,mode,simulation_lenght,time_before_measure, max_size,f_BE,f_CRAN,f_ANSWERS);
	}
	printf("\n");

	fclose(f_CRAN);
	fclose(f_ANSWERS);
	fclose(f_BE);
	
	return 0;
}