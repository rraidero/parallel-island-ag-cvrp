#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
//#include <Windows.h>
#include "utils.h"

// Função para remover espaços em branco do início e do fim da string
void Strip(char *str)
{
    if (str == NULL)
        return;

    // Remover espaços em branco do início da string
    while (isspace((unsigned char)*str))
        str++;

    // Se a string estiver vazia após remover os espaços do início, retornar
    if (*str == '\0')
        return;

    // Remover espaços em branco do fim da string
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Adicionar o caractere nulo para finalizar a nova string
    *(end + 1) = '\0';
}

// Função para gerar uma cópia de uma string alocando-a dinâmicamente
char *DuplicateSTR(char *str)
{
    size_t len = strlen(str) + 1; // +1 para o caractere nulo
    char *new_str = (char *)malloc(len);

    if (new_str != NULL)
    {
        strcpy(new_str, str);
    }

    return new_str;
}

// Função para calcular a distância euclidiana
double DistanciaEuclidiana(int x1, int y1, int x2, int y2)
{
    double distancia;
    distancia = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return distancia;
}

float GerarFloatAleatorio()
{
    // Obter um número inteiro aleatório entre 0 e RAND_MAX
    int random_int = rand();

    // Dividir o número inteiro pelo valor máximo possível (RAND_MAX + 1)
    float random_float = (float)random_int / (RAND_MAX + 1.0);

    return random_float;
}

int GerarInteiroAleatorio(int min, int max)
{
    return min + rand() % (max - min + 1);
}

void ExtractStringKeyValue(char *string_lida, char **key, char **value)
{
    // Usando strchr() para encontrar a primeira ocorrência de ':'
    Strip(string_lida);
    char *posicao = strchr(string_lida, ':');
    *key = string_lida;
    *posicao = '\0'; // Substitui ':' por '\0' para terminar a chave
    *value = posicao + 1;
}

// Verificar se um valor está no vetor
bool CheckInArray(int *array, int size, int value)
{

    if (array[value] != 0)
    {
        return true;
    }

    return false;
}

// Empurrar posicao x1 de um vetor de inteiros até x0
void VetIntMoveAndInsert(int vetor[], int tamanho, int x1, int x0)
{
    int valor = vetor[x0];
    if (x0 < x1)
    {
        for (int i = x0; i < x1; i++)
        {
            vetor[i] = vetor[i + 1];
        }
    }
    else
    {
        for (int i = x0; i > x1; i--)
        {
            vetor[i] = vetor[i - 1];
        }
    }
    if (valor == 0)
    {
        printf("\nErro de valor zerado %d - %d", x1, x0);
    }
    vetor[x1] = valor;
}
