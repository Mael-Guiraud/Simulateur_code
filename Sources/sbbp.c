#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//INIT FUNCTIONS

float ** init_chain(int nb_states)
{
	float ** chain;
	assert(chain = (float **)malloc(sizeof(float*)*nb_states));
	for(int i=0;i<nb_states;i++)
	{
		assert(chain[i]=(float*)malloc(sizeof(float)*nb_states));
	}	
	return chain;
}
float *** init_vectors(int nb_states)
{
	float *** vectors;
	assert(vectors = (float ***)malloc(sizeof(float**)*nb_states));
	return vectors;
}

void read_SBBP_file(float ** chain, float *** vectors, int nb_states)
{
	FILE* file = fopen("SBBP_PARAMETERS/parameters","r");
	if(!file){perror("Opening \"/SBBP_PARAMETERS/parameters\" failure\n");exit(2);}

	//Read the markov chain matrix
	for(int i = 0;i<nb_states;i++)
	{
		for(int j=0;j<nb_states;j++)
		{
			fscanf(file,"%f ",&chain[i][j]);
		}
	}

	int nb_elems;
	//Read the vectors
	for(int i=0;i<nb_states;i++)
	{
		//get the number of elements in the vector
		fscanf(file,"%d ",&nb_elems);
		assert(vectors[i]=(float**)malloc(sizeof(float*)*nb_elems));
		for(int j=0;j<nb_elems;j++)
		{
			assert(vectors[i][j]= (float*)malloc(sizeof(float)*2));
			fscanf(file,"%f %f ",&vectors[i][j][0],&vectors[i][j][1]);
		}
	}
	fclose(file);
}



//generator functions
int change_state(float ** chain, int actual_state)
{
	float random = ((float)rand() / RAND_MAX );
	int id = 0;
	float sum = chain[actual_state][id];
	while(random >= sum)
	{
		id++;
		sum += chain[actual_state][id];
	}
	return id;
}


int sbbp_generation(float *** vectors, int state)
{
	float random = ((float)rand() / RAND_MAX );
	int id = 0;
	float sum = vectors[state][id][1];
	while(random >= sum)
	{
		id++;
		sum += vectors[state][id][1];
	}
	return vectors[state][id][0];
}




//Trial functions
void print_matrix(float ** chain, int nb_states)
{
	for(int i = 0;i<nb_states;i++)
	{
		for(int j=0;j<nb_states;j++)
		{
			printf("%f ",chain[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
void print_vectors(float *** vectors, int nb_states)
{
	float sum ;
	for(int i = 0;i<nb_states;i++)
	{
		sum = 0.0;
		for(int j=0;sum < 1;j++)
		{
			printf("%f %f\n",vectors[i][j][0],vectors[i][j][1]);
			sum += vectors[i][j][1];
		}
		printf("\n");
	}
}
int equals(float * t1, float * t2, int size)
{
	for(int i=0;i<size;i++)
	{
		if(t1[i]!=t2[i])return 0;
	}
	return 1;
}
void stationary_distribution(float ** chain, int nb_states)
{

	//init state
	float pi_0[nb_states];
	float pi_n[nb_states];
	int iteration =0;
	pi_n[0]=1;
	for(int i=1;i<nb_states;i++)
	{
		pi_0[i] = 0.0;
		pi_n[i] = 0.0;
	}

	
	do
	{
		//cpy 
		for(int i=0;i<nb_states;i++)
		{
			pi_0[i] = pi_n[i];
			pi_n[i] = 0;
		}
		//calculate new 
		for(int i=0;i<nb_states;i++)
		{
			for(int j=0;j<nb_states;j++)
			{
				pi_n[i] += pi_0[j]*chain[j][i];
			}
		}
		iteration++;

	}while(!equals(pi_0,pi_n,nb_states));
	/*
	printf("Stationary Distribution found at n = %d \n",iteration);
	for(int i=0;i<nb_states;i++)
	{
		printf("%f ",pi_n[i]);
	}printf("\n");
	*/

}
