#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int MAX_SIZE = 500;
	int tab[MAX_SIZE];
	int nb_read;
	int reading;
	int max = 0;


	for(int file = 0;file < 3; file++)
	{
		nb_read = 0;
		for(int i=0;i<MAX_SIZE;i++)tab[i]=0;
		char name[124];
		char name2[124];

		sprintf(name,"../datas/");
		sprintf(name2,"../datas/");
		switch(file)
		{
			case 2:
				strcat(name,"answers");
				strcat(name2,"answers");
			break;
			case 1:
				strcat(name,"cran");
				strcat(name2,"cran");
			break;
			default:
				strcat(name,"be");
				strcat(name2,"be");
			break;
		}
		strcat(name,"_distrib.data");
		strcat(name2,"_distrib_short.data");
		FILE* f = fopen(name,"r");
		FILE* f2 = fopen(name2,"w");

		printf("Reduction of %s ...\n",name);

		while(fscanf(f,"%d\n",&reading) != EOF)
		{
			nb_read++;
			if(reading< MAX_SIZE)
			{
				if(reading > max)
					max = reading;
				tab[reading]++;
			}

		}
		printf("Writting results on %s ...\n",name2);
		for(int i=0;i<max;i++)
		{
			if(tab[i])
				fprintf(f2,"%d %f \n", i, (float)tab[i]/nb_read);
		}
		fclose(f);
		fclose(f2);
	}

}