#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "listas.h"

// Busca em lista ligada de palavras definidas em diretivas ".set", retorna apontador para o no da lista ou null caso nao ache
SET *buscaSET(SET *set, char *str)
{

    SET *aux = set; // Guarda referencia para inicio da lista
    while(aux != NULL) // Enquanto houver nos
    {
        if(!strcmp(aux->original, str)) // Compara as strings
            return aux; // Caso encontre retorna o apontador para o no
        aux = aux->prox; // Casos contrario continua iterando sobre a lista
    }
    return NULL; // Caso nao encontre, retorna NULL
}

// Insere novo no no comeco de lista de palavras definidas em diretivas ".set", retorna apontador para o comeco da lista.
SET *inserirSET(SET *set, char *str1, char *str2)
{

    SET *novo;
    novo = (SET *)malloc(sizeof(SET)); // Alloca memoria para novo no
    if(novo == NULL) // Caso haja erro na hora de allocar termina o programa
        exit(0);

    // Atribui os valores fornecidos como entrada da funcao
    strcpy(novo->original, str1);
    strcpy(novo->substituto, str2);

    // Insere no inicio e retorna
    novo->prox = set;
    set = novo;
    return set;

}

// Busca em lista ligada de rotulos, retorna apontador para o endereco do no da lista ou null caso nao ache
Rotulo *buscaROT(Rotulo *rot, char *str)
{

    Rotulo *aux = rot; // Mantem referencia para o inicio da lista
    while(aux != NULL) // Enquato houver nos
    {
        if(!strcmp(aux->nome, str)) // Compara as strings para ver se encontrou o rotulo procurado
            return aux; // Caso positivo retorna
        aux = aux->prox; // Casos contrario continua iterando sobre a lista
    }
    return NULL; // Senao encontrar, retorna NULL
}

// Insere rotulo novo na lista de rotulos, insere sempre no comeco, retorna apontador para inicio da lista
Rotulo *inserirROT(Rotulo *rot, char *str, int end, LADO lado)
{

    Rotulo *novo;
    novo = (Rotulo *)malloc(sizeof(Rotulo)); // Alloca memoria para novo no
    if(novo == NULL) // Caso haja erro aocando memoria termina o programa
        exit(0);

    // Inicia os valores
    strcpy(novo->nome, str);
    novo->endereco = end;

    // Insere no inicio da lista e retorna
    novo->lado = lado;
    novo->prox = rot;
    rot = novo;
    return rot;

}

// Insere instrucao ou dado (20bits) para lista ligada de intrucoes, insere ordenadamente por endereco de memoria e lado
void inserirMAPA(MapaMem **mapa, int opcode, int endereco, LADO lado, int memoria)
{

    MapaMem *novo, *aux, *aux2 = NULL;

    novo = (MapaMem *)malloc(sizeof(MapaMem)); // Alloca memoria para novo no
    if(novo == NULL) // Caso haja erro na hora de allocar termina o programa
        exit(0);

    // Atribui os valores do no a partir dos fornecidos como entrada
    novo->prox = NULL;
    novo->opcode = opcode;
    novo->endereco = endereco;
    novo->lado = lado;
    novo->memoria = memoria;


    aux = *mapa; // Para nao perder referencia ao inicio da lista

    // Casos a lista esteja vazia atribui o novo no para o ponteiro de inicio
    if(aux == NULL)
        *mapa = novo;

    // Casos contrario
    else
    {
        while(aux != NULL ) // Enquanto houver nos
        {

            // Se encontrar um no com o mesmo endereco e lado emite um erro
            if(aux->endereco == endereco && aux->lado == lado)
            {
                printf("Erro, duas instrucoes no mesmo endereco e lado");
                exit(0);
            }

            // Caso o endereco do no atual seja maior que o novo, entao deve inserir e retornar
            else if(aux->endereco > endereco)
            {
                novo->prox = aux;
                if(aux2 == NULL)
                    *mapa = novo;
                else
                    aux2->prox = novo;
                return;

            }

            // Caso o endereco do no atual seja igual ao novo porem o novo no fique a esquerda, deve-se inserir e retornar
            else if(aux->endereco == endereco && aux->lado > lado )
            {
                novo->prox = aux->prox;
                aux->prox = novo;
                return;

            }
            aux2 = aux; // Guarda apontador para o no anterior
            aux = aux->prox; // Proximo no
        }
        aux2->prox = novo; // Caso seja o ultimo no da lista


    }
}

// Imprime mapa de memoria atraves de lista ligada de intrucoes e dados no arquivo fornecido
void imprimirMAPA(MapaMem *mapa, FILE *output_file)
{

    MapaMem *aux = mapa; // Para nao perder referencia ao inicio da lista
    while(aux != NULL) // Enquanto houver nos na lista
    {
        // Caso o nó seja uma instrucao a esquerda imprime o endereco, opcode, e endereco utilizado pela intrucao
        if(aux->lado == esq)
            fprintf(output_file, "%03X %02X %03X ", aux->endereco, aux->opcode, aux->memoria);

        // Caso o nó seja uma instrucao a direita nao imprime o endereco, apenas opcode, e endereco utilizado pela intrucao e quebra de linha
        else
            fprintf(output_file, "%02X %03X\n", aux->opcode, aux->memoria);

        // Caso seja o ultimo no da lista, e este é uma intrucao a esquerda, completa a palavra de memoria com zeros
        if(aux->prox == NULL && aux->lado == esq)
        {
            fprintf(output_file, "00 000\n");
            return;
        }

        // Caso duas instrucoes seguidas sejam a esquerda, indicando que ha um buraco no mapa de memoria, completa a palavra com zeros
        if(aux->lado == esq && aux->prox->lado == esq)
            fprintf(output_file, "00 000\n");
        aux = aux->prox; // Passa para o proximo no
    }
}

// Funcoes para liberar memoria das listas ligadas
void freeSET(SET *set)
{

    SET *aux;

    while (set != NULL)
    {
        aux = set;
        set = set->prox;
        free(aux);
    }

}

void freeROT(Rotulo *rot)
{

    Rotulo *aux;

    while (rot != NULL)
    {
        aux = rot;
        rot = rot->prox;
        free(aux);
    }

}

void freeMAPA(MapaMem *mapa)
{

    MapaMem *aux;

    while (mapa != NULL)
    {
        aux = mapa;
        mapa = mapa->prox;
        free(aux);
    }

}
