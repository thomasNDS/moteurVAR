#include <stdio.h>
#include <stdlib.h>

#include "cgiu.h"
#include "proc.h"

#define  DEBUG 1

void exitfail()
{
  print_html_tail(); 
  exit(0);
}

int main(int argc, char *argv[])
{
  int x,m;
  char *qs;
  entry *entries;
    
  int nbTrain,nbVal,nbDims,nbConcepts,c,**trainAnns,**valAnns,ret;
  char **trainList,**valList,**concepts,*name;
  float **trainDescriptors,**valDescriptors;
  
  /* r�cup�ration de la cha�ne de param�tres */
  qs = get_query_string(argc,argv);

  /* r�cup�ration des couples (nom,valeur) */
  entries = get_entries(qs,&m);

  /* �mission de l'ent�te */
  print_html_head("R&eacute;sultat");

  /* affichage �ventuel des informations de debug */
  if (DEBUG) print_debug_query(entries,m,qs,argc,argv);

  trainList = readList("/usr/lib/cgi-bin/RIM/images/train/list.txt",&nbTrain);
  if (trainList == NULL) {printf("Failed to read %s file.\n","train/list.txt"); exitfail();}
  if (DEBUG) printf("Read train list, %d images.<BR>\n",nbTrain);
  
  valList = readList("/usr/lib/cgi-bin/RIM/images/val/list.txt",&nbVal);
  if (valList == NULL) {printf("Failed to read %s file.\n","val/list.txt"); exitfail();}
  if (DEBUG) printf("Read val list, %d images.<BR>\n",nbVal);
  
  trainDescriptors = readDescriptors("/usr/lib/cgi-bin/RIM/images/train/descriptors.bin",nbTrain,&nbDims);
  if (trainDescriptors == NULL) {printf("Failed to read %s file.\n","train/descriptors.bin"); exitfail();}
  if (DEBUG) printf("Read train descripors, %d images, %d dimensions.<BR>\n",nbTrain,nbDims);
  
  valDescriptors = readDescriptors("/usr/lib/cgi-bin/RIM/images/val/descriptors.bin",nbVal,&nbDims);
  if (valDescriptors == NULL) {printf("Failed to read %s file.\n","val/descriptors.bin"); exitfail();}
  if (DEBUG) printf("Read val descripors, %d images, %d dimensions.<BR>\n",nbVal,nbDims);
  
  concepts = readList("/usr/lib/cgi-bin/RIM/images/concepts.txt",&nbConcepts);
  if (concepts == NULL) {printf("Failed to read %s file.\n","concepts.txt"); exitfail();}
  if (DEBUG) printf("Read concept list, %d concepts.<BR>\n",nbConcepts);
  
  trainAnns = malloc(nbConcepts*sizeof(int *));
  if (trainAnns == NULL) {printf("malloc() failed.\n"); exitfail();}
  if (DEBUG) printf("Read train annotations:");
  for (c = 0; c < nbConcepts; c ++) {
    ret = asprintf(&name,"/usr/lib/cgi-bin/RIM/images/train/ann/%s.ann",concepts[c]);
    if (ret < 0) {printf("asprintf() failed.\n"); exitfail();}
    trainAnns[c] = readAnnotations(name,nbTrain);
    if (trainAnns[c] == NULL) {printf("Failed to read train/ann%s.ann file.\n",concepts[c]); exitfail();}
    if (DEBUG) printf(" %s",concepts[c]);
  }
  if (DEBUG) printf(".<BR>\n");
  
  valAnns = malloc(nbConcepts*sizeof(int *));
  if (valAnns == NULL) {printf("malloc() failed.\n"); exitfail();}
  if (DEBUG) printf("Read val annotations:");
  for (c = 0; c < nbConcepts; c ++) {
    ret = asprintf(&name,"/usr/lib/cgi-bin/RIM/images/val/ann/%s.ann",concepts[c]);
    if (ret < 0) {printf("asprintf() failed.\n"); exitfail();}
    valAnns[c] = readAnnotations(name,nbVal);
    if (valAnns[c] == NULL) {printf("Failed to read val/ann%s.ann file.\n",concepts[c]); exitfail();}
    if (DEBUG) printf(" %s",concepts[c]);
  }
  if (DEBUG) printf(".<BR>\n");
  
  /* �mission de la fin de corps et de document */
  print_html_tail();
    
  exit(0);
}
