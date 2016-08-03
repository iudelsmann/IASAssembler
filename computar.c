#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include "listas.h"
#include "auxiliares.h"
#include "computar.h"



/************************************************************************************************************
*                                                                                                           *
*        Esta funcao le o arquivo linha a linha e cria a lista ligada que representa a tabela de rotulos.   *
*    Ela tambem cria a lista ligada que representa as strings definidas em diretivas ".set" apenas para     *
*    facilitar na execucao do proximo passo.                                                                *
*        Alguns erros sao verificados nesta funcao, que retorna 1 caso haja erro e 0 caso contrario.        *
*                                                                                                           *
************************************************************************************************************/
int compRotulos( Rotulo **rot, PC pc, FILE *input_file, SET **set)
{

    char linha[256]; // String para uma linha
    int line = 0; // Contador de linhas lidas (para saber em qual linha foi o erro
    char *palavra; // String para guardar as palavras;
    Boolean achouRot, achouInstOuDir; // Auxiliar para encontrar dois rotulos na mesma linha

    SET *SETaux = NULL; // Apontador para no da lista ligada de strings definidas em diretivas ".set", utilizado para retorno de buscas
    Rotulo *ROTaux = NULL; // Apontador para no da lista ligada rotulos, utilizado para retorno de buscas

    int i, j; // Inteiros auxiliares


    while(fgets(linha, 256, input_file) != NULL) // Le linha a linha o arquivo de entrada enquanto houver linhas
    {


        line++; // Contaddor de linhas

        achouRot = false; // Seta a variavel auxiliar que indica a presenca de um rotulo na linha para false ja que e uma linha nova
        achouInstOuDir = false; // Seta a variavel auxiliar que indica a presenca de um intrucao ou diretiva na linha para false ja que e uma linha nova

        // Remove tudo q vier a partir do caracter #, se presente, removendo comentários
        if((palavra = strchr(linha, '#')))
        {
            linha[palavra-linha] = '\0';
        }
        stringToUpper(linha); // Passa a string inteira para maiusculas

        palavra = strtok (linha," \n\t"); // Quebra a linha em palavras
        while (palavra != NULL) // Enquanto houver palavreas
        {


            SETaux = buscaSET((*set), palavra); // Verifica se a palavra encontrada esta definida na lista de strings definidas em ".set"
            if(SETaux != NULL) // Caso encontre a string, atualiza a palavra a ser verificada
            {
                strcpy(palavra, SETaux->substituto);
            }

            // Verifica se a palavra e um rotulo, caso positivo, adiciona-a na tabela de rotulos
            if(verRotulo( palavra ))
            {

                // Caso um rotulo ja tenha sido encontrado nesta linha, gera um erro
                if(achouRot)
                {
                    printf("Erro, dois rotulos na linha: %d\n", line);
                    return 1;
                }

                // Se uma intrucao ou diretiva ja foi encontrada nesta linha, gera um erro
                if(achouInstOuDir)
                {
                    printf("Erro, rotulo apos intrucao ou diretiva na linha: %d\n", line);
                    return 1;
                }

                // Remove os : do fim da palavra
                i = strlen(palavra);
                palavra[i-1] = '\0';

                // Caso o rotulo por algum motivo esteja em um endereco maior dos que os enderecos representaveis gera erro
                if(pc.endereco > 4095)
                {
                    printf("Erro, rotulo com endereco muito grande na linha %d\n", line);
                    return 1;
                }

                // Verifica se um rotulo com mesmo nome ja nao foi definido anteriormente e gera erro
                ROTaux = buscaROT((*rot), palavra);
                if(ROTaux != NULL)
                {
                    printf("Erro, rotulo ja definido com mesmo nome na linha %d\n", line);
                    return 1;
                }

                // Adiciona para a tabela de rotulos
                (*rot) = inserirROT(*rot, palavra, pc.endereco, pc.lado);

                achouRot = true; // Seta a variavel indicando que ja foi encontrado um rotulo nesta linha
            }

            // Verifica se a palavra encontrada e uma instrucao
            else if( (i = verIntrucao( palavra, esq)))
            {

                // Caso outra instrucao ja tenha sido encontrada na linha, gera erro
                if(achouInstOuDir)
                {
                    printf("Erro, linha invalida, duas instrucoes ou instrucao apos diretiva na linha %d\n", line);
                    return 1;
                }

                // Atualiza o PC
                if(pc.lado == esq)
                    pc.lado = dir;
                else
                {
                    pc.lado = esq;
                    pc.endereco += 1;
                }

                // Caso a instrucao precise de endereco para ser executada, procura pela proxima palavra, caso nao haja, gera erro
                if( i != 10 && i != 20 && i != 21)
                {
                    palavra = strtok (NULL, " \n\t");
                    if(palavra == NULL )
                    {
                        printf("Erro, sem endereco para instrucao na linha %d\n", line);
                        return 1;
                    }
                }

                achouInstOuDir = true;// Seta a variavel indicando que ja foi encontrado uma instrucao ou diretiva nesta linha
            }

            // Verifica se a palavra encontrada e uma diretiva
            else if((i = verDiretiva( palavra)))
            {

                // Caso ja tenha encontrado uma instrucao ou rotulo, gera erro
                if(achouInstOuDir)
                {
                    printf("Erro, linha invalida, duas diretivas ou diretiva apos instrucao na linha %d\n", line);
                    return 1;
                }

                // Analisa o retorno da funcao verDiretiva() para saber qual diretiva foi encontrada
                switch(i)
                {

                // Caso seja um diretiva .org atualiza o PC de acordo
                case 1:

                    // Procura a proxima palavra para saber o endereco apos a diretiva ".org", caso nao haja palavra gera erro
                    palavra = strtok (NULL, " \n\t");
                    if(palavra == NULL)
                    {
                        printf("Erro apos diretiva \".org\" na linha %d\n", line);
                        return 1;
                    }

                    // Verifica se a palavra apos a diretiva foi definida em um ".set"
                    SETaux = buscaSET((*set), palavra);
                    if(SETaux != NULL)
                    {
                        strcpy(palavra, SETaux->substituto);
                    }

                    // Verifica se o endereco fornecido e negativo e gera erro
                    if(palavra[0] == '-')
                    {
                        printf("Erro, endereco negativo apos diretiva \".org\" na linha %d\n", line);
                        return 1;
                    }

                    // Verifica a base numerica do endereco encontrado
                    if(( j = verBase(palavra)))
                    {
                        if(j != 10)
                            palavra += 2;

                        // Transforma a string em inteiro com valor coerente e atualiza PC
                        pc.endereco = (int)strtol(palavra, NULL, j);
                        pc.lado = esq;


                        // Gera warning caso o enderço supere os 1024 disponiveis
                        if(pc.endereco > 1023)
                        {
                            printf("Warning, endereco muito grande apor diretiva \".org\" na linha %d\n", line);
                        }
                    }

                    // Caso nao seja um numero valido apos a diretiva, gera erro
                    else
                    {
                        printf("Erro apos diretiva \".org\" na linha %d\n,", line);
                        return 1;
                    }

                    break;

                // Caso seja um diretiva .align
                case 2:

                    // Procura a proxima palavra para saber qual align deve ser feito, caso nao haja palavra gera erro
                    palavra = strtok (NULL, " \n\t");
                    if(palavra == NULL)
                    {
                        printf("Erro apos diretiva .align na linha %d\n", line);
                        return 1;
                    }

                    // Verifica se a palavra apos a diretiva foi definida em um ".set"
                    SETaux = buscaSET((*set), palavra);
                    if(SETaux != NULL)
                    {
                        printf("%s\n", SETaux->substituto);
                        strcpy(palavra, SETaux->substituto);
                        printf("%s\n", palavra);
                    }

                    // Caso seja um numero negativo gera erro
                    if(palavra[0] == '-')
                    {
                        printf("Erro, valor negativo apos diretiva .align na linha %d\n", line);
                        return 1;
                    }

                    // Verifica a base da palavra
                    if(( j = verBase(palavra)))
                    {
                        j = (int)strtol(palavra, NULL, j); // Converte a string para seu valor inteiro
                    }

                    // Caso nao seja um numero ou seja 0, gera erro
                    else
                    {
                        printf("Erro apos diretiva \".align\" na linha %d\n", line);
                        return 1;
                    }
                    if(j==0)
                    {
                        printf("Erro, valot nulo apos diretiva \".align\" na linha %d\n", line);
                        return 1;
                    }

                    // Atualiza PC de acordo
                    if( (pc.endereco % j) == 0)
                    {
                        if(pc.lado == dir)
                        {
                            pc.lado = esq;
                            pc.endereco += j;
                        }

                    }
                    else
                    {
                        pc.endereco += (pc.endereco % j);
                        pc.lado = esq;

                    }
                    break;

                // Caso seja um .wfill
                case 3:

                    // Gera erro caso o codigo nao esteja alinhado
                    if(pc.lado == dir)
                    {
                        printf("Erro, codigo nao alinhado na linha %d\n", line);
                        return 1;
                    }
                    // Caso esteja alinhado
                    else
                    {
                        // Procura a proxima palavra e caso nao haja, gera erro
                        palavra = strtok (NULL, " \n\t,");
                        if(palavra == NULL)
                        {
                            printf("Erro apos diretiva \".wfill\" na linha %d\n", line);
                            return 1;
                        }

                        // Verifica se a palavra apos a diretiva foi definida em um ".set"
                        SETaux = buscaSET((*set), palavra);
                        if(SETaux != NULL)
                        {
                            printf("%s\n", SETaux->substituto);
                            strcpy(palavra, SETaux->substituto);
                            printf("%s\n", palavra);
                        }

                        // Caso valor seja negativo gera erro
                        if(palavra[0] == '-')
                        {
                            printf("Erro, valor negativo apos diretiva \".wfill\" na linha %d\n", line);
                            return 1;
                        }

                        // Verifica e converte o valor
                        if((j = verBase(palavra)))
                        {
                            if(j != 10)
                                palavra += 2;

                            // Caso valor seja nulo
                            if( (i = (int)strtol(palavra, NULL, j)) == 0)
                            {
                                printf("Erro, valor nulo apos diretiva \".wfill\" na linha %d\n", line);
                                return 1;
                            }

                            // Procura proxima palavra, caso nao haja, gera erro
                            palavra = strtok (NULL, " \n\t,");
                            if(palavra == NULL)
                            {
                                printf("Erro apos diretiva \".wfill\" na linha %d\n", line);
                                return 1;
                            }

                            // Atualiza PC de acordo
                            pc.endereco += i;
                        }

                        // Caso nao seja um numero valido
                        else
                        {
                            printf("Erro apos diretiva \".wfill\" na linha %d\n", line);
                            return 1;
                        }
                    }
                    break;

                // Caso seja um .word
                case 4:

                    // Gera erro caso o codigo nao esteja alinhado
                    if(pc.lado == dir)
                    {
                        printf("Erro, codigo nao alinhado na linha %d\n", line);
                        return 1;
                    }

                    // Se estiver alinhado atualiza o PC de acordo
                    else
                    {
                        pc.endereco += 1;
                        palavra = strtok (NULL, " \n\t");
                    }
                    break;

                // Caso seja a diretiva .set
                case 5:

                    // Procura pela proxima palavra, caso nao haja, gera erro
                    palavra = strtok (NULL, " \n\t");
                    if(palavra == NULL)
                        {
                            printf("Erro apos diretiva \".set\" na linha %d\n", line);
                            return 1;
                        }

                    // Verifica se a string definida e valida, caso nao seja, gera erro
                    if(!validSET(palavra))
                    {
                        printf("Erro, string invalida apos diretiva \".set\" na linha %d\n", line);
                        return 1;
                    }

                    // Verifica se a string ja foi definida, caso nao insere na lista
                    SETaux = buscaSET((*set), palavra);
                    if( SETaux == NULL)
                    {
                        (*set) = inserirSET(*set, palavra, strtok (NULL, " \n\t"));
                    }

                    // Caso ja exista, apenas atualiza o valor da segunda string
                    else
                    {
                        strcpy(SETaux->substituto, strtok (NULL, " \n\t"));
                    }

                    break;

                }
                achouInstOuDir = true; // Seta a variavel indicando que ja foi encontrado uma instrucao ou diretiva nesta linha
            }

            // Caso nao seja rotulo, instrucao ou diretiva, a palavra eh invalida e gera um erro
            else
            {
                printf("Palavra %s nao reconhecida na linha %d\n", palavra, line);
                return 1;
            }

            // Procura a proxima palavra antes de reiniciar o loop
            palavra = strtok (NULL, " \n\t");
        }

    }
    return 0; // Retorna sem erros
}


