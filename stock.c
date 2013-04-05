#include <stdio.h>
#include <stdlib.h>
#include "cgiu.h"

typedef struct {
    char *nom;
    char *adresse;
    char *telephone;
} personne;

int main (void){
	FILE *stream;
	int i,nbligne;
	char buffer[512];

	stream = fopen("data.txt", "r");
	if(stream = NULL){
		printf("Le fichier n'existe pas");
	}
	else{
		printf("Le fichier existe et est ouvert en lecture seule");
		fscanf(stream,"%d \n",&nbligne);

		personne* p[nbligne];

		for(i=0; i<nbligne; i++){

		  personne* pers= (personne*) malloc(sizeof(personne));
			if ( fgets (buffer , 512 , stream) != NULL )
       				pers->nom=getword(buffer,';');
				pers->adresse=getword(buffer,';');
				pers->telephone=getword(buffer,'\n');
				p[i]= pers;				
		}	
		
	}
	fclose(stream);
	return 0;
}


