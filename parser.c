/* **************************** parser.c ************************************ */
/* Soubor:              parser.c - Syntakticka a Semanticka analyza           */
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

LEX_STRUCT *LEX_STRUCTPTR;          ///sktruktura pre Tokeny
LEX_STRUCT *ARRAY_PARAM;          ///pole pre parametre funkcie
int IF_ENABLE=0;                ///na pouzitie else v progcondition
inf_array *POLE_ID_GLOBAL;   ///nekonecne pole ID pre globalnu
inf_array *POLE_ID_LOCAL;   ///nekonecne pole ID pre lokalnu
inf_array *POLE_ID_GLOBALFUN;   ///nekonecne pole ID pre globalnu

inf_array *SUPPORT_POLE;  /// pomocna
THash_table *GlobalnaTAB; ///globalna hashovacia tabulka
THash_table *LokalnaTAB; ///lokalna hashovacia tabulka

int FUNCTION_ENABLE=0;        ///Moze nasledovat telo funkcie
int POLE_ID_INDEX=0;        ///Index Dalsieho zaciatku ID
struct record *ELEMENT;   ///zaznam pre hashovaciu funkciu
struct record *SUPPORT;   ///zaznam pre hashovaciu funkciu
int CHECK_FUN=0;
int Declar=0;


///funckia na overenie LEXIKALNA vs SYNTAKTICKA CHYBA
int ERRORRET(int value)
{
    if(value==E_LEXICAL)
        exit(E_LEXICAL);
    else
        exit(E_SYNTAX);
}



///<PROGRAM>  <FUNCTION> v <VAR> v <PROG>
/** \brief Koren derivacneho stromu, neterminal,
 plati pre tokeny < var > or <begin> or  <function>
 alebo ich kombinacia pricom <function> musi zacinat ako prve
 * \param int token
 * \param
 * \return SUCCESS alebo chyba
 */
int program(int token)
{
    ///program zacina var a pokracuje begin bez funkcii
    if(token==VAR && FUNCTION_ENABLE==0)  ///nebola definovana ziadna funkcia moze ist var
    {

            token=declarelist();

            if(token==BEGIN)
            {
                FUNCTION_ENABLE=1;            ///ked nasleduje begin tak uz ziadna funkcia nebude moct ist
                token=prog();
            }
    else if(token==FUNCTION && FUNCTION_ENABLE==0)                      ///za var nasleduje funckia
    {
        FUNCTION_ENABLE=1;                                              ///pokazde vymaze tabulku symbolov pre kazdu funkciu
        hashtable_clear(LokalnaTAB);                                 ///vymazeme aj pole lokalnych identifikatorov
        POLE_ID_LOCAL->length=0;

        IN_FUNCTION=1;                                              ///nachadzame sa vo funkcii , budeme sa pozerat do Lokalnej tabulky
        funkcia();                                                 /// pri chybe program skonci sam
        strClear(ARRAY_PARAM);                                     ///vymazeme  pomocne pole pre parametre funkcie
        token=getnextToken(LEX_STRUCTPTR);
        if(token==FORWARD)                                          /// len hlavicka funkcie ziadne telo za nou nenasleduje
        {
            CHECK_FUN++;

            if(SUPPORT->defined==true_hash)
                exit(E_SEMANTIC_UNDEF);

            IN_FUNCTION=0;
            if(SUPPORT->doubledefinition==1)                        ///flag aby neboli definovane dve hlavicky
                exit(E_SEMANTIC_UNDEF);


            if(SUPPORT->doubledefinition==0)
                SUPPORT->doubledefinition=1;

            if(SUPPORT->defined==3)
                SUPPORT->defined=false_hash;

            token=getnextToken(LEX_STRUCTPTR);
                if(token==BODKOCIARKA)
                {
                    token=getnextToken(LEX_STRUCTPTR);
                    return program(token);
                }
                else
                    return ERRORRET(token);

        }
        else if(token==BEGIN || token==VAR)        ///nasleduje telo funkcie takze bude definovana
        {
            if(SUPPORT->defined==true_hash)
                exit(E_SEMANTIC_UNDEF);

            SUPPORT->defined=true_hash;              ///funkcia je definovana


            if(token==VAR)
            {
                declarelist();
            }
            progfunction();
            token=getnextToken(LEX_STRUCTPTR);
            if(token==E_LEXICAL)exit(E_LEXICAL);  ///bud ide dalsia funkcia alebo ide uz telo programu
            IN_FUNCTION=0;
            return program(token);
        }
        else
            return ERRORRET(token);
    }
    else
        return ERRORRET(token);

}
///program obsahuje len telo hlavneho programu
    else if(token==BEGIN)
    {
        if(CHECK_FUN!=0)                                                ///POCET HLAVICIEK SA ROVNA POCTU FUNKCII
            exit(E_SEMANTIC_UNDEF);
        FUNCTION_ENABLE=1;
        token=prog();
    }

 ///program sa sklada z funkcii a programu BEZ VAR
    else if(token == FUNCTION)
    {
        FUNCTION_ENABLE=1;
        hashtable_clear(LokalnaTAB);                               ///vymazeme aj pole lokalnych identifikatorov
        POLE_ID_LOCAL->length=0;
        IN_FUNCTION=1;                      ///pokazde vymaze tabulku symbolov pre kazdu funkciu
        funkcia();
                                                        /// pri chybe program skonci sam
        strClear(ARRAY_PARAM);                                     ///vymazeme  pomocne pole pre parametre funkcie
        token=getnextToken(LEX_STRUCTPTR);
        if(token==FORWARD)                                          /// len hlavicka funkcie ziadne telo za nou nenasleduje
        {
            CHECK_FUN++;
            if(SUPPORT->defined==true_hash)
                exit(E_SEMANTIC_UNDEF);
            IN_FUNCTION=0;
               if(SUPPORT->doubledefinition==1)                         ///REDEFINICIA HLAVICKY
                    exit(E_SEMANTIC_UNDEF);
            if(SUPPORT->doubledefinition==0)
                SUPPORT->doubledefinition=1;
            if(SUPPORT->defined==3)
                SUPPORT->defined=false_hash;
                                                     ///nebola definova funckia

            token=getnextToken(LEX_STRUCTPTR);
                if(token==BODKOCIARKA)
                {
                    token=getnextToken(LEX_STRUCTPTR);
                    return program(token);
                }
                else
                    return ERRORRET(token);

        }
        else if(token==BEGIN || token==VAR)
        {
                if(SUPPORT->defined==true_hash)
                    exit(E_SEMANTIC_UNDEF);

                SUPPORT->defined=true_hash;              ///podpora rekurzivneho volania bez hlavicky funkcie

                                                            ///funkcia je definovana
            if(token==VAR)
            {
                declarelist();
            }
            progfunction();
            token=getnextToken(LEX_STRUCTPTR);
            if(token==E_LEXICAL)exit(E_LEXICAL);
                                                    ///bud ide dalsia funkcia alebo ide uz telo programu
            IN_FUNCTION=0;
            return program(token);
        }
        else
            return ERRORRET(token);
    }

if(token==SUCCESS && (token=getnextToken(LEX_STRUCTPTR))==EOFILE) /// za end. nesmie nasledovat nic ine
    return SUCCESS;
else
    return ERRORRET(token);

}

