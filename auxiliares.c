#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include "listas.h"
#include "auxiliares.h"


// Rece uma string e altera todos os caracteres para maiuscula
void stringToUpper(char *str)
{
    int size = strlen(str);
    int i;
    for(i=0; i<size; i++)
        str[i] = toupper(str[i]); // Utiliza a funcao "toupper" para alterar caracter a caracter para maiuscula
}

// Verifica se uma string passada e um rotulo
Boolean verRotulo ( char *str)
{
    // Se o primeiro caracter for numerico, ja retorna falso
    if(isdigit(str[0]))
        return false;

    int j;
    int i = strlen(str);

    // Se o ultimo caracter for ':'
    if (str[i-1] == ':')
    {
        // Verifica se todos os caracteres são alfanumericos ou underscore
        for(j=0; j<i-1; j++)
        {
            if(!isalnum(str[j]) && str[j] != '_')
                return false;
        }
        return true; // Retorna verdadeiro caso nao ache erros
    }
    return false; // Retorna falso caso nao haja ":" ao fim

}

// Verrifica se uma string fornecida e uma diretiva e retorna um valor especificando qual diretiva foi encontrada
int verDiretiva ( char *str)
{
    if(!strcmp(str,".ORG"))
        return 1;
    else if(!strcmp(str,".ALIGN"))
        return 2;
    else if(!strcmp(str,".WFILL"))
        return 3;
    else if(!strcmp(str,".WORD"))
        return 4;
    else if(!strcmp(str,".SET"))
        return 5;
    else
        return 0;
}



// Verifica se uma string esta escrita em binario, decimal, octal ou hexadecimal
int verBase( char *str)
{
    int i = strlen(str);
    int j, h=0;
    char aux[2];

    // Se o numero for negativo, atribui valor 1 para h
    if(str[0] == '-')
        h=1;

    // Se o primeiro caracter da string (apos o sinal) for 0
    if(str[h] == '0')
    {

        // Verifica se o segundo caracter e um X, indicando numero hexadecimal
        if(str[h+1] == 'X')
        {
            // Verifica se todos os digitos sao validos para numeros hexadecimais
            for(j=h+2; j<i; j++)
            {
                if(!isxdigit(str[j]))
                    return 0;
            }
            return 16; // Retorna a base numerica
        }

        // Verifica se Verifica se o segundo caracter e um B, indicando numero binario
        else if(str[h+1] == 'B')
        {
            // Verifica se todos os digitos sao validos para numeros binarios
            for(j=h+2; j<i; j++)
            {
                aux[0] = str[j];
                if(atoi(aux) > 1)
                    return 0;
            }
            return 2; // Retorna a base numerica
        }

        // Verifica se Verifica se o segundo caracter e um O, indicando numero octal
        else if(str[h+1] == 'O')
        {
            // Verifica se todos os digitos sao validos para numeros octais
            for(j=h+2; j<i; j++)
            {
                aux[0] = str[j];
                if(atoi(aux) > 8)
                    return 0;
            }
            return 8; // Retorna a base numerica
        }

        // Caso o segundo caracter nao caracterizar nenhuma base, assume-se base decimal
        else
        {
            // Verifica se todos os digitos sao validos para numeros decimais
            for(j=h; j<i; j++)
            {
                if(!isdigit(str[j]))
                    return 0;
            }
            return 10; // Retorna a base numerica
        }
    }
    // Caso o numero nao comece por 0, indica-se base decimal
    else
    {
        // Verifica se todos os digitos sao validos para numeros decimais
        for(j=h; j<i; j++)
        {
            if(!isdigit(str[j]))
                return 0;
        }
        return 10; // Retorna a base numerica
    }
}



// Recebe um string e um lado, verifica se a string e uma instrucao e retorna o opcode dela ou zero caso nao seja, o lado define o opcode correto para instrucoes de jump e STM
int verIntrucao(char *str, LADO lado)
{

    // Para instrucoes que possuem opcode unico, apenas compara strings e retorna o opcode
    if(!strcmp(str,"LDMQ"))
        return 10;
    else if(!strcmp(str,"LDMQM"))
        return 9;

    else if(!strcmp(str,"STR"))
        return 33;
    else if(!strcmp(str,"LOAD"))
        return 1;
    else if(!strcmp(str,"LDN"))
        return 2;
    else if(!strcmp(str,"LDABS"))
        return 3;
    else if(!strcmp(str,"ADD"))
        return 5;
    else if(!strcmp(str,"ADDABS"))
        return 7;
    else if(!strcmp(str,"SUB"))
        return 6;
    else if(!strcmp(str,"SUBABS"))
        return 8;
    else if(!strcmp(str,"MUL"))
        return 11;
    else if(!strcmp(str,"DIV"))
        return 12;
    else if(!strcmp(str,"LSH"))
        return 20;
    else if(!strcmp(str,"RSH"))
        return 21;

    // Para instrucoes que o lado influencia o opcode retornado, verifica o lado para retornar valor correto
    else if(!strcmp(str,"STM"))
    {
        if(lado == dir)
            return 19;
        return 18;
    }
    else if(!strcmp(str,"JMP"))
    {
        if(lado == dir)
            return 14;
        return 13;
    }
    else if(!strcmp(str,"JGEZ"))
    {
        if(lado == dir)
            return 16;
        return 15;
    }

    // Caso nao seja uma instrucao retorna 0
    else
        return 0;
}

// Recebe uma string e retorna verdadeiro se ela seguir os padroes definidos para strings apos ".set", ou falso caso contrario.
Boolean validSET(char *str)
{

    unsigned int j;

    // Verifica se o primeiro caracter e numerico e retorna falso caso positivo
    if(isdigit(str[0]))
        return false;

    // Itera sobre todos os caracteres verificando se sao alfanumericos ou underscore e retorna falso caso haje algum invalido
    for(j=0; j<strlen(str); j++)
    {
        if(!isalnum(str[j]) && str[j] != '_')
            return false;
    }

    return true; // Retorna verdadeiro caso nao ache erros

}

/*char *getln( FILE *input_file)
{
#define CHUNK 200
   char* input = NULL;
   char tempbuf[CHUNK];
   size_t inputlen = 0, templen = 0;
   do {
       fgets(tempbuf, CHUNK, input_file);
       templen = strlen(tempbuf);
       inputlen += templen;
       input = realloc(input, inputlen+1);
       strcat(input, tempbuf);
    } while (templen==CHUNK-1 && tempbuf[CHUNK-2]!='\n');
    return input;
}*/
