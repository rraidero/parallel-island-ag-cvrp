#ifndef METADATA_H
#define METADATA_H

#include <stdbool.h>
#include "node.h"

// estrutura com as variaveis de execucao do sistema
typedef struct metadata
{

    bool RING_BIDIRECTIONAL; // determina as conexoes das ilhas
    char *FILE_INPUT;        // nome do arquivo em que os dados serao lidos
    char *FILE_OUTPUT;       // nome do arquivo em que os resultados serao impressos
    int COUNT_REPETITIONS;   // quantidade de execucoes que o algoritmo ira repetir
    int COUNT_EXECUTIONS;    // quantidade de execucoes que cada ilha ira fazer antes de trocar informacao
    int COUNT_THREADS;       // quantidade de threads (ilhas) a serem executadas

    char *NAME;               // nome da instancia
    char *COMMENT;            // comentarios
    char *TYPE;               // tipo de problema
    int DIMENSION;            // dimensao do problema
    char *EDGE_WEIGHT_TYPE;   // tipo de distancia entre os nós
    float CAPACITY;           // capacidade do veiculo
    int COUNT_VEHICLES;       // quantidade de veiculos
    float OPTIMAL;            // valor otimo
    Node *NODE_COORD_SECTION; // coordenadas dos nós
    double *NODE_COST_MATRIX; // custo de deslocamento

} Metadata;

// Criar novo metadata
Metadata *MetadataNew(char *fileConfig);

// Apagar metadata
void MetadataRemove(Metadata *metadata);

// Imprimir dados de metadata
void MetadataPrint(Metadata *metadata);

// Checar se metadados esta completo
bool MetadataCheckIsValid(Metadata *metadata);

// Setar variaveis
void MetadataSet(Metadata *metadata, char *key, char *value);

// gerar custo de deslocamento da matriz
void MetadataCostGenerate(Metadata *metadata);

// Setar nome do arquivo de saida dos resultados
// void MetadataSetFileOutput(Metadata* metadata, char* count);

// Funcao para extrair dos comentarios a quantidade de veiculos e o valor otimo
void ExtractNroTrucksAndOptimalValue(Metadata *metadata);

// Funcao para obter o custo de X para Y
double MetadataNodeCost(Metadata *metadata, int x, int y);

// Funcao para obter a demanda do no
int MetadataNodeDemand(Metadata *metadata, int index);

#endif