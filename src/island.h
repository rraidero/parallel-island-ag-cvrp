#ifndef ISLAND_H
#define ISLAND_H

#include "genetic.h"
#include "metadata.h"
#include "list.h"

// definicao da estrutura ilha
typedef struct island
{
    Metadata *METADATA; // Metadados do modelo de ilhas
    List *GENETIC;      // Lista com os algoritmos geneticos

} Island;

// Criar nova instancia de ilha
Island *IslandNew(char *arq_config_genetic, Metadata *metadata);

// Apagar instancia de ilhas
void IslandRemove(Island *island);

// Iniciar execucao das instancias das ilhas
void IslandExecute(Island *island);

#endif