/* **************************** codegenerate.c ************************************ */
/* Soubor:             codegenerate.c - Generovanie kodu a jeho vykonanie      */
/* Kodovani:            UTF-8                                                 */
/* Datum:               11.2014                                               */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ14                  */
/* Varianta zadani:     a/2/II                                                */
/* Titul,Autori, login:         Ing. Natalya Loginova   xlogin00              */
/*                              Jindrich Dudek          xdudek04              */
/*                              Norbert Durcansky       xdurca01              */
/*                              Jan Jusko               xjusko00              */
/*                              Jiøí Dostál             xdosta40              */
/* ****************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scaner.h"
#include "parser.h"
#include "string.h"
#include "err.h"
#include "ial.h"
#include "stack.h"
#include "precedent.h"
#include "codegenerate.h"
int JUMPL=0;

int init_generate()
{
    InstructionTape=(inf_pointer_array*)malloc(sizeof(inf_pointer_array));
    init_pointer_array(InstructionTape);
return 0;
}


///Hlavicka sa mozno zmeni este
int generate_inst(char *A,char *A2,float B,float C,int CODE,int what)
{

INSTape *Instruction;
if((Instruction=(INSTape*)malloc(sizeof(INSTape)))==NULL)
        exit(E_INTERNAL);

switch (CODE)
{
case BEGIN_MAIN:
    {
    Instruction->a=NULL;
    Instruction->b=0;
    Instruction->CODE=BEGIN_MAIN;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case LENGTH:
    {
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->CODE=LENGTH;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case LENGTHID:
    {
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->CODE=LENGTHID;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case END_MAIN:
    {
    Instruction->a=NULL;
    Instruction->b=0;
    Instruction->CODE=END_MAIN;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case MOV:
    {
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->CODE=MOV;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case WRITESTRING:
    {
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->CODE=WRITESTRING;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case WRITEINT:
    {
    Instruction->a=NULL;
    Instruction->b=B;
    Instruction->CODE=WRITEINT;
    add_new_pointer(InstructionTape,Instruction);
    break;
    }
case WRITEBOOL:
    Instruction->a=NULL;
    Instruction->b=B;
    Instruction->CODE=WRITEBOOL;
    add_new_pointer(InstructionTape,Instruction);
    break;
case WRITEREAL:
    Instruction->a=NULL;
    Instruction->b=B;
    Instruction->CODE=WRITEREAL;
    add_new_pointer(InstructionTape,Instruction);
    break;
case WRITEID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->CODE=WRITEID;
    add_new_pointer(InstructionTape,Instruction);
    break;
case READLN:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->CODE=READLN;
    add_new_pointer(InstructionTape,Instruction);
    break;

case COPYSTRING:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYSTRING;
    add_new_pointer(InstructionTape,Instruction);
    break;
case COPYID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYID;
    add_new_pointer(InstructionTape,Instruction);
    break;
case SORTSTRING:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->c=0;
    Instruction->CODE=SORTSTRING;
    add_new_pointer(InstructionTape,Instruction);
     break;

case SORTID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->c=0;
    Instruction->CODE=SORTID;
    add_new_pointer(InstructionTape,Instruction);
     break;
case CONCATESTRING:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->c=0;
    Instruction->CODE=CONCATESTRING;
    add_new_pointer(InstructionTape,Instruction);
    break;
case CONCATEID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=0;
    Instruction->c=0;
    Instruction->CODE=CONCATEID;
    add_new_pointer(InstructionTape,Instruction);
    break;
case ADD:
    Instruction->a=A;
    Instruction->a2=A2;
    Instruction->specialcode=what;
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=ADD;
    add_new_pointer(InstructionTape,Instruction);
    break;

case MULTIPLY:
    Instruction->a=A;
    Instruction->a2=A2;
    Instruction->specialcode=what;
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=MULTIPLY;
    add_new_pointer(InstructionTape,Instruction);
    break;

case DIVIDE:
    Instruction->a=A;
    Instruction->a2=A2;
    Instruction->specialcode=what;
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=DIVIDE;
    add_new_pointer(InstructionTape,Instruction);
    break;
case MINUS:
     Instruction->a=A;
    Instruction->a2=A2;
    Instruction->specialcode=what;
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=MINUS;
    add_new_pointer(InstructionTape,Instruction);
    break;
case EQUAL:
    Instruction->a=NULL;
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=EQUAL;
    add_new_pointer(InstructionTape,Instruction);
    break;
case FINDSTRSTR:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    Instruction->a2=(char*)malloc(sizeof(char)*length(A2));
    strcpy(Instruction->a,A);
    strcpy(Instruction->a2,A2);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=FINDSTRSTR;
    add_new_pointer(InstructionTape,Instruction);
    break;

case FINDIDSTR:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    Instruction->a2=(char*)malloc(sizeof(char)*length(A2));
    strcpy(Instruction->a,A);
    strcpy(Instruction->a2,A2);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=FINDIDSTR;
    add_new_pointer(InstructionTape,Instruction);
    break;

case FINDSTRID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    Instruction->a2=(char*)malloc(sizeof(char)*length(A2));
    strcpy(Instruction->a,A);
    strcpy(Instruction->a2,A2);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=FINDSTRID;
    add_new_pointer(InstructionTape,Instruction);
    break;

case FINDIDID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    Instruction->a2=(char*)malloc(sizeof(char)*length(A2));
    strcpy(Instruction->a,A);
    strcpy(Instruction->a2,A2);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=FINDIDID;
    add_new_pointer(InstructionTape,Instruction);
    break;


case COPYSTRINGID_:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYSTRINGID_;
    add_new_pointer(InstructionTape,Instruction);
    break;

case COPYSTRING_ID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYSTRING_ID;
    add_new_pointer(InstructionTape,Instruction);
    break;

case COPYSTRINGIDID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYSTRINGIDID;
    add_new_pointer(InstructionTape,Instruction);
    break;


case COPYIDID_:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYIDID_;
    add_new_pointer(InstructionTape,Instruction);
    break;

case COPYID_ID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYID_ID;
    add_new_pointer(InstructionTape,Instruction);
    break;

case COPYIDIDID:
    Instruction->a=(char*)malloc(sizeof(char)*length(A));
    strcpy(Instruction->a,A);
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=COPYIDIDID;
    add_new_pointer(InstructionTape,Instruction);
    break;

case JUMP:
    Instruction->a=NULL;
    Instruction->b=B;
    Instruction->c=C;
    Instruction->CODE=JUMP;
    add_new_pointer(InstructionTape,Instruction);
    break;
}

    return 0;
}



















































int init_pointer_array(inf_pointer_array *a)
{
    a->pole = (INSTape **)malloc(sizeof(INSTape)*100);
    if(a->pole == NULL)
        return -1;

    a->allocSize = 100;
    a->length = 0;
return 0;
}


void free_pointer_array(inf_pointer_array *a)
{
    free(a->pole);
    return;
}


int add_new_pointer(inf_pointer_array *a, INSTape* pointer)
{
    if((a->allocSize - a->length) <= 2) //ak je nedostatok volneho miesta v poli tak realokuj
    {
        a->pole = realloc(a->pole, a->allocSize + (100 * sizeof(INSTape*)) );
        if (a->pole == NULL)
            return -1;

        a->allocSize = a->allocSize + 100; //aktualizacia premennej
    }

    a->pole[a->length] = pointer;
    a->length = a->length + 1;

    return 1;
}




