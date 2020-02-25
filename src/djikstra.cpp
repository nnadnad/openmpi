#include <limits.h> 
#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
 
// Number of vertices in the graph 
#define V 100


void generateRandomArray(int **b, int node){
    srand(13517035);
    for(int i = 0; i < node; i++){
        for(int j = 0; j < node; j++){
            b[i][j] = rand() % 7;
        }
    }

    // Cleaning Process
    for(int i=0;i<node;i++){
        for(int j=i;j<node;j++){
            if(j == i){
                b[i][j] = 0;
            }
            else{
                b[i][j] = b[j][i];
            }
        }
    }
}

void printMatrix(int **arr){
    for(int i = 0; i < V; i++){
        for(int j = 0; j < V-1; j++){
            printf("%d ", arr[i][j]);
        }
        printf("%d\n", arr[i][V-1]);
    }
}

int minDistance(int  dist[], bool sptSet[]){ 
    int min = INT_MAX, min_index; 
  
    for (int v = 0; v < V; v++) 
        if (sptSet[v] == false && dist[v] <= min) 
            min = dist[v], min_index = v; 
  
    return min_index; 
} 
  
int printSolution(int dist[]){ 
    printf("Vertex \t\t Distance from Source\n"); 
    for (int i = 0; i < V; i++) 
        printf("%d \t\t %d\n", i, dist[i]); 
} 
  
void dijkstra(int** graph, int src){ 
    int dist[V];  
    bool sptSet[V];
    
    for (int i = 0; i < V; i++) 
        dist[i] = INT_MAX, sptSet[i] = false; 
  
    // Distance of source vertex from itself is always 0 
    dist[src] = 0; 
  
    // Find shortest path for all vertices 
    for (int count = 0; count < V - 1; count++) { 
        // Pick the minimum distance vertex from the set of vertices not 
        // yet processed. u is always equal to src in the first iteration. 
        int u = minDistance(dist, sptSet); 
  
        // Mark the picked vertex as processed 
        sptSet[u] = true; 
  
        // Update dist value of the adjacent vertices of the picked vertex. 
        for (int v = 0; v < V; v++) 
  
            // Update dist[v] only if is not in sptSet, there is an edge from 
            // u to v, and total weight of path from src to  v through u is 
            // smaller than current value of dist[v] 
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX 
                && dist[u] + graph[u][v] < dist[v]) 
                dist[v] = dist[u] + graph[u][v]; 
    } 
  
    // print the constructed distance array 
    printSolution(dist); 
} 
  
// driver program to test above function 
int main() {
    int **arr;
    int j;
    arr = (int **)malloc(V*sizeof(int*));
    for(j=0;j<V;j++)
        arr[j]=(int*)malloc(V*sizeof(int));
    
    generateRandomArray(arr, V);
    printMatrix(arr);
    dijkstra(arr, 0);

    return 0; 
} 
