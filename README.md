# Algoritmo Genético Paralelo com Modelo de Ilhas para Solucionar o Problema de Roteamento de Veículos Capacitado (CVRP) 

Este repositório contém o algoritmo desenvolvido para o Projeto de Diplomação (TCC) intitulado **Solucionando o Problema de Roteamento de Veículos Capacitado com uso de Algoritmos Genéticos e Paralelismo**, desenvolvido como requisito para conclusão do curso Bacharelado em Sistemas de Informação, no IFES campus Serra.

Autor: Rodrigo Raider de Oliveira
Orientador: Prof. Dr. Leandro Colombi Resendo

## Visão Geral

Este repositório armazena um Algoritmo Genético implementado na linguagem C, adaptado para ambientes baseado em Linux.

## Pré-requisitos de Compilação

Para execução do algoritmo, segue sugestão das ferramentas com suas respectivas versões:
  - Ambiente baseado em Linux (Ubuntu 23.10)
  - GCC versão 13.2.0

## Compilando os Códigos

1ª Etapa: **acessar diretório** e gerar os binários das dependências:

```
gcc -Wall -pedantic -c utils.c -std=c99
gcc -Wall -pedantic -c list.c -std=c99
gcc -Wall -pedantic -c node.c -std=c99
gcc -Wall -pedantic -c metadata.c -std=c99
gcc -Wall -pedantic -c genetic.c -std=c99
gcc -Wall -pedantic -c island.c -std=c99
gcc -Wall -pedantic -c main.c -std=c99
```
ou 
```
gcc -Wall -pedantic -c utils.c list.c node.c metadata.c genetic.c island.c main.c -std=c99
```

2ª Etapa: Realizar o link no executável final:
```
gcc -Wall -pedantic -o main main.o metadata.o utils.o node.o list.o genetic.o island.o -lm -LPthread -pthread -lm -std=c99
```

## Sobre a Execução

