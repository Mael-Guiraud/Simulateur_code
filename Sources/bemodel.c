#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void read_param_file(float * tab, int size)
{
	FILE* file = fopen("beparameters","r");
	if(!file){perror("Opening \"beparameters\" failure\n");exit(2);}

	int trash;
	for(int i = 0;i<size;i++)
	{
		fscanf(file,"%d %f ",&trash, &tab[i]);
	}

}

int inverse_transform(float * tab, int size)
{
	float random = ((float)rand() / RAND_MAX );
	int id = 0;
	float sum = tab[0];
	while(random >= sum)
	{
		id++;
		sum += tab[id];
	}
	return id;
}

