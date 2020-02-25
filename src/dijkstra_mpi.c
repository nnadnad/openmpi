#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <mpi.h>
#include <memory.h>


#define SEND_NODEPOS_TAG 1
#define SEND_ALLOCATION_TAG 2
#define SEND_COUNTS_TAG 3
#define SEND_SUBGRAPH_TAG 4
//#define data[i*n][j] data[i*n + j]

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
    int **allocation, 
    int **nodePosition, 
    int numberOfProcessor) {
    //printf("<------allocate_matrix-------->\n");
    //printf("allocate matrix func:\n");
    int i,position;
    *allocation = malloc(numberOfProcessor * sizeof(**allocation));
    
    if(n<numberOfProcessor) { //rows of matrix <= number of processor
        
        for(i=0;i<numberOfProcessor;i++) {
            if (i<n) { 
                (*allocation)[i] = 1;
            }
            else { //unused processor
                (*allocation)[i] = 0;
            }
        }
    }
    else {
        int divide, remainder;
        divide = n / numberOfProcessor; //rows divided equally for each processor
        remainder = n % numberOfProcessor; //remainder of divided rows

        for(i=0;i<numberOfProcessor;i++) {
            (*allocation)[i] = divide;

            if (i==numberOfProcessor-1) {
                (*allocation)[i] += remainder; //put remainder on the last processor
            }
        }
    }
    position = 0;
    *nodePosition = malloc(numberOfProcessor * sizeof(**nodePosition));
    for (i=0;i<numberOfProcessor;i++) {
        (*nodePosition)[i] = position;
        position += (*allocation)[i];
    }
    //printf(">------allocate_matrix--------<\n");
}

