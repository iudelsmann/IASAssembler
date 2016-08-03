#include<stdio.h>


/************************************************************************************************************
*                                                                                                           *
*        Esta funcao le o arquivo linha a linha e cria a lista ligada que representa a tabela de rotulos.   *
*    Ela tambem cria a lista ligada que representa as strings definidas em diretivas ".set" apenas para     *
*    facilitar na execucao do proximo passo.                                                                *
*        Alguns erros sao verificados nesta funcao, que retorna 1 caso haja erro e 0 caso contrario.        *
*                                                                                                           *
************************************************************************************************************/
int compRotulos( Rotulo **rot, PC pc, FILE *input_file, SET **set);


/************************************************************************************************************
*                                                                                                           *
*        Esta funcao le o arquivo linha a linha e cria a lista ligada que representa o mapa de memoria.     *
*    Usa-se e atualiza-se a tabela de strings definidas em ".set", e usa-se, obviamente, a tabela de        *
*    rotulos.                                                                                               *
*       Diversos erros sao verificados nesta funcao, que retorna 1 caso haja erro e 0 caso contrario.       *
*                                                                                                           *
************************************************************************************************************/
int compInstrucoes(MapaMem **mapa, PC pc, FILE *input_file, SET **set, Rotulo *rot);
