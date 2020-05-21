#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
/* Commands to run:
   mpicc -o mpi mpi_primes.c -lm
   mpirun -np 8 ./mpi
*/

int range = 10000000; 
int process_count;  
int procrank; 
double elapsed;
              

int is_prime(int n) {
   int top,d;
   top = (int)(sqrt((double)n)+0.0001);
   for (d = 2; d <= top; d++)
      if ( n % d == 0 )
         return 0;
   return 1;
}

void main(int argc, char **argv) {

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &procrank);
   MPI_Comm_size(MPI_COMM_WORLD, &process_count);
   int min;  
   int max; 
   double dt = (double)range / process_count;
   double start = MPI_Wtime();

   min = (procrank == 0)? 2 : (int)(procrank*dt) + 1;
   max = (procrank == process_count-1)? range : (int)((procrank+1)*dt);

   int count = 0;  
   
   int i;

   if ( procrank != 0) {

      for (i = min; i <= max; i++)   
         if (is_prime(i))
            count++;
      int ds[3];
      ds[0] = count;
      ds[1] = min;
      ds[2] = max;
      MPI_Send( ds, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
   }
   else {
       
      for (i = min; i <= max; i++)    
         if (is_prime(i))
            count++;

      int ct;
      elapsed = MPI_Wtime() - start;

      for (ct = 1; ct < process_count; ct++) {

         int data_received[3];
         MPI_Status status;
         MPI_Recv( data_received, 3, MPI_INT, MPI_ANY_SOURCE,
                        MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         count += data_received[0];

      }
    
      printf("\nNumber of primes in 1 to %d are \n%d", range, count);
      printf("\nTime elapsed : %f seconds.\n", elapsed);
   }

   
   MPI_Finalize();

}