/** \brief Hlavna funkcia syntaktickej analyzi
Nepatri medzi neterminaly ale spusta dolezite akcie
 * \return SUCCESS alebo EXIT(CHYBA)
 *
 */
int SyntacticAnalys ()
{
ID_ENABLE = 0;
if(((LEX_STRUCTPTR =(LEX_STRUCT*)malloc(sizeof(LEX_STRUCT))) == NULL) ||    ///alokacia pomocnej struktury lexikalnej analyzy
    (Init_str(LEX_STRUCTPTR)==E_INTERNAL))
exit(E_INTERNAL);

if(((POLE_ID_GLOBAL=(inf_array*)malloc(sizeof(inf_array))) == NULL) ||
    (init_array(POLE_ID_GLOBAL)==-1))                                        ///alokujeme pole ID glob
exit(E_INTERNAL);

if(((POLE_ID_GLOBALFUN=(inf_array*)malloc(sizeof(inf_array))) == NULL) ||
    (init_array(POLE_ID_GLOBALFUN)==-1))                                        ///alokujeme pole ID glob pre funkcie
exit(E_INTERNAL);

if(((POLE_ID_LOCAL=(inf_array*)malloc(sizeof(inf_array))) == NULL) ||
    (init_array(POLE_ID_LOCAL)==-1))                                        ///alokujeme pole ID loc
exit(E_INTERNAL);

if(((GlobalnaTAB=(THash_table*)malloc(sizeof(THash_table))) == NULL) ||
    ((GlobalnaTAB=hashtable_init(100))==0))                                        ///alokujeme hashovaciu tabulku
exit(E_INTERNAL);


if(((LokalnaTAB=(THash_table*)malloc(sizeof(THash_table))) == NULL) ||
    ((LokalnaTAB=hashtable_init(100))==0))                                        ///alokujeme hashovaciu tabulku
exit(E_INTERNAL);


if(((ARRAY_PARAM =(LEX_STRUCT*)malloc(sizeof(LEX_STRUCT))) == NULL) ||      ///alkokujeme pole pre parametre funkcii
    (Init_str(ARRAY_PARAM)==E_INTERNAL))
exit(E_INTERNAL);



if((ELEMENT=(struct record*)malloc(sizeof(struct record))) == NULL)          ///alokujeme hashovaciu tabulku
exit(E_INTERNAL);

if((SUPPORT=(struct record*)malloc(sizeof(struct record))) == NULL)          ///alokujeme hashovaciu tabulku
exit(E_INTERNAL);

IN_FUNCTION=0;                                                               ///nastavime,nenachadzame sa vo funkcii
int token=getnextToken(LEX_STRUCTPTR);              ///nacitame prvy token
if(token==E_LEXICAL)exit(E_LEXICAL);                   ///pri chybe v lexikalnej analyze skonci
if(token==EOFILE) exit(E_SYNTAX);
int ok=program(token);
if(ok==SUCCESS)
    return SUCCESS;
///DEALOKACIE PRIDAT DALSIE + volat pri exit
strClear(LEX_STRUCTPTR);
free_array(POLE_ID_GLOBAL);
free_array(POLE_ID_LOCAL);
hashtable_free(GlobalnaTAB);
hashtable_free(LokalnaTAB);
free(LEX_STRUCTPTR);
return 0;
}///



