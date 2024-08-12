#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

// Find minimum in an array, and the process/index where the minimum was.
/*
  Compile: mpicc -O -o 10_mpi 10_mpi.c
  Execute: mpirun -np 4 10_mpi
*/

int main(int argc, char *argv[]) {
	int id, p;
	int source;
	int *array, count;
	int local_min;
	int data_pair[2], data_pair_result[2];
	MPI_Status status;

	MPI_Init(&argc, &argv);
		
	MPI_Comm_size(MPI_COMM_WORLD, &p); 
	MPI_Comm_rank(MPI_COMM_WORLD, &id); 
	
	source = 0;

	if (id == source) {
		count = 20;
		array = (int *) malloc(count * sizeof(int));
		for (int i = 0; i < count; i++) {
			array[i] = rand() % 50;
		}
	}

	MPI_Bcast(&count, 1, MPI_INT, source, MPI_COMM_WORLD);
	if (id != source) {
		array = (int *) malloc(count * sizeof(int));
	}

	MPI_Bcast(array, count, MPI_INT, source, MPI_COMM_WORLD);

	if (id != source) {
		printf("Process %d ja primi nizata. Nizata ima %d elementi i e:\n", id, count);
		for (int i = 0; i < count; i++) {
			printf("%d, ", array[i]); 
		}
		printf("\n");
	}

	local_min = array[id];
	for (int i = id; i < count; i += p) {
		if (array[i] < local_min) {
			local_min = array[i];
		}
	}
	data_pair[0] = local_min;
	data_pair[1] = id;

	MPI_Reduce(&data_pair, &data_pair_result, 1, MPI_2INT, MPI_MINLOC, source, MPI_COMM_WORLD);

	if (id == source) {
		printf("Minimum e %d i e od procesot %d.\n", data_pair_result[0], data_pair_result[1]);
	}
	
	MPI_Finalize();
}