static void distribute_graph(
    int n,
    int numberOfProcessor,
    long **graph,
    long **data,
    int rank,
    int **allocation,
    int **nodePosition) {
    //printf("<-----------rank%d----------->\n",rank);
    //printf("distribute_graph for rank %d:\n",rank);
    int i,j,k,count;
    long *subGraph = NULL;
    if (rank==0) { //at main processor
        //printf("gate 5-1\n");
        allocate_matrix(n,allocation,nodePosition,numberOfProcessor);
        //printf("gate 5-2\n");
        *data = malloc(n * (*allocation)[rank] *sizeof(**data)); //rows x col x sizeof data
        //printf("gate 5-3\n");
        
        for(i=(*nodePosition)[rank];i<((*allocation)[rank]+(*nodePosition)[rank]);i++) {
            for (j=0;j<n;j++) {    
                (*data)[j + (i - (*nodePosition)[rank])*n] = graph[i][j];
            }
        }
        //printf("gate 5-4\n");
        for (i=1;i<numberOfProcessor;i++) { //send subgraph to other processor
            subGraph = malloc(n * (*allocation)[i] *sizeof(*subGraph));
            
            for(j=(*nodePosition)[i];j<((*allocation)[i] + (*nodePosition)[i]);j++) {
                for (k=0;k<n;k++) {    
                subGraph[k + (j - (*nodePosition)[i])*n] = graph[j][k];
                }
            }

            count = n * (*allocation)[i];
            //send every info to other processor
            //MPI_Send(&n,1,MPI_INTEGER,i,SEND_NUM_TAG,MPI_COMM_WORLD);
            MPI_Send(*nodePosition,numberOfProcessor,MPI_INTEGER,i,SEND_NODEPOS_TAG,MPI_COMM_WORLD);
            MPI_Send(*allocation,numberOfProcessor,MPI_INTEGER,i,SEND_ALLOCATION_TAG,MPI_COMM_WORLD);
            MPI_Send(&count,1,MPI_INTEGER,i,SEND_COUNTS_TAG,MPI_COMM_WORLD);
            MPI_Send(subGraph,count,MPI_LONG,i,SEND_SUBGRAPH_TAG,MPI_COMM_WORLD);

            free(subGraph);
            subGraph = NULL;
        }
        //printf("gate 5-5\n");

    }

    else {
        //MPI_Recv(&n,1,);
        //printf("gate 5-6\n");
        *nodePosition = malloc(numberOfProcessor * sizeof(**nodePosition));
        *allocation = malloc(numberOfProcessor * sizeof(**allocation));
        MPI_Recv(*nodePosition,numberOfProcessor,MPI_INTEGER,0,SEND_NODEPOS_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(*allocation,numberOfProcessor,MPI_INTEGER,0,SEND_ALLOCATION_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&count,1,MPI_INTEGER,0,SEND_COUNTS_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        subGraph = malloc(count * sizeof(*subGraph));
        MPI_Recv(subGraph,count,MPI_LONG,0,SEND_SUBGRAPH_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        //printf("gate 5-7\n");
        *data = subGraph;
        
        free(subGraph);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    //printf(">-----------rank%d-----------<\n",rank);
}

static long** create_graph(int n) {
    //printf("<---------create_graph-------->\n");
    //printf("create_graph func:\n");
    int i,j;
    //printf("gate 4-1\n");
    long **graph = malloc(n * sizeof(*graph));
    for (i=0;i<n;i++) {
        graph[i] = malloc(n * sizeof(**graph));
    }
    //printf("gate 4-2\n");

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
    //printf("gate 4-3\n");
    //printf(">---------create_graph--------<\n");
    return graph;
}

static void print_graph(long **data, int n) {
    //printf("<------print_graph------>\n");
    //printf("print_graph func:\n");
    int i,j;
    for (i=0;i<n;i++) {
        for (j=0;j<n;j++) {
            printf("%li ",data[i][j]);
        }
        printf("\n");
    }
    //printf(">------print_graph------<\n");
}

static void free_graph(long **data, int n) {
    //printf("<------free_graph------>\n");
    int i;
    for (i=0;i<n;i++) {
        free(data[i]);
    }
    
    free(data);
    //printf(">------free_graph------<\n");
}

static void dijkstra(
    int n,
    int numberOfProcessor,
    int sourceNode,
    long *data,
    int rank,
    int *allocation,
    int *nodePosition,
    long **result) {
    
    int i,j,sourceProcessor,minIndex;
    long minDistance;
    long *allResult = NULL;
    long *localResult = NULL;
    int *visitedNode=NULL;
    //find which processor have the source node
    //printf("<------dijkstra rank %d------>\n",rank);
    for (i=0;i<numberOfProcessor;i++) {
        if (sourceNode<nodePosition[i]) {
            sourceProcessor = i-1;
            break;
        }
        else if (i==numberOfProcessor-1) {
            sourceProcessor=i;
        }
    }

    //initialize visited node
    visitedNode = malloc(n * sizeof(*visitedNode));
    for (i=0;i<n;i++) {
        visitedNode[i] = 0;
    }

    //intialize the result of min distance for each vertex from the source
    localResult = malloc(n * sizeof(*localResult));
    allResult = malloc(n * sizeof(*allResult));
    
    if (rank==sourceProcessor) {
        for(i=0;i<n;i++) {
            
            allResult[i] = data[n*(sourceNode-nodePosition[sourceProcessor]) + i];
            
        }
    }
    //broadcast initial result to other process
    MPI_Bcast(allResult,n,MPI_LONG,sourceProcessor,MPI_COMM_WORLD);
    //synchronizing
    MPI_Barrier(MPI_COMM_WORLD);
    //set distance at sourceNode
    minIndex = -1;
    visitedNode[sourceNode] = 1;
    
    //iterate until every node is visited, each iteration must select the minimum distance
    for (i=1;i<n;i++) {
        minDistance = LONG_MAX;
        //find min distance
        for (j=0;j<n;j++) {
            if (visitedNode[j]==0 && allResult[j]<minDistance && allResult[j]!=0) {
                minDistance = allResult[j];
                minIndex = j;
            }
            localResult[j] = allResult[j];
        }
        visitedNode[minIndex] = 1; //node visited

        //update dist value
        /*
        for (j=0;j<n;j++) {
            if (visitedNode[j]==0 && data[n*(sourceNode-nodePosition[sourceProcessor]) + j] && 
            allResult[minIndex]!=INT_MAX && minDistance+data[n*(sourceNode-nodePosition[sourceProcessor]) + j] < allResult[j]) {
                localResult[j] = minDistance + data[n*(sourceNode-nodePosition[sourceProcessor]) + j];
            }
        }
        */
        for (j=0;j<allocation[rank];j++) {
            if (visitedNode[j+nodePosition[rank]]) {
                continue;
            }
            if (data[j*n+minIndex]+minDistance < localResult[j+nodePosition[rank]] && data[j*n+minIndex]+minDistance!=0) {
                localResult[j+nodePosition[rank]] = data[j*n+minIndex]+minDistance;
            }
        }

        MPI_Allreduce(localResult,allResult,n,MPI_LONG,MPI_MIN,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    free(visitedNode);

    *result = allResult;

    free(localResult);
    //printf(">------dijkstra rank %d------<\n",rank);

}

void write_to_txt(int n, long **const graph, char *filename) {
    //printf("<------write_to_text------>\n");
    FILE *fout;
    int i,j;
    if (NULL == (fout = fopen(filename,"w"))) {
        fprintf(stderr,"error opening output file");
        abort();
    }

    for (i=0;i<n;i++) {
        for(j=0;j<n;j++) {
            fprintf(fout,"%li ",graph[i][j]);
        }
        fprintf(fout,"\n");
    }
    printf("Result has been written to %s ...\n",filename);
}

void print_subgraph(int n, long *data, int *allocation, int rank) {
    int i;
    int count = 0;
    //printf("<------print_subgraph rank%d------>\n",rank);
    //printf("print_subGraph func:\n");
    //printf("allocation[%d] = %d\n",rank,allocation[rank]);
    for (i=0;i<n*allocation[rank];i++) {
        printf("data[%d] = %li ",i,data[i]);
    }
    //printf(">------print_subgraph rank%d------<\n",rank);
    //printf("\n");
}
int main(int argc, char **argv) {
    //printf("gate 1\n");
    //check argument
    if (argc!=3) {
        fprintf(stderr,"Usage: Dijkstra num_of_node output_filename");
        return EXIT_FAILURE;
    }

    int i,source,n,size,rank;
    double start_time, end_time, total_time;
    long **graph = NULL;
    int *allocation = NULL; 
    int *nodePosition = NULL; 
    long *data = NULL;
    long *result = NULL;
    //printf("gate 2\n");
    //init mpi
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    n = atoi(argv[1]); // num_of_node
    //printf("gate 3\n");
    srand(ID);
    //create graph
    if (rank==0) { //graph created only on main processor
        //printf("gate 4\n");
        graph = create_graph(n);
        print_graph(graph, n); //print created graph
    }

    //distribute graph to each processor
    //printf("gate 5\n");
    distribute_graph(n, size, graph, &data, rank, &allocation, &nodePosition);
    
    MPI_Barrier(MPI_COMM_WORLD);

    //print_subgraph(n,data,allocation,rank);
    //init time
    total_time = 0;
    
    for(source=0;source < n; source++) {
        start_time = MPI_Wtime();
        //dijkstra algorithm
        dijkstra(n,size,source,data,rank,allocation,nodePosition,&result);
        end_time = MPI_Wtime();
        
        //collect result in main processor
        if (rank==0) {
            for (i=0;i<n;i++) {
                graph[source][i] = result[i];
            }
        }
        
        total_time += end_time-start_time;
        free(result);
    }

    //write result to file
    if (rank==0) {
        write_to_txt(n, graph,argv[2]);
        printf("\n");
        printf("processing time: %lf sec ...\n",total_time);
    }

    //print time
    
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank==0) {
        free_graph(graph,n);
    }
    free(nodePosition);
    free(allocation);
    free(data);
    MPI_Finalize();
    
    return EXIT_SUCCESS;
}