/** \brief ///telo programu pre funkciu zacina begin a konci end;
Tento neterminal sa vola len pri prijati terminalu/tokenu <function>
**/
/** *
 * \return SUCCESS
 *
 */
int progfunction()
{
int token;
token = getnextToken(LEX_STRUCTPTR);
///vstavane funkcie copy,length,find,sort + ostatne mozne prikazy
    switch(token)
    {
        case BEGIN:
        {
          token=progcondition();
          break;
        }
        case ID:
        case WHILE:
        case IF:
        case  LENGTH:
        case  COPY:
        case  FIND:
        case  SORT:
        case  WRITE:
        case  READLN:
        {
            token=command(token);
            break;
        }
        default:
            return ERRORRET(token);
    }

    if(token==SUCCESS && ID_ENABLE==0)  /// id prikaz nebol volany id:=
    {
        token=getnextToken(LEX_STRUCTPTR);
        if(token==END)
        {
            token=getnextToken(LEX_STRUCTPTR);
               if(token==BODKOCIARKA)
                    return SUCCESS;
               else return ERRORRET(token);
        }
        else if(token==BODKOCIARKA)
        return progfunction();
        else
            return ERRORRET(token);



    }
    else if(token==SUCCESS && ID_ENABLE == 1) ///za id:= nasleduje strednik
    {
        ID_ENABLE = 0;
        return progfunction();
    }else if(token==SUCCESS && ID_ENABLE == 2)   ///za id:= nasleduje end
    {
        ID_ENABLE = 0;
        token=getnextToken(LEX_STRUCTPTR);
        if(token==BODKOCIARKA)
            return SUCCESS;
        else return ERRORRET(token);
    }
return ERRORRET(token);
}

/** \brief telo programu pre TELA cyklov  IF a While, su to neterminaly volane neterminalom/funckiou
"command".Tento neterminal sa vola len pri prijati terminalu/tokenu <if> alebo <while>
pretoze su ukoncene "end" bez bodky ci strednika
 * \return SUCCESS
 */
int progcondition()
{
int token;
token = getnextToken(LEX_STRUCTPTR);
  ///vstavane funkcie copy,length,find,sort + ostatne mozne prikazy
    switch(token)
    {
        case BEGIN:
        {
          token=progcondition();
          break;
        }
        case ID:
        case WHILE:
        case IF:
        case  LENGTH:
        case  COPY:
        case  FIND:
        case  SORT:
        case  WRITE:
        case  READLN:
        {
            token=command(token);
            break;
        }
        default: return ERRORRET(token);
    }
    if(token==SUCCESS && ID_ENABLE == 0)
    {
        token=getnextToken(LEX_STRUCTPTR);
        if(token==END)
        {
            return SUCCESS;
        }
        else if(token==BODKOCIARKA)
            return progcondition();
        else
            return ERRORRET(token);
    }else if(token==SUCCESS && ID_ENABLE == 1)
    {
        ID_ENABLE = 0;
        return progcondition();
    }else if(token==SUCCESS && ID_ENABLE == 2)   ///za end vo While a IF nenasleduje nic
    {
        ID_ENABLE = 0;
        return SUCCESS;
    }
return ERRORRET(token);
}


