/* **************************** precedence.c ************************************ */
/* Soubor:              precedence.c - Precedencna syntakticka analyza        */
/* Kodovani:            UTF-8                                                 */
/* Datum:               14.12.2014                                             */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ14                  */
/* Varianta zadani:     a/2/II                                                */
/* Titul,Autori, login:         Ing. Natalya Loginova   xlogin00              */
/*                              Jindrich Dudek          xdudek04              */
/*                              Norbert Durcansky       xdurca01              */
/*                              Jan Jusko               xjusko00              */
/*                              Jiri� Dostal          xdosta40           */
/* ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scaner.h"
#include "parser.h"
#include "string.h"
#include "precedent.h"
#include "codegenerate.h"
#include "interpreter.h"
#include "err.h"
#include "stack.h"
#include "ial.h"
#define EXPRESION 100
int PrecedenceSA(LEX_STRUCT*,int,THash_table*,THash_table*, struct record*);
int PrecedentAnal(LEX_STRUCT*,int,THash_table*,THash_table*, struct record*);
int concate(LEX_STRUCT*,int ,THash_table *,THash_table*,struct record *);
struct record* lookforElement(LEX_STRUCT *,int ,THash_table *,THash_table*,struct record*);
int recorderSEM(char);
struct record *SUPPORT2;
int operator=0;
float E1=0;
float E2=0;
float E3=0;
int ZEROIN=0;
TStack *stackPSA;                    /// stack pre tokeny
TStack *stackSEM;                   /// stack pre pravidla
TStack *Helper;                  ///na expresion
int term ;                       ///token
int TOP_Stack;                      ///vrchol zasobnika
int TOP_Stdin;                ///vrchol vstupneho retazca
int concateT;                    ///token
int PODMIENKA_POD=0;            /// vsetky premenne v IF a While musia byt stejneho typu
int PODMIENKA_POD1=0;
int LAST=0;
int EVAL=0;
int LLAST=0;
int RULE=0;
char *X1=NULL;
char *X2=NULL;
char *X3=NULL;
int LASTindex=0;
int CONTROL=0;
int switch_control=0;
int PSA_Stalker;    ///polozka v PSA Table
int ZERO=0;

int PrecedenceTABLE[13][13];

///Pravidla PSA
static int rule0[3]={EXPRESION,PSA_PLUS,EXPRESION};/// pravidlo E->E+E
static int rule1[3]={EXPRESION,PSA_MINUS,EXPRESION};///pravidlo E->E-E
static int rule2[3]={EXPRESION,PSA_MULT,EXPRESION};///pravidlo E->E*E
static int rule3[3]={EXPRESION,PSA_DIVIDE,EXPRESION};/// pravidlo E->E/E
static int rule4[3]={PSA_RIGHT,EXPRESION,PSA_LEFT};///pravidlo E->(E)
static int rule5[3]={PSA_ID,0,0};///pravidlo E->i
static int rule6[3]={EXPRESION,PSA_LESS,EXPRESION};///E->E < E
static int rule7[3]={EXPRESION,PSA_GREATER,EXPRESION};///E->E>E
static int rule8[3]={EXPRESION,PSA_LESSEQ,EXPRESION}; ///E->E<=E
static int rule9[3]={EXPRESION,PSA_GREATEREQ,EXPRESION};///E->E>=E
static int rule10[3]={EXPRESION,PSA_NOTEQUAL,EXPRESION};///E->E<>E
static int *rules[12]={rule0,rule1,rule2,rule3,rule4,rule5,rule6,rule7,rule8,rule9,rule10};
static int scan[3];///bude sa naplnat hodnotami zo zasobnika a nasledne porovnavat s pravidlami

///Funckia nam porovna so vsetkymi pravidlami ak najde
int Comparerule(int *scan)
{
int i=0;
while(i<11)
{
  if(rules[i][0]==scan[0] &&  rules[i][1]==scan[1] &&  rules[i][2]==scan[2])
  {

if(i==0 && CONTROL!=7)
     {

         RULE=1;

         if((E2!=0 && E1!=0 && E3==0) || ZERO==1)
         {
             if(CONTROL==0)
             {
                 generate_inst(X1,X2,E1,E2,ADD,0,0);
                    CONTROL=1;
                    X2=NULL;
                    X1=NULL;
                    E2=0;
             }
             else if(CONTROL==1)
             {

               generate_inst(X2,0,E2,0,ADD,1,0);
               X2=NULL;
               E2=0;
             }
             else if(CONTROL==3)
             {
               generate_inst(X1,X2,E1,0,ADD,3,0);
               X2=NULL;
               X1=NULL;
               E2=0;
               CONTROL=1;
             }
             else if(CONTROL==4)
             {
               generate_inst(0,0,E1,0,ADD,4,0);
               E2=0;
               CONTROL=1;
             }
     }}
         else if (i==2 && CONTROL!=7)
         {

             RULE=1;
            if((E1!=0 && E2!=0 && E3==0)|| ZERO==1)
            {
                if(CONTROL==0)
             {
                 generate_inst(X1,X2,E1,E2,MULTIPLY,0,0);
                 X2=NULL;
                 X1=NULL;
                 CONTROL=1;
                 E2=0;
             }
             else if(CONTROL==1)
             {

               generate_inst(X2,0,E2,0,MULTIPLY,1,0);
                X2=NULL;
               E2=0;
             }
            }else if ((E1!=0 && E2!=0 && E3!=0 )||ZERO==1)
            {

               if(CONTROL==0)
               {
                 generate_inst(X2,X3,E3,E2,MULTIPLY,0,0);
                 X3=NULL;
                 X2=NULL;
                 CONTROL=3;
                 E3=0;
               }
               else if(CONTROL==1)
               {

                 generate_inst(X2,X3,E3,E2,MULTIPLY,0,0);
                 CONTROL=4;
                  X3=NULL;
                  X2=NULL;
                 E3=0;
               }
               else if(CONTROL==3)
               {
                 generate_inst(X3,NULL,E3,E2,MULTIPLY,1,0);
                 CONTROL=3;
                  X3=NULL;
                 E3=0;
               }
               else if(CONTROL==4){
                generate_inst(X2,X3,E3,E2,MULTIPLY,1,0);
                 CONTROL=4;
                  X3=NULL;
                  X2=NULL;
                 E3=0;
               }
            }
         }
         else if(i==1 && CONTROL!=7)
         {
             RULE=1;
             if((E2!=0 && E1!=0 && E3==0)||ZERO==1)
         {
             if(CONTROL==0)
             {
                 generate_inst(X1,X2,E1,E2,MINUS,0,0);
                 CONTROL=1;
                 X2=NULL;
                 X1=NULL;
                 E2=0;
             }
             else if(CONTROL==1)
             {
               generate_inst(X2,0,E2,0,MINUS,1,0);
               X2=NULL;
               E2=0;
             }
             else if(CONTROL==3)
             {
               generate_inst(X1,X2,E1,0,MINUS,3,0);
               X2=NULL;
               X1=NULL;
               E2=0;
               CONTROL=1;
             }
             else if(CONTROL==4)
             {
               generate_inst(NULL,0,E1,0,MINUS,4,0);
               E2=0;
               CONTROL=1;
             }
     }  }


     else if (i==3 && CONTROL!=7)
         {
             RULE=1;
            if((E1!=0 && E2!=0 && E3==0)||ZERO==1)
            {
                if(CONTROL==0)
             {

                 generate_inst(X1,X2,E1,E2,DIVIDE,0,0);
                 X1=NULL;
                 X2=NULL;
                 CONTROL=1;
                 E2=0;
             }
             else if(CONTROL==1)
             {
               generate_inst(X2,0,E2,0,DIVIDE,1,0);
               X2=NULL;
               E2=0;
             }
            }else if (E1!=0 && E2!=0 && E3!=0 )
            {
               if(CONTROL==0)
               {
                 generate_inst(X2,X3,E2,E3,DIVIDE,0,0);
                 CONTROL=3;
                 X2=NULL;
                 X3=NULL;
                 E3=0;
               }
               else if(CONTROL==1)
               {
                 generate_inst(X2,X3,E2,E3,DIVIDE,0,0);
                 CONTROL=4;
                  X3=NULL;
                  X2=NULL;
                 E3=0;
               }
               else if(CONTROL==3)
               {
                 generate_inst(X3,0,E3,E2,DIVIDE,1,0);
                 CONTROL=3;
                 X3=NULL;
                 E3=0;
               }
               else if(CONTROL==4){
                generate_inst(X2,X3,E3,E2,DIVIDE,1,0);
                 CONTROL=4;
                 X3=NULL;
                 X2=NULL;
                 E3=0;
               }
            }
         }
         else if(i==6 )                    ///menej
         {
             if(RULE==0)                       ///ak sa len porovnavaju hodnoty
             generate_inst(X1,0,E1,0,ADD,0,0);
             generate_inst(0,0,0,0,LESS,0,0);
         }
          else if(i==7 )                    ///menej
         {
             if(RULE==0)                       ///ak sa len porovnavaju hodnoty
             generate_inst(X1,0,E1,0,ADD,0,0);
             generate_inst(0,0,0,0,GREATER,0,0);
         }
          else if(i==8 && CONTROL!=7)                    ///menej
         {
             if(RULE==0)                       ///ak sa len porovnavaju hodnoty
             generate_inst(X1,0,E1,0,ADD,0,0);
             generate_inst(0,0,0,0,LESSEQUAL,0,0);
         }
          else if(i==9 )                    ///menej
         {
             if(RULE==0)                       ///ak sa len porovnavaju hodnoty
             generate_inst(X1,0,E1,0,ADD,0,0);
             generate_inst(0,0,0,0,GREATEREQUAL,0,0);
         }
          else if(i==10 )                    ///menej
         {
             if(RULE==0)                       ///ak sa len porovnavaju hodnoty
             generate_inst(X1,0,E1,0,ADD,0,0);
             generate_inst(0,0,0,0,NOTEQUAL,0,0);
         }


      if((LASTindex %2)==0)
        LAST=i;
      else
        LLAST=i;
      LASTindex++;
    return 0;
  }
        i++;
}
return 1;
}
int checklex(int token)
{
 return ((token==E_LEXICAL) ? ERRORRET(token) : E_LEXICAL);
}

///Inicializacia PSA
void initPrecedenceTABLE()
{
PrecedenceTABLE[PSA_PLUS][PSA_PLUS]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_MINUS]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_PLUS][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_PLUS][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_PLUS][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_PLUS][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_LESS]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_GREATER]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_LESSEQ]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_GREATEREQ]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_PLUS][PSA_NOTEQUAL]=PT_GREATER;

PrecedenceTABLE[PSA_MINUS][PSA_PLUS]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_MINUS]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_MINUS][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_MINUS][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_MINUS][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_MINUS][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_LESS]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_GREATER]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_LESSEQ]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_GREATEREQ]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_MINUS][PSA_NOTEQUAL]=PT_GREATER;

PrecedenceTABLE[PSA_MULT][PSA_PLUS]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_MINUS]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_MULT]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_DIVIDE]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_MULT][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_MULT][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_LESS]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_GREATER]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_LESSEQ]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_GREATEREQ]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_MULT][PSA_NOTEQUAL]=PT_GREATER;

PrecedenceTABLE[PSA_DIVIDE][PSA_PLUS]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_MINUS]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_MULT]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_DIVIDE]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_DIVIDE][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_DIVIDE][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_LESS]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_GREATER]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_LESSEQ]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_GREATEREQ]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_DIVIDE][PSA_NOTEQUAL]=PT_GREATER;

PrecedenceTABLE[PSA_ID][PSA_PLUS]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_MINUS]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_MULT]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_DIVIDE]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_ID]=0;
PrecedenceTABLE[PSA_ID][PSA_LEFT]=0;
PrecedenceTABLE[PSA_ID][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_LESS]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_GREATER]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_LESSEQ]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_GREATEREQ]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_ID][PSA_NOTEQUAL]=PT_GREATER;

PrecedenceTABLE[PSA_LEFT][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_RIGHT]=PT_EQUAL;
PrecedenceTABLE[PSA_LEFT][PSA_LESS]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_GREATER]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_LESSEQ]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_GREATEREQ]=PT_LESS;
PrecedenceTABLE[PSA_LEFT][PSA_DOLAR]=0;
PrecedenceTABLE[PSA_LEFT][PSA_NOTEQUAL]=PT_LESS;

PrecedenceTABLE[PSA_RIGHT][PSA_PLUS]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_MINUS]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_MULT]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_DIVIDE]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_ID]=0;
PrecedenceTABLE[PSA_RIGHT][PSA_LEFT]=0;
PrecedenceTABLE[PSA_RIGHT][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_LESS]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_GREATER]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_LESSEQ]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_GREATEREQ]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_RIGHT][PSA_NOTEQUAL]=PT_GREATER;

PrecedenceTABLE[PSA_LESS][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_LESS][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_LESS][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_LESS][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_LESS][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_LESS][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_LESS][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_LESS][PSA_LESS]=0;
PrecedenceTABLE[PSA_LESS][PSA_GREATER]=0;
PrecedenceTABLE[PSA_LESS][PSA_LESSEQ]=0;
PrecedenceTABLE[PSA_LESS][PSA_GREATEREQ]=0;
PrecedenceTABLE[PSA_LESS][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_LESS][PSA_NOTEQUAL]=0;

PrecedenceTABLE[PSA_GREATER][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_GREATER][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_GREATER][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_GREATER][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_GREATER][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_GREATER][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_GREATER][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_GREATER][PSA_LESS]=0;
PrecedenceTABLE[PSA_GREATER][PSA_GREATER]=0;
PrecedenceTABLE[PSA_GREATER][PSA_LESSEQ]=0;
PrecedenceTABLE[PSA_GREATER][PSA_GREATEREQ]=0;
PrecedenceTABLE[PSA_GREATER][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_GREATER][PSA_NOTEQUAL]=0;

PrecedenceTABLE[PSA_GREATEREQ][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_GREATEREQ][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_GREATEREQ][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_GREATEREQ][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_GREATEREQ][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_GREATEREQ][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_GREATEREQ][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_GREATEREQ][PSA_LESS]=0;
PrecedenceTABLE[PSA_GREATEREQ][PSA_GREATER]=0;
PrecedenceTABLE[PSA_GREATEREQ][PSA_LESSEQ]=0;
PrecedenceTABLE[PSA_GREATEREQ][PSA_GREATEREQ]=0;
PrecedenceTABLE[PSA_GREATEREQ][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_GREATEREQ][PSA_NOTEQUAL]=0;

PrecedenceTABLE[PSA_LESSEQ][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_LESSEQ][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_LESSEQ][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_LESSEQ][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_LESSEQ][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_LESSEQ][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_LESSEQ][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_LESSEQ][PSA_LESS]=0;
PrecedenceTABLE[PSA_LESSEQ][PSA_GREATER]=0;
PrecedenceTABLE[PSA_LESSEQ][PSA_LESSEQ]=0;
PrecedenceTABLE[PSA_LESSEQ][PSA_GREATEREQ]=0;
PrecedenceTABLE[PSA_LESSEQ][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_LESSEQ][PSA_NOTEQUAL]=0;

PrecedenceTABLE[PSA_DOLAR][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_RIGHT]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_LESS]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_GREATER]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_LESSEQ]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_GREATEREQ]=PT_LESS;
PrecedenceTABLE[PSA_DOLAR][PSA_DOLAR]=PT_OK;
PrecedenceTABLE[PSA_DOLAR][PSA_NOTEQUAL]=PT_LESS;

PrecedenceTABLE[PSA_NOTEQUAL][PSA_PLUS]=PT_LESS;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_MINUS]=PT_LESS;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_MULT]=PT_LESS;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_DIVIDE]=PT_LESS;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_ID]=PT_LESS;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_LEFT]=PT_LESS;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_RIGHT]=PT_GREATER;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_LESS]=0;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_GREATER]=0;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_LESSEQ]=0;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_GREATEREQ]=0;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_DOLAR]=PT_GREATER;
PrecedenceTABLE[PSA_NOTEQUAL][PSA_NOTEQUAL]=0;

}

///Kontroluje  koniec prikazu
int CheckEND(int end,int type)

{
 switch (end)
 {
   case THEN:
       {
    if(type==IF)
       return 0;
       }
   case DO:
       {
        if(type==WHILE)
        return 0;
       }
   case BODKOCIARKA:
       {
        if(type==ID)
		{
		ID_ENABLE=1;
		return 0;
		}
       }
	case END:
    case END_DOT:
	{
		if(type==ID)
		{
            	ID_ENABLE = 2;
		return 0;
		}
	}
 }
       return ERRORRET(end);
}

///Funkcia dekoduje jednotlive tokeny na indexy do PSA tabulky
int decodeSA(int token)
{
switch(token)
{
case DO:
    return PSA_DOLAR;
case BODKOCIARKA:
    ID_ENABLE = 1;
    return PSA_DOLAR;
case  END_DOT:
    ID_ENABLE = 2;
    return PSA_DOLAR;
case END:
    ID_ENABLE = 2;
    return PSA_DOLAR;
case TRUE:
    return PSA_ID;
case FALSE:
    return PSA_ID;
case THEN:
   return PSA_DOLAR;
case CONST:
    return PSA_ID;
case CONST_STRING:
    return PSA_ID;
case REALo:
    return PSA_ID;
case PLUS:
    return PSA_PLUS;
case MINUS:
    return PSA_MINUS;
case MULTIPLY:
    return PSA_MULT;
case DIVIDE:
    return PSA_DIVIDE;
case ID:
    return PSA_ID;
case LEFT_ROUND:
    return PSA_LEFT;
case RIGHT_ROUND:
    return PSA_RIGHT;
case LESS:
    return PSA_LESS;
case GREATER:
    return PSA_GREATER;
case LESSEQUAL:
    return PSA_LESSEQ;
case GREATEREQUAL:
    return PSA_GREATEREQ;

case NOTEQUAL:
    return PSA_NOTEQUAL;
default:
    return ERRORRET(token);
}
}

///Rekurzivna funkcia redukuje vyraz pomocou rekurze
int reduce(LEX_STRUCT *LEX_STRUCTPTR)
{
	int i = 0;

	while(term != ZARAZKA && term != PSA_DOLAR)
	{
        stack_top(stackPSA, &term);
		scan[i] = term;
		stack_pop(stackPSA);
		stack_top(stackPSA, &term);
		if(i == 2)
			break;
		i++;
	}
		if(term==ZARAZKA)
		stack_pop(stackPSA);
        stack_top(stackPSA, &term);
		stack_push(stackPSA, EXPRESION);



	if(i < 2)
	{
		scan[1] = 0;
		scan[2] = 0;
	}

	if(Comparerule(scan) != 0){
		return ERRORRET(TOP_Stdin);
	}

	PSA_Stalker = PrecedenceTABLE[term][decodeSA(TOP_Stdin)];
	if(PSA_Stalker == 0)
		exit(E_SYNTAX);
	else if(PSA_Stalker == PT_GREATER)
		return reduce(LEX_STRUCTPTR);
	else if(PSA_Stalker == PT_LESS)
        return 0;
    else if (PSA_Stalker==PT_OK)
        return SUCCESS;
return 0;
}


/** \brief Funckia prevadza  PSA

 * \return Succcess ked je top_stdin $ a top_stack $
 *
 */
