#include <stdio.h>
#include <stdlib.h>

#include "cgiu.h"
#include "proc.h"
#include "RechercheImage.h"

#define  DEBUG 0



int main(int argc, char *argv[]){
  int x,m;
  char *qs;
  entry *entries;
    
  int nbTrain,nbVal,nbDims,nbConcepts,c,**trainAnns,**valAnns,ret;
  char **trainList,**valList,**concepts,*name;
  float **trainDescriptors,**valDescriptors;
  
  /* récupération de la chaîne de paramètres */
  qs = get_query_string(argc,argv);

  /* récupération des couples (nom,valeur) */
  entries = get_entries(qs,&m);

  char* nbAff = getparam(entries,m,"NB");
  char* kchar = getparam(entries,m,"KVAL");
  /* affichage éventuel des informations de debug */

  trainList = readList("/usr/lib/cgi-bin/RIM/images/train/list.txt",&nbTrain);
  if (trainList == NULL) {printf("Failed to read %s file.\n","train/list.txt"); exitfail();}
  
  valList = readList("/usr/lib/cgi-bin/RIM/images/val/list.txt",&nbVal);
  if (valList == NULL) {printf("Failed to read %s file.\n","val/list.txt"); exitfail();}
  
  trainDescriptors = readDescriptors("/usr/lib/cgi-bin/RIM/images/train/descriptors.bin",nbTrain,&nbDims);
  if (trainDescriptors == NULL) {printf("Failed to read %s file.\n","train/descriptors.bin"); exitfail();}
  
  valDescriptors = readDescriptors("/usr/lib/cgi-bin/RIM/images/val/descriptors.bin",nbVal,&nbDims);
  if (valDescriptors == NULL) {printf("Failed to read %s file.\n","val/descriptors.bin"); exitfail();}
  
  concepts = readList("/usr/lib/cgi-bin/RIM/images/concepts.txt",&nbConcepts);
  if (concepts == NULL) {printf("Failed to read %s file.\n","concepts.txt"); exitfail();}
  
  trainAnns = malloc(nbConcepts*sizeof(int *));
  if (trainAnns == NULL) {printf("malloc() failed.\n"); exitfail();}
  for (c = 0; c < nbConcepts; c ++) {
    ret = asprintf(&name,"/usr/lib/cgi-bin/RIM/images/train/ann/%s.ann",concepts[c]);
    if (ret < 0) {printf("asprintf() failed.\n"); exitfail();}
    trainAnns[c] = readAnnotations(name,nbTrain);
    if (trainAnns[c] == NULL) {printf("Failed to read train/ann%s.ann file.\n",concepts[c]); exitfail();}
  }
  valAnns = malloc(nbConcepts*sizeof(int *));
  if (valAnns == NULL) { exitfail();}
  for (c = 0; c < nbConcepts; c ++) {
    ret = asprintf(&name,"/usr/lib/cgi-bin/RIM/images/val/ann/%s.ann",concepts[c]);
    if (ret < 0) { exitfail();}
    valAnns[c] = readAnnotations(name,nbVal);
    if (valAnns[c] == NULL) { exitfail();}

  }
  int nbDocs=atoi(nbAff);
  float seuil=0.2,moyenne=0.0, moyenneTotal=0.0;
  int k = atoi(kchar);
  int i,indConcept;
  float moyennePrecision[k];
  for(i=0;i<k;i++)
	 moyennePrecision[i]=0;

  //Pour toutes les images dans Val:
  for(i=0;i<nbDocs;i++){
 	float moyConcept[nbConcepts];
	float ** dist =Recherche(moyConcept,seuil, trainDescriptors, valDescriptors,(i),nbConcepts, trainList,valList, concepts, trainAnns,k);

	int conceptVal[nbConcepts];
	//Init conceptVal.
	for (indConcept=0;indConcept<nbConcepts;indConcept++)
	  conceptVal[indConcept]=0;

	//Remplissage.
	for (indConcept=0;indConcept<nbConcepts;indConcept++){
	  if (valAnns[indConcept][i]>0){
	    conceptVal[indConcept]=1;
	   // printf(" val: %s",concepts[indConcept]);
	  }
    	}
	int indDist =0;
	for (indDist=0;indDist<k;indDist++){
		int nbPertinent=0;
		float precision=0.0;
		//Init.
	  	int conceptTrain[nbConcepts];
		for (indConcept=0;indConcept<nbConcepts;indConcept++)
			conceptTrain[indConcept]=0;
		//Remplissage.
		for (indConcept=0;indConcept<nbConcepts;indConcept++){
		  if (trainAnns[indConcept][indDist]>=0){
		    conceptTrain[indConcept]=1;
 		    nbPertinent++;
		  }
	  	}
		//calcul precision
		for (indConcept=0;indConcept<nbConcepts;indConcept++){
	 	  if (conceptTrain[indConcept]>0 && conceptTrain[indConcept]==conceptVal[indConcept]){
			precision= precision+1.0;
	  	  }
		}
		if (nbPertinent>0)
		  precision= precision/(float)nbPertinent;
		moyennePrecision[indDist] += precision;
		//printf("precision %d: %f </br>",i,precision);		  
	}
	//determine moyenne.
	for (indConcept=0;indConcept<nbConcepts;indConcept++){
	  if ((moyConcept[indConcept]>seuil && conceptVal[indConcept]>=1 )){
		moyenne= moyenne+1.0;
	  }
	}
	moyenne= moyenne/(float)nbConcepts;
	moyenneTotal+=moyenne;
	moyenne=0.0;
  }
  //tab precision/rappel
  for(i=0;i<k;i++){
	moyennePrecision[i] +=moyennePrecision[i]/(float)nbDocs;
  }

  /* émission de l'entête */
     printf("Content-type: text/html%c%c",10,10);
    printf("<HTML>\n");
    printf("<HEAD>\n");
    afficherStyle(); 
    printf("<TITLE>Evaluation</TITLE>\n");
    printf("<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
    "<script type=\"text/javascript\">"
      "google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});"
    "  google.setOnLoadCallback(drawChart);"
   "   function drawChart() {"
  "      var data = google.visualization.arrayToDataTable([");
   printf("['Rappel', 'Precision'],");
   int indK=0;
   for (indK=0; indK<=k;indK++)
   	printf(" ['%f'  ,  %f],",((float)indK)/10.0,moyennePrecision[indK]/1000.);

   printf("  ]);"
      "  var options = {"
     "     title: 'Courbe Precision/rappel'"
    "    };"
   "     var chart = new google.visualization.LineChart(document.getElementById('chart_div'));"
  "      chart.draw(data, options);"
 "     }"
"    </script>");
  printf("</HEAD>\n");
  printf("<BODY>\n");
  afficherBarreImage();
  printf("</br><b>seuil:</b> %f </br>",seuil);
  printf("<b>moyenne total:</b> %f \n</br>",moyenneTotal/nbDocs);
  //graph
  printf("<div id=\"chart_div\" style=\"width: 900px; height: 500px;\"></div>");
  
  /* Emission de la fin de corps et de document. */
  print_html_tail();
    
  exit(0);
}



