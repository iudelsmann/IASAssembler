#include<stdio.h>
#include<string.h>
#include<stdlib.h>


/** Neste arquivo estao definidas todas as estruturas de dados que sao utilizadas pelo montador e funcoes sobre listas ligadas **/


// ESTRUTURAS

typedef enum LADO {esq, dir} LADO; // Define valores 0 e 1 para esq e dir respectivamente para tipo LADO
typedef enum Boolean {false, true} Boolean; // Define tipo booleano

// Estrutura de no para strings definidas por diretivas ".set"
typedef struct SET
{
    char original[64]; // A string original
    char substituto[64]; // A string a ser subtituida quando a original for encontrada
    struct SET *prox; // Apontador para proximo no, gerando a lista ligada
} SET;

// Estrutura para tabela de rotulos
typedef struct Rotulo
{
    char nome[64]; // Guarda o rotulo
    int endereco; // Endereco do rotulo
    LADO lado; // Lado do rotulo
    struct Rotulo *prox; // Apontador para proximo no, gerando a lista ligada
} Rotulo;

// Estrutura que guarda o mapa de memoria, cada no e meia palavra (20bits)
typedef struct MapaMem
{
    int endereco;
    LADO lado;
    int opcode;
    int memoria;

    struct MapaMem *prox;
} MapaMem;

// Estrutura que implementa o registrador PC
typedef struct PC
{
    int endereco; // Endereco de PC
    LADO lado; // Lado de PC
} PC;


// FUNCOES

// Busca em lista ligada de palavras definidas em diretivas ".set", retorna apontador para o no da lista ou null caso nao ache
SET *buscaSET(SET *set, char *str);

// Insere novo no no comeco de lista de palavras definidas em diretivas ".set", retorna apontador para o comeco da lista.
SET *inserirSET(SET *set, char *str1, char *str2);

// Busca em lista ligada de rotulos, retorna apontador para o endereco do no da lista ou null caso nao ache
Rotulo *buscaROT(Rotulo *rot, char *str);

// Insere rotulo novo na lista de rotulos, insere sempre no comeco, retorna apontador para inicio da lista
Rotulo *inserirROT(Rotulo *rot, char *str, int end, LADO lado);

// Insere instrucao ou dado (20bits) para lista ligada de intrucoes, insere ordenadamente por endereco de memoria e lado
void inserirMAPA(MapaMem **mapa, int opcode, int endereco, LADO lado, int memoria);

// Imprime mapa de memoria atraves de lista ligada de intrucoes e dados no arquivo fornecido
void imprimirMAPA(MapaMem *mapa, FILE *output_file);

// Funcoes para liberar memoria das listas ligadas
void freeSET(SET *set);
void freeROT(Rotulo *rot);
void freeMAPA(MapaMem *mapa);
