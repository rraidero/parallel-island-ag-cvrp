#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "genetic.h"
#include "metadata.h"
#include "utils.h"
//#include <Windows.h>
#include <time.h>

// Entrada: arquivo de configuracao do genetico e os metadados
Genetic *GeneticNew(FILE *file_config, Metadata *metadata)
{

    Genetic *genetic = (Genetic *)malloc(sizeof(Genetic));

    char string_lida[150] = ""; // buffer linha arquivo
    char *key = NULL;
    char *value = NULL;
    char *tmp = NULL;

    fgets(string_lida, sizeof(string_lida), file_config); // ler linha em branco

    fgets(string_lida, sizeof(string_lida), file_config); // ler qnt geracoes
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->COUNT_GENERATIONS = atoi(value);

    fgets(string_lida, sizeof(string_lida), file_config); // ler tam populacao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->SIZE_POPULATION = atoi(value);

    fgets(string_lida, sizeof(string_lida), file_config); // ler tam populacao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->GENERATION_USE = atoi(value);

    fgets(string_lida, sizeof(string_lida), file_config); // ler metodo de selecao
    ExtractStringKeyValue(string_lida, &key, &value);
    tmp = value;
    genetic->SELECTION_METHOD = DuplicateSTR(++tmp);

    fgets(string_lida, sizeof(string_lida), file_config); // ler rate de selecao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->SELECTION_RATE = strtod(value, NULL);

    fgets(string_lida, sizeof(string_lida), file_config); // ler metodo de crossover
    ExtractStringKeyValue(string_lida, &key, &value);
    tmp = value;
    genetic->CROSSOVER_METHOD = DuplicateSTR(++tmp);

    fgets(string_lida, sizeof(string_lida), file_config); // ler metodo de mutacao
    ExtractStringKeyValue(string_lida, &key, &value);
    tmp = value;
    genetic->MUTATION_METHOD = DuplicateSTR(++tmp);

    fgets(string_lida, sizeof(string_lida), file_config); // ler rate de mutacao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->MUTATION_RATE_SWAP = strtod(value, NULL);

    fgets(string_lida, sizeof(string_lida), file_config); // ler rate de mutacao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->MUTATION_RATE_INSERTION = strtod(value, NULL);

    fgets(string_lida, sizeof(string_lida), file_config); // ler rate de mutacao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->MUTATION_RATE_SHUFFLE = strtod(value, NULL);

    fgets(string_lida, sizeof(string_lida), file_config); // ler rate de mutacao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->MUTATION_RATE_INVERSION = strtod(value, NULL);

    fgets(string_lida, sizeof(string_lida), file_config); // ler tam imigracao
    ExtractStringKeyValue(string_lida, &key, &value);
    genetic->COUNT_IMMIGRATION = atoi(value);

    fgets(string_lida, sizeof(string_lida), file_config); // ler metodo de crossover
    ExtractStringKeyValue(string_lida, &key, &value);
    tmp = value;
    genetic->FILE_OUTPUT = DuplicateSTR(++tmp);

    genetic->POPULATION = NULL;
    genetic->POPULATION_NEW = NULL;
    genetic->METADATA = metadata;
    genetic->IMMIGRATION_LEFT = NULL;
    genetic->MUTEX_IMMIGRATION_LEFT = NULL;
    genetic->IMMIGRATION_RIGHT = NULL;
    genetic->MUTEX_IMMIGRATION_RIGHT = NULL;
    genetic->IMMIGRATION = NULL;
    genetic->SIZE_CROMOSSOME = (metadata->DIMENSION) + (metadata->COUNT_VEHICLES) - 2;

    pthread_mutex_init(&(genetic->MUTEX_IMMIGRATION), NULL);

    return genetic;
}

// recebe uma referencia de genetico, e realiza a limpeza da memoria alocada dinamicamente
void GeneticRemove(void *ref)
{
    Genetic *genetic = (Genetic *)ref;
    ListSortedRemove((genetic)->POPULATION);
    ListSortedRemove((genetic)->POPULATION_NEW);
    ListSortedRemove((genetic)->IMMIGRATION);
    free(genetic->MUTATION_METHOD);
    free(genetic->CROSSOVER_METHOD);
    free(genetic->SELECTION_METHOD);
    free(genetic->FILE_OUTPUT);
    pthread_mutex_destroy(&(genetic)->MUTEX_IMMIGRATION);
    //free(genetic->MUTEX_IMMIGRATION);
    free(genetic);
}