int PrecedentAnal(LEX_STRUCT *LEX_STRUCTPTR,int type,THash_table *GlobalnaTAB,THash_table*LokalnaTAB,struct record *ELEMENT)
{


if ( TOP_Stdin==LESS || TOP_Stdin==GREATER|| TOP_Stdin==LESSEQUAL || TOP_Stdin==GREATEREQUAL|| TOP_Stdin==NOTEQUAL)
{
    EVAL=1;
    if(type==ID)
        exit(E_SEMANTIC_TYPE);
   switch_control=1;
   if(operator==0)
        operator=1;
   else if(operator==1)
       exit(E_SYNTAX);
}

PSA_Stalker=PrecedenceTABLE[TOP_Stack][decodeSA(TOP_Stdin)];

    if(PSA_Stalker!=0)
    {
     if(PSA_Stalker==PT_LESS)
     {
         stack_push(stackPSA,ZARAZKA);
         stack_push(stackPSA,decodeSA(TOP_Stdin));
         stack_top(stackPSA,&TOP_Stack);
              if(TOP_Stdin==ID)
         {

            ELEMENT=lookforElement(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);

            struct record *SUPPORT=hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str);
            struct record *SUPPORT1=hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str);

             if((ELEMENT->defined!=true_hash && IN_FUNCTION==0 ) || (IN_FUNCTION==1 &&  SUPPORT==0 && SUPPORT1!=0 && ELEMENT->defined!=true_hash) || (IN_FUNCTION==1 &&  SUPPORT!=0 && SUPPORT1!=0 && ELEMENT->defined!=true_hash) || (IN_FUNCTION==1 &&  SUPPORT!=0 && SUPPORT1==0 && ELEMENT->id==FUNCTION_hash && ELEMENT->valuedef!=true_hash))
               exit(E_UNINITIALIZED_VAR);
              if((IN_FUNCTION==0 && ELEMENT->id==FUNCTION_hash))
                exit(E_SEMANTIC_TYPE);


             if (switch_control==0)
             {
                if(PODMIENKA_POD==0)
                PODMIENKA_POD=ELEMENT->type;
             }
             else
             {
               if(PODMIENKA_POD1==0)
                PODMIENKA_POD1=ELEMENT->type;
             }

             VysledokID(Vysledok,ELEMENT->type);

            if(ELEMENT->type==STRING_hash)
              {
               generate_inst(LEX_STRUCTPTR->str,0,0,0,CONCATEID,0,0) ;
                CONTROL=7;
            }

              if(ELEMENT->type==BOOLEAN_hash)
              CONTROL=6;


         if(E1==0 && ZERO!=1 && CONTROL!=7)
         {
         X1=malloc(sizeof(char)*length(LEX_STRUCTPTR->str));
         strcpy(X1,LEX_STRUCTPTR->str);
         E1=-1;
         }
         else if(E2==0 && CONTROL!=7)
         {
         X2=malloc(sizeof(char)*length(LEX_STRUCTPTR->str));
         strcpy(X2,LEX_STRUCTPTR->str);
         E2=-1;
         }else if(E2!=0 && CONTROL!=7)
         {

             X3=malloc(sizeof(char)*length(LEX_STRUCTPTR->str));
             strcpy(X3,LEX_STRUCTPTR->str);
             E3=-1;
         }
         }

         else if(TOP_Stdin ==CONST || TOP_Stdin==TRUE || TOP_Stdin==FALSE|| TOP_Stdin==CONST_STRING || TOP_Stdin==REALo)
         {
             if(TOP_Stdin==TRUE)
                {E1=2;
                CONTROL=6;
                }
             else if(TOP_Stdin==FALSE)
               {E1=1;
               CONTROL=6;
               }

             if(TOP_Stdin==CONST_STRING)
              {CONTROL=7;
               generate_inst(LEX_STRUCTPTR->str,0,0,0,CONCATESTRING,0,0) ;
               }



              if(LEX_STRUCTPTR->value==0 && TOP_Stdin==TRUE && TOP_Stdin==FALSE)
               ZERO=1;

             if (switch_control==0)
             {
                if(PODMIENKA_POD==0)
                PODMIENKA_POD=decodederSEM(TOP_Stdin);

             }
             else
             {
               if(PODMIENKA_POD1==0)
                PODMIENKA_POD1=decodederSEM(TOP_Stdin);
             }
             VysledokID(Vysledok,decodederSEM(TOP_Stdin));

            if(E1==0 && ZERO!=1  && CONTROL!=7)
            {
                 E1=LEX_STRUCTPTR->value;
            }
             else if(((E1!=0 && E2==0)||ZERO==1)&& CONTROL!=7)
                 E2=LEX_STRUCTPTR->value;
             else
                 E3=LEX_STRUCTPTR->value;
         }
                                                                                   ///jedna sa o konstantu overime ci jej hodnota moze byt prirade
         TOP_Stdin=getnextToken(LEX_STRUCTPTR);
            checklex(TOP_Stdin);
          if(PODMIENKA_POD==BOOLEAN_hash)
          {
             if(TOP_Stdin ==PLUS || TOP_Stdin==MULTIPLY || TOP_Stdin==DIVIDE || TOP_Stdin==MINUS)
                exit(E_SEMANTIC_TYPE);
          }

          else if(PODMIENKA_POD==STRING_hash)
          {
             if(TOP_Stdin==MULTIPLY || TOP_Stdin==DIVIDE || TOP_Stdin==MINUS)
                exit(E_SEMANTIC_TYPE);
          }

          else if(TOP_Stdin==DIVIDE)
          {
              if (switch_control==0)
                PODMIENKA_POD=REAL_hash;
              else
                PODMIENKA_POD1=REAL_hash;
          }
        return PrecedentAnal(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
     }
     else if (PSA_Stalker==PT_GREATER)
     {

           term= TOP_Stack;

           if((reduce(LEX_STRUCTPTR))==SUCCESS)
           {
                return 0;
           }

               if(EVAL==1 && CONTROL!=7)
              {
                if(RULE==0)
                    generate_inst(X1,0,E1,0,ADD,0,0);
                    E1=0;
                    E2=0;
                    E3=0;
                    X1=NULL;
                    X2=NULL;
                    X3=NULL;
                    CONTROL=0;
                    RULE=0;
                    generate_inst(0,0,0,0,SAVE,0,0);
                    EVAL=3;
               }else if (CONTROL==7 && EVAL==1)
               {
                 generate_inst(0,0,0,0,SAVE,0,0);
                    CONTROL=0;
                    RULE=0;
                    EVAL=3;
               }

           stack_push(stackPSA,decodeSA(TOP_Stdin));
           stack_top(stackPSA,&TOP_Stack);

           TOP_Stdin=getnextToken(LEX_STRUCTPTR);
           checklex(TOP_Stdin);

           if(PODMIENKA_POD==BOOLEAN_hash)
          {
             if(TOP_Stdin ==PLUS || TOP_Stdin==MULTIPLY || TOP_Stdin==DIVIDE || TOP_Stdin==MINUS)
                exit(E_SEMANTIC_TYPE);
          }
          else
          if(PODMIENKA_POD==STRING_hash)
          {
             if(TOP_Stdin==MULTIPLY || TOP_Stdin==DIVIDE || TOP_Stdin==MINUS)
                exit(E_SEMANTIC_TYPE);
          }
           return PrecedentAnal(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
     }
    }else
    return ERRORRET(TOP_Stdin);
///PRAZDNA ZATVORKA vo vyraze
if(decodeSA(TOP_Stdin)!=term)
    exit(E_SYNTAX);

return 0;
}