/** \brief ///<prog>  BEGIN <PRIKAZ> V <CYKLY> V <DEFINOVANA FUNKCIA> V <VSTAVANAFUNKCIA> END.
Hlavne telo MAIN  program oddeleny Begin a end. pricom obsahuje prikazy.
 *
 * \return SUCCESS
 *
*/
int prog()
{

int token;
token = getnextToken(LEX_STRUCTPTR);
///vstavane funkcie copy,length,find,sort+dalsie
    switch(token)
    {
        case BEGIN:
        {
          token=progcondition();
          break;
        }
        case ID:
        case WHILE:
        case IF:
        case  LENGTH:
        case  COPY:
        case  FIND:
        case  SORT:
        case  WRITE:
        case  READLN:
        {
            token=command(token);
            break;
        }
        default:
            return ERRORRET(token);
    }
    if(token==SUCCESS && ID_ENABLE == 0)
    {
        token=getnextToken(LEX_STRUCTPTR);
        if(token==END)
        {
            token=getnextToken(LEX_STRUCTPTR);
            if(token==DOT)
                return SUCCESS;
        }
        else if(token==BODKOCIARKA)
            return prog();
        ///if podmienka s else + vnorene IF
        else if(token==ELSE && IF_ENABLE==1)
        {
             IF_ENABLE=0;
             token=getnextToken(LEX_STRUCTPTR);
             if(token==BEGIN)
             {
                token=progcondition();
                if(token==SUCCESS)
                {
                    token=getnextToken(LEX_STRUCTPTR);
                if(token==END)
                {
                    token=getnextToken(LEX_STRUCTPTR);
                if(token==DOT)
                    return SUCCESS;
                }
                else if(token==BODKOCIARKA)
                    return prog();

                else
                    return ERRORRET(token);
                }else
                return ERRORRET(token);

            }else
                return ERRORRET(token);
        }
        else
            return ERRORRET(token);
    }else if (token == SUCCESS && ID_ENABLE == 1)
    {
        ID_ENABLE = 0;
        return prog();
    }else if (token == SUCCESS && ID_ENABLE == 2)
    {
        ID_ENABLE = 0;
        token=getnextToken(LEX_STRUCTPTR);
                if(token==DOT)
                    return SUCCESS;
    }
return ERRORRET(token);
}


/** \brief /** <command>  |  <id>  := <hodnota> v <cykly> v <vstavanafunkcia> v <definovanefunkcie>
Neterminal/funckia Command , ktora je volana z neterminalov progcondition/prog/progfunction
Pri chybe sa hned vola exit(cislo chyby)
 *
 * \param int  hodnota prikazu
 * \return SUCCESS
 *
 */
int command(int value)
{
    int token;
    ///Vstavane funkcie
    if(value==COPY || value==FIND ||value==LENGTH || value==SORT)
        return Libraryfunction(value);                                      ///zavola sa funckia ktora kontroluje vstavane funkcie

    else if(value==ID)
    {
                      ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);                                     ///Kontrola ci je definovana
                        if(ELEMENT==0)
                            exit(E_SEMANTIC_UNDEF);
                        if(ELEMENT->id==FUNCTION_hash)
                            exit(E_SEMANTIC_TYPE);
                        Vysledok=ELEMENT->type;                                      ///V COM MAME OCAKAVAT VYSLEDOK


            token=getnextToken(LEX_STRUCTPTR);
            if(token==DVOJBODKA)
            {
                token=getnextToken(LEX_STRUCTPTR);
                if(token==EQUAL)
                {
                    PrecedenceSA(LEX_STRUCTPTR,ID,GlobalnaTAB,LokalnaTAB,ELEMENT);  ///Precedencna analyza

                    ELEMENT->defined=true_hash;
                    return SUCCESS;
                }else return ERRORRET(token);

            }
            else return ERRORRET(token);    ///Pridat pre funckie  do precedencnej
    }

    ///Prikaz readln
    else if(value==READLN)

    {
        token=getnextToken(LEX_STRUCTPTR);
        if (token==LEFT_ROUND)
        {
            token=getnextToken(LEX_STRUCTPTR);
            if(token==ID)
            {

                    if(IN_FUNCTION==0)///Kontrola ci je definovana
                        ELEMENT=(hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str));
                    else
                    {
                        ELEMENT=(hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str));
                    }

                    if(ELEMENT==0)
                        exit(E_SEMANTIC_UNDEF);
                                                                                                ///readln definuje variable
                ELEMENT->defined=true_hash;

                token=getnextToken(LEX_STRUCTPTR);
                if(token==RIGHT_ROUND)
                    return SUCCESS;
                else
                    return ERRORRET(token);
            }
            else
                return ERRORRET(token);
        }
        else
            return ERRORRET(token);

    }
    ///IF podmienka sa vyhodnocuje zvlast v precendencnej analyze
    else if(value==IF)
    {
        ///dopisat
       IF_ENABLE=1;
       Vysledok=PODMIENKA;

       PrecedenceSA(LEX_STRUCTPTR,IF,GlobalnaTAB,LokalnaTAB,ELEMENT);///  VYHODNOTI SA PODMIENKA + NACITA SA THEN


        if((token=getnextToken(LEX_STRUCTPTR))==BEGIN)
      {
          return progcondition();
      }else ERRORRET(token);
    }
    ///WHILE CYKLUS + VNORENE WHILE CYKLY
    else if(value==WHILE)
    {
        Vysledok=PODMIENKA;
        PrecedenceSA(LEX_STRUCTPTR,WHILE,GlobalnaTAB,LokalnaTAB,ELEMENT);  //VYHODNOTI SA PODMIENKA + NACITA SA do

            if((token=getnextToken(LEX_STRUCTPTR))==BEGIN)
      {
          return progcondition();
      }else ERRORRET(token);

    }
    ///write funkcia pre lubovolny pocet parametrov
    else if(value==WRITE)
    {
        token=getnextToken(LEX_STRUCTPTR);
        if(token==LEFT_ROUND)
        {
         return Libraryfunction(WRITE);

        }else ERRORRET(token);
    }

