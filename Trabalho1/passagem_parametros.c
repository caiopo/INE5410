#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//cria uma estrutura contendo 2 elementos do tipo inteiro
//a estrutura representará um novo tipo de dados, chamado "parametros_thread_t"
//logo, será possível criar variáveis do tipo "parametros_thread_t"
typedef struct {
    int parametro1;
    int parametro2;
} parametros_thread_t;

void* thread(void *param) {
    //transforma o de (void *) para (parametros_thread_t *)
    //*p é um ponteiro apontando para a posição de memória onde está armazenado os valores de "param"
    parametros_thread_t *p = (parametros_thread_t *) param;

    //recupera os valores dos parâmetros
    //como p é um ponteiro para uma estrutura do tipo "parametros_thread_t", o acesso aos elementos é
    //feito utilizando-se "->"
    int param1 = p->parametro1;
    int param2 = p->parametro2;

    //imprime os parâmetros
    printf("Thread %lu: Parametro 1 = %d, Parametro 2 = %d\n", (unsigned long) pthread_self(), param1, param2);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int i;

    //recupera o numero de threads passado na linha de comando: ex. ./programa 5
    //os parâmetros passados na linha de comando são strings, logo é necessário converter
    //para o tipo de dados desejado. Nesse caso o valor é inteiro, logo utiliza-se atoi()
    int n_threads = atoi(argv[1]);

    //cria o vetor do tipo pthread_t para armazenar o retorno de pthread_create() de cada thread
    //o número de elementos do vetor será igual ao número de threads passado na linha de comando (n_threads)
    pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * n_threads);

    //aloca dinamicamente um vetor que armazenará os parametros passados para cada thread
    //o vetor terá tamanho n_threads
    //cada parâmetro será do tipo "parametros_thread_t", ou seja, conterá 2 elementos inteiros que serão passados a cada thread
    parametros_thread_t *params = (parametros_thread_t *) malloc(sizeof(parametros_thread_t) * n_threads);

    //preenche os parâmetros a serem passados a cada thread e cria as threads
    for(i = 0; i < n_threads; i++) {
        params[i].parametro1 = i;
        params[i].parametro2 = i + 1;
        pthread_create(&threads[i], NULL, thread, (void *) &params[i]);
    }

    //aguarda a finalização de todas as threads
    for(i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);

    //libera o espaço alocado para os vetores de threads e parâmetros
    free(threads);
    free(params);

    return 0;
}