#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>
#include <sys/resource.h>  // for getrusage
#include <unistd.h>        // for sysconf
 
#define V 4 // Number of vertices in the graph
 
// Function to find the vertex with minimum key value
int minKey(int key[], int mstSet[], int n) {
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < n; v++) {
        if (mstSet[v] == 0 && key[v] < min) {
            min = key[v];
            min_index = v;
        }
    }
 
    return min_index;
}
 
// Function to print the constructed MST
void printMST(int parent[], int graph[V][V]) {
    printf("Edge   Weight\n");
    for (int i = 1; i < V; i++)
        printf("%d - %d    %d \n", parent[i], i, graph[i][parent[i]]);
}
 
// Function to construct and print MST for a graph represented using an adjacency matrix representation
void primMST(int graph[V][V], int n, int rank, int size) {
    int parent[V]; // Array to store constructed MST
    int key[V];    // Key values used to pick the minimum weight edge in the cut
    int mstSet[V]; // To represent the set of vertices included in MST
 
    // Initialize all keys as INFINITE
    for (int i = 0; i < V; i++) {
        key[i] = INT_MAX;
        mstSet[i] = 0;
    }
 
    // Always include the first vertex in MST
    key[0] = 0;    // Make key 0 so that this vertex is picked as the first vertex
    parent[0] = -1; // First node is always the root of MST
 
    // The MST will have V vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum key vertex from the set of vertices not yet included in MST
        int local_u = minKey(key, mstSet, V);
 
        int global_u;
        MPI_Allreduce(&local_u, &global_u, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
 
        // Add the picked vertex to the MST Set
        mstSet[global_u] = 1;
 
        // Update key value and parent index of the adjacent vertices of the picked vertex.
        // Consider only those vertices which are not yet included in the MST
        for (int v = 0; v < V; v++) {
            if (graph[global_u][v] && mstSet[v] == 0 && graph[global_u][v] < key[v]) {
                parent[v] = global_u;
                key[v] = graph[global_u][v];
            }
        }
    }
 
    // Print the constructed MST
    if (rank == 0) {
        printMST(parent, graph);
    }
}
 
double getCPUUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
 
    double total_time = (double)(usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) +
                        (double)(usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) * 1e-6;
 
    return total_time;
}
 
long getMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
 
    // Memory usage is reported in kilobytes
    return usage.ru_maxrss;
}
 
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
 
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
 
    double start_time = MPI_Wtime(); // Record start time
 
    int graph[V][V] = {{0, 2, 0, 1},
                       {2, 0, 3, 4},
                       {0, 3, 0, 5},
                       {1, 4, 5, 0}};
 
    // Print the solution in serial
    if (rank == 0) {
        printf("Serial Execution:\n");
        double serial_start_time = MPI_Wtime();
        primMST(graph, V, rank, size);
        double serial_execution_time = MPI_Wtime() - serial_start_time;
        printf("Serial Execution Time: %f seconds\n", serial_execution_time);
        printf("CPU Usage: %f seconds\n", getCPUUsage());
        printf("Memory Usage: %ld KB\n", getMemoryUsage());
    }
 
    // Print the solution in parallel
    printf("\nParallel Execution:\n");
    double parallel_start_time = MPI_Wtime();
    primMST(graph, V, rank, size);
    double parallel_execution_time = MPI_Wtime() - parallel_start_time;
    printf("Parallel Execution Time: %f seconds\n", parallel_execution_time);
    printf("CPU Usage: %f seconds\n", getCPUUsage());
    printf("Memory Usage: %ld KB\n", getMemoryUsage());
 
    // Record and print the total execution time of your program
    double execution_time = MPI_Wtime() - start_time;
    if (rank == 0) {
        printf("\nTotal Execution Time: %f seconds\n", execution_time);
    }
 
    MPI_Finalize();
 
    return 0;
}