/************************************************************************************************************
*                                                                                                           *
*        Esta funcao le o arquivo linha a linha e cria a lista ligada que representa o mapa de memoria.     *
*    Usa-se e atualiza-se a tabela de strings definidas em ".set", e usa-se, obviamente, a tabela de        *
*    rotulos.                                                                                               *
*       Diversos erros sao verificados nesta funcao, que retorna 1 caso haja erro e 0 caso contrario.       *
*                                                                                                           *
************************************************************************************************************/
int compInstrucoes(MapaMem **mapa, PC pc, FILE *input_file, SET **set, Rotulo *rot)
{

    char linha[256]; // String para guardar uma linha lida
    char *palavra, *aux; // Strings para guardar palavra e auxiliar respectivamente
    int line = 0; // Contador de linhas

    SET *SETaux = NULL; // Apontador para no da lista ligada de strings definidas em diretivas ".set", utilizado para retorno de buscas
    Rotulo *ROTaux = NULL; // Apontador para no da lista ligada rotulos, utilizado para retorno de buscas

    int i, j, k; // Inteiros auxiliares
    long long int lint; // Inteiro de 64bits auxiliar



    while(fgets(linha, 256, input_file) != NULL) // Le linha a linha o arquivo de entrada
    {


        line++; // Contador de linhas


        // Remove tudo q vier a partir do caracter #, se presente, removendo comentários
        if((palavra = strchr(linha, '#')))
        {
            linha[palavra-linha] = '\0';
        }
        stringToUpper(linha); // Passa a string inteira para maiusculas

        palavra = strtok (linha," \n\t"); // Quebra a linha em palavras
        while (palavra != NULL) // Enquanto houver palavreas
        {

            //Caso a palavra seja comentário pula para a proxima linha

            SETaux = buscaSET((*set), palavra); // Verifica se a palavra encontrada esta definida na lista de strings definidas em ".set"
            if(SETaux != NULL) // Caso encontre a string, atualiza a palavra a ser verificada
            {
                strcpy(palavra, SETaux->substituto);
            }

            // Verifica se a palavra eh um rotulo, caso positivo, nada se faz, reinicia o loop
            if(verRotulo( palavra ))
            {
                palavra = strtok(NULL," \n\t");
                continue;
            }

            // Caso seja um instrucao
            else if( (i = verIntrucao( palavra, esq)))
            {

                // Caso a instrucao precise de endereco
                if(i != 10 && i != 20 && i != 21 )
                {
                    aux = palavra; // guar a instrucao
                    palavra = strtok(NULL," \n\t"); // Pega a proxima palavra, o enereco

                    // Se for endereco numerico atualiza a string removendo o M()
                    if(palavra[0] == 'M' && palavra[1] == '(')
                    {
                        palavra[strlen(palavra)-1] = '\0';
                        palavra += 2;

                        SETaux = buscaSET((*set), palavra); // Verifica se a palavra encontrada esta definida na lista de strings definidas em ".set"
                        if(SETaux != NULL) // Caso encontre a string, atualiza a palavra a ser verificada
                        {
                            strcpy(palavra, SETaux->substituto);
                        }

                        // Verifica se o endereco e valido
                        if((j = verBase(palavra)))
                        {

                            // Pega o valor do endereco
                            k = (int)strtol(palavra, NULL, j);

                            // Caso o endereco seja muito grande, gera erro
                            if( k > 4095)
                            {
                                printf("Erro, tentando acessar endereco de memoria muito grande na linha %d\n", line);
                                return 1;
                            }

                            // Caso seja negativo idem
                            else if( k<0)
                            {
                                printf("Erro, tentando acessar endereco de memoria negativo na linha %d\n", line);
                                return 1;
                            }

                            // Caso seja superior ao disponiveis, porem ainda representavel, apenas gera warning
                            if( k > 1023 && k < 4096)
                                printf("Warning, operando sobre endereco de memoria maior que memoria disponivel na linha %d\n", line);

                            // Caso a instrucao for ocupar endereco superior aos disponiveis, gera erro
                            if(pc.endereco > 1023)
                            {
                                printf("Erro, tentando acessar endereco de memoria muito grande na linha %d\n", line);
                                return 1;
                            }

                            // Caso tudo ocorra como esperado, insere a instrucao no mapa de intrucoes
                            inserirMAPA(mapa, verIntrucao(aux, esq), pc.endereco, pc.lado, k);
                        }

                        // Caso o endereco nao seja um numero valido
                        else
                        {
                            printf("Erro apos instrucao na linha %d\n", line);
                            return 1;
                        }
                    }

                    // Caso nao comece com M( entao deve ser um rotulo, busco o rotulo, e caso ache, insere a instrucao no mapa de memoria
                    else if( (ROTaux = buscaROT(rot, palavra)))
                    {
                        // Caso o endereco do rotulo seja grande mais ainda representavel, gera warning
                        if(ROTaux->endereco > 1023 && ROTaux->endereco < 4096)
                        {
                            printf("Warning, operando sobre endereco de memoria maior que memoria disponivel na linha %d\n", line);

                        }

                        // Caso nao seja uma instrucao de JUMP ou STM, se o endereco do rotulo for a direita, gera erro
                        else if( strcmp(aux, "JMP") && strcmp(aux, "JGEZ") && strcmp(aux, "STM"))
                        {
                            if(ROTaux->lado == dir)
                            {
                                printf("Erro, carregando endereco a direita na linha %d\n", line);
                                return 1;
                            }

                        }

                        // Insere a instrucao no mapa de memoria
                        inserirMAPA(mapa, verIntrucao(aux, ROTaux->lado), pc.endereco, pc.lado, ROTaux->endereco);
                    }

                    // Caso nao seja nem comecado com M( e nem seja um rotulo, entao gera erro
                    else
                    {
                        printf("Erro apos instrucao na linha %d\n", line);
                        return 1;
                    }
                }

                // Caso nao seja uma instrucao com endereco (como RSH)
                else
                {
                    if(pc.endereco > 1023)
                    {
                        printf("Erro, tentando acessar endereco de memoria muito grande na linha %d\n", line);
                        return 1;
                    }
                    inserirMAPA(mapa, verIntrucao(palavra, esq), pc.endereco, pc.lado, 0);
                }

                // Atualiza o PC
                if(pc.lado == esq)
                    pc.lado = dir;
                else
                {
                    pc.lado = esq;
                    pc.endereco += 1;
                }
            }
            else if((i = verDiretiva( palavra)))
            {
                switch(i) // Analisa o retorno para saber qual diretiva foi encontrada
                {

                // Caso seja um diretiva .org
                case 1:

                    // Procura a proxima palavra
                    palavra = strtok (NULL, " \n\t");


                    SETaux = buscaSET((*set), palavra);
                    if(SETaux != NULL)
                    {
                        strcpy(palavra, SETaux->substituto);
                    }

                    // Verifica a base do numero
                    if(( j = verBase(palavra)))
                    {
                        // Remove o "0b" 0x" "0o" caso existam
                        if(j != 10)
                            palavra += 2;

                        // Atribui o valor da string para uma variavel de inteiro
                        i = (int)strtol(palavra, NULL, j);

                        // Caso endereco seja muito grande gera erro
                        if( i > 4095)
                        {
                            printf("Erro, tentando acessar endereco de memoria muito grande na linha %d\n", line);
                            return 1;
                        }

                        // Atualiza PC
                        pc.endereco = i;
                        pc.lado = esq;
                    }

                    break;

                // Caso seja um diretiva .align
                case 2:

                    // Procura a proxima palavra
                    palavra = strtok (NULL, " \n\t");

                    // Verifica se a palavra foi definida em uma diretiva ".set" e atualiza caso positivo
                    SETaux = buscaSET((*set), palavra);
                    if(SETaux != NULL)
                    {
                        strcpy(palavra, SETaux->substituto);
                    }

                    // Verifica a base do numero
                    if((j = verBase(palavra)))
                    {
                        // Remove o "0b" 0x" "0o" caso existam
                        if(j != 10)
                            palavra += 2;

                        // Atribui o valor da string para uma variavel de inteiro
                        j = (int)strtol(palavra, NULL, j);

                        // Verifica se precisa mesmo ser feito alinhamento
                        if( (pc.endereco % j) == 0)
                        {
                            // Atualiza PC de acordo
                            if(pc.lado == dir)
                            {

                                pc.lado = esq;
                                pc.endereco += j;
                            }
                        }
                        else
                        {
                            // Atualiza PC de acordo
                            pc.endereco += (pc.endereco % j);
                            pc.lado = esq;
                        }
                    }

                    break;

                // Caso seja um .wfill
                case 3:

                    // Gera erro se o endereco superar as palavras disponiveis
                    if(pc.endereco > 1023)
                    {
                        printf("Erro, tentando acessar endereco de memoria muito grande na linha %d\n", line);
                        return 1;
                    }

                    // Procura a proxima palavra
                    palavra = strtok (NULL, " \n\t,");

                    // Verifica se a palavra foi definida em uma diretiva ".set" e atualiza caso positivo
                    SETaux = buscaSET((*set), palavra);
                    if(SETaux != NULL)
                    {
                        strcpy(palavra, SETaux->substituto);
                    }

                    // Verifica a base do numero
                    if((j = verBase(palavra)))
                    {

                        // Remove o "0b" 0x" "0o" caso existam
                        if(j != 10)
                        {
                            palavra += 2;
                        }

                         // Atribui o valor da string para uma variavel de inteiro
                        i = (int)strtol(palavra, NULL, j);

                        // Busca proxima palavra
                        palavra = strtok (NULL, " \n\t,");

                        // Pega a base da proxima palavra
                        k = verBase(palavra);
                        if(k != 10)
                        {
                            if(palavra[0] == '-')
                                palavra[2] = '-';
                            palavra += 2;
                        }

                        lint = strtoll(palavra, NULL, k); // Atribui o valor da string para variavel long long (64 bits porem apenas 40 poderam ser usados)

                        // Verifica se numero supera menor valor representavel e gera erro
                        if(lint < -549755813888LL)
                        {
                            printf("Erro, valor supera numeros representaveis com 40 bits na linha %d\n", line);
                            return 1;
                        }

                        // Se numero for negativo converte para valor em complemento de 2
                        else if(lint < 0)
                            lint = lint - 18446742974197923840ULL;

                        // Verifica se numero supera maior valor representavel e gera erro
                        else if(lint > 1099511627775LL)
                        {
                            printf("Erro, valor supera numeros representaveis com 40 bits na linha %d\n", line);
                            return 1;
                        }

                        // Gera warning caso numero seja escrito como positivo mas tenha o ulltimo bit setado
                        else if ( lint > 549755813887LL)
                        {
                            printf("Warning, numero %lld inserido podera ser interpretado como %lld na linha %d\n", lint, -549755813888LL + ( lint - 549755813888LL ) , line);
                        }

                        // Nesta parte, o valor a ser inserido sofre diferentes transformacoes para ser dividido na forma como a estrutura de dados foi definida,
                        // com opcode de 8bits, endereco de 12 bits, 2 palavras por endereco
                        for(k=0; k<i; k++)
                        {

                            int a1, a2, a3, a4;

                            a1 = lint/4294967296LL;
                            a2 = (lint % 4294967296LL)/1048576;
                            a3 = ((lint % 4294967296LL)%1048576)/4096;
                            a4 = (((lint % 4294967296LL)%1048576)%4096);

                            // Insere os valores calculador no mapa de memoria
                            inserirMAPA(mapa, (int)a1, pc.endereco, esq, (int)a2  );
                            inserirMAPA(mapa, (int)a3, pc.endereco, dir, (int)a4  );

                            pc.endereco += 1; // Atualiza PC
                        }
                    }
                    // Caso encontre erro
                    else
                    {
                        printf("Erro apos diretiva \".wfill\" na linha %d\n", line);
                        return 1;
                    }

                    break;

                // Caso seja um .word atualiza o pc de acordo
                case 4:

                    // Gera erro se o endereco superar as palavras disponiveis
                    if(pc.endereco > 1023)
                    {
                        printf("Erro, tentando acessar endereco de memoria muito grande na linha %d\n", line);
                        return 1;
                    }

                     // Procura a proxima palavra
                    palavra = strtok (NULL, " \n\t,");

                    // Verifica se a palavra foi definida em uma diretiva ".set" e atualiza caso positivo
                    SETaux = buscaSET((*set), palavra);
                    if(SETaux != NULL)
                    {
                        strcpy(palavra, SETaux->substituto);
                    }

                    // Verifica se palavra eh na verdade um rotulo e ja insere no mapa caso positivo
                    if( (ROTaux = buscaROT(rot, palavra)))
                    {
                        if(ROTaux->lado == dir)
                        {
                            printf("Warning, carregando endereco a direita apor diretiva \".word\" na linha %d\n", line);

                        }
                        inserirMAPA(mapa, 0, pc.endereco, esq, 0  );
                        inserirMAPA(mapa, (ROTaux->endereco)/4096, pc.endereco, dir,  (ROTaux->endereco)%4096  );
                        pc.endereco += 1;
                    }

                    // Verifica base do valor
                    else if((j = verBase(palavra)))
                    {

                        // Remove "0x" "0b" "0o" caso necessario
                        if(j != 10)
                        {
                            if(palavra[0] == '-')
                                palavra[2] = '-';
                            palavra += 2;
                        }

                        lint = strtoll(palavra, NULL, j); // Atribui o valor da string para variavel long long (64 bits porem apenas 40 poderam ser usados)

                        // Verifica se numero supera menor valor representavel e gera erro
                        if(lint < -549755813888LL)
                        {
                            printf("Erro, valor supera numeros representaveis com 40 bits na linha %d\n", line);
                            return 1;
                        }

                         // Se numero for negativo converte para valor em complemento de 2
                        else if(lint < 0)
                            lint = lint - 18446742974197923840ULL;

                        // Verifica se numero supera maior valor representavel e gera erro
                        else if(lint > 1099511627775LL)
                        {
                            printf("Erro, valor supera numeros representaveis com 40 bits na linha %d\n", line);
                            return 1;
                        }

                       // Gera warning caso numero seja escrito como positivo mas tenha o ulltimo bit setado
                        else if ( lint > 549755813887LL)
                        {
                            printf("Warning, numero %lld inserido sera interpretado como %lld na linha %d\n", lint, -549755813888LL + ( lint - 549755813888LL ) , line);
                        }

                        // Nesta parte, o valor a ser inserido sofre diferentes transformacoes para ser dividido na forma como a estrutura de dados foi definida,
                        // com opcode de 8bits, endereco de 12 bits, 2 palavras por endereco
                        int a1, a2, a3, a4;

                        a1 = lint/4294967296LL;
                        a2 = (lint % 4294967296LL)/1048576;
                        a3 = ((lint % 4294967296LL)%1048576)/4096;
                        a4 = (((lint % 4294967296LL)%1048576)%4096);

                        // Insere os valores calculador no mapa de memoria
                        inserirMAPA(mapa, (int)a1, pc.endereco, esq, (int)a2  );
                        inserirMAPA(mapa, (int)a3, pc.endereco, dir, (int)a4  );

                        pc.endereco += 1; // Atualiza PC
                    }

                    // Caso encontre erro
                    else
                    {
                        printf("Erro apos diretiva \".word\" na linha %d\n", line);
                        return 1;
                    }

                    break;

                // Caso seja a diretiva .set
                case 5:

                    // Procura proxima palavra
                    palavra = strtok (NULL, " \n\t");

                    // Atualiza string definida
                    SETaux = buscaSET((*set), palavra);
                    strcpy(SETaux->substituto, strtok (NULL, " \n\t"));

                    break;
                }
            }
            palavra = strtok (NULL," \n\t"); // Busca proxima palavra antes de reiniciar o loop
        }
    }
    return 0;
}
