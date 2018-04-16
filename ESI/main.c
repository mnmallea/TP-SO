#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
//#include "config_esi.h"


/*#include <parser.h>*/


int main(int argc, char* argv[]){

	char* j;

	if(argc!=2){
		printf("Cantidad de incorrecta argumentos.\n");
	return -1;
	}

	if((j=malloc(sizeof(char)*50))==NULL)
		printf("No pudo alocar en memoria.\n");

	FILE * f;
	f=fopen(argv[1],"r");
	fscanf(f,"%s",j);
	//parser(f)
	printf("La cadena es: %s\n",j);

	free(j);

	return 1;
}
