#include <stdio.h> /* for using printf */
#include <chrono> /* for measuring time taken */
#include <omp.h> /* for using openmp compiler directives */
#include <iostream> /* for using cin */
#include <vector> /* for using vector */
#include <cstdlib> /* for using system in visualize graph function */
#include <fstream>  /* for using file output operations */

#define INF 10000000000000000  /* setting INFINITY = 10 ^ 16 */ 
#define NUM_THREADS 8

using namespace std;
using namespace std::chrono;

typedef long long int lli; /* redefining long long int to be lli for ease of use/understanding */

/* function to print the adjacency matrix */
void printMatrix(vector<vector<lli>> matrix){
    printf("\n\t+-");
    for(int j = 0; j < matrix.size(); ++j)
        printf("------+");
    printf("\n");
    for(int i = 0; i < matrix.size(); ++i){
        printf("\t| ");
        for(int j = 0; j < matrix.size(); ++j)
            if(matrix[i][j] == INF)
                printf("%-5s |", "INF");
            else  
                printf("%-5lli |", matrix[i][j]);
        printf("\n");
        printf("\t+-");
        for(int j = 0; j < matrix.size(); ++j)
            printf("------+");
        printf("\n");
    }
    printf("\n\n");
}

/* function for visualizing entered graph, it creates a File.txt file in the same directory */
void visualize_graph(vector<vector<lli>> graph)
{
    int n = graph.size();   /* for storing number of edges in the graph */
    int a; int b;    /* for storing the edges */
    system("> File.txt");   /* for cleaning the contents of File.txt */
    fstream fout;    /* Object of fstream class */
    fout.open("File.txt");    /* opens File.txt */
    fout<<"digraph G {\n";      /* writes the dot language code in File.txt */
    for(int i = 0;i < n;++i) /* a loop which writes all the edges information to the dot file */
    {
        a=i;
        for(int j = 0; j < n;j++)
        {
            b=j;
            if(graph[a][b]!=INF and a!=b)
            {
            fout << "\t" << a << " -> " << b << " [label=" << graph[a][b] << ",weight=" << graph[a][b] << "]" << ";\n";
            }
        }
    }
    fout<<"}\n";   /* ending curly brackets in dot language */
    fout.close();   /* closing File.txt file */
    system("dot -Tpng File.txt -o diagram.png");    /* running command in terminal to generate diagram of tree in png format */
    system("shotwell diagram.png");
    return;
}

/* returns true if there is a negative weight cycle present in the graph, else returns fales */
bool checkNegativeWeightCycle(vector<vector<lli>> graph){
    for(int i = 0; i < graph.size(); ++i){
        if(graph[i][i] < 0)
            return true;
    }
    return false;
}

/* serial implementation of all pairs shortest paths */
vector<vector<lli>> allPairShortestPaths_serial(vector<vector<lli>> graph){
    int v = graph.size();
    vector<vector<lli>> dist = graph;
    

    for (int k = 0; k < v; k++)
    {
        /* select all vertices as source one by one */
        for (int i = 0; i < v; i++)
        {
            /* select all vertices as destination for the above selected source */
            for (int j = 0; j < v; j++)
            {
                /* If k is on the shortest path from i to j, then update dist[i][j] */
                if (dist[i][k] != INF and dist[k][j] != INF and dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    /* return the final distance found/computed */
    return dist;
}

/* parallel implementation of all pairs shortest paths */
vector<vector<lli>> allPairShortestPaths_parallel(vector<vector<lli>> graph){
    int v = graph.size();
    vector<vector<lli>> dist = graph;

    for (int k = 0; k < v; k++)
    {
        /* parallelizing the two (i and j) for loops */
        #pragma omp parallel for collapse(2) num_threads(NUM_THREADS)      
        /* select all vertices as source one by one */
        for (int i = 0; i < v; i++)
        {
            /* select all vertices as destination for the above selected source */
            for (int j = 0; j < v; j++)
            {
                /* If k is on the shortest path from i to j, then update dist[i][j] */
                if (dist[i][k] != INF and dist[k][j] != INF and dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    /* return the final distances found/computed */
    return dist;
}

/* a helper function, which if flag is true returns "Yes" else returns "No" */
const char* yesOrNo(bool flag){
    return (flag)? "Yes" : "No";
}

int main(){
    printf("Enter the number of vertices in the graph: ");
    int v; cin >> v;
    printf("Enter the number of edges in the graph: ");
    int e; cin >> e;

    vector<vector<lli>> graph(v, vector<lli>(v, INF));
    printf("Enter edges in the following %d lines in the form \"source destination weight\"\n", e);
    for(int i = 0; i < e; ++i){
        int a, b; lli w;
        cin >> a >> b >> w;
        graph[a][b] = w;
    }

    for(int i = 0; i < v; ++i){ /* distance is 0 when source and destination are same */
        graph[i][i] = 0;
    }
    
    /* executing serial all pair shortest path algorithm and computing/calculating its runtime */
    auto start_serial = high_resolution_clock::now();
    vector<vector<lli>> shortest_paths_serial = allPairShortestPaths_serial(graph);
    auto stop_serial = high_resolution_clock::now();
    auto duration_serial = duration_cast<microseconds>(stop_serial - start_serial);
    
    /* executing parallel all pair shortest path algorithm and computing/calculating its runtime */
    auto start_parallel = high_resolution_clock::now();
    vector<vector<lli>> shortest_paths_parallel = allPairShortestPaths_parallel(graph);
    auto stop_parallel = high_resolution_clock::now();
    auto duration_parallel = duration_cast<microseconds>(stop_parallel - start_parallel);
    
    /* printing the computed distances by both the algorithms */
    printf("Distances calculated by the Serial Algorithm are as follows:\n");
    printMatrix(shortest_paths_serial);

    printf("Distances calculated by the Parallel Algorithm are as follows:\n");
    printMatrix(shortest_paths_parallel);

    /* printing other helpful data */
    printf("ParallelDist = SerialDist: %s\n", yesOrNo(shortest_paths_serial == shortest_paths_parallel));
    printf("Negative Weight Cycle:     %s\n\n", yesOrNo(checkNegativeWeightCycle(shortest_paths_serial)));

    /* printing the obtained results in a tabular format */
    printf("+----------------------+----------------------+\n");
    printf("| %-20s | %-20s |\n", " ", "timeTaken (us)");
    printf("+----------------------+----------------------+\n");
    printf("| %-20s | %-20lli |\n", "Serial", (lli)duration_serial.count());
    printf("+----------------------+----------------------+\n");
    printf("| %-20s | %-20lli |\n", "Parallel", (lli)duration_parallel.count());
    printf("+----------------------+----------------------+\n");
    
    /* computing the SpeedUp = serial_time/parallel_time */
    printf("\n\tSpeed Up = %f\n\n", (float(duration_serial.count()))/ duration_parallel.count());
    
    visualize_graph(graph);
    return 0;
}