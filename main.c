/*************************************************************************
*                                                                        *
*    Ivo Udelsmann                                      RA: 091599       *
*                                                                        *
*                    Trabalho 1 - Montador IAS                           *
*                                                                        *
*        Este e meu montador para a IAS, abaixo algumas especificacoes   *
*    de como ele funciona para facilitar a corecao.                      *
*                                                                        *
*        Inicialmente le-se o arquivo linha por linha e guarda-se os     *
*    rotulos em uma lista ligada de estruturas com dois elementos,       *
*    a string que define o rotulo e um inteiro que representa o          *
*    endereco de memoria do rotulo.                                      *
*        Nesta mesma leitura guarda-se strings definidas em diretivas    *
*    ".set" em uma lista ligada de estruturas com dois elementos, a      *
*    string original e a substituta.                                     *
*        Tambem sao verificados diversos erros de sintaxe, como falta    *
*    de palavras, enderecos invalidos etc...                             *
*                                                                        *
*        Feito isso executa-se uma segunda leitura linha a linha do      *
*    arquivo, e dessa vez as instrucoes sao computadas e inseridas em    *
*    uma lista ligada que representa a o mapa de memoria. Esta lista     *
*    ligada e formada por estruturas com inteiros representando o        *
*    endereco dessa instrucao no mapa de memoria, inteiro representando  *
*    o opcode da instrucao, outro inteiro representando o endereco       *
*    que a instrucao utilizara e o lado da instrucao no mapa de memoria. *
*        As insercoes sao feitas ordenademente de acordo com o endereco. *
*    Quando um dado for inserido ele e dividido por valores que isolam   *
*    seus digitos de forma que este dado pode ser inserido no mesmo      *
*    formato que instrucoes, com opcode e endereco a ser utilizado.      *
*                                                                        *
*        Terminada esta execucao garante-se que nao ha erros no programa,*
*    entao cria-se ou atualiza-se o arquivo de saida imprimindo a lista  *
*    ligada que representa o mapa de memoria de acordo com as definicoes *
*    do enunciado, com cada linha no formato "AAA DD DDD DD DDD".        *
*                                                                        *
*        Mais detalhes podem ser vistos no proprio codigo, ontem tentei  *
*    comentar da forma mais clara possivel.                              *
*                                                                        *
*                                                                        *
**************************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "listas.h"
#include "auxiliares.h"
#include "computar.h"


// A funcao main apenas faz o controle dos arquivos de entrada e saida. Ela chama as funcoes que computam os rotulos, as intrucoes e dados, e depois a que imprime o resultado
int main(int args, char **argv)
{

    char *input; // ponteiro para string com nome do arquivo de entrada
    char *output; // ponteiro para string com nome do arquivo de saida

    Rotulo *rot = NULL; // Lista ligada com rotulos
    SET *set = NULL; // Lista ligada para guardar strings definidas pela diretiva ".set"
    MapaMem *mapa = NULL; // Lista ligada que representa o mapa de memoria
    PC pc; // Variavel representando PC

    //int i; // Variavel auxiliar
    char temp[64]; // Variavel auxiliar


    if(args < 2) // Caso nao seja passado arquivo de entrada imprime erro na tela
    {
        printf("Sem arquivo de entrada\n");
        return 0;
    }
    if(args == 3) // Caso haja arquivo de entrada e arquivo de saida atribui os path para as variaveis definidas
    {
        input = argv[1];
        output = argv[2];
    }
    else if(args == 2) // Caso so haja arquivo de entrada copia o nome do mesmo e concatena com ".hex"
    {
        input = argv[1];
        output = strcpy(temp, argv[1]);
        strcat(output, ".hex");
    }
    else // Caso haja mais q dois arquivos emite um erro
    {
        printf("Entrada incorreta, verifique espaços\n");
        return 0;
    }
    FILE *input_file  = fopen(input,"r"); // Abre arquivo para leitura

    if(input_file == NULL) // Em caso de arquivo inexistente emite um erro
    {
        printf("Arquivo de entrada nao encontrado");
        return 0;
    }

    // Inicializa PC com endereco 0 a esquerda
    pc.endereco = 0;
    pc.lado = esq;

    if(compRotulos(&rot, pc, input_file, &set))   // Chama funcao que completa a tabela de rotulos e verifica grande partes dos erros de sintaxe
    {
        // Caso haja um erro libera memoria e termina a execução sem criar ou alterar o arquivo de saida
        freeROT(rot);
        freeSET(set);
        fclose(input_file);
        return 0;
    }
    rewind(input_file); // Altera apontador para o inicio do arquivo;

    if(compInstrucoes(&mapa, pc, input_file, &set, rot))   // Chama funcao que computa as intrucoes e gera o mapa de memoria
    {
        // Caso haja um erro libera memoria e termina a execução sem criar ou alterar o arquivo de saida
        freeROT(rot);
        freeSET(set);
        fclose(input_file);
        return 0;
    }

    // Libera memoria
    freeROT(rot);
    freeSET(set);

    FILE *output_file = fopen(output,"w"); // Abre o arquivo de saida
    imprimirMAPA(mapa, output_file); // Imprime o mapa de memoria no arquivo de saida

    //libera memoria
    freeMAPA(mapa);

    // Fecha os arquivos
    fclose(input_file);
    fclose(output_file);

    return 0;

}


