#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//#include <Windows.h>
#include "metadata.h"
#include "island.h"
#include "genetic.h"

int main(int argc, char *argv[])
{

    srand(time(NULL));
    Metadata *metadata = MetadataNew(argv[1]);
    // MetadataPrint(metadata); //funcao para imprimir os metadados lidos, util para debug
    printf("Problema\tIlhas\tFitness\tTempo(ms)\tMelhor Individuo\n");

    for (int i = 0; i < metadata->COUNT_REPETITIONS; i++)
    {
        // 2: configuracao do modelo de ilhas, 3: configuracao dos algoritmos geneticos
        Island *island = IslandNew(argv[1], metadata);

        //  caso nao consiga montar o arquivo
        if (metadata == NULL || !MetadataCheckIsValid(metadata))
        {
            printf("Ocorreu um erro ao tentar processar o arquivo!");
            return 0;
        }

        // obter tempo para calculo do tempo de execucao das ilhas
        clock_t tempo_inicio = clock();

        // executar ilhas
        IslandExecute(island);

        clock_t tempo_fim = clock();
        double tempo_gasto = (double)((tempo_fim - tempo_inicio) *1000 / CLOCKS_PER_SEC);

        // checagem do melhor individuo dentre as ilhas
        Genetic *genetic = (Genetic *)ListGetPosicao(island->GENETIC, 0);
        Cromossome *cromossome = ListSortedGetPosition(genetic->POPULATION, 0);
        double best_fitness = cromossome->FITNESS;
        Cromossome *best_cromossome = cromossome;

        int k = ListSize(island->GENETIC);
        for (int j = 1; j < k; j++)
        {
            genetic = (Genetic *)ListGetPosicao(island->GENETIC, j);
            cromossome = ListSortedGetPosition(genetic->POPULATION, 0);
            if (best_fitness > cromossome->FITNESS)
            {
                best_fitness = cromossome->FITNESS;
                best_cromossome = cromossome;
            }
        }

        printf("\n%s", metadata->NAME);
        printf("\t%d", metadata->COUNT_THREADS);
        printf("\t%.0lf", best_fitness);
        printf("\t%.0lf\t\t", tempo_gasto);
        // CromossomePrint(best_cromossome, genetic->SIZE_CROMOSSOME);
        CromossomeRouteGenerator(genetic, best_cromossome);

        // imprimir resumo das ilhas na main caso necessario
        // for (int k = 0; k < ListSize(island->GENETIC); k++)
        // {
        //     GeneticPrint((Genetic *)ListGetPosicao(island->GENETIC, k));
        // }

        IslandRemove(island);
    }

    // MetadataPrint(metadata);
    MetadataRemove(metadata);
    printf("\n\n");
    return 0;
}