// funcao para inicializacao de uma populacao
void GeneticInitializePopulation(Genetic *genetic)
{

    float soma = 0;
    int vet[genetic->METADATA->DIMENSION - 1];
    for (int i = 2; i <= genetic->METADATA->DIMENSION; i++)
    {
        soma += MetadataNodeDemand(genetic->METADATA, i);
        vet[i - 2] = i;
    }

    for (int i = 0; i < genetic->SIZE_POPULATION; i++)
    {
        Cromossome *cromossomo = (Cromossome *)malloc(sizeof(Cromossome));
        cromossomo->CROMOSSOME = (int *)malloc(genetic->SIZE_CROMOSSOME * sizeof(int));
        cromossomo->FITNESS = 0.0;
        GeneticShuffleAndValid(genetic, cromossomo, vet, soma);
        CromossomeFitness(genetic, cromossomo);
        ListSortedInsert(&(genetic->POPULATION), cromossomo, GeneticCromossomeDealocate, cromossomo->FITNESS);
    }
}

// funcao para embaralhar e retornar uma rota valida
void GeneticShuffleAndValid(Genetic *genetic, Cromossome *cromossomo, int *vet, float soma)
{
    // embaralhar vetor
    for (int j = 0; j < genetic->METADATA->DIMENSION - 1; j++)
    {
        int index = GerarInteiroAleatorio(0, genetic->METADATA->DIMENSION - 2);
        int aux = vet[j];
        vet[j] = vet[index];
        vet[index] = aux;
    }

    // inserir os divisores de rotas visando uma distribuicao mais proxima da media em que cada veiculo deve carregar
    float partes = soma / genetic->METADATA->COUNT_VEHICLES;
    int vehicle_insert = 0;
    float total = 0;

    // iterar ate que uma rota supere a media de cada parte, quando ocorrer inserir um divisor
    for (int k = 0; k < genetic->METADATA->DIMENSION - 1; k++)
    {
        cromossomo->CROMOSSOME[k + vehicle_insert] = vet[k];
        total += MetadataNodeDemand(genetic->METADATA, vet[k]);

        if (k < genetic->METADATA->DIMENSION - 2)
        {
            MetadataNodeDemand(genetic->METADATA, vet[k]);
        }

        if (total > partes)
        {
            vehicle_insert++;
            total = 0;
            cromossomo->CROMOSSOME[k + vehicle_insert] = genetic->METADATA->DIMENSION + vehicle_insert;
        }
    }

    while (vehicle_insert < genetic->METADATA->COUNT_VEHICLES - 1)
    {
        vehicle_insert++;
        cromossomo->CROMOSSOME[genetic->METADATA->DIMENSION + vehicle_insert - 2] = genetic->METADATA->DIMENSION + vehicle_insert;
    }

    // invocar a reparacao de cromossomo para corrigir problemas de capacidade eventuais nao superados anteriormente
    GeneticCromossomeRepair(genetic, cromossomo);
}

// consulta matriz de custos e gera o fitness do cromossomo
void CromossomeFitness(Genetic *genetic, Cromossome *cromossomo)
{
    double fitness = 0.0;
    fitness += MetadataNodeCost(genetic->METADATA, 1, cromossomo->CROMOSSOME[0]);
    for (int i = 1; i < genetic->SIZE_CROMOSSOME; i++)
    {
        fitness += MetadataNodeCost(genetic->METADATA, cromossomo->CROMOSSOME[i - 1], cromossomo->CROMOSSOME[i]);
    }
    fitness += MetadataNodeCost(genetic->METADATA, cromossomo->CROMOSSOME[genetic->SIZE_CROMOSSOME - 1], 1);
    cromossomo->FITNESS = fitness;
}

// gerar uma copia dinamicamente alocada de um cromossomo
Cromossome *CromossomeCopy(Genetic *genetic, Cromossome *cromossome)
{
    // alocar vetor de inteiros do tamanho das dimensões do problema
    Cromossome *new = (Cromossome *)malloc(sizeof(Cromossome));
    new->CROMOSSOME = (int *)malloc(genetic->SIZE_CROMOSSOME * sizeof(int));
    new->FITNESS = 0.0;
    for (int i = 0; i < genetic->SIZE_CROMOSSOME; i++)
    {
        new->CROMOSSOME[i] = cromossome->CROMOSSOME[i];
    }
    new->FITNESS = cromossome->FITNESS;
    return new;
}

