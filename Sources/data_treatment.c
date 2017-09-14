#include <stdio.h>
#include <stdlib.h>

int id_max_divide(float * tab, int size)
{
	int id = 0;
	long long int cardinal = 0;
	for(int i=0;i<size;i++)
	{
		cardinal += tab[i];
		if(tab[i] >0)
			id = i;
	}
	for(int i=0;i<=id;i++)
	{
		tab[i]/= cardinal;
	}
	return id;
}
int max(int a, int b)
{
	return a>b?a:b;
}

void write_files(float* tab_BE,float * tab_CRAN, float* tab_ANSWERS, int table_size)
{
	int id = id_max_divide(tab_BE,table_size);
	id = max(id,id_max_divide(tab_CRAN,table_size));
	id = max(id,id_max_divide(tab_ANSWERS,table_size));


	FILE* f_CRAN = fopen("../datas/cran_distrib.data","w");
	if(!f_CRAN){perror("Opening \"../datas/cran_distrib.data\" failure\n");exit(2);}
	FILE* f_BE = fopen("../datas/be_distrib.data","w");
	if(!f_BE){perror("Opening \"../datas/be_distrib.data\" failure\n");exit(2);}
	FILE* f_ANSWERS = fopen("../datas/answers_distrib.data","w");		
	if(!f_ANSWERS){perror("Opening \"../datas/answers_distrib.data\" failure\n");exit(2);}

	for(int i=0;i<id;i++)
	{
		if(tab_BE[i])
			fprintf(f_BE,"%d %f\n",i,tab_BE[i]);
		if(tab_CRAN[i])	
			fprintf(f_CRAN,"%d %f\n",i,tab_CRAN[i]);
		if(tab_ANSWERS[i])
			fprintf(f_ANSWERS,"%d %f\n",i,tab_ANSWERS[i]);
	}
	fclose(f_CRAN);
	fclose(f_ANSWERS);
	fclose(f_BE);
}

void print_gnuplot(char * name)
{

	FILE* f_GPLT = fopen("../gnuplot/distribs.gplt","w");
	if(!f_GPLT){perror("Opening gplt file failure\n");exit(2);}

	fprintf(f_GPLT,"plot '../datas/be_distrib.data' title \"BE\" \nreplot '../datas/cran_distrib.data' title \"Uplink\" \nreplot '../datas/answers_distrib.data' title \"Downlink\"\nset term postscript color solid\nset output '| ps2pdf - %s'\nreplot\n",name);
	fclose(f_GPLT);

}