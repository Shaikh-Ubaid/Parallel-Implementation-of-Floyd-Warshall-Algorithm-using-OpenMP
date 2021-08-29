#include <stdio.h> /* for using printf */
#include <chrono> /* for measuring time taken */
#include <omp.h> /* for using openmp compiler directives */
#include <iostream> /* for using cin and cout */
#include <vector> /* for using vector */
#include <cstdlib> /* for using system in visualize graph function */
#include <fstream>  /* for using file output operations */
#include <time.h> /* for using time(0) for seeding rand() function */

#define INF 10000000000000000  /* setting INFINITY = 10 ^ 16 */ 

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
        #pragma omp parallel for collapse(2)    
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

/* a helper function, which if flag is true returns "Yes" else returns "No" */
const char* yesOrNo(bool flag){
    return (flag)? "Yes" : "No";
}

int main(){
    vector<int> threads_list;
    vector<double> speedup_list;
    // vector<lli> serial_time_list;
    // vector<lli> parallel_time_list;

    for(int threadCnt = 1; threadCnt <= 20; threadCnt++){
        omp_set_num_threads(threadCnt);

        srand(time(0));
        int v = 500;
        int e = (v * (v - 1)) / 2;

        vector<vector<lli>> graph(v, vector<lli>(v, INF));
        for(int i = 0; i < e; ++i){
            int a = rand() % v;
            int b = rand() % v;
            graph[a][b] = rand() % (2 * v);
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
        
        /* printing other helpful data */
        if(shortest_paths_serial != shortest_paths_parallel){
            printf("\n\nParallelDist = SerialDist: %s\n", yesOrNo(shortest_paths_serial == shortest_paths_parallel));
            printf("\nNegative Weight Cycle:     %s\n\n", yesOrNo(checkNegativeWeightCycle(shortest_paths_serial)));
            break;
        }

        /* computing the SpeedUp = serial_time/parallel_time */
        double speed_up = (double(duration_serial.count()))/ duration_parallel.count();

        threads_list.push_back(threadCnt);
        speedup_list.push_back(speed_up);
        // serial_time_list.push_back(duration_serial.count() / 1000);
        // parallel_time_list.push_back(duration_parallel.count() / 1000);
    }

    printf("\n\nno_of_threads = \n");
    for(auto i:threads_list){
        printf("%d, ", i);
    }
    printf("\n\n");

    printf("speedup = \n");
    for(auto i:speedup_list){
        printf("%f, ", i);
    }
    printf("\n\n");

    // printf("serial_time = \n");
    // for(auto i:serial_time_list){
    //     printf("%lli, ", i);
    // }
    // printf("\n\n");

    // printf("parallel_time = \n");
    // for(auto i:parallel_time_list){
    //     printf("%lli, ", i);
    // }
    // printf("\n\n");

    return 0;
}