// verificar se o cromossomo atende as restricoes
bool GeneticCheckCromossomeIsValid(Genetic *genetic, Cromossome *cromossomo)
{
    //  checagem de demanda superior ao teto de carga do veiculo
    int cost = 0;

    for (int i = 0; i < genetic->SIZE_CROMOSSOME; i++)
    {
        int y = cromossomo->CROMOSSOME[i];
        if (y > genetic->METADATA->DIMENSION)
        {
            y = 1;
        }

        if (y != 1)
        {
            cost += MetadataNodeDemand(genetic->METADATA, y);
        }
        else
        {
            if (cost > genetic->METADATA->CAPACITY)
            {
                return false;
            }
            else
            {
                cost = 0;
            }
        }
    }
    if (cost > genetic->METADATA->CAPACITY)
    {
        return false;
    }
    return true;
}

// recebe um cromossomo e chega se ele ja existe na populacao
bool CromossomeCheckIfExists(Genetic *genetic, Cromossome *cromossomo)
{
    for (int i = 0; i < genetic->SIZE_POPULATION; i++)
    {
        Cromossome *cromossome_pop = (Cromossome *)ListSortedGetPosition(genetic->POPULATION, i);
        if (cromossomo->FITNESS == cromossome_pop->FITNESS)
        {
            for (int i = 0; i < genetic->SIZE_CROMOSSOME; i++)
            {
                if ((cromossomo->CROMOSSOME[i] != cromossome_pop->CROMOSSOME[i]))
                {
                    break;
                }
                else if (i + 1 == genetic->SIZE_CROMOSSOME)
                {
                    return true;
                }
            }
        }
        else if (cromossomo->FITNESS > cromossome_pop->FITNESS)
        {
            return false;
        }
    }
    return false;
}

// Procurar posicao valida pelo qual a posicao pode ser inserida, retorna -1 se nao for possivel
int GeneticCromossomeSwapSearch(Genetic *genetic, Cromossome *cromossomo, int position)
{
    double cost = 0;
    int init = 0;
    for (int i = 0; i < genetic->SIZE_CROMOSSOME; i++)
    {
        int y = cromossomo->CROMOSSOME[i];
        if (!(y > genetic->METADATA->DIMENSION))
        {
            cost += MetadataNodeDemand(genetic->METADATA, y);
        }
        else
        {
            if ((cost + MetadataNodeDemand(genetic->METADATA, cromossomo->CROMOSSOME[position])) <= genetic->METADATA->CAPACITY)
            {
                if (init == i)
                {
                    if (init > position)
                        return init - 1;
                    else
                        return init - 1;
                }
                else
                    return GerarInteiroAleatorio(init, i - 1);
            }
            else
            {
                cost = 0;
                init = i + 1;
            }
        }
    }

    if ((cost + MetadataNodeDemand(genetic->METADATA, cromossomo->CROMOSSOME[position])) <= genetic->METADATA->CAPACITY)
    {
        if (init == genetic->SIZE_CROMOSSOME)
            init--;
        return GerarInteiroAleatorio(init, genetic->SIZE_CROMOSSOME - 1);
    }

    return -1;
}

// funcao para reparar cromossomo que quebra as restricoes de capacidade
void GeneticCromossomeRepair(Genetic *genetic, Cromossome *cromossomo)
{
    // se o primeiro ou o ultimo sao pontos de destino, consertar
    if (cromossomo->CROMOSSOME[0] > genetic->METADATA->DIMENSION)
    {
        int j = 1;
        while (cromossomo->CROMOSSOME[j] > genetic->METADATA->DIMENSION)
        {
            j++;
        }
        VetIntMoveAndInsert(cromossomo->CROMOSSOME, genetic->SIZE_CROMOSSOME, j, 0);
    }

    if (cromossomo->CROMOSSOME[genetic->SIZE_CROMOSSOME - 1] > genetic->METADATA->DIMENSION)
    {
        int j = genetic->SIZE_CROMOSSOME - 2;
        while (cromossomo->CROMOSSOME[j] > genetic->METADATA->DIMENSION)
        {
            j--;
        }
        VetIntMoveAndInsert(cromossomo->CROMOSSOME, genetic->SIZE_CROMOSSOME, j, genetic->SIZE_CROMOSSOME - 1);
    }

    // checagem de demanda superior ao teto do veiculo
    double cost = 0;

    for (int i = 0; i < genetic->SIZE_CROMOSSOME; i++)
    {
        int y = cromossomo->CROMOSSOME[i];
        if (!(y > genetic->METADATA->DIMENSION))
        {
            cost += MetadataNodeDemand(genetic->METADATA, y);
        }
        else
        {
            cost = 0;
            continue;
        }

        if (cost > genetic->METADATA->CAPACITY)
        {

            int j = GeneticCromossomeSwapSearch(genetic, cromossomo, i);

            //  se eu nao encontrar um posicao valida para inseri-lo
            if (j == -1)
            {
                //  enviar ele para outra posicao aleatoria, refazer a reparacao
                int pos = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
                while (pos == i)
                {
                    pos = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
                }
                VetIntMoveAndInsert(cromossomo->CROMOSSOME, genetic->SIZE_CROMOSSOME, pos, i);
                GeneticCromossomeRepair(genetic, cromossomo);
                break;
            }

            cost -= MetadataNodeDemand(genetic->METADATA, y);
            VetIntMoveAndInsert(cromossomo->CROMOSSOME, genetic->SIZE_CROMOSSOME, j, i);

            if (i < j)
                i--;
        }
    }
}

