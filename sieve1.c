#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

int main (int argc, char ** argv) {
  int i,n;
  int index,size;
  int prime, count, global_count,first;
  long int high_value, low_value;
  int comm_rank, comm_size;
  char * crossed;
  double runtime;
  char* filename = argc[1];
  char* ending = '.txt';
  FILE *fp;

  strncar(filename, ending,4); 
  //starting MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  
  MPI_Barrier(MPI_COMM_WORLD);
  //starting clock
  runtime = -MPI_Wtime();
  n = atoi(argv[1]);


  //calculating the low and high values of each processors array and its size
  low_value  = 2 + (long int)(comm_rank) * (long int)(n - 1) / (long int)comm_size;
  high_value = 1 + (long int)(comm_rank + 1) * (long int)(n - 1) / (long int)comm_size;
  size = high_value - low_value + 1;
  
  //only need to deal with whether the numbers are crossed,
  //we know it's an array from 2 to n
  crossed = (char *)calloc(size, sizeof(char));
  if (comm_rank == 0){
    index = 0;
  }
  prime = 2;

  while (prime * prime <= n){
    //finding next unused prime
    if (prime * prime > low_value) {
      first = prime * prime - low_value;
    }
    else{
      if ((low_value % prime) == 0){
        first = 0;
      }
      else{
        first = prime - (low_value % prime);
      }
    }

    for (i = first; i < size; i += prime){
      crossed[i] = 1;
      //cross out current prime's multiples
    }
    if (comm_rank == 0) {
      while (crossed[++index]){
        //set process 0 to find the next prime
        prime = index + 2;
      }
    }
    if (comm_size > 1){
      //set the other processes to recieve the new prime
      MPI_Bcast(&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
    }

  }

  runtime += MPI_Wtime(); //stopping timer
  MPI_Finalize();
  //opening and writing to text file
  fp = fopen(filename, "w");
  for(i = 0; i<sizeof(crossed);i++){
    if(crossed[i]==0){
      fputs("%d ",i+2);
    }
  }
  fclose(fp);
  return 0;
}