1º - Obter um conjunto de dados de um problema CVRP. Na pasta de instâncias estão localizadas as utilizadas no trabalho. Mais podem ser obtidas em: CVRP - Puc Rio [Problemas CVRP](http://vrp.galgos.inf.puc-rio.br/index.php/en/plotted-instances?data=A-n32-k5)

2º - Configurar os casos de testes em arquivos que serão usados como entrada para o algoritmo, segue a estrutura básica do arquivo, mais exemplos podem ser consultados na pasta configs.

| Parâmetro                | Valor                     | Comentário                                                    |
|--------------------------|---------------------------|---------------------------------------------------------------|
| RING_BIDIRECTIONAL:      | 0                         | 0 - unidirecional 1 - bidirecional                            |
| FILE_INPUT:              | ../instances/A-n34-k5.vrp | Arquivo de entrada com o problema com o caminho               |
| FILE_OUTPUT:             | ../output/A-n34-k5.vrp    | Arquivo de saída *necessário retirar comentário do código     |
| COUNT_REPETITIONS:       | 10                        | Quantas repetições do teste serão realizadas                  |
| COUNT_EXECUTIONS:        | 15                        | Em cada teste, quantas execuções de cada ilha                 |
| COUNT_ISLAND:            | 1                         | Quantidade de Ilhas                                           |
| COUNT_GENERATIONS:       | 300                       | Quantidade de Gerações                                        |
| SIZE_POPULATION:         | 100                       | Tamanho da População                                          |
| GENERATION_USE:          | 10                        | Quantidade Limite de Indivíduos Reaproveitados entre Gerações |
| SELECTION_METHOD:        | DEFAULT                   | Método de Seleção                                             |
| SELECTION_RATE:          | 0.8                       | Taxa de Seleção Default                                       |
| CROSSOVER_METHOD:        | DEFAULT                   | Método de Cruzamento                                          |
| MUTATION_METHOD:         | DEFAULT                   | Método de Mutação                                             |
| MUTATION_RATE_SWAP:      | 0.07                      | Taxa da Mutação do Tipo Troca                                 |
| MUTATION_RATE_INSERTION: | 0.07                      | Taxa da Mutação do Tipo Inserção                              |
| MUTATION_RATE_SHUFFLE:   | 0.07                      | Taxa da Mutação do Tipo Embaralhar                            |
| MUTATION_RATE_INVERSION: | 0.07                      | Taxa da Mutação do Tipo Inverter                              |
| COUNT_IMMIGRATION:       | 2                         | Quantos Imigrantes são Enviados                               |
| FILE_OUTPUT:             | ../output/thread-1.txt    | Arquivo com Relatório da Ilha (para debug)                    |

3º - Usar o arquivo obtido na 2ª etapa como entrada para o programa principal, exemplo:

```
./main ../config/config-A-n34-k5-1.txt
```

## Saída e Resultados

Os resultados foram configurados para serem gerados de forma simplificada, caso trechos mais detalhados sejam necessário, funções que foram inseridas no algoritmo precisam ser descomentadas. Um exemplo de saída atual é:

```
Problema	Ilhas	Fitness	Tempo(ms)	Melhor Individuo

 E-n22-k4	2	375	1000		Rotas [1:(1, 17, 20, 22, 15, 1), 2:(1, 13, 16, 19, 21, 18, 1), 3:(1, 7, 2, 3, 6, 8, 10, 1), 4:(1, 14, 12, 5, 4, 9, 11, 1)]
 E-n22-k4	2	382	983		Rotas [1:(1, 11, 8, 6, 10, 13, 1), 2:(1, 12, 5, 4, 2, 3, 7, 9, 1), 3:(1, 15, 18, 22, 21, 19, 16, 1), 4:(1, 14, 20, 17, 1)]
 E-n22-k4	2	382	970		Rotas [1:(1, 16, 19, 21, 17, 1), 2:(1, 9, 7, 3, 2, 4, 5, 12, 1), 3:(1, 14, 20, 22, 18, 15, 1), 4:(1, 11, 8, 6, 10, 13, 1)]
 E-n22-k4	2	375	1004		Rotas [1:(1, 15, 22, 20, 17, 1), 2:(1, 7, 2, 3, 6, 8, 10, 1), 3:(1, 11, 9, 4, 5, 12, 14, 1), 4:(1, 18, 21, 19, 16, 13, 1)]
 E-n22-k4	2	382	969		Rotas [1:(1, 13, 10, 8, 6, 11, 1), 2:(1, 12, 5, 4, 2, 3, 7, 9, 1), 3:(1, 14, 20, 22, 18, 15, 1), 4:(1, 17, 21, 19, 16, 1)]
 E-n22-k4	2	375	963		Rotas [1:(1, 18, 21, 19, 16, 13, 1), 2:(1, 14, 12, 5, 4, 9, 11, 1), 3:(1, 7, 2, 3, 6, 8, 10, 1), 4:(1, 17, 20, 22, 15, 1)]
 E-n22-k4	2	375	970		Rotas [1:(1, 18, 21, 19, 16, 13, 1), 2:(1, 14, 12, 5, 4, 9, 11, 1), 3:(1, 7, 2, 3, 6, 8, 10, 1), 4:(1, 17, 20, 22, 15, 1)]
 E-n22-k4	2	375	939		Rotas [1:(1, 11, 9, 4, 5, 12, 14, 1), 2:(1, 18, 21, 19, 16, 13, 1), 3:(1, 10, 8, 6, 3, 2, 7, 1), 4:(1, 17, 20, 22, 15, 1)]
 E-n22-k4	2	375	950		Rotas [1:(1, 11, 9, 4, 5, 12, 14, 1), 2:(1, 13, 16, 19, 21, 18, 1), 3:(1, 7, 2, 3, 6, 8, 10, 1), 4:(1, 17, 20, 22, 15, 1)]
 E-n22-k4	2	375	993		Rotas [1:(1, 15, 22, 20, 17, 1), 2:(1, 13, 16, 19, 21, 18, 1), 3:(1, 7, 2, 3, 6, 8, 10, 1), 4:(1, 14, 12, 5, 4, 9, 11, 1)]
```