void GeneticCrossoverPMX(Cromossome *cromossomeX1, Cromossome *cromossomeY1, Cromossome *cromossomeX2, Cromossome *cromossomeY2, int size)
{
    int a = GerarInteiroAleatorio(0, size - 1);
    int b = GerarInteiroAleatorio(0, size - 1);
    if (b < a)
    {
        int aux = a;
        a = b;
        b = aux;
    }

    // resetar valores <<<<<<<<<< apagar
    for (int i = 0; i < size; i++)
    {
        cromossomeX2->CROMOSSOME[i] = cromossomeX1->CROMOSSOME[i];
        cromossomeY2->CROMOSSOME[i] = cromossomeY1->CROMOSSOME[i];
    }

    // Mapeia os elementos correspondentes do segmento copiado
    int *mapping1 = (int *)malloc((size + 2) * sizeof(int));
    int *mapping2 = (int *)malloc((size + 2) * sizeof(int));

    for (int i = 0; i < size + 2; i++)
    {
        mapping1[i] = 0;
        mapping2[i] = 0;
    }

    // fazer a troca
    for (int i = a; i <= b; i++)
    {
        mapping1[cromossomeY1->CROMOSSOME[i]] = cromossomeX1->CROMOSSOME[i];
        cromossomeX2->CROMOSSOME[i] = cromossomeY1->CROMOSSOME[i];

        mapping2[cromossomeX1->CROMOSSOME[i]] = cromossomeY1->CROMOSSOME[i];
        cromossomeY2->CROMOSSOME[i] = cromossomeX1->CROMOSSOME[i];
    }

    // repair lower Slice
    for (int i = 0; i < a; i++)
    {
        while (CheckInArray(mapping1, size + 2, cromossomeX2->CROMOSSOME[i]))
        {
            cromossomeX2->CROMOSSOME[i] = mapping1[cromossomeX2->CROMOSSOME[i]];
        }
        while (CheckInArray(mapping2, size + 2, cromossomeY2->CROMOSSOME[i]))
        {
            cromossomeY2->CROMOSSOME[i] = mapping2[cromossomeY2->CROMOSSOME[i]];
        }
    }

    // repair upper Slice
    for (int i = b + 1; i < size; i++)
    {
        while (CheckInArray(mapping1, size + 2, cromossomeX2->CROMOSSOME[i]))
        {
            cromossomeX2->CROMOSSOME[i] = mapping1[cromossomeX2->CROMOSSOME[i]];
        }
        while (CheckInArray(mapping2, size + 2, cromossomeY2->CROMOSSOME[i]))
        {
            cromossomeY2->CROMOSSOME[i] = mapping2[cromossomeY2->CROMOSSOME[i]];
        }
    }

    free(mapping1);
    free(mapping2);
}

// limpar cromossomo
void GeneticCromossomeDealocate(void *cromossome)
{
    Cromossome *aux = (Cromossome *)cromossome;
    free(aux->CROMOSSOME);
    free(aux);
}

