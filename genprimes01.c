#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]){
    long timeCPU;
    int rank, size, k, i, v, n, scatterSize,remainder;
    int **tmpArray, *myArray, firstElement, lastElement, arraySize;
    FILE *fp;

    char filename = argv[1];
    char* text = ".txt";
    strncat(filename,text,4);
    fp = fopen(filename,"w");
    char N = atoi(argv[1]);

    //start parallel part
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //setting up sizes of each of the cores' arrays and filling them with numbers 2-N
    remainder = N-1%size;
    arraySize = N-1/size;
    if (rank < remainder){
        firstElement = rank * (arraySize + 1);
        lastElement = firstElement + arraySize;
    } else {
        firstElement = rank * arraySize + remainder;
        lastElement = firstElement + (arraySize - 1);

    }
    //creation of original array from 2 to N
    myArray = (int*) (malloc((arraySize)*sizeof(int)));
    for(i=0, v=firstElement+2; i<=arraySize; i++, v++){
        myArray[i] = v;
    }

    //Start crossing multiples of primes until halfway
    k = 2;
    while(k*k <=N){
        markMultiples(k, myArray, lastElement-firstElement);
        k = nextNumber(k, myArray, lastElement-firstElement);
        MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    //all processes send their arrays to process 0
    MPI_Send(myArray, arraySize, MPI_INT, 0, 50, MPI_COMM_WORLD);


    if (rank == 0){
        //set up a 2D array for recieving
        tmpArray = (int**) (malloc(size*sizeof(int)));
        for(i=0;i<size;i++){
            tmpArray[i] = (int*) (malloc((arraySize)*sizeof(int)));

        }
    for (i=0; i<size;i++){
        MPI_Recv(tmpArray[i], arraySize, MPI_INT, i, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    //printing all non-crossed number into text file
   for(int x = 0; x<=size;x++){
       for(int y = 0; y<=tmpArray[x].size(); y++){
           if(tmpArray[x][y] != -1){
               char *output = itoa(tmpArray[x][y]);
               strncat(output, " ");
               fputs(output,fp);
                }
            }
        }
    }
    fclose(fp);
    free(myArray);
    MPI_Finalize();
    return 0;
}

int nextNumber(int k, int *array, int n){
    //helper function to find the next prime
    for(int i=0; i<=n; i++){
        if(array[i] > k){
           return array[i];
            }
        }
    return -1;
}

void markMultiples(int k, int *array, int n){
    //helper function to cross out multiples of the prime
    for (int i=0; i<=n; i++){
        if(array[i] % k == 0 && array[i] != k){
            array[i] = -1;
        }
    }
}

void printArray(int *array, int n){
    //helper to print a 1-D array
    for(int i=0; i<=n; i++){
        printf("array[%d] = %d\n", i, array[i]);
    }
}