///Funckia ktora spravy precedencnu analyzu vyrazu alebo podmienky
int PrecedenceSA(LEX_STRUCT *LEX_STRUCTPTR,int type,THash_table *GlobalnaTAB,THash_table*LokalnaTAB,struct record *ELEMENT)
{
if(((stackPSA=stack_init())==NULL))
    exit(E_INTERNAL);
stack_push(stackPSA,PSA_DOLAR);///inicializacia vrcholu na dolar
stack_top(stackPSA,&TOP_Stack);
initPrecedenceTABLE();

   TOP_Stdin=getnextToken(LEX_STRUCTPTR);
   checklex(TOP_Stdin);
    if(TOP_Stdin!=ID && TOP_Stdin!=LEFT_ROUND && TOP_Stdin!=CONST && TOP_Stdin!=CONST_STRING && TOP_Stdin!=TRUE && TOP_Stdin!=FALSE && TOP_Stdin!=REALo && TOP_Stdin==COPY &&  TOP_Stdin==LENGTH &&  TOP_Stdin==FIND &&   TOP_Stdin==SORT)
        return ERRORRET(TOP_Stdin);

///KONKATENACIA STRINGOV
///********************
///Kontrolujeme ci sa nejedna o priradenie funkcie alebo CONST_string
if((TOP_Stdin==ID || TOP_Stdin==COPY || TOP_Stdin==LENGTH || TOP_Stdin==FIND ||  TOP_Stdin==SORT ) && type==ID)
{
    if(TOP_Stdin==COPY || TOP_Stdin==LENGTH || TOP_Stdin==FIND || TOP_Stdin==SORT)
    {
        switch(TOP_Stdin)
        {
        case COPY:
            VysledokID(Vysledok,STRING_hash);
            break;
        case LENGTH:
            VysledokID(Vysledok,INTEGER_hash);
             break;
        case FIND:
            VysledokID(Vysledok,INTEGER_hash);
            break;
        case SORT:
            VysledokID(Vysledok,STRING_hash);
            break;
        }
        Libraryfunction(TOP_Stdin);
        TOP_Stdin=getnextToken(LEX_STRUCTPTR);
        if(decodeSA(TOP_Stdin)==PSA_DOLAR)
            return SUCCESS;
        else return ERRORRET(TOP_Stdin);
    }
                  ELEMENT=lookforElement(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
                 struct record *SUP=hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str);
                    if(ELEMENT->id==FUNCTION_hash)
                    {
                            if(ELEMENT->defined!=3)
                            {

                                struct record *SUPP=ELEMENT;
                                char *navratova=malloc(sizeof(char)*strlen(LEX_STRUCTPTR->str));
                                strcpy(navratova,LEX_STRUCTPTR->str);

                                 char hack=getc(source);
                                 if(hack=='(')
                                 {
                                generate_inst(LEX_STRUCTPTR->str,0,0,0,CALLFUN,0,0);
                                fseek(source,ftell(source)-1,SEEK_SET);
                                TOP_Stdin=getnextToken(LEX_STRUCTPTR);
                                if(TOP_Stdin==LEFT_ROUND)
                                {


                                    int i=0;

                                    if(SUPP->params!=NULL)
                                    {
                                        while( i< length(SUPP->params))
                                    {

                                        if(i>0)
                                        {
                                             TOP_Stdin=getnextToken(LEX_STRUCTPTR);
                                               if(TOP_Stdin==RIGHT_ROUND || TOP_Stdin!=CIARKA)
                                               exit(E_SEMANTIC_TYPE);
                                        }

                                        TOP_Stdin=getnextToken(LEX_STRUCTPTR);
                                        checklex(TOP_Stdin);


                                        if(TOP_Stdin==ID)
                                        {
                                           ELEMENT=lookforElement(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                           struct record *SUP=hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str);
                                            if(ELEMENT->type != recorderSEM2(SUPP->params[i]))
                                               {

                                                exit(E_SEMANTIC_TYPE);
                                                }


                                           if((ELEMENT->defined!=true_hash && IN_FUNCTION==0) || ( IN_FUNCTION==1 && SUP!=0 && ELEMENT->valuedef!=true_hash))
                                            exit(E_UNINITIALIZED_VAR);

                                            generate_inst(LEX_STRUCTPTR->str,0,LEX_STRUCTPTR->value,0,VALUE,3,0);

                                        }

                                        else if(TOP_Stdin == CONST || TOP_Stdin==REALo || TOP_Stdin==TRUE || TOP_Stdin==FALSE|| TOP_Stdin==CONST_STRING)
                                        {
                                            if( TOP_Stdin!=TRUE && TOP_Stdin!=FALSE)
                                              generate_inst(LEX_STRUCTPTR->str,NULL,LEX_STRUCTPTR->value,0,VALUE,0,0);
                                            else
                                               generate_inst(LEX_STRUCTPTR->str,0,TOP_Stdin,0,VALUE,1,0);

                                            if(TOP_Stdin==TRUE || TOP_Stdin==FALSE)
                                            {
                                                TOP_Stdin=BOOLEAN;
                                            }
                                            if(TOP_Stdin!=recorderSEM(SUPP->params[i]))
                                            {

                                                exit(E_SEMANTIC_TYPE);
                                            }

                                        }else if(TOP_Stdin==RIGHT_ROUND)
                                            exit(E_SEMANTIC_TYPE);

                                         else return ERRORRET(TOP_Stdin);
                                        i++;
                                    }
                                    }
                                    generate_inst(navratova,0,0,0,FMOF,0,0);
                                    free(navratova);
                                    TOP_Stdin=getnextToken(LEX_STRUCTPTR);
                                    if(TOP_Stdin==RIGHT_ROUND || TOP_Stdin==CIARKA)
                                    {
                                        VysledokID(Vysledok,SUPP->type);            ///SKONTROLUJ NAVRATOVY TYP FUNKCIE

                                        if(TOP_Stdin==CIARKA)
                                        {
                                            exit(E_SEMANTIC_TYPE);
                                        }

                                        TOP_Stdin=getnextToken(LEX_STRUCTPTR);
                                        if(decodeSA(TOP_Stdin)==PSA_DOLAR)
                                            return SUCCESS;
                                        else return ERRORRET(TOP_Stdin);

                                    }else return ERRORRET(TOP_Stdin);


                                    }else return ERRORRET(TOP_Stdin);
                                    }
                                    else{
                                    fseek(source,ftell(source)-1,SEEK_SET);
                                    PrecedentAnal(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                        }                                                               ///dorobit priradovanie funkcie

                            }
                            else exit(E_SEMANTIC_UNDEF);
                    }
                    else if(ELEMENT->type==STRING_hash)                 ///semanticka kontrola pri konkatenaci
                    {
                        if((ELEMENT->defined!=true_hash && IN_FUNCTION==0) || ( IN_FUNCTION==1 && SUP!=0 && ELEMENT->valuedef!=true_hash))
                            exit(E_UNINITIALIZED_VAR);
                      VysledokID(Vysledok,ELEMENT->type);
                      generate_inst(LEX_STRUCTPTR->str,0,0,0,CONCATEID,0,0);
                      concate(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
                       ///zavolaj konkatenaciu retazcov
                      return 0;
                    }
                    else if(ELEMENT->type==BOOLEAN_hash)
                    {
                      VysledokID(Vysledok,ELEMENT->type);
                      int token=getnextToken(LEX_STRUCTPTR);
                      if((ELEMENT->defined!=true_hash && IN_FUNCTION==0) || ( IN_FUNCTION==1 && SUP!=0 && ELEMENT->valuedef!=true_hash))
                            exit(E_UNINITIALIZED_VAR);
                      if(decodeSA(token)==PSA_DOLAR)
                       {
                           CheckEND(token,type);
                            return SUCCESS;
                       }else
                        if(token==PLUS || token==MINUS || token==MULTIPLY || token==DIVIDE)
                              exit(E_SEMANTIC_TYPE);
                        else return ERRORRET(token);
                    }
                    else
                    PrecedentAnal(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                ///jedna sa o normalne priradenie
}
///Bude sa jednat o kokatenaciu retazcov  ++ dodat ked ID je CONST_STRING tak nech tiez konkatenuje +semanticku kontrolu
else if(TOP_Stdin==CONST_STRING && type==ID)
{
    VysledokID(Vysledok,STRING_hash);
    generate_inst(LEX_STRUCTPTR->str,0,0,0,CONCATESTRING,0,0);
    concate(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
    return SUCCESS;
}
else if ((TOP_Stdin==TRUE || TOP_Stdin==FALSE)&& type==ID)
{
   VysledokID(Vysledok,BOOLEAN_hash);
   int token=getnextToken(LEX_STRUCTPTR);
   if(decodeSA(token)==PSA_DOLAR)
   {
       generate_inst(NULL,0,TOP_Stdin,0,ADD,0,0);    ///OK
        CheckEND(token,type);
        return SUCCESS;
   }else
   if(token==PLUS || token==MINUS || token==MULTIPLY || token==DIVIDE)
      exit(E_SEMANTIC_TYPE);
   else return ERRORRET(token);
}
///***********************
else
{
PrecedentAnal(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
}

if(type==IF || type==WHILE )
{
if(switch_control==0 && PODMIENKA_POD==BOOLEAN_hash)         ///VYRAZ BEZ <>= a je typu boolean
    PODMIENKA_POD1=BOOLEAN_hash;

else if(PODMIENKA_POD!=PODMIENKA_POD1 && Vysledok==PODMIENKA)
    exit(E_SEMANTIC_TYPE);
    ///cely vyraz sa vyhodnoti ako boolean a nic sa k nemu nesmie pricitat
if((Vysledok==PODMIENKA && PODMIENKA_POD!=BOOLEAN_hash )  && (LLAST!=10 && LLAST!=9 && LLAST!=8 && LLAST!=7 && LLAST!=6 && LLAST!=4 && LAST!=10 && LAST!=9 && LAST!=8 && LAST!=7&& LAST!=6 && LAST!=4))
exit(E_SEMANTIC_TYPE);
}
if(type==ID && Vysledok!=PODMIENKA_POD)
exit(E_SEMANTIC_TYPE);

///Generovat mov pri ziadnych operatoroch
if(RULE==0 && CONTROL!=7 && type==ID)
{
generate_inst(X1,0,E1,0,ADD,0,0);
}


if(EVAL==0 && Vysledok==PODMIENKA && CONTROL==6 && (E1==2 || E1==1||E1==-1))           /// WHILE TRUE alebo IF true,false samostatna podmienka alebo cyklus
 {
  generate_inst(X1,0,E1,1,MINUS,0,0);
 }
RULE=0;
CONTROL=0;
E1=0;
E2=0;
E3=0;
X1=NULL;
X2=NULL;
X3=NULL;
ZERO=0;
EVAL=0;
PODMIENKA_POD=0;
PODMIENKA_POD1=0;
switch_control=0;
LAST=0;
LLAST=0;
LASTindex=0;
stack_free(stackPSA);
operator=0;
  return SUCCESS;
}

///FUNKCIA sa stara o konkatenaciu 2 retazcov ID:='PR'+'D'
int concate(LEX_STRUCT *LEX_STRUCTPTR,int type,THash_table *GlobalnaTAB,THash_table*LokalnaTAB,struct record *ELEMENT)
{
    concateT=getnextToken(LEX_STRUCTPTR);


    if(concateT == PLUS)
    {

       concateT=getnextToken(LEX_STRUCTPTR);

       if(concateT==ID)
       {

           ELEMENT=lookforElement(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
           struct record *SUP=hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str);
                ///zistime ci existuje v tabulke
           VysledokID(Vysledok,ELEMENT->type);
           if((ELEMENT->defined!=true_hash && IN_FUNCTION==0) || ( IN_FUNCTION==1 && SUP!=0 && ELEMENT->valuedef!=true_hash))
                            exit(E_UNINITIALIZED_VAR);
                generate_inst(LEX_STRUCTPTR->str,0,0,0,CONCATEID,0,0);
           return concate(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);
       }
       else if(concateT==CONST_STRING)
       {

           generate_inst(LEX_STRUCTPTR->str,0,0,0,CONCATESTRING,0,0);
           return concate(LEX_STRUCTPTR,type,GlobalnaTAB,LokalnaTAB,ELEMENT);


       }
            else return ERRORRET(concateT);

    }else if((decodeSA(concateT)==PSA_DOLAR) || (type==PODMIENKA && ( concateT==LESS ||concateT==GREATER||concateT==LESSEQUAL||concateT==GREATEREQUAL||concateT==NOTEQUAL)))
    {
        CheckEND(concateT,type);
        return SUCCESS;
    }
    else
    if(concateT==MINUS || concateT==MULTIPLY || concateT==DIVIDE)
      exit(E_SEMANTIC_TYPE);
    else
        return ERRORRET(concateT);
}
////KONTROLA PRE PRIRADENIE HODNOT
int VysledokID(int Vysledok,int id )
{
    switch(Vysledok)
    {
    case INTEGER_hash:
        if(id!=INTEGER_hash)
            exit(E_SEMANTIC_TYPE);
            break;
    case STRING_hash:
        if(id!=STRING_hash)
            exit(E_SEMANTIC_TYPE);
            break;
    case REAL_hash:
        if(id==STRING_hash || id==BOOLEAN_hash )
           exit(E_SEMANTIC_TYPE);
        else
            if(id==INTEGER_hash && PODMIENKA_POD!=REAL_hash)
            PODMIENKA_POD=INTEGER_hash;
            else
            PODMIENKA_POD=REAL_hash;
           break;
    case BOOLEAN_hash:
           if(id!=BOOLEAN_hash)
            exit(E_SEMANTIC_TYPE);
            break;

    case PODMIENKA:
    {
             if(switch_control==0)
             {
                 if((id==INTEGER_hash && PODMIENKA_POD==0) || (id==INTEGER_hash && PODMIENKA_POD==INTEGER_hash))
                    PODMIENKA_POD=INTEGER_hash;
                 else if(id==INTEGER_hash || id==REAL_hash)
                    PODMIENKA_POD=REAL_hash;
                  else  if(id!=PODMIENKA_POD)
                     exit(E_SEMANTIC_TYPE);
             }
             else if (switch_control==1)
             {
                 if((id==INTEGER_hash && PODMIENKA_POD1==0) || (id==INTEGER_hash && PODMIENKA_POD1==INTEGER_hash))
                    PODMIENKA_POD1=INTEGER_hash;
                 else if(id==INTEGER_hash || id==REAL_hash)
                    PODMIENKA_POD1=REAL_hash;
                 else  if(id!=PODMIENKA_POD)
                     exit(E_SEMANTIC_TYPE);
             }
      }
    }
   return 0;
}
///VYHLADAVANIE V TABULKACH CI MAME DEFINOVANY ID
 struct record* lookforElement(LEX_STRUCT *LEX_STRUCTPTR,int type,THash_table *GlobalnaTAB,THash_table*LokalnaTAB,struct record *ELEMENT)
{
                    if(IN_FUNCTION==0)///Kontrola ci je definovana
                     {ELEMENT=(hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str));

                     }
                    else
                    {

                     ELEMENT=(hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str));
                    }
                    if(ELEMENT==0)
                    {
                        ELEMENT=(hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str));
                        if(ELEMENT!=0)
                        {
                                return ELEMENT;
                        }else{

                            exit(E_SEMANTIC_UNDEF);}
                    }
return ELEMENT;
}

///ZISTUJEME Z POLA PARAMETROV FUNCKIE JEDNOTLIVE TYPY
int recorderSEM(char c)
{
    switch(c){

case 'i':
    return CONST;
case 'b':
    return BOOLEAN;
case 'r':
    return REALo;
case 's':
    return CONST_STRING;
default:
    exit(E_SEMANTIC_TYPE);
    }
    return 0;
}

int recorderSEM2(char c)
{
     switch(c){

case 'i':
    return INTEGER_hash;
case 'b':
    return BOOLEAN_hash;
case 'r':
    return REAL_hash;
case 's':
    return STRING_hash;
default:
    exit(E_SEMANTIC_TYPE);
}
    return 0;

}









