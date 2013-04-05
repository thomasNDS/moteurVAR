#include <stdio.h>
#include <stdlib.h>
#include "cgiu.h"

#define  DEBUG 0


main(int argc, char *argv[]){
    int x,m;
    char *qs,*nom,*info;
    entry *entries;


    /* récupération de la chaîne de paramètres */
    qs = get_query_string(argc,argv);

    /* récupération des couples (nom,valeur) */
    entries = get_entries(qs,&m);

    /* émission de l'entête */
    print_html_head("Moteur de Recherche");
    printDebutBody();
    
    /* affichage éventuel des informations de debug */
    if (DEBUG) print_debug_query(entries,m,qs,argc,argv);
    /* récupération des paramètres */
    nom  = getparam(entries,m,"NOM");
   // info = getparam(entries,m,"INFO");
    char* nbReq = getparam(entries,m,"NB");	

     /* affichage des paramètres 
    printf("\n\n<b>Param&egrave;tres re&ccedil;u(s) en entr&eacute;e</b>\n");
    printf("<ul>\n");
    if (nom != NULL) {
        printf("<li> recherche : <code>%s</code>\n",nom);
    }
    if (info != NULL) {
        printf("<li> type de recherche : <code>%s</code>\n",info);
    }
    printf("</ul>\n");*/
    printf(" <hr color=#000444 size=3px>  <br/><br/> </FORM>");
   //On passe la main au perl
  char str[200];
  strcpy (str,"perl main.pl ");
  strcat (str,"TEXTE"); 
  strcat (str," ");
  strcat (str,nbReq); 
  strcat (str," ");
  strcat (str,nom); 
  char buf1[BUFSIZ]; //BUFSIZ est une constante connue du système
  FILE *ptr;
 double temps;
 clock_t start;

  start = clock();
  if ((ptr = popen(str, "r")) != NULL){
    // chrnomètre.
   // temps = (double)(clock()-start)*(double) 1e-6;
  //  printf("\n<h1>Recherche terminée en %.2f seconde(s)!\n</h1>", temps);

    while (fgets(buf1, BUFSIZ, ptr) != NULL) {
      (void) printf("%s", buf1);
    }
    pclose(ptr);
  } else {
    fprintf(stderr, "Echec de popen\n");
    exit(1);
  }
   
  
    /* émission de la fin de corps et de document */
    print_html_tail();
}
