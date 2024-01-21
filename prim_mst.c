#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include <sys/resource.h> // for getrusage
#include <unistd.h> // for sysconf
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define V 5 // Number of vertices in the graph

// Function to find the vertex with minimum key value
int minKey(int key[], int mstSet[]) {
 int min = INT_MAX, min_index;

 #pragma omp parallel for
 for (int v = 0; v < V; v++) {
 if (mstSet[v] == 0 && key[v] < min) {
 #pragma omp critical
 {
 min = key[v];
 min_index = v;
 }
 }
 }

 return min_index;
}

// Function to print the constructed MST
void printMST(int parent[], int graph[V][V]) {
 printf("Edge Weight\n");
 for (int i = 1; i < V; i++)
 printf("%d - %d %d \n", parent[i], i, graph[i][parent[i]]);
}

// Function to construct and print MST for a graph represented using adjacency matrix
representation
void primMST(int graph[V][V]) {
 int parent[V]; // Array to store constructed MST
 int key[V]; // Key values used to pick the minimum weight edge in cut
 int mstSet[V]; // To represent the set of vertices included in MST

 // Initialize all keys as INFINITE
 #pragma omp parallel for
 for (int i = 0; i < V; i++) {
 key[i] = INT_MAX;
 mstSet[i] = 0;
 }

 // Always include the first vertex in MST
 key[0] = 0; // Make key 0 so that this vertex is picked as the first vertex
 parent[0] = -1; // First node is always the root of MST

 // The MST will have V vertices
 #pragma omp parallel for
 for (int count = 0; count < V - 1; count++) {
 // Pick the minimum key vertex from the set of vertices not yet included in MST
 int u = minKey(key, mstSet);

 // Add the picked vertex to the MST Set
 mstSet[u] = 1;

 // Update key value and parent index of the adjacent vertices of the picked vertex.
 // Consider only those vertices which are not yet included in the MST
 #pragma omp parallel for
 for (int v = 0; v < V; v++) {
 if (graph[u][v] && mstSet[v] == 0 && graph[u][v] < key[v]) {
 #pragma omp critical
 {
 parent[v] = u;
 key[v] = graph[u][v];
 }
 }
 }
 }

 // Print the constructed MST
 printMST(parent, graph);
}

double getCPUUsage() {
 int fd = open("/proc/stat", O_RDONLY);
 if (fd == -1) {
 perror("Error opening /proc/stat");
 return -1.0;
 }

 char stat[4096];
 ssize_t bytesRead = read(fd, stat, sizeof(stat) - 1);
 close(fd);

 if (bytesRead == -1) {
 perror("Error reading /proc/stat");
 return -1.0;
 }

 stat[bytesRead] = '\0';

 // Parse /proc/stat for CPU usage
 unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
 sscanf(stat, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
 &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

 double total_time = (double)(user + nice + system + idle + iowait + irq + softirq + steal + guest
+ guest_nice);
 double seconds = omp_get_wtime(); // get the current time
 double cpu_usage = ((total_time - idle) / total_time) / seconds;

 return cpu_usage;
}

long getMemoryUsage() {
 struct rusage usage;
 getrusage(RUSAGE_SELF, &usage);

 // Memory usage is reported in kilobytes
 return usage.ru_maxrss;
}

int main() {
 double start_time = omp_get_wtime(); // Record start time

 int graph[V][V] = {{0, 2, 0, 6, 0},
 {2, 0, 3, 8, 5},
 {0, 3, 0, 0, 7},
 {6, 8, 0, 0, 9},
 {0, 5, 7, 9, 0}};
// Print the solution in serial printf("Serial Execution:\n");
 double serial_start_time = omp_get_wtime();primMST(graph);
 double serial_execution_time = omp_get_wtime() -serial_start_time; printf("Serial Execution
Time: %f seconds\n", serial_execution_time);
 printf("CPU Usage: %f\n", getCPUUsage()); printf("Memory Usage: %ld KB\n",
getMemoryUsage());
 // Print the solution in parallel
 printf("\nParallel Execution:\n"); double parallel_start_time = omp_get_wtime();
 primMST(graph); double parallel_execution_time = omp_get_wtime() - parallel_start_time;
 printf("Parallel Execution Time: %f seconds\n", parallel_execution_time); printf("CPU
Usage: %f\n", getCPUUsage());
 printf("Memory Usage: %ld KB\n", getMemoryUsage());
 // Record and print the total execution time of your program double execution_time =
omp_get_wtime() - start_time;
 printf("\nTotal Execution Time: %f seconds\n", execution_time);
 return 0; }
