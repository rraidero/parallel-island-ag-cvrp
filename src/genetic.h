#ifndef GENETIC_H
#define GENETIC_H

#include <pthread.h>
#include <stdbool.h>
#include "list.h"
#include "genetic.h"
#include "metadata.h"

// definicao da estrutura coordenada
typedef struct genetic
{

    pthread_t THREAD_ID; // thread do algoritmo genetico

    int COUNT_GENERATIONS;  // quantidade de geracoes
    int SIZE_POPULATION;    // tamanho da populacao
    int SIZE_CROMOSSOME;    // tamanho do cromossomo
    int GENERATION_USE;     // quantidade de individuos "reaproveitados" das novas geracoes
    char *SELECTION_METHOD; // metodo de selecao
    double SELECTION_RATE;
    char *CROSSOVER_METHOD; // metodo de reproducao

    char *FILE_OUTPUT; // arquivo para resultados
    FILE *ARQ_OUTPUT;  // ponteiro para o arquivo

    char *MUTATION_METHOD; // metodo de mutacao
    double MUTATION_RATE_SWAP;
    double MUTATION_RATE_INSERTION;
    double MUTATION_RATE_SHUFFLE;
    double MUTATION_RATE_INVERSION;

    Metadata *METADATA;
    ListSorted *POPULATION;     // lista populacao atual
    ListSorted *POPULATION_NEW; // lista para os filhos gerados

    ListSorted *IMMIGRATION;           // lista de imigrantes
    pthread_mutex_t MUTEX_IMMIGRATION; // mutex para acesso a lista de imigrantes
    int COUNT_IMMIGRATION;             // parametro quantidade de imigrantes a serem enviados

    ListSorted **IMMIGRATION_LEFT;           // lista de imigrantes da ilha a esquerda
    pthread_mutex_t *MUTEX_IMMIGRATION_LEFT; // mutex para acesso a lista de imigrantes

    ListSorted **IMMIGRATION_RIGHT;           // lista de imigrantes da ilha a direita
    pthread_mutex_t *MUTEX_IMMIGRATION_RIGHT; // mutex para acesso a lista de imigrantes

} Genetic;

typedef struct cromossome
{
    int *CROMOSSOME;
    double FITNESS;
} Cromossome;

// inicializacao da estrutura genetica
Genetic *GeneticNew(FILE *file_config, Metadata *metadata);
// remocao da estrutura genetica
void GeneticRemove(void *genetic);
// inicializacao da populacao
void GeneticInitializePopulation(Genetic *genetic);
// desalocar cromossomo
void GeneticCromossomeDealocate(void *cromossome);
// embaralhar e validar cromossomo
void GeneticShuffleAndValid(Genetic *genetic, Cromossome *cromossomo, int *vet, float soma);
// checar se cromossomo eh valido
bool GeneticCheckCromossomeIsValid(Genetic *genetic, Cromossome *cromossomo);
// checar se cromossomo ja existe na populacao
bool CromossomeCheckIfExists(Genetic *genetic, Cromossome *cromossomo);
// realiza o partially-mapped crossover de 2 cromossomos
void GeneticCrossoverPMX(Cromossome *cromossomeX1, Cromossome *cromossomeY1, Cromossome *cromossomeX2, Cromossome *cromossomeY2, int size);
// printar o genetico
void GeneticPrint(Genetic *genetic);
// executar o genetico
void *GeneticExecute(void *genetic);
// printar o cromossomo
void CromossomePrint(Cromossome *cromossome, int size);
// obter fitness do cromossomo
void CromossomeFitness(Genetic *genetic, Cromossome *cromossomo);
// gerar uma copia do cromossomo
Cromossome *CromossomeCopy(Genetic *genetic, Cromossome *cromossome);
// funcao generica para o crossover
void GeneticCrossover(Genetic *genetic);
// funcao generica mutacao
void GeneticMutation(Genetic *genetic);
// funcao para consolidar geracoes em genetico
void GeneticConsolidate(Genetic *genetic);
//reparar cromossomos invalidos
void GeneticCromossomeRepair(Genetic *genetic, Cromossome *cromossomo);
//operador de selecao
void GeneticSelect(Genetic *genetic, Cromossome **x1, Cromossome **y1);
// Procurar posicao valida pelo qual a posicao pode ser inserida
int GeneticCromossomeSwapSearch(Genetic *genetic, Cromossome *cromossomo, int position);
//funcao para tipos de mutacao
void GeneticMutation_Swap(Genetic *genetic, Cromossome *cromossome);
void GeneticMutation_Insertion(Genetic *genetic, Cromossome *cromossome);
void GeneticMutation_Shuffle(Genetic *genetic, Cromossome *cromossome);
void GeneticMutation_Inversion(Genetic *genetic, Cromossome *cromossome);
//funcao gerar arquivo de relatorio
void GeneticFileReport(Genetic *genetic);
//imprimir resumo
void GeneticPrintResume(Genetic *genetic);
//converte solucao da representacao AG para o dominio do problema
void CromossomeRouteGenerator(Genetic *genetic, Cromossome *cromossome);

#endif