void GeneticPrint(Genetic *genetic)
{

    printf("\n\n------ GENETIC ------\n\n");

    printf("THREAD_ID: \t\t%p", (void *) genetic->THREAD_ID);
    printf("\nFILE_OUTPUT: \t\t%s", genetic->FILE_OUTPUT);
    printf("\nCOUNT_GENERATIONS: \t%d", genetic->COUNT_GENERATIONS);
    printf("\nCOUNT_EXECUTIONS: \t%d", genetic->METADATA->COUNT_EXECUTIONS);
    printf("\nSIZE_POPULATION: \t%d", genetic->SIZE_POPULATION);
    printf("\nGENERATION_USE: \t%d", genetic->GENERATION_USE);
    printf("\nSIZE_CROMOSSOME: \t%d", genetic->SIZE_CROMOSSOME);
    printf("\nSELECTION_METHOD: \t%s\t\t(RATE: %lf)", genetic->SELECTION_METHOD, genetic->SELECTION_RATE);

    printf("\nCROSSOVER_METHOD:\t%s", genetic->CROSSOVER_METHOD);
    printf("\nMUTATION:\t\t%s \t(", genetic->MUTATION_METHOD);
    printf("SWAP: %lf; ", genetic->MUTATION_RATE_SWAP);
    printf("INSERTION: %lf; ", genetic->MUTATION_RATE_INSERTION);
    printf("SHUFFLE: %lf; ", genetic->MUTATION_RATE_SHUFFLE);
    printf("INVERSION: %lf)", genetic->MUTATION_RATE_INVERSION);

    printf("\nMEM_METADATA: \t\t%p", (void *) genetic->METADATA);
    printf("\nMEM_POPULATION: \t%p", (void *) genetic->POPULATION);

    printf("\nCOUNT_IMMIGRATION: \t%d", genetic->COUNT_IMMIGRATION);

    printf("\nIMMIGRATION:\t\t%p \t MUTEX: %p", (void *) genetic->IMMIGRATION, (void *) &(genetic->MUTEX_IMMIGRATION));
    if (genetic->MUTEX_IMMIGRATION_LEFT != NULL)
        printf("\nIMMIGRATION_LEFT: \t%p \t MUTEX: %p", (void *) genetic->IMMIGRATION_LEFT, (void *) (genetic->MUTEX_IMMIGRATION_LEFT));
    if (genetic->MUTEX_IMMIGRATION_RIGHT != NULL)
        printf("\nIMMIGRATION_RIGHT: \t%p \t MUTEX: %p", (void *) genetic->IMMIGRATION_RIGHT, (void *) (genetic->MUTEX_IMMIGRATION_RIGHT));
    printf("\n----------------------\n");

    printf("\n >>> Populacao atual: ");
    // printf("\n Cromossomo - Fitness: ");
    // for (int i = 0; i < genetic->SIZE_POPULATION; i++)
    for (int i = 0; i < genetic->SIZE_POPULATION; i++)
    {
        CromossomePrint(ListSortedGetPosition(genetic->POPULATION, i), genetic->SIZE_CROMOSSOME);
    }

    int newpopsize = ListSortedSize(genetic->POPULATION_NEW);
    // printf("\n>>> %d\n", newpopsize);

    printf("\n\n >>> Populacao nova");
    for (int i = 0; i < newpopsize; i++)
    {
        CromossomePrint(ListSortedGetPosition(genetic->POPULATION_NEW, i), genetic->SIZE_CROMOSSOME);
    }

    printf("\n\n >>> Populacao imigrante: ");
    int popimmigrant = ListSortedSize(genetic->IMMIGRATION);
    for (int i = 0; i < popimmigrant; i++)
    {
        CromossomePrint(ListSortedGetPosition(genetic->IMMIGRATION, i), genetic->SIZE_CROMOSSOME);
    }
}

void GeneticPrintResume(Genetic *genetic)
{
    for (int i = 0; i < 1; i++)
    {
        CromossomePrint(ListSortedGetPosition(genetic->POPULATION, i), genetic->SIZE_CROMOSSOME);
    }
}

//imprimir cromossomo
void CromossomePrint(Cromossome *cromossome, int size)
{
    printf("\n");
    printf("(%.1lf) ", cromossome->FITNESS);
    for (int i = 0; i < size - 1; i++)
    {
        printf("%d-", cromossome->CROMOSSOME[i]);
    }
    printf("%d", cromossome->CROMOSSOME[size - 1]);
}

//extrair e imprimir rotas
void CromossomeRouteGenerator(Genetic *genetic, Cromossome *cromossome)
{

    int dimensao = genetic->METADATA->DIMENSION;
    int rota = 1;

    printf("Rotas [");
    printf("%d:(1, ", rota++);

    for (int i = 0; i < genetic->SIZE_CROMOSSOME; ++i)
    {
        if (cromossome->CROMOSSOME[i] > dimensao)
        {
            printf(", 1), %d:(1, ", rota++);
        }
        else
        {
            printf("%d", cromossome->CROMOSSOME[i]);
            if (i < genetic->SIZE_CROMOSSOME - 1)
            {
                if (cromossome->CROMOSSOME[i + 1] <= dimensao)
                {
                    printf(", ");
                }
            }
        }
    }
    printf(", 1)");
    printf("]");
}

// seleciona pares de individuos
void GeneticSelect(Genetic *genetic, Cromossome **varx1, Cromossome **vary1)
{

    Cromossome *x1 = NULL;
    Cromossome *x2 = NULL;
    Cromossome *y1 = NULL;
    Cromossome *y2 = NULL;

    int posx1 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);
    int posx2 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);
    int posy1 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);
    int posy2 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);

    // sortear ate obter 2 distintos
    while (posx2 == posx1)
    {
        posx2 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);
    }
    while (posy1 == posx2 || posy1 == posx1)
    {
        posy1 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);
    }
    while (posy2 == posy1 || posy2 == posx2 || posy2 == posx1)
    {
        posy2 = GerarInteiroAleatorio(0, genetic->SIZE_POPULATION - 1);
    }

    x1 = ListSortedGetPosition(genetic->POPULATION, posx1);
    x2 = ListSortedGetPosition(genetic->POPULATION, posx2);
    y1 = ListSortedGetPosition(genetic->POPULATION, posy1);
    y2 = ListSortedGetPosition(genetic->POPULATION, posy2);

    if (x1->FITNESS < x2->FITNESS)
    {
        Cromossome *tmp = x2;
        x2 = x1;
        x1 = tmp;
    }

    if (y1->FITNESS < y2->FITNESS)
    {
        Cromossome *tmp = y2;
        y2 = y1;
        y1 = tmp;
    }

    double random1 = GerarFloatAleatorio();
    double random2 = GerarFloatAleatorio();

    if (random1 <= genetic->SELECTION_RATE)
    {
        *varx1 = x1;
    }
    else
    {
        *varx1 = x2;
    }
    if (random2 <= genetic->SELECTION_RATE)
    {
        *vary1 = y1;
    }
    else
    {
        *vary1 = y2;
    }
}

void GeneticCrossover(Genetic *genetic)
{

    Cromossome *x1 = NULL;
    Cromossome *y1 = NULL;
    Cromossome *x2 = NULL;
    Cromossome *y2 = NULL;

    for (int i = 0; i < genetic->SIZE_POPULATION - 1; i = i + 2)
    {
        GeneticSelect(genetic, &x1, &y1);
        x2 = (Cromossome *)malloc(sizeof(Cromossome));
        x2->CROMOSSOME = (int *)malloc(genetic->SIZE_CROMOSSOME * sizeof(int));
        x2->FITNESS = 0.0;
        y2 = (Cromossome *)malloc(sizeof(Cromossome));
        y2->CROMOSSOME = (int *)malloc(genetic->SIZE_CROMOSSOME * sizeof(int));
        y2->FITNESS = 0.0;
        GeneticCrossoverPMX(x1, y1, x2, y2, genetic->SIZE_CROMOSSOME);
        ListSortedInsert(&(genetic->POPULATION_NEW), x2, GeneticCromossomeDealocate, x2->FITNESS);
        ListSortedInsert(&(genetic->POPULATION_NEW), y2, GeneticCromossomeDealocate, y2->FITNESS);
    }
}

//mutacao por troca
void GeneticMutation_Swap(Genetic *genetic, Cromossome *cromossome)
{
    int a = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    int b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    while (a == b)
    {
        b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    }
    int tmp = cromossome->CROMOSSOME[a];
    cromossome->CROMOSSOME[a] = cromossome->CROMOSSOME[b];
    cromossome->CROMOSSOME[b] = tmp;
}

void GeneticMutation_Insertion(Genetic *genetic, Cromossome *cromossome)
{
    int a = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    int b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    while (a == b)
    {
        b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    }
    if (b < a)
    {
        int aux = a;
        a = b;
        b = aux;
    }

    if (GerarFloatAleatorio() > 0.5)
    {
        for (int i = a; i + 1 < b; i++)
        {
            int tmp = cromossome->CROMOSSOME[i];
            cromossome->CROMOSSOME[i] = cromossome->CROMOSSOME[i + 1];
            cromossome->CROMOSSOME[i + 1] = tmp;
        }
    }
    else
    {
        for (int i = b; i > a + 1; i--)
        {
            int tmp = cromossome->CROMOSSOME[i];
            cromossome->CROMOSSOME[i] = cromossome->CROMOSSOME[i - 1];
            cromossome->CROMOSSOME[i - 1] = tmp;
        }
    }
}

//mutacao por embaralhamento de substring
void GeneticMutation_Shuffle(Genetic *genetic, Cromossome *cromossome)
{
    int a = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    int b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    while (a == b)
    {
        b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    }
    if (b < a)
    {
        int aux = a;
        a = b;
        b = aux;
    }
    for (int i = a; i <= b; i++)
    {
        int rnd = GerarInteiroAleatorio(a, b);
        while (rnd == i)
        {
            rnd = GerarInteiroAleatorio(a, b);
        }
        int tmp = cromossome->CROMOSSOME[i];
        cromossome->CROMOSSOME[i] = cromossome->CROMOSSOME[rnd];
        cromossome->CROMOSSOME[rnd] = tmp;
    }
}