return ERRORRET(token);
}
/**
///Vstavane funkcie COPY...
Neterminal Libraryfunction je volana z neterminalu command kontroluje syntax vstavanych funckii
*/
int Libraryfunction(int value){
    int token;

if(value==WRITE)
{

    token=getnextToken(LEX_STRUCTPTR);
            if(token==ID || token==CONST_STRING)
            {
                        if(token==ID)
                        {
                    ///Kontrola ze vsetky vstupne hodnoty su String
                        ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                            ///Vrati na ukazatel na prvek v hash table
                        if(ELEMENT==0)
                            exit(E_SEMANTIC_UNDEF);
                        else if(ELEMENT->defined!=true_hash)
                            exit(E_SEMANTIC_TYPE);

                        }
              token=getnextToken(LEX_STRUCTPTR);
              if(token==CIARKA)
              {
                  return Libraryfunction(value);

              }
              else if(token==RIGHT_ROUND)
                return SUCCESS;

              else ERRORRET(token);
            }else ERRORRET(token);

}

else if(value==LENGTH)
    {
     token=getnextToken(LEX_STRUCTPTR);
     if(token==LEFT_ROUND)
        {
            token=getnextToken(LEX_STRUCTPTR);
            if(token==CONST_STRING || token==ID)
            {
                        if(token==ID)
                        {
                    ///Kontrola ze vsetky vstupne hodnoty su String
                        ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);

                                                                ///Vrati na ukazatel na prvek v hash table
                        if(ELEMENT==0)
                            exit(E_SEMANTIC_UNDEF);
                        else if(ELEMENT->type!=STRING_hash)
                            exit(E_SEMANTIC_TYPE);
                        else if(ELEMENT->defined!=true_hash)
                            exit(E_SEMANTIC_TYPE);
                        }

                        token=getnextToken(LEX_STRUCTPTR);
                        if(token==RIGHT_ROUND)
                            return SUCCESS;
                        else return ERRORRET(token);

            }else return ERRORRET(token);

        }else return ERRORRET(token);
    }
