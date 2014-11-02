/* **************************** scaner.c ************************************ */
/* Soubor:              scaner.c - Lexikalna analyza                          */
/* Kodovani:            UTF-8                                                 */
/* Datum:               11.2014                                               */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ14                  */
/* Varianta zadani:     a/2/II                                                */
/* Titul,Autori, login:         Ing. Natalya Loginova   xlogin00              */
/*                              Jindrich Dudek          xdudek04              */
/*                              Norbert Durcansky       xdurca01              */
/*                              Jan Jusko               xjusko00              */
/*                              Jiří Dostál             xdosta40              */
/* ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include "parser.h"
#include "err.h"
#include "string.h"
#include "scaner.h"

FILE *source;

//klucove slova jazyka IFJ14
char *key[]={"begin","end","boolean","do",
"else","false","find","forward",
"function","if","integer","readln","real","sort",
"string","then","true","var","while","write"
};

/** \brief funkcia inicializuje zdrojovy subor
 *
 * \param  File pointer
 *
 */
void SourceInitialize(FILE *f)
{

source=f;

}

/** \brief funkcia vrati dalsi LEX_STRUCT , volana syntaktickou analyzou
 * \param  char pointer
 * \return LEX_STRUCT alebo lexikalna chyba
 */

int getnextToken (LEX_STRUCT *LEX_STRUCTPTR)    // parameter sa bude predavat ukazatel na strukturu LEX_STRUCT  viz string.h + ukazatel na pole
{
  LEX_STRUCTPTR->value=0;    //vynulujeme hodnotu  value
  strClear(LEX_STRUCTPTR);   //vymazeme pole aby sme mohli znova nacitat

char c;                     // premenna do ktorej si ukladame znak
int state=0;                // stav k ktorom sme po nacitani znaku
int plusminus=0;            //hodnota ci bol nacitany plus alebo minus
double value=0;            // pomocna premenna na vypocet const
int u=1;                   // pomocna premenna do cyklu
int plus_s=0; //           // premenna ma hodnotu 1 ked sa nacita +

//hlavny cyklus vypoctu Tokenu
    while(1)
    {

c=getc(source);    //nacitame znak

//stav kedy po nacitani apostrofu nacitame CONST_STRING
if(state==APOSTROF)
{
   if(c!=39)
    {
        AddChar_str(LEX_STRUCTPTR,c);  //dokym nemame dalsi apostrof nacitame do pola
    }
  else
    if(c==39)              //mame dalsi apostrof tak zistime ci dalsi znak je apostrof alebo hashtag alebo nieco ine
   {
       c=getc(source);
        if(c=='#')
        {
        c=getc(source);
        state=HASHTAG;
        }

    else
    if(c==39)
    {
     AddChar_str(LEX_STRUCTPTR,c);
    }
    else
        if(c==EOF)
    {
        return CONST_STRING;
    }
    else
   {
       fseek(source,ftell(source)-1,SEEK_SET);
       return CONST_STRING;
   }
   }
}

//pri nacitani HASHTAGU ma nasledovat cislo
if(state==HASHTAG)
{
if((c >= '0') && (c <='9'))
{
LEX_STRUCTPTR->value=(LEX_STRUCTPTR->value*10)+(c-'0');
plus_s=1;
}
else
    if((plus_s==1) && (LEX_STRUCTPTR->value<256) &&(c==39) && (LEX_STRUCTPTR->value>0))
{
    AddChar_str(LEX_STRUCTPTR,(int)LEX_STRUCTPTR->value);
    LEX_STRUCTPTR->value=0;
    state=APOSTROF;
}
    else
    return E_LEXICAL;
}


///ODSTRANENIE KOMENTOV
if(c=='{' )                 //komentar zacina zlozenou zatvorkou
    state=KOMENT;
if ((state==KOMENT) && (c=='}'))
    state=0;
///********************************

if(state==REALo)
{
if((c >= '0') && (c <='9'))  //nacitame cisla
{
value=(value*10)+(c-'0');
}

else if(value==0)
{
if(plusminus==0)
{
fseek(source,ftell(source)-1,SEEK_SET);
return E_LEXICAL;
}
if(c==EOF)
{
fseek(source,ftell(source)-2,SEEK_SET);   //mame nacita napr 12+e a za "e" neni ziadne cislo
return CONST;
}
else
fseek(source,ftell(source)-3,SEEK_SET);   //mame nacita napr 12+e a za "e" neni ziadne cislo
return CONST;
}

else if((isspace(c)!=0) || c=='+'|| c=='-'|| c=='*'|| c=='/'|| c==';' || c==')'|| c==']' || c==','|| c== '<'|| c=='>' || c=='='||c==':' ) //tieto znaky nasleduju za cislom
{
LEX_STRUCTPTR->value= LEX_STRUCTPTR->value*pow(10,value);
if(plus_s==1)
LEX_STRUCTPTR->value=LEX_STRUCTPTR->value*(-1);

fseek(source,ftell(source)-1,SEEK_SET);
return CONST;
}
else
if (c==EOF )
{
LEX_STRUCTPTR->value= LEX_STRUCTPTR->value*pow(10,value);
if(plus_s==1)
LEX_STRUCTPTR->value=LEX_STRUCTPTR->value*(-1);
return CONST;
}

else
    if((((c>=65)&&(c<=90))||  ((c>=97)&&(c<=122))||(c=='_')))
    return E_LEXICAL;
}

if(state==DOT)
{

if((c >= '0') && (c <='9')){
LEX_STRUCTPTR->value=(LEX_STRUCTPTR->value+((c-'0')*pow(10,-u)));
u++;
}
if((c < '0') || (c >'9'))
{
state=CONST;
}
}

///STAVY TVORENE JEDNOU LEXEMOU
if(state==0){
switch(c) {
case '<' :
    return LESS;
case '>' :
       return GREATER;
case ',' :
    return CIARKA;
case '=' :
    return EQUAL;
case '.':
    return DOT;
case ':' :
     return DVOJBODKA;
case ';' :
     return BODKOCIARKA;
case '+' :
     return PLUS;
case '-' :
     return MINUS;
case '/' :
     return DIVIDE;
case '*' :
    return MULTIPLY;
case '(' :
    return LEFT_ROUND;
case ')' :
    return RIGHT_ROUND;
case 39 :
    {
        state=APOSTROF;
        break;
    }
case '[' :
    return LEFT_HRANATA;
case ']' :
    return RIGHT_HRANATA;
}
}
///CISLA A KONSTANTY

if((c >= '0') && (c <='9')&&(state==0))
 state=CONST;

if((c >= '0') && (c <='9')&& (state==CONST)){
LEX_STRUCTPTR->value=(LEX_STRUCTPTR->value*10)+(c-'0');
}

if((c < '0') || (c >'9')){
if(state==CONST){
 if(isspace(c)!=0)
{
    return CONST;
 }

else if(c=='-')
{
state=REAL;
plus_s=1;
}
else if(c=='.')
state=DOT;


else if(c=='+')
state=REAL;


else if((c=='e') || (c=='E'))
state=REALo;

else if ((c==';')|| (c=='*') || (c=='/')||(c==')') || c==']' || c==','|| c== '<'|| c=='>' || c=='=' ||c==':')
{
     fseek(source,ftell(source)-1,SEEK_SET);
     return CONST;
 }
 else
    if(c==EOF)
 {
return CONST;
 }
 else
    return E_LEXICAL;

}
}


if(state==REAL)
{
plusminus=1;
c=getc(source);
if((c=='e') || (c=='E'))
   state=REALo;
else
{
    fseek(source,ftell(source)-2,SEEK_SET);
     return CONST;
}
}
///*********************************************************** identifikatory a klucove slova

if((((c>=65)&&(c<=90))||  ((c>=97)&&(c<=122))||(c=='_'))&& (state==0))
{
state=STRING;
}
if(state==STRING)
{
if(((((c>=65)&&(c<=90))||  ((c>=97)&&(c<=122))||(c=='_'))) || ((c >= '0') && (c <='9')))

    AddChar_str(LEX_STRUCTPTR,c);

    else
    {
    for(int i=0;i<20;i++)
    {

     if((CmpConst_str(LEX_STRUCTPTR,key[i]))==0)
     {
        if(c!=EOF)
        fseek(source,ftell(source)-1,SEEK_SET);
         switch(i)
         {
             case 0:return BEGIN;
             case 1:return END;
             case 2:return BOOLEAN;
             case 3:return DO;
             case 4:return ELSE;
             case 5:return FALSE;
             case 6:return FIND;
             case 7:return FORWARD;
             case 8:return FUNCTION;
             case 9:return IF;
             case 10:return INTEGER;
             case 11:return READLN;
             case 12:return REAL;
             case 13:return SORT;
             case 14:return STRING;
             case 15:return THEN;
             case 16:return TRUE;
             case 17:return VAR;
             case 18:return WHILE;
             case 19:return WRITE;
         }
     }
    }
    if (c==EOF)
    {
    return ID;
    }
    else{
   fseek(source,ftell(source)-1,SEEK_SET);
    return ID;}
    }

}


if(c==EOF)
{   if(state==KOMENT)
        return SUCCESS;
    else
        if(state!=0)
        return E_LEXICAL;
    else
        return SUCCESS;
}


if((isspace(c)==0)&&(state==0)&&(c!='}'))   //chyba ak mame nedefinovany stav znak ktory nepozname
{
  return E_LEXICAL;
}

    }

}


