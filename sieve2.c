#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 60

int main(int argc, char *argv[])
{
long timeCPU;
int rank, size, k, i, v, n, scatterSize;
int **tmpArray, *myArray, firstElement, lastElement;

MPI_Init(&argc,&argv);

MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

if(N%size != 0)
{
    MPI_Finalize();
    printf("ERROR!");
    exit(0);
}

firstElement = (rank*N)/size;
lastElement = floor(((rank+1)*N)/size);
myArray = (int*) (malloc(((N/size)-1)*sizeof(int)));

for(i=0, v=firstElement+2; i<=(N/size)-1; i++, v++)
{
    myArray[i] = v;
}
    printArray(myArray, (N/size)-1);
/* All processes have to execute above code first */
MPI_Barrier(MPI_COMM_WORLD);

k = 2;

do
{
    markMultiples(k, myArray, lastElement-firstElement);
    k = nextNumber(k, myArray, lastElement-firstElement);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
}
while(k*k <=N);

MPI_Send(myArray, (N/size)-1, MPI_INT, 0, 50, MPI_COMM_WORLD);

MPI_Barrier(MPI_COMM_WORLD);

if (rank == 0)
{
    // Initialize 2D array
    tmpArray = (int**) (malloc(size*sizeof(int)));
    for(i=0;i<size;i++)
    {
        tmpArray[i] = (int*) (malloc(((N/size)-1)*sizeof(int)));
    }

    // Gather from every process his own array
    for (i=0; i<size;i++)
    {
        MPI_Recv(tmpArray[i], (N/size)-1, MPI_INT, i, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printArray(tmpArray[i], (N/size)-1);
        printf("-----------------------------------------\n");
    }

    // Build 1d array with primes - TODO

}

//printArray(array, lastElement-firstElement);
free(myArray);
MPI_Finalize();
return 0;
}

int nextNumber(int k, int *array, int n)
{
int i;

for(i=0; i<=n; i++)
{
    if(array[i] > k)
        return array[i];
}
return -1;
}

void markMultiples(int k, int *array, int n)
{
int i;

for (i=0; i<=n; i++)
{
    if(array[i] % k == 0 && array[i] != k)
    {
        array[i] = -1;
    }
}
}

void printArray(int *array, int n)
{
int i;

for(i=0; i<=n; i++)
{
    printf("array[%d] = %d\n", i, array[i]);
}
}