///Funkcia copy
else if(value==COPY)
{
       token=getnextToken(LEX_STRUCTPTR);
       if(token==LEFT_ROUND)
       {
            token=getnextToken(LEX_STRUCTPTR);
            if(token==CONST_STRING || token==ID)
            {
                        if(token==ID)
                        {
                    ///Kontrola ze vsetky vstupne hodnoty su String
                        ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                            ///Vrati na ukazatel na prvek v hash table
                        if(ELEMENT==0)
                            exit(E_SEMANTIC_UNDEF);
                        else if(ELEMENT->type!=STRING_hash)
                            exit(E_SEMANTIC_TYPE);
                        else if(ELEMENT->defined!=true_hash)
                            exit(E_SEMANTIC_TYPE);
                        }

                        token=getnextToken(LEX_STRUCTPTR);
                        if(token==CIARKA)
                        {
                            token=getnextToken(LEX_STRUCTPTR);
                            if(token==ID || token== CONST || token==REALo || token==BOOLEAN)
                            {
                                if(token==REALo || token==BOOLEAN)
                                exit(E_SEMANTIC_TYPE);
                                if(token==ID)
                                {
                                ///Kontrola ze vsetky vstupne hodnoty su INT
                                ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                            ///Vrati na ukazatel na prvek v hash table
                                if(ELEMENT==0)
                                    exit(E_SEMANTIC_UNDEF);
                                else if(ELEMENT->type!=INTEGER_hash)
                                    exit(E_SEMANTIC_TYPE);
                                else if(ELEMENT->defined!=true_hash)
                                    exit(E_SEMANTIC_TYPE);
                                }
                                token=getnextToken(LEX_STRUCTPTR);
                                if(token==CIARKA)
                                {
                                    token=getnextToken(LEX_STRUCTPTR);
                                    if(token==ID || token==CONST || token==REALo || token==BOOLEAN )
                                    {
                                            if(token==REALo || token==BOOLEAN)
                                                exit(E_SEMANTIC_TYPE);
                                            if(token==ID)
                                            {
                                            ///Kontrola ze vsetky vstupne hodnoty su INT
                                                ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                            ///Vrati na ukazatel na prvek v hash table
                                            if(ELEMENT==0)
                                                exit(E_SEMANTIC_UNDEF);
                                            else if(ELEMENT->type!=INTEGER_hash)
                                                exit(E_SEMANTIC_TYPE);
                                            else if(ELEMENT->defined!=true_hash)
                                                exit(E_SEMANTIC_TYPE);
                                            }
                                            token=getnextToken(LEX_STRUCTPTR);
                                            if(token==RIGHT_ROUND)
                                                return SUCCESS;
                                            else return ERRORRET(token);
                                    }else return ERRORRET(token);
                                }else return ERRORRET(token);
                            }else return ERRORRET(token);
                        }else return ERRORRET(token);
            }else return ERRORRET(token);
       }else return ERRORRET(token);

}
    ///funkcia find
else if(value==FIND)
{
        token=getnextToken(LEX_STRUCTPTR);
        if(token==LEFT_ROUND)
        {
                token=getnextToken(LEX_STRUCTPTR);
                if(token==CONST_STRING || token==ID)
                {
                       if(token==ID)
                        {
                    ///Kontrola ze vsetky vstupne hodnoty su String
                        ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                            ///Vrati na ukazatel na prvek v hash table
                        if(ELEMENT==0)
                            exit(E_SEMANTIC_UNDEF);
                        else if(ELEMENT->type!=STRING_hash)
                            exit(E_SEMANTIC_TYPE);
                        else if(ELEMENT->defined!=true_hash)
                            exit(E_SEMANTIC_TYPE);
                        }

                        token=getnextToken(LEX_STRUCTPTR);
                        if(token==CIARKA)
                        {
                            token=getnextToken(LEX_STRUCTPTR);
                            if(token==CONST_STRING || token==ID)
                            {
                                if(token==ID)
                                {
                                ///Kontrola ze vsetky vstupne hodnoty su String
                                ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                            ///Vrati na ukazatel na prvek v hash table
                                if(ELEMENT==0)
                                    exit(E_SEMANTIC_UNDEF);
                                else if(ELEMENT->type!=STRING_hash)
                                    exit(E_SEMANTIC_TYPE);
                                else if(ELEMENT->defined!=true_hash)
                                    exit(E_SEMANTIC_TYPE);
                                }
                                token=getnextToken(LEX_STRUCTPTR);
                                if(token==RIGHT_ROUND)
                                    return SUCCESS;
                                else return ERRORRET(token);
                            }else return ERRORRET(token);
                        }else return ERRORRET(token);
                }else return ERRORRET(token);
        }else return ERRORRET(token);
}
else if(value==SORT)
{
      token=getnextToken(LEX_STRUCTPTR);
      if(token==LEFT_ROUND)
      {
          token=getnextToken(LEX_STRUCTPTR);
          if(token==CONST_STRING || token==ID)
          {
                if(token==ID)
                {
                    ///Kontrola ze vsetky vstupne hodnoty su String
                    ELEMENT=lookforElement(LEX_STRUCTPTR,0,GlobalnaTAB,LokalnaTAB,ELEMENT);
                                                                                    ///Vrati na ukazatel na prvek v hash table
                        if(ELEMENT==0)
                            exit(E_SEMANTIC_UNDEF);
                        else if(ELEMENT->type!=STRING_hash)
                            exit(E_SEMANTIC_TYPE);
                        else if(ELEMENT->defined!=true_hash)
                            exit(E_SEMANTIC_TYPE);
                }
                token=getnextToken(LEX_STRUCTPTR);
                if(token==RIGHT_ROUND)
                    return SUCCESS;
                else return ERRORRET(token);
          }else return ERRORRET(token);
      }else return ERRORRET(token);
}
return ERRORRET(token);
}



