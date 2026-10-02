/*
Alunos:
- Pedro Henrique Carvalho Pereira - 10418861
- Mateus Ribeiro Cerqueira - 10443901

Compilação e execução:
mpicc -o soma_quadrados soma_quadrados.c
mpirun -np 4 ./soma_quadrados

Referências extra aula:
- https://www.geeksforgeeks.org/c/sum-of-an-array-using-mpi/
- https://stackoverflow.com/questions/15658145/how-to-share-work-roughly-evenly-between-processes-in-mpi-despite-the-array-size

*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define N 40




// ======================================================

// calculo da soma por iteração
long soma_quadrados(const int *vetor, int tamanho) {
    long soma = 0;
    for (int i = 0; i < tamanho; i++)
        soma += (long)vetor[i] * vetor[i];
    return soma;
}



int main(int argc, char *argv[]) {

    //init
    int rank;
    int tam;



    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam);



    // divisão exata do vet
    if (N % tam != 0) {
        if (rank == 0) {
            printf("[Erro] Precisa ser um número que divida %d.\n", N);
        }


        MPI_Finalize();
        return 1;
    }

    int pedaco = N / tam;

    int *global = NULL;
    // 1 rank 0 cria o vetor 1->N
    if (rank == 0) {
        global = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            global[i] = i + 1;
    }

    int *local = malloc(pedaco * sizeof(int));

    // 2 Divisão igual entre os processos
    MPI_Scatter(global, pedaco, MPI_INT, local, pedaco, MPI_INT, 0, MPI_COMM_WORLD);

    // 3 chama soma dos quadrados
    long soma_local = soma_quadrados(local, pedaco);



    // ========== resutls ============
    printf("Processo %d recebeu:", rank);
    for (int i = 0; i < pedaco; i++)
        printf(" %d", local[i]);
    printf("\n");
    printf("Processo %d: soma local dos quadrados = %ld\n", rank, soma_local);




    // 4 reduce pro root
    long soma_paralela = 0;
    MPI_Reduce(&soma_local, &soma_paralela, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);



    // 5 e 6 root na soma sequencial
    if (rank == 0) {
        long soma_seq = soma_quadrados(global, N);

        printf("\nProcesso 0: soma paralela dos quadrados = %ld\n", soma_paralela);
        printf("Processo 0: soma sequencial esperada = %ld\n", soma_seq);




    // ===================

        if (soma_paralela == soma_seq)
            printf("\nOs valores conferem\n");
        else
            printf("\nOs valores não conferem\n");

        free(global);
    }
    // free
    free(local);

    MPI_Finalize();
    return 0;
}