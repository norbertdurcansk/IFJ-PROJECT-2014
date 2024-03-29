/* **************************** main.c ************************************** */
/* Soubor:              main.c - Riadiacy cast interpretu                     */
/* Kodovani:            UTF-8                                                 */
/* Datum:               14.12.2014                                               */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ14                  */
/* Varianta zadani:     a/2/II                                                */
/* Titul,Autori, login:         Ing. Natalya Loginova   xlogin00              */
/*                              Jindrich Dudek          xdudek04              */
/*                              Norbert Durcansky       xdurca01              */
/*                              Jan Jusko               xjusko00              */
/*                              Jiri� Dostal             xdosta40              */
/* ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "scaner.h"
#include "parser.h"
#include "interpreter.h"
#include "codegenerate.h"

int main(int argc, char *argv[])
{


FILE *source;
/*source file opening*/

  if (argc == 1)
   {
      fprintf(stderr,"No input file\n");
      return 0;
   }

  if ((source = fopen(argv[1],"r")) == NULL)
   {
      fprintf(stderr,"Failed to open\n");

      return 0;
   }


/* Inicializcia zdrojoveho suboru pre Lexikalnu analyzu */

SourceInitialize(source);
SyntacticAnalys(); /// SYNTAKTICKY ANALYZATOR BUDE VOLAT GET NEXT TOKEN
searchrecord(InstructionTape);


fclose(source);
return 0;
}
