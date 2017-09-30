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
		if(cardinal)
			tab[i]/= cardinal;
		else
			tab[i]=0;
	}
	return id;
}
int max(int a, int b)
{
	return a>b?a:b;
}
float f_max(float a, float b)
{
	return a>b?a:b;
}
float f_min(float a, float b)
{
	return a<b?a:b;
}
void write_files(float* tab_BE,float * tab_CRAN, float* tab_ANSWERS,float* tab_BE_BBU, int table_size)
{
	int id = id_max_divide(tab_BE,table_size);
	id = max(id,id_max_divide(tab_CRAN,table_size));
	id = max(id,id_max_divide(tab_ANSWERS,table_size));
	id = max(id,id_max_divide(tab_BE_BBU,table_size));

	FILE* f_CRAN = fopen("../datas/cran_distrib.data","w");
	if(!f_CRAN){perror("Opening \"../datas/cran_distrib.data\" failure\n");exit(2);}
	FILE* f_BE = fopen("../datas/be_distrib.data","w");
	if(!f_BE){perror("Opening \"../datas/be_distrib.data\" failure\n");exit(2);}
	FILE* f_ANSWERS = fopen("../datas/answers_distrib.data","w");		
	if(!f_ANSWERS){perror("Opening \"../datas/answers_distrib.data\" failure\n");exit(2);}
	FILE* f_BE_BBU = fopen("../datas/be_bbu_distrib.data","w");		
	if(!f_BE_BBU){perror("Opening \"../datas/be_bbu_distrib.data\" failure\n");exit(2);}


	//Always write something in the files
	fprintf(f_BE,"%d %f\n",0,tab_BE[0]);
	fprintf(f_CRAN,"%d %f\n",0,tab_CRAN[0]);
	fprintf(f_ANSWERS,"%d %f\n",0,tab_ANSWERS[0]);
	fprintf(f_BE_BBU,"%d %f\n",0,tab_BE_BBU[0]);

	for(int i=1;i<id;i++)
	{
		if(tab_BE[i])
			fprintf(f_BE,"%d %f\n",i,tab_BE[i]);
		if(tab_CRAN[i])	
			fprintf(f_CRAN,"%d %f\n",i,tab_CRAN[i]);
		if(tab_ANSWERS[i])
			fprintf(f_ANSWERS,"%d %f\n",i,tab_ANSWERS[i]);
		if(tab_BE_BBU[i])
			fprintf(f_BE_BBU,"%d %f\n",i,tab_BE_BBU[i]);
	}
	fclose(f_CRAN);
	fclose(f_ANSWERS);
	fclose(f_BE);
	fclose(f_BE_BBU);
}

void print_gnuplot(char * name)
{

	FILE* f_GPLT = fopen("../gnuplot/distribs.gplt","w");
	if(!f_GPLT){perror("Opening gplt file failure\n");exit(2);}

	fprintf(f_GPLT,"plot '../datas/be_distrib.data' title \"BE\"  smooth bezier\nreplot '../datas/be_bbu_distrib.data' title \"BBU BE\"  smooth bezier \nreplot '../datas/cran_distrib.data' title \"Uplink\"  smooth bezier\nreplot '../datas/answers_distrib.data' title \"Downlink\"  smooth bezier\nset term postscript color solid\nset output '| ps2pdf - %s'\nreplot\n",name);
	fclose(f_GPLT);

}