///VAR DEKLARACIA GLOBALNYCH PREMENNYCH  < VAR >
/**
Neterminal na kontrolu syntaxe deklaracie premennych
*/
 int declarelist()
{
    int token=getnextToken(LEX_STRUCTPTR);
    if(token==ID)
    {
     Declar=1;

         ///VKLADANIE DO TABULIEK SYMBOLOV
    if(IN_FUNCTION==0)
    {
                         ///ulozime ID do pola ID a ulozime si nove posunutie pre dalsi ID
                                        ///Zistime ci uz nemame taku polozku
        ELEMENT=hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str);

                                  ///Vrati na ukazatel na prvek v hash table
         if(ELEMENT!=0)
            exit(E_SEMANTIC_UNDEF);
           else
            POLE_ID_INDEX=add_str(POLE_ID_GLOBAL,LEX_STRUCTPTR->str);
    }else
    {
        ELEMENT=hashtable_search(LokalnaTAB,LEX_STRUCTPTR->str);                   ///ulozime ID do pola ID a ulozime si nove posunutie pre dalsi ID
                                                                                                    ///Zistime ci uz nemame taku polozku

                         ///Vrati na ukazatel na prvek v hash table
         if(ELEMENT!=0)
         {
            exit(E_SEMANTIC_UNDEF);
         }
         else
        POLE_ID_INDEX=add_str(POLE_ID_LOCAL,LEX_STRUCTPTR->str);
                                                                             ///dva krat definovany ten isty nazov
    }

                       ///neni jej priradena hodnota
                    if((token=getnextToken(LEX_STRUCTPTR))== DVOJBODKA)
                    {
                        token=getnextToken(LEX_STRUCTPTR);
                    if(token==REAL || token==INTEGER || token==STRING || token == BOOLEAN)
                    {

                        if(IN_FUNCTION==0)
                            hashtable_add(GlobalnaTAB,VARIABLE_hash,POLE_ID_GLOBAL->str+POLE_ID_INDEX,decodederSEM(token),NULL);  ///pridame ID do tabulky symbolov GLOB
                         else
                            hashtable_add(LokalnaTAB,VARIABLE_hash,POLE_ID_LOCAL->str+POLE_ID_INDEX,decodederSEM(token),NULL);  ///pridame ID do tabulky symbolov    LOC

                        token=getnextToken(LEX_STRUCTPTR);
                            ///pridat do tabulky
                        {
                         if(token==BODKOCIARKA)
                         {
                         return declarelist();
                         }
                         else return ERRORRET(token);
                        }
                    }
                    else return ERRORRET(token);
                    }
                    else return ERRORRET(token);
    }
    else  if(Declar==0)           ///neboli zadane ziadne premenne
     return  ERRORRET(token);
    else
    {
        Declar=0;
        return token;
    }
}


///DEFINOVANIE FUNKCIE
/**
Neterminal volany z neterminalu "program" ktory kontroluje syntax funckie
*/ ///zo semantickej do tejto funckie uz neni co dodat asi
int funkcia()
{
    int token;
    if((token = getnextToken(LEX_STRUCTPTR)) == ID)
    {

        ELEMENT=hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str);
            if(ELEMENT==0)
            {
                POLE_ID_INDEX=add_str(POLE_ID_GLOBALFUN,LEX_STRUCTPTR->str);
                hashtable_add(GlobalnaTAB,FUNCTION_hash,POLE_ID_GLOBALFUN->str+POLE_ID_INDEX,0,0);
                ELEMENT=hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str);
                ELEMENT->doubledefinition=0;
                ELEMENT->defined=3;
            }
            else
                CHECK_FUN--;

            ELEMENT=hashtable_search(GlobalnaTAB,LEX_STRUCTPTR->str);
            SUPPORT=ELEMENT;

        if ((token = getnextToken(LEX_STRUCTPTR)) == LEFT_ROUND)
        {

                token = fun_params();
                if(token==E_SYNTAX || token==E_LEXICAL)
                    return ERRORRET(token);
                if(token == RIGHT_ROUND)
                {
                    token = getnextToken(LEX_STRUCTPTR);
                    if(token==DVOJBODKA)
                    {
                     token = getnextToken(LEX_STRUCTPTR);
                        if (token == INTEGER || token  == BOOLEAN || token == STRING || token == REAL)
                        {
                         if(SUPPORT->defined==3)             ///neexistuje hlavicka ani telo
                         {

                            SUPPORT->type=decodederSEM(token);

                         }
                        else if(SUPPORT->defined==false_hash)
                        {
                            if(SUPPORT->type!=decodederSEM(token))
                                exit(E_SEMANTIC_TYPE);
                        }
                        SUPPORT->type=decodederSEM(token);

                        token = getnextToken(LEX_STRUCTPTR);
                        if(token==BODKOCIARKA)
                        {
                            return SUCCESS;
                                                 ///hlavicka je OK
                        }else return ERRORRET(token);

                        }else return ERRORRET(token);
                    }else return ERRORRET(token);


                }else return ERRORRET(token);
        }
        else return ERRORRET(token); // za ID nenasleduje zatvorka
    }
    else return ERRORRET(token); //za function nenasleduje ID
}


/**
Neterminal ktory je volany neterminalom "funkcia()"
Tato funkcia kontroluje sytax parametrov vo vnutri zatvorky
*/
int fun_params()
{
    int token;
    token = getnextToken(LEX_STRUCTPTR);
    if(token == RIGHT_ROUND)
        return token;
    else if(token == ID)
    {
        POLE_ID_INDEX=add_str(POLE_ID_LOCAL,LEX_STRUCTPTR->str);
                                                                              ///ulozime ID do pola ID a ulozime si nove posunutie pre dalsi ID
                                                                                                    ///Zistime ci uz nemame taku polozku
        ELEMENT=((hashtable_search(LokalnaTAB,POLE_ID_LOCAL->str+POLE_ID_INDEX)));
                                                                                        ///Vrati na ukazatel na prvek v hash table
         if(ELEMENT==0)
         {
            hashtable_add(LokalnaTAB,VARIABLE_hash,POLE_ID_LOCAL->str+POLE_ID_INDEX,NULL,NULL);  ///PRIDA DEFINICIU funkcie
            ELEMENT=((hashtable_search(LokalnaTAB,POLE_ID_LOCAL->str+POLE_ID_INDEX)));
         }else
           exit(E_SEMANTIC_UNDEF);

            ELEMENT->defined=true_hash;             ///premenna pride jej zavolanim

            token = getnextToken(LEX_STRUCTPTR);
        if(token == DVOJBODKA)
        {
            token = getnextToken(LEX_STRUCTPTR);
            if (token == INTEGER || token  == BOOLEAN || token == STRING || token == REAL)
            {
                ELEMENT->type=decodederSEM(token);
                addparam(token);

                 ///Support je ukazatel na ID funkcie

                token = getnextToken(LEX_STRUCTPTR);
                if(token == BODKOCIARKA)
                {
                    return fun_params();
                }
                else if(token == RIGHT_ROUND)
                {


                if(SUPPORT->defined!=true_hash  && SUPPORT->defined!=false_hash)                                ///ked hlavicka uz bola deklarovana nealokujeme a zaroven redefinice parametrov funckie
                {
                    int newLength = length(ARRAY_PARAM->str);                           ///pridavame
                    SUPPORT->params=malloc(sizeof(char)*(newLength+1));
                     strcpy(SUPPORT->params,ARRAY_PARAM->str);
                }
                else
                {
                    int ok=strcmp(SUPPORT->params,ARRAY_PARAM->str);
                    if(ok!=0)
                        exit(E_SEMANTIC_TYPE);
                    ///ROZNE POLIA
                }
                    ///pridat kontrolu ze ze udana hlavicka je tototzna

                    return token;
                }
                else return ERRORRET(token); // po DTYPE nenasleduje ani zatvorka ani bodkociarka
            }
            else return ERRORRET(token); //po dvojbodke nenasleduje DTYPE
        }
        else return ERRORRET(token); //po ID nenasleduje dvojbodka
    }

    else return ERRORRET(token);   // po zatvorke nenasleduje ID ani zatvorka
}



///Funckia dekoduje nase tokeny na hodnoty pre hashovaciu funkciu
int decodederSEM(int token)
{
    switch(token)
    {
    case INTEGER :
        return INTEGER_hash;
    case BOOLEAN:
        return BOOLEAN_hash;
    case STRING:
        return STRING_hash;
    case REAL:
        return REAL_hash;
    case REALo:
        return REAL_hash;
    case CONST:
        return INTEGER_hash;
    case TRUE:
        return BOOLEAN_hash;
    case FALSE:
        return BOOLEAN_hash;
    case CONST_STRING:
        return STRING_hash;
    }
}

///ulozenie parametrov do tabulky symbolov
void addparam(token)
{
     switch(token)
    {
    case INTEGER :
        AddChar_str(ARRAY_PARAM,'i');
        return;
    case BOOLEAN:
        AddChar_str(ARRAY_PARAM,'b');
        return;
    case STRING:
        AddChar_str(ARRAY_PARAM,'s');
        return;
    case REAL:
        AddChar_str(ARRAY_PARAM,'r');
        return;
    }

}





















