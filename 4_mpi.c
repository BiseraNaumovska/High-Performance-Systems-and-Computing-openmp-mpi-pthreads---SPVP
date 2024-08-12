#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

/*
  Compile: mpicc -O -o 4_mpi 4_mpi.c
  Execute: mpirun -np 4 4_mpi
*/

int main(int argc, char *argv[]) {
	int id, p, t;
	int tag, source;
	int *array, count;
	MPI_Status status;

	MPI_Init(&argc, &argv); 
		
	MPI_Comm_size(MPI_COMM_WORLD, &p); 
	MPI_Comm_rank(MPI_COMM_WORLD, &id); 
	
	source = 0;
	tag = 1234; 

	if (id == source) { 
		count = 5;
		array = (int *) malloc(count * sizeof(int));
		for (int i = 0; i < count; i++) {
			t = scanf("%d", &array[i]);
		}
		for (int i = 1; i < p; i++) {
			MPI_Send(&count, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			MPI_Send(array, count, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
	}

	if (id != source) { 
		MPI_Recv(&count, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
		array = (int *) malloc(count * sizeof(int));
		MPI_Recv(array, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

		printf("Process %d ja primi nizata. Nizata ima %d elementi i e: ", id, count);
		for (int i = 0; i < count; i++) {
			printf("%d, ", array[i]); 
		}
	}
	
	MPI_Finalize();
}


