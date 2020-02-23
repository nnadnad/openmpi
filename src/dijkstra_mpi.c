#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>


#define SEND_NODEPOS_TAG 1
#define SEND_ALLOCATION_TAG 2
#define SEND_COUNTS_TAG 3
#define SEND_SUBGRAPH_TAG 4

#define ID 13517035
/*........................................................................
 Note:         =        1.  The number of processes, p, should evenly divide n.
                        2.  You should free the MPI_Datatype object created by
                            the program with a call to MPI_Type_free:  see the
                            main function.
                        3.  Example:  Suppose the matrix is
                               0 1 2 3
                               4 0 5 6
                               7 8 0 9
                               8 7 6 0
                        Then if there are two processes, the matrix will be
                        distributed as follows:
                           Proc 0:  0 1    Proc 1:  2 3
                                    4 0             5 6
                                    7 8             0 9
                                    8 7             6 0
  .......................................................................... */

static void allocate_matrix(
    int n, 
    int** allocation, 
    int** nodePosition, 
    int numberOfProcessor) {
    
    int i,position;
    *allocation = malloc(numberOfProcessor * sizeof(**allocation));
    
    if(n<=numberOfProcessor) { //rows of matrix <= number of processor
        
        for(i=0;i<numberOfProcessor;i++) {
            if (i<n) { 
                *allocation[i] = 1;
            }
            else { //unused processor
                *allocation[i] = 0;
            }
        }
    }
    else {
        int divide, remainder;
        divide = n / numberOfProcessor; //rows divided equally for each processor
        remainder = n % numberOfProcessor; //remainder of divided rows

        for(i=0;i<numberOfProcessor;i++) {
            *allocation[i] = divide;

            if (i==numberOfProcessor-1) {
                *allocation[i] += remainder; //put remainder on the last processor
            }
        }
    }
    position = 0;
    *nodePosition = malloc(numberOfProcessor * sizeof(**nodePosition));
    for (i=0;i<numberOfProcessor;i++) {
        *nodePosition[i] = position;
        position += *allocation[i];
    }
}

static void distribute_graph(
    int n,
    int numberOfProcessor,
    int** graph,
    int** data,
    int rank,
    int** allocation,
    int** nodePosition) {
    
    int i,j,k,count;
    int* subGraph = NULL;
    if (rank==0) { //at main processor
        
        allocate_matrix(n,allocation,nodePosition,numberOfProcessor);

        *data = malloc(n * *allocation[rank] *sizeof(**data)); //rows x col x sizeof data

        for (i=0;i<n;i++) {
            for(j=*nodePosition[rank];j<(*allocation[rank]+*nodePosition[rank]);j++) {
                *data[i*n + j] = graph[i][j];
            }
        }

        for (i=1;i<numberOfProcessor;i++) { //send subgraph to other processor
            subGraph = malloc(n * *allocation[i] *sizeof(*subGraph));

            for(j=0;j<n;j++) {
                for(k=*nodePosition[i];k<(*allocation[rank]+*nodePosition[rank]);k++) {
                    subGraph[j*n + (k-*nodePosition[rank])] = graph[j][k]; //
                }
            }
            count = n * (*allocation[i]);
            //send every info to other processor
            //MPI_Send(&n,1,MPI_INTEGER,i,SEND_NUM_TAG,MPI_COMM_WORLD);
            MPI_Send(*nodePosition,numberOfProcessor,MPI_INTEGER,i,SEND_NODEPOS_TAG,MPI_COMM_WORLD);
            MPI_Send(*allocation,numberOfProcessor,MPI_INTEGER,i,SEND_ALLOCATION_TAG,MPI_COMM_WORLD);
            MPI_Send(&count,1,MPI_INTEGER,i,SEND_COUNTS_TAG,MPI_COMM_WORLD);
            MPI_Send(subGraph,count,MPI_INTEGER,i,SEND_SUBGRAPH_TAG,MPI_COMM_WORLD);

            free(subGraph);
        }

    }
    else {
        //MPI_Recv(&n,1,);
        *nodePosition = malloc(numberOfProcessor * sizeof(**nodePosition));
        *allocation = malloc(numberOfProcessor * sizeof(**allocation));
        MPI_Recv(*nodePosition,numberOfProcessor,MPI_INTEGER,0,SEND_NODEPOS_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(*allocation,numberOfProcessor,MPI_INTEGER,0,SEND_ALLOCATION_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&count,1,MPI_INTEGER,0,SEND_COUNTS_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        *data = malloc(n * *allocation[rank] * sizeof(**data));
        MPI_Recv(*data,count,MPI_INTEGER,0,SEND_SUBGRAPH_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    }
    MPI_Barrier(MPI_COMM_WORLD);
}

static int** create_graph(int n) {
    int i,j;
    int** graph = NULL;

    **graph = malloc(n * sizeof(*graph));
    for (i=0;i<n;i++) {
        graph[i] = malloc(n * sizeof(int));
    }


    for (i=0;i<n;i++) {
            for (j=i;j<n;j++) {
                if (i==j) {
                    graph[i][j] = 0;
                }
                else {
                    graph[i][j] = rand();
                    graph[j][i] = graph[i][j];
                }
            }
        }
    
    return graph;
}

void print_graph(int**data, int n) {
    int i,j;
    for (i=0;i<n;i++) {
        for (j=0;j<n;j++) {
            printf("%d ",data[i][j]);
        }
        printf("\n");
    }
}

void free_graph(int** data, int n) {
    int i;
    for (i=0;i<n;i++) {
        free(data[i]);
    }
    
    free(data);
}

int main(int argc, char **argv) {

    //check argument
    if (argc!=2) {
        fprintf(stderr,"Usage: Dijkstra num_of_node");
    }

    int n,size,rank;
    double start_time, end_time;
    int **graph = NULL;
    int *matrix = NULL, *allocation = NULL, *nodePosition = NULL, *data = NULL;
    
    //init mpi
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    n = atoi(argv[1]); // num_of_node
    
    //create graph
    if (rank==0) { //graph created only on main processor
        graph = create_graph(n);
        print_graph(graph, n); //print created graph
    }

    //distribute graph to each processor
    distribute_graph(n, size, graph, &data, rank, &allocation, &nodePosition);
    free_graph(graph,n);
    MPI_Barrier(MPI_COMM_WORLD);

    //init time
    start_time = MPI_Wtime();
    //do dijkstra algorithm
    //dijsktra
    //end time
    end_time = MPI_Wtime();
    //free mpi
}

