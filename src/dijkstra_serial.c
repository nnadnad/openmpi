#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <memory.h>

#define ID 13517035

static long** create_graph(int n) {
    //printf("<---------create_graph-------->\n");
    //printf("create_graph func:\n");
    int i,j;
    //printf("gate 4-1\n");
    long **graph = malloc(n * sizeof(*graph));
    for (i=0;i<n;i++) {
        graph[i] = malloc(n * sizeof(**graph));
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

void dijkstra_serial(int n, int source, long *data, long **result) {
    int i,j,minIndex;
    long minDistance;
    int *visitedNode = malloc(n*sizeof(*visitedNode));
    long *allResult = malloc(n * sizeof(*allResult));

    for (i=0;i<n;i++) {
        visitedNode[i] = 0;
        allResult[i] = data[source*n + i];
    }

    minIndex = -1;
    visitedNode[source] = 1;

    for (i=1;i<n;i++) {
        minDistance = LONG_MAX;

        for (j=0;j<n;j++) {
            if (visitedNode[j]==0 && allResult[j]<minDistance && allResult[j]!=0) {
                minDistance = allResult[j];
                minIndex = j;
            }
        }
        visitedNode[minIndex] = 1;

        for (j=0;j<n;j++) {
            if (visitedNode[j]) {
                continue;
            }
            else if (data[j*n+minIndex]+minDistance<allResult[j] && data[j*n+minIndex]+minDistance!=0) {
                allResult[j] = data[j*n+minIndex]+minDistance;
            }
        }
    }
    
    *result = allResult;
    free(visitedNode);
}

void write_to_txt(int n, long **const graph, char *filename) {
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
    printf("Result has been written to serial_%s ...\n",filename);
}

static long get_micros(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((long)ts.tv_sec * 1000000000L + ts.tv_nsec)/1000;
}

int main(int argc, char **argv) {
    long **graph = NULL;
    long *data = NULL, *result = NULL;
    int n,i,j,source;
    long start, end, total_time = 0;

    if (argc!=3) {
        fprintf(stderr,"Usage: Dijkstra num_of_node output_filename\n");
        return EXIT_FAILURE;
    }
    srand(ID);
    n = atoi(argv[1]);

    graph = create_graph(n);
    
    //print_graph(graph,n);
    //printf("\n\n");
    data = malloc(n * n * sizeof(*data));
    
    //print_graph(graph,n);
    //printf("\n\n");

    for (i=0;i<n;i++) {
        for (j=0;j<n;j++) {
            data[i*n + j] = graph[i][j];
        }
    }
    
    //print_graph(graph,n);
    //printf("\n\n");

    for (source=0;source<n;source++) {
        result = malloc(n*sizeof(*result));
        
        start = get_micros();
        dijkstra_serial(n,source,data,&result);
        end = get_micros();
        
        for (j=0;j<n;j++) {
            graph[source][j] = result[j];
        }

        total_time += end-start;

        free(result);
        result = NULL;
    }
    
    //print_graph(graph,n);
    printf("\n");

    write_to_txt(n,graph,argv[2]);
    printf("processing time: %ld us ...\n",total_time);

    free(data);
    free_graph(graph,n);

    return EXIT_SUCCESS;


}