#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "utils.h"
#include "metadata.h"
#include "node.h"

// Criar novo metadata a partir da entrada de um arquivo de configuracoes
Metadata *MetadataNew(char *fileConfig)
{

    Metadata *metadata = (Metadata *)malloc(sizeof(Metadata));
    FILE *arq;

    arq = fopen(fileConfig, "r");
    char string_lida[150]; // buffer linha arquivo

    Node *node_current = NULL;
    char *key;
    char *value;

    if (arq == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
        return NULL;
    }

    fgets(string_lida, sizeof(string_lida), arq);
    ExtractStringKeyValue(string_lida, &key, &value);
    MetadataSet(metadata, "RING_BIDIRECTIONAL", value);

    fgets(string_lida, sizeof(string_lida), arq);
    ExtractStringKeyValue(string_lida, &key, &value);
    MetadataSet(metadata, "FILE_INPUT", value);

    fgets(string_lida, sizeof(string_lida), arq);
    ExtractStringKeyValue(string_lida, &key, &value);
    MetadataSet(metadata, "FILE_OUTPUT", value);

    fgets(string_lida, sizeof(string_lida), arq);
    ExtractStringKeyValue(string_lida, &key, &value);
    MetadataSet(metadata, "COUNT_REPETITIONS", value);

    fgets(string_lida, sizeof(string_lida), arq);
    ExtractStringKeyValue(string_lida, &key, &value);
    MetadataSet(metadata, "COUNT_EXECUTIONS", value);

    fgets(string_lida, sizeof(string_lida), arq);
    ExtractStringKeyValue(string_lida, &key, &value);
    MetadataSet(metadata, "COUNT_THREADS", value);

    fclose(arq);

    arq = fopen(metadata->FILE_INPUT, "rt");
    if (arq == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
        return NULL;
    }
    else
    {
        while (!feof(arq) && !strcmp(key, "CAPACITY") == 0)
        {
            fgets(string_lida, sizeof(string_lida), arq);
            char *posicao = strchr(string_lida, ':'); // Usando strchr() para encontrar a primeira ocorrência de ':'
            key = string_lida;
            *posicao = '\0'; // Substitui ':' por '\0' para terminar a chave
            value = posicao + 1;
            Strip(key);
            Strip(value);

            if (key != NULL && value != NULL)
                MetadataSet(metadata, key, value);

            else
                return NULL;
        }

        metadata->NODE_COORD_SECTION = (Node *)malloc(metadata->DIMENSION * sizeof(Node));

        // Ler base NODE_COORD_SECTION
        fgets(string_lida, sizeof(string_lida), arq);

        for (int i = 0; i < metadata->DIMENSION; i++)
        {
            // node_current = (Node *)(&metadata->NODE_COORD_SECTION + (i * sizeof(Node)));
            node_current = &metadata->NODE_COORD_SECTION[i];
            fgets(string_lida, sizeof(string_lida), arq);
            Strip(string_lida);
            NodeSetValues(node_current, string_lida);
            // NodePrint(node_current);
        }

        // Ler base DEMAND_SECTION
        fgets(string_lida, sizeof(string_lida), arq);

        for (int i = 0; i < metadata->DIMENSION; i++)
        {
            int id, demanda = 0;
            fgets(string_lida, sizeof(string_lida), arq);
            Strip(string_lida);
            sscanf(string_lida, " %d %d", &id, &demanda);
            // node_current = (Node *)(&metadata->NODE_COORD_SECTION + (i * sizeof(Node)));
            node_current = &metadata->NODE_COORD_SECTION[i];
            node_current->demand = demanda;
            // NodePrint(node_current);
        }
    }
    fclose(arq);

    MetadataCostGenerate(metadata);

    return metadata;
}

void MetadataRemove(Metadata *metadata)
{
    free(metadata->FILE_INPUT);
    free(metadata->FILE_OUTPUT);
    free(metadata->NAME);
    free(metadata->COMMENT);
    free(metadata->TYPE);
    free(metadata->EDGE_WEIGHT_TYPE);
    free(metadata->NODE_COST_MATRIX);
    free(metadata->NODE_COORD_SECTION);
    free(metadata);
}

void MetadataPrint(Metadata *metadata)
{

    printf("\n\n------ Metadata ------\n\n");

    printf(" - RING_BIDIRECTIONAL\t %s\n", metadata->RING_BIDIRECTIONAL ? "true" : "false");
    printf(" - FILE_INPUT\t\t %s\n", metadata->FILE_INPUT);
    printf(" - FILE_OUTPUT\t\t %s\n", metadata->FILE_OUTPUT);
    printf(" - COUNT_REPETITIONS: \t %d\n", metadata->COUNT_REPETITIONS);
    printf(" - COUNT_EXECUTIONS: \t %d\n", metadata->COUNT_EXECUTIONS);
    printf(" - COUNT_THREADS\t %d\n", metadata->COUNT_THREADS);
    printf(" - NAME\t\t\t%s\n", metadata->NAME);
    printf(" - COMMENT\t\t%s\n", metadata->COMMENT);
    printf(" - TYPE\t\t\t%s\n", metadata->TYPE);
    printf(" - CAPACITY\t\t %f\n", metadata->CAPACITY);
    printf(" - DIMENSION\t\t %d\n", metadata->DIMENSION);
    printf(" - EDGE_WEIGHT_TYPE\t%s\n", metadata->EDGE_WEIGHT_TYPE);
    printf(" - COUNT_VEHICLES\t %d\n", metadata->COUNT_VEHICLES);
    printf(" - OPTIMAL\t\t %f\n", metadata->OPTIMAL);
    printf("\n----------------------\n");
    printf(" - Matriz de Custo");

    for (int i = 0; i < metadata->DIMENSION; i++)
    {
        printf("\n");
        for (int j = 0; j < metadata->DIMENSION; j++)
        {
            printf("%.0lf,", metadata->NODE_COST_MATRIX[(i * metadata->DIMENSION) + j]);
        }
    }

    printf("\n----------------------\n");
    for (int j = 0; j < metadata->DIMENSION; j++)
    {
        NodePrint(&(metadata->NODE_COORD_SECTION[j]));
    }
    printf("\n----------------------\n");
}

