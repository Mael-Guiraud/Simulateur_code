#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int poisson_distribution(float lambda)
{
	float random_drawed = ((float)rand() / RAND_MAX );
	double proba = exp (- lambda);
	int x = 0;
	double cumul = proba;
	while (random_drawed > cumul)
	{
		x++;
		proba *=lambda/x;
		cumul += proba;
		if(cumul > 0.9999999)//optimisation to avoid a too long computing
			return x;
	} 
	return x;
}

int main()
{
	srand(time(NULL));

	float lambda = 7.0;
	int tab_size = 50;
	int nb_simuls = 1000000;


	int occurs[tab_size];
	for(int i=0;i<tab_size;i++)
		occurs[i]=0;

	int val;
	
	for(int i=0;i<nb_simuls;i++)
	{
		val = poisson_distribution(lambda);
		if(val <tab_size)
			occurs[val]++;
	}
	for(int i=0;i<tab_size;i++)
	{
		printf("%d %f\n",i,(float)occurs[i]/nb_simuls);
	}
}