#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "simulator.h"

int main()
{
	int ring_size = 100;
	int nb_nodes=5;
	int nb_BBU=1;

	int nb_antenas=10;
	int period=1000;

	float burst_proba=0.05;
	float lambda_burst=27;
	float lambda_regular=7;

	int minimal_buffer_size = 700;
	int size_CRAN = 500;
	int size_BE = 5;
	int packet_size = 1000;
	Policy mode = CRAN_FIRST;
	int simulation_lenght = 1000;

	simulate(ring_size,nb_nodes,nb_antenas,period,burst_proba,lambda_burst,lambda_regular,minimal_buffer_size,nb_BBU,size_CRAN,size_BE,packet_size,mode,simulation_lenght);
	return 0;
}