//mutacao por inversao
void GeneticMutation_Inversion(Genetic *genetic, Cromossome *cromossome)
{
    int a = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    int b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    while (a == b)
    {
        b = GerarInteiroAleatorio(0, genetic->SIZE_CROMOSSOME - 1);
    }
    if (b < a)
    {
        int aux = a;
        a = b;
        b = aux;
    }
    while (a < b)
    {
        int tmp = cromossome->CROMOSSOME[a];
        cromossome->CROMOSSOME[a] = cromossome->CROMOSSOME[b];
        cromossome->CROMOSSOME[b] = tmp;
        a++;
        b--;
    }
}

//realizar metodos de mutacao
void GeneticMutationDefault(Genetic *genetic)
{
    int size_new = ListSortedSize(genetic->POPULATION_NEW);
    for (int i = 0; i < size_new; i++)
    {
        Cromossome *cromossome = ListSortedGetPosition(genetic->POPULATION_NEW, i);
        double var = GerarFloatAleatorio();
        if (var < genetic->MUTATION_RATE_SWAP)
            GeneticMutation_Swap(genetic, cromossome);
        var = GerarFloatAleatorio();
        if (var < genetic->MUTATION_RATE_INSERTION)
            GeneticMutation_Insertion(genetic, cromossome);
        var = GerarFloatAleatorio();
        if (var < genetic->MUTATION_RATE_SHUFFLE)
            GeneticMutation_Shuffle(genetic, cromossome);
        var = GerarFloatAleatorio();
        if (var < genetic->MUTATION_RATE_INVERSION)
            GeneticMutation_Inversion(genetic, cromossome);
        CromossomeFitness(genetic, cromossome);
        ListSortedUpdateFitness(genetic->POPULATION_NEW, i, cromossome->FITNESS);
    }
}

//unificar populacao dos progenitores com os filhos
void GeneticConsolidate(Genetic *genetic)
{
    int count = 0;
    int limit = ListSortedSize(genetic->POPULATION_NEW);
    if (limit > genetic->GENERATION_USE)
    {
        limit = genetic->GENERATION_USE;
    }

    while (count < limit)
    {
        if (genetic->POPULATION_NEW == NULL)
        {
            break;
        }
        Cromossome *cromossome_pop = (Cromossome *)ListSortedGetPosition(genetic->POPULATION, genetic->SIZE_POPULATION - 1);
        Cromossome *cromossome_new = (Cromossome *)ListSortedPopPosition(&(genetic->POPULATION_NEW), 0);
        // caso o melhor resultado registrado seja melhor, skipar o restante
        if (cromossome_pop->FITNESS < cromossome_new->FITNESS)
        {
            GeneticCromossomeDealocate(cromossome_new);
            break;
        }

        if (GeneticCheckCromossomeIsValid(genetic, cromossome_new) && !CromossomeCheckIfExists(genetic, cromossome_new))
        {
            // remover pior posicao
            ListSortedRemovePosition(genetic->POPULATION, genetic->SIZE_POPULATION - 1);

            // inserir novo cromossomo
            ListSortedInsert(&(genetic->POPULATION), cromossome_new, GeneticCromossomeDealocate, cromossome_new->FITNESS);

            count++;
        }
        else
        {
            GeneticCromossomeDealocate(cromossome_new);
        }
    }
    ListSortedRemove(genetic->POPULATION_NEW);
    genetic->POPULATION_NEW = NULL;
}

//funcao para impressao de relatorio da ilha
void GeneticFileReport(Genetic *genetic)
{
    for (int i = 0; i < genetic->SIZE_POPULATION; i++)
    // for (int i = 0; i < 1; i++)
    {
        Cromossome *cromossome = ListSortedGetPosition(genetic->POPULATION, i);
        fprintf(genetic->ARQ_OUTPUT, "Fitness = %.1lf \t\t", cromossome->FITNESS);
        for (int i = 0; i < genetic->SIZE_CROMOSSOME - 1; i++)
        {
            fprintf(genetic->ARQ_OUTPUT, "%d-", cromossome->CROMOSSOME[i]);
        }
        fprintf(genetic->ARQ_OUTPUT, "%d\n", cromossome->CROMOSSOME[genetic->SIZE_CROMOSSOME - 1]);
    }
}

