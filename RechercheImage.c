#include <stdio.h>
#include <stdlib.h>

#include "cgiu.h"
#include "proc.h"

#define  DEBUG 0


void exitfail(){
  print_html_tail(); 
  exit(0);
}

void triBulles(float** tab, int taille){
    int tab_en_ordre = 0,i;
    float key,val;
    while(!tab_en_ordre){
        tab_en_ordre = 1;
        for( i=0 ; i < taille-1 ; i++){
            if(tab[i][1] > tab[i+1][1]) {
		key=tab[i][0];
		val=tab[i][1];
		tab[i][0]=tab[i+1][0];
		tab[i][1]=tab[i+1][1];		
		tab[i+1][0]=key;
		tab[i+1][1]=val;
                tab_en_ordre = 0;
            }
        }
        taille--;
    }
}

float produitScalaire(float* U, float* V,int N) { 
 float ps ; 
 int i ; 
  for (ps=0.0, i=0; i<N; i++) 
        if(U[i]-V[i]>0)
          ps +=(U[i]-V[i]);
	else
          ps -=(U[i]-V[i]);

 return ps; 
}

float distanceEuclidienne(float* trainDescriptor,float* valDescriptor){
	return produitScalaire(trainDescriptor,valDescriptor,104);
}

float** trueKnn(float** trainDescriptor, float** valDescriptor, int k, int indVal){
	int indTrain,i;
	float **dist;// dist k*2
	
	//On alloue dist resultat
	dist = malloc( k * sizeof(float*));
	if( dist == NULL ){
	     fprintf(stderr,"Allocation impossible");
	     exit(EXIT_FAILURE);
	}
	for( i = 0 ; i < k ; i++ ){
	     dist[i] = calloc (2, sizeof(float));
	     
	     if( dist[i] == NULL ){
		  fprintf(stderr,"Allocation impossible");
		  exit(EXIT_FAILURE);
	     }
	}
	/* init dist resultat */
	for( i = 0 ; i<k ; i++ )
	     dist[i][1] = 150.0;

	//Pour toutes les images dans train:
	for ( indTrain=1; indTrain<4998; indTrain++){
		float d=distanceEuclidienne(trainDescriptor[indTrain],valDescriptor[indVal]);
		int indMax=1;
		float max=dist[indMax][1];
		for ( i=k-1; i>=0; i--){
			if (dist[i][1] > max ) {
				indMax=i;
				max=dist[indMax][1];
			}	
		}
		//printf(" %f ",max);
		//On ajoute si meilleure distance
		if ( d<max){
			dist[indMax][0]=indTrain;
			dist[indMax][1]=d;
		}
	}
	triBulles(dist,k);
	return dist;
}

float** Recherche(float* moyConcept,int seuil,float** trainDescriptors,float** valDescriptors,int indVal,int nbConcepts,char** trainList,char** valList,char** concepts,int** trainAnns,int k){
  float kf= k;
  int i;

  int indDist,indVoisin;
  //algo KNN
  float** dist =trueKnn(trainDescriptors,valDescriptors,k,indVal);

  //Init moyconcept.
  for (i=0;i<nbConcepts;i++){
	moyConcept[i]=0.0;
  }
  //Calcul de chaque moyenne par concept.
  for (indVoisin=0;indVoisin<k;indVoisin++){//pour les k voisins
	int indice =dist[indVoisin][0];
	//pour tout les concept de chaque voisin
	for ( i=0; i<nbConcepts; i++){
	
		if(trainAnns[i][indice]>=1)
		   moyConcept[i]+=1.0;
	}
  }
  for ( i=0; i<nbConcepts; i++){
	moyConcept[i]=moyConcept[i]/(kf);
  }

  //Decision selon seuil.
  for ( i=0; i<nbConcepts; i++){
	//printf("%f | %f </br> \n",moyConcept[i],seuil );
	if (moyConcept[i] > (float)seuil/10){
	   moyConcept[i]=1;
	}
  }
 return dist;
}

void affichageResultat(char** concepts,int k,int nbConcepts,float** dist,char** trainList,float* moyConcept, float seuil){
int i;
  //affichage des concepts
  printf("</br>Les concepts:");
  for ( i=0; i<nbConcepts; i++)
	if (moyConcept[i] > seuil)
	   printf(" %s \n",concepts[i]);
  printf("</br>");

  //affichage des images.
  for ( i=0; i<k; i++){
	int indice =dist[i][0];
	//printf (" %d | %f \n",indice,dist[i][1]); 
	printf ("<div class=zoom>  <img src=\"http://mrim.imag.fr/voc10/images/%s\"\n",trainList[indice]);
	printf ("/> </div> \n");
   }
}