// extrair valores (chave: valor) do arquivo de configuracoes
void MetadataSet(Metadata *metadata, char *key, char *value)
{
    if (strcmp(key, "NAME") == 0)
        metadata->NAME = DuplicateSTR(value);
    else if (strcmp(key, "COMMENT") == 0)
    {
        metadata->COMMENT = DuplicateSTR(value);
        ExtractNroTrucksAndOptimalValue(metadata);
    }
    else if (strcmp(key, "TYPE") == 0)
        metadata->TYPE = DuplicateSTR(value);
    else if (strcmp(key, "CAPACITY") == 0)
        metadata->CAPACITY = atof(value);
    else if (strcmp(key, "DIMENSION") == 0)
        metadata->DIMENSION = atoi(value);
    else if (strcmp(key, "EDGE_WEIGHT_TYPE") == 0)
        metadata->EDGE_WEIGHT_TYPE = DuplicateSTR(value);
    else if (strcmp(key, "COUNT_REPETITIONS") == 0)
        metadata->COUNT_REPETITIONS = atoi(value);
    else if (strcmp(key, "COUNT_EXECUTIONS") == 0)
        metadata->COUNT_EXECUTIONS = atoi(value);
    else if (strcmp(key, "COUNT_THREADS") == 0)
        metadata->COUNT_THREADS = atoi(value);
    else if (strcmp(key, "FILE_INPUT") == 0)
    {
        value++;
        metadata->FILE_INPUT = DuplicateSTR(value);
    }
    else if (strcmp(key, "FILE_OUTPUT") == 0)
    {
        value++;
        metadata->FILE_OUTPUT = DuplicateSTR(value);
    }
    else if (strcmp(key, "RING_BIDIRECTIONAL") == 0)
        metadata->RING_BIDIRECTIONAL = atoi(value);
    else
        printf("\nNao foi possivel inicializar a chave %s", key);
}

// adicao de funcoes para checar se o metada eh valido
bool MetadataCheckIsValid(Metadata *metadata)
{
    return true;
}

// ler do arquivo do problema a quantidade de caminhoes e o valor otimo, se possivel
void ExtractNroTrucksAndOptimalValue(Metadata *metadata)
{
    const char *inicioTotalTrucks = "trucks: ";
    const char *inicioValorOtimo = "value: ";
    char *posicaoTotalTrucks = strstr(metadata->COMMENT, inicioTotalTrucks);
    char *posicaoValorOtimo = strstr(metadata->COMMENT, inicioValorOtimo);

    if (posicaoTotalTrucks != NULL && posicaoValorOtimo != NULL)
    {
        sscanf(posicaoTotalTrucks + strlen(inicioTotalTrucks), "%d", &(metadata->COUNT_VEHICLES));
        sscanf(posicaoValorOtimo + strlen(inicioValorOtimo), "%f", &(metadata->OPTIMAL));
    }
    else
    {
        metadata->COUNT_VEHICLES = -1; // Indica que não foi possível extrair o valor
        metadata->OPTIMAL = -1;        // Indica que não foi possível extrair o valor
    }
}

// gerar matrix de custos
void MetadataCostGenerate(Metadata *metadata)
{
    metadata->NODE_COST_MATRIX = (double *)malloc((metadata->DIMENSION * metadata->DIMENSION) * sizeof(double));

    Node *a = NULL;
    Node *b = NULL;
    for (int i = 0; i < metadata->DIMENSION; i++)
    {
        for (int j = 0; j < metadata->DIMENSION; j++)
        {
            a = &metadata->NODE_COORD_SECTION[i];
            b = &metadata->NODE_COORD_SECTION[j];
            metadata->NODE_COST_MATRIX[(i * metadata->DIMENSION) + j] = round(DistanciaEuclidiana(a->x, a->y, b->x, b->y));
        }
    }
}

// funcao para obter o custo do vertice X para Y
double MetadataNodeCost(Metadata *metadata, int x, int y)
{
    if (x > metadata->DIMENSION)
        x = 1;
    if (y > metadata->DIMENSION)
        y = 1;
    int pos = ((x - 1) * metadata->DIMENSION) + (y - 1);
    return (metadata->NODE_COST_MATRIX[pos]);
}

// Funcao para obter a demanda do no
int MetadataNodeDemand(Metadata *metadata, int index)
{
    return (&metadata->NODE_COORD_SECTION[index - 1])->demand;
}
