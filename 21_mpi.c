#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

/*
  Compile: mpicc -O -o 21_mpi 21_mpi.c
  Execute: mpirun -np 4 21_mpi
*/

#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(int argc, char *argv[]){

    int p, id;
    int i, j, k;
    int m1 = 6, m2 = 4, m3 = 3; //m2 e zaednichko
    double a[m1][m2] , b[m2][m3] , c[m1][m3];
    int count, overflow, offset, rows;
    MPI_Status status;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    //ako treba sami da gi vnesuvame elementite vo matricata i brojot na redici i koloni
    /*
    matrica = (int*)malloc(r*k*sizeof(int)); //ova ke e niza ama ke ja gledame kako matrica
    

    // ili moze i vaka:

    redici = (int*)malloc(r*sizeof(int));
    for(j<r)
        koloni = (int*)malloc(k*sizeof(int));
    
    */
    
    if(id == 0){
        for(i=0; i<m1;i++){
            for(j=0;j<m2;j++){
                a[i][j] = i+j;
               // printf("a[%d][%d]=%f", i , j , a[i][j]);
            }
        }
        for(i=0; i<m2;i++){
            for(j=0;j<m3;j++){
                b[i][j] = i+j;
                //printf("b[%d][%d]=%f", i , j , b[i][j]);
            }
        }
        //da ja pratime cela b matrica i del od a (da se podeli rabotata megju procesi)
        // ke ja pratime cela b matrica, taka shto ke pratime m2*m3 vrednosti
        // za a: treba da se presmeta kaj sekoj proces kolku prakjam, dali za nekoj proces postoi vishok redica(overflow), od kade vo matricata kje prakjam za sekoj proces
        count = m1 / p; // kolku minimum redici ke primi sekoj proces !!
        overflow = m1 % p ; //na kolku procesi ke treba da dademe plus redica
        offset = (id < overflow) ? count + 1 : count;   // presmetuva samo za nulti proces, a za sekoj sleden treba da se update-ne
        printf("%d | %d | %d", count, overflow, offset);

        for(i=1 ; i<p ; i++){
            rows = (i<overflow) ? count + 1 : count; // proces so id=i kje dobie volku redici
            MPI_Send(&offset, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows*m2, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            MPI_Send(&b, m2*m3, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            offset += rows; //mora da se zgolemi za da prodolzi za sledniot proces
        }
    }

    if( id > 0){
        MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, rows*m2, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, m2*m3, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
    }

    if(id == 0){
        rows = (id<overflow) ? count + 1 : count; 
    }




// vo globala isto se pishuvaat zadachite so matrici, osven ovoj del, t.e. matematikata se menuva
    for(i=0; i<m3 ; i++){
        for(j=0 ; j<rows ; j++){
            c[j][i]  = 0.0;
            for(k=0 ; k<m2 ; k++){
                c[j][i] = c[j][i] + a[j][k] * b[k][i];
            }
            printf("c[%d][%d] = %f" , j,i,c[j][i]);
        }
    }







    if(id > 0){
        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c, rows * m3, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }else{
        for(i = 1 ; i < p ; i++){
            MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows*m3, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
        }
        printf("***************************\n");
        for(i=0;i<m1;i++){
            for(j=0;j<m3;j++){
                printf(" %6.2f" , c[i][j]);
            }
            printf("\n");
        }
        printf("***************************\n");
    }

    

    MPI_Finalize();
    return 0;
}