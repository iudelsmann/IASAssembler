#include<stdio.h>


// Converte uma string para maiusculas
void stringToUpper(char *str);

// Verifica se string e rotulo
Boolean verRotulo ( char *str);

// Verrifica se uma string fornecida e uma diretiva e retorna um valor especificando qual diretiva foi encontrada
int verDiretiva ( char *str);

// Retorna a base decimal de uma string fornecida ou zero caso nao seja um valor numerico valido
int verBase( char *str);

// Recebe um string e um lado, verifica se a string e uma instrucao e retorna o opcode dela ou zero caso nao seja, o lado define o opcode correto para instrucoes de jump e STM
int verIntrucao(char *str, LADO lado);

// Recebe uma string e retorna verdadeiro se ela seguir os padroes definidos para strings apos ".set", ou falso caso contrario.
Boolean validSET(char *str);
