#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int** create_graph(int n) {
    int i,j;
    int** data = (int**) malloc(sizeof(int*) * n);

    for (i=0;i<n;i++) {
        data[i] = (int*) malloc(sizeof(int) * n);
    }

    for (i=0;i<n;i++) {
        for (j=i;j<n;j++) {
            if (i==j) {
                data[i][j] = 0;
            }
            else {
                data[i][j] = rand();
                data[j][i] = data[i][j];
            }
        }
    }

    return data;
}

int main(int argc, char **argv) {

    //create graph
    if (argc!=2) {
        fprintf(stderr,"Usage: ");
    }
    
    //init mpi
}

