#ifndef PRECEDENT_H_
#define PRECEDENT_H_

int decodeSA(int);
void initPrecedenceTABLE(); ///vytvori precedencnu tabulku
int reduce(LEX_STRUCT*);
int VysledokID(int,int);
int recorderSEM2(char );
#define PT_LESS 80
#define PT_GREATER 81
#define PT_EQUAL   82
#define PT_OK	   83
#define ZARAZKA 90

#define  PSA_PLUS 0
#define  PSA_MINUS 1
#define  PSA_MULT 2
#define  PSA_DIVIDE 3
#define  PSA_ID 4
#define  PSA_LEFT 5
#define  PSA_RIGHT 6
#define  PSA_LESS 7
#define  PSA_GREATER 8
#define  PSA_LESSEQ 9
#define  PSA_GREATEREQ 10
#define  PSA_DOLAR 11
#define  PSA_NOTEQUAL 12

#endif
