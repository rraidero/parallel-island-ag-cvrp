#include <stdio.h>
#include <stdlib.h>
#include "island.h"
#include "genetic.h"
#include "list.h"

//construir estrutura de uma ilha
Island *IslandNew(char *arq_config_genetic, Metadata *metadata)
{

    Island *island = (Island *)malloc(sizeof(Island));
    island->GENETIC = NULL;
    island->METADATA = metadata;

    // abrir e carregar dados do arquivo de configurações do modelo
    // abrir e carregar dados dos algoritmos geneticos
    FILE *arq_genetics;
    char string_lida[100] = ""; // buffer linha arquivo
    arq_genetics = fopen(arq_config_genetic, "rt");

    if (arq_genetics == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
        return NULL;
    }

    int count_threads = metadata->COUNT_THREADS;

    fgets(string_lida, sizeof(string_lida), arq_genetics); // ler ring
    fgets(string_lida, sizeof(string_lida), arq_genetics); // ler input
    fgets(string_lida, sizeof(string_lida), arq_genetics); // ler output
    fgets(string_lida, sizeof(string_lida), arq_genetics); // repeticoes
    fgets(string_lida, sizeof(string_lida), arq_genetics); // execucoes
    fgets(string_lida, sizeof(string_lida), arq_genetics); // islands

    for (int i = 0; i < count_threads; i++)
    {
        Genetic *genetic = GeneticNew(arq_genetics, metadata);
        island->GENETIC = ListInsertEnd(island->GENETIC, genetic, GeneticRemove);
        //GeneticPrint(genetic);
    }

    //se possuir mais que 1 ilha
    if (count_threads > 1)
    {

        Genetic *genetic_last = ListGetPosicao(island->GENETIC, 0);
        Genetic *genetic_current = ListGetPosicao(island->GENETIC, count_threads - 1);

        if (metadata->RING_BIDIRECTIONAL == 1)
        {
            genetic_last->IMMIGRATION_LEFT = &(genetic_current->IMMIGRATION);
            genetic_last->MUTEX_IMMIGRATION_LEFT = &(genetic_current->MUTEX_IMMIGRATION);
        }
        else
        {
            genetic_last->IMMIGRATION_LEFT = NULL;
            genetic_last->MUTEX_IMMIGRATION_LEFT = NULL;
        }
        genetic_current->IMMIGRATION_RIGHT = &(genetic_last->IMMIGRATION);
        genetic_current->MUTEX_IMMIGRATION_RIGHT = &(genetic_last->MUTEX_IMMIGRATION);

        //se possuir mais que 2 ilhas, referenciar os vizinhos a direita e a esquerda
        if (count_threads > 2)
        {

            for (int i = 1; i < count_threads; i++)
            {
                genetic_current = ListGetPosicao(island->GENETIC, i);
                genetic_last->IMMIGRATION_RIGHT = &(genetic_current->IMMIGRATION);
                genetic_last->MUTEX_IMMIGRATION_RIGHT = &(genetic_current->MUTEX_IMMIGRATION);

                if (metadata->RING_BIDIRECTIONAL == 1)
                {
                    genetic_current->IMMIGRATION_LEFT = &(genetic_last->IMMIGRATION);
                    genetic_current->MUTEX_IMMIGRATION_LEFT = &(genetic_last->MUTEX_IMMIGRATION);
                }
                else
                {
                    genetic_last->IMMIGRATION_LEFT = NULL;
                    genetic_last->MUTEX_IMMIGRATION_LEFT = NULL;
                }
                genetic_last = genetic_current;
            }
        }
    }

    fclose(arq_genetics);

    return island;
}

// desalocar  instancia de ilhas
void IslandRemove(Island *island)
{
    if (island->GENETIC != NULL)
    {
        ListRemove(island->GENETIC);
    }
    free(island);
}

//funcao que executa as ilhas
void IslandExecute(Island *island)
{
    int i = ListSize(island->GENETIC);

    //criar a pthreads
    for (int j = 0; j < i; j++)
    {
        Genetic *genetic = (Genetic *)ListGetPosicao(island->GENETIC, j);
        pthread_create(&genetic->THREAD_ID, NULL, GeneticExecute, genetic);
    }

    //enquanto todas nao encerram
    for (int j = 0; j < i; j++)
    {
        Genetic *genetic = (Genetic *)ListGetPosicao(island->GENETIC, j);
        pthread_join(genetic->THREAD_ID, NULL);
    }

    //imprimir resultados se necessario
    // for (int j = 0; j < i; j++)
    // {
    //     Genetic *genetic = (Genetic *)ListGetPosicao(island->GENETIC, j);
    //     //GeneticPrint(genetic);
    //     GeneticPrintResume(genetic);
    // }
}