//fluxo principal de execucao do algoritmo genetico
void *GeneticExecute(void *reference)
{

    //printf("\nInicializando thread %ld", seed);

    Genetic *genetic = (Genetic *)reference;
    long int seed = (long int) genetic;
    srand(time(NULL) + seed + GerarInteiroAleatorio(1, 10000));
    GeneticInitializePopulation(genetic);
    ListSortedMergeSort(&(genetic->POPULATION));

    // genetic->ARQ_OUTPUT = fopen(genetic->FILE_OUTPUT, "a");
    // genetic->ARQ_OUTPUT = fopen(genetic->FILE_OUTPUT, "w");
    // GeneticFileReport(genetic);
    // GeneticPrint(genetic);
    // GeneticPrintResume(genetic);

    for (int j = 0; j < genetic->METADATA->COUNT_EXECUTIONS; j++)
    {
        // executar por N geracoes
        for (int i = 0; i < genetic->COUNT_GENERATIONS; i++)
        {

            GeneticCrossover(genetic);
            GeneticMutationDefault(genetic);
            ListSortedMergeSort(&(genetic->POPULATION_NEW));
            GeneticConsolidate(genetic);
            ListSortedMergeSort(&(genetic->POPULATION));
            // GeneticPrint(genetic);
        }

        // Enviar para referencia a esquerda - migracao
        if (genetic->MUTEX_IMMIGRATION_LEFT != NULL)
        {
            pthread_mutex_lock(genetic->MUTEX_IMMIGRATION_LEFT); // solicitar mutex
            for (int i = 0; i < genetic->COUNT_IMMIGRATION; i++)
            {
                Cromossome *cromossome = ListSortedGetPosition(genetic->POPULATION, i);
                Cromossome *copy = CromossomeCopy(genetic, cromossome);
                // CromossomePrint(copy, genetic->SIZE_CROMOSSOME);
                ListSortedInsert(&(*genetic->IMMIGRATION_LEFT), (void *)copy, GeneticCromossomeDealocate, copy->FITNESS);
            }
            pthread_mutex_unlock(genetic->MUTEX_IMMIGRATION_LEFT); //  liberar mutex
        }

        // Enviar para referencia a direita - migracao

        if (genetic->MUTEX_IMMIGRATION_RIGHT != NULL)
        {
            pthread_mutex_lock(genetic->MUTEX_IMMIGRATION_RIGHT); // solicitar mutex
            for (int i = 0; i < genetic->COUNT_IMMIGRATION; i++)
            {
                Cromossome *cromossome = ListSortedGetPosition(genetic->POPULATION, i);
                Cromossome *copy = CromossomeCopy(genetic, cromossome);
                // CromossomePrint(copy, genetic->SIZE_CROMOSSOME);
                ListSortedInsert(&(*genetic->IMMIGRATION_RIGHT), (void *)copy, GeneticCromossomeDealocate, copy->FITNESS);
            }
            pthread_mutex_unlock(genetic->MUTEX_IMMIGRATION_RIGHT); //  liberar mutex
        }

        // Receber novos individuos e inseri-los na populacao (se necessario) - migracao
        pthread_mutex_lock(&genetic->MUTEX_IMMIGRATION); // solicitar mutex
        ListSortedMergeSort(&(genetic->IMMIGRATION));

        while (genetic->IMMIGRATION != NULL)
        {
            Cromossome *cromossome_pop = (Cromossome *)ListSortedGetPosition(genetic->POPULATION, genetic->SIZE_POPULATION - 1);
            Cromossome *cromossome_new = ListSortedPopPosition(&(genetic->IMMIGRATION), 0);

            if (cromossome_new->FITNESS >= cromossome_pop->FITNESS || CromossomeCheckIfExists(genetic, cromossome_new))
            {
                GeneticCromossomeDealocate(cromossome_new);
                break;
            }
            else
            {
                // remover pior posicao
                ListSortedRemovePosition(genetic->POPULATION, genetic->SIZE_POPULATION - 1);
                // inserir novo cromossomo
                ListSortedInsert(&(genetic->POPULATION), cromossome_new, GeneticCromossomeDealocate, cromossome_new->FITNESS);
            }
        }

        ListSortedRemove(genetic->IMMIGRATION); //limpar o que restou na imigracao
        genetic->IMMIGRATION = NULL;
        ListSortedMergeSort(&(genetic->POPULATION));        // re-ordenar populacao, caso individuos migrados
        pthread_mutex_unlock(&genetic->MUTEX_IMMIGRATION);  // liberar mutex
        //  GeneticPrint(genetic);
    }

    // GeneticFileReport(genetic);
    // fclose(genetic->ARQ_OUTPUT);
    //printf("\nFinalizando thread %ld", seed);

    return NULL;
}
