#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define PADDING 16
#define THRESHOLD 10000000
//#define VERBOSE

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.
void top_down_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances)
{
    int node, start_edge, end_edge, outgoing;
    #pragma omp parallel for private(node, start_edge, end_edge)
    for (int i = 0; i < frontier->count; i++)
    {

        node = frontier->vertices[i];

        start_edge = g->outgoing_starts[node];
        end_edge = (node == g->num_nodes - 1)
                           ? g->num_edges
                           : g->outgoing_starts[node + 1];

        // attempt to add all neighbors to the new frontier
        //int index = new_frontier->count;
        int current, next;
        #pragma omp parallel for private(outgoing)
        for (int neighbor = start_edge; neighbor < end_edge; neighbor++)
        {
            outgoing = g->outgoing_edges[neighbor];
            if (distances[outgoing] == NOT_VISITED_MARKER)
            {
                distances[outgoing] = distances[node] + 1;
                // cout<<"new_frontier->count = "<<new_frontier->count<<endl;
                //int index = new_frontier->count++;
                // cout<<"index = "<<index<<endl;
                // cout<<"new_frontier->count(after) = "<<new_frontier->count<<endl;
                // do{
                //     current = new_frontier->count;
                //     next = current;
                //     new_frontier->vertices[new_frontier->count] = outgoing;
                //     next++;
                // }while
                //#pragma omp critical
                //{
                    new_frontier->vertices[new_frontier->count] = outgoing;
                    new_frontier->count++;
                }
            }
        }
    }
}

void bottom_up_step(
    graph* g,
    vertex_set* frontier,    
    int* distances,
    int iteration) {

    int temp = 0;
    int current, next;
    //#pragma omp parallel for shared(temp) private(current, next)
    #pragma omp parallel for reduction(+:temp)
    for (int i=0; i < g->num_nodes; i++) {                   
        if (frontier->vertices[i] == 0) {
            int start_edge = g->incoming_starts[i];
            int end_edge = (i == g->num_nodes-1)? g->num_edges : g->incoming_starts[i + 1];
            for(int neighbor = start_edge; neighbor < end_edge; neighbor++) {
                int incoming = g->incoming_edges[neighbor];
                if(frontier->vertices[incoming] == iteration) {
                    distances[i] = distances[incoming] + 1;                        
                    // do{
                    //     current = temp;
                    //     next = current;
                    //     next++;
                    // }while(!__sync_bool_compare_and_swap(&temp, current, next));
                    temp++;
                    frontier->vertices[i] = iteration + 1;
                    break;
                }
            }
        }
    }
    
    frontier->count = temp;
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol)
{   
    
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);

        top_down_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
        
    }
    //cout <<"sol 4002 = "<< sol->distances[4002] <<endl;
    
   
    // vertex_set list1;
    
    // vertex_set_init(&list1, graph->num_nodes);
    
    // int iteration = 1;

    // vertex_set* frontier = &list1; 
    
    // memset(frontier->vertices, 0, sizeof(int) * graph->num_nodes);
            
    // // setup frontier with the root node    
    // // just like put the root into queue
    // frontier->vertices[frontier->count++] = 1;

    // // set the root distance with 0
    // // sol->distances[ROOT_NODE_ID] = 0;
    // for (int i=0; i<graph->num_nodes; i++)
    //     sol->distances[i] = 0;

    
    // // printf("!!!!!!!!!!!!!!!!!!!!fro2: %-10d\n", frontier->count);
    // // just like pop the queue
    // while (frontier->count != 0) {
        
    //     frontier->count = 0;
    //     // double start_time = CycleTimer::currentSeconds();
        

    //     bottom_up_step(graph, frontier, sol->distances, iteration);

    //     // double end_time = CycleTimer::currentSeconds();
    //     // printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);

    //     iteration++;

    // }

    // for (int i=1; i<graph->num_nodes; i++)
    //     if (sol->distances[i] == 0)
    //         sol->distances[i]=-1;
}

void bfs_bottom_up(Graph graph, solution *sol)
{
    // For PP students:
    //
    // You will need to implement the "bottom up" BFS here as
    // described in the handout.
    //
    // As a result of your code's execution, sol.distances should be
    // correctly populated for all nodes in the graph.
    //
    // As was done in the top-down case, you may wish to organize your
    // code by creating subroutine bottom_up_step() that is called in
    // each step of the BFS process.


    vertex_set list1;
    
    vertex_set_init(&list1, graph->num_nodes);
    
    int iteration = 1;

    vertex_set* frontier = &list1; 
    
    memset(frontier->vertices, 0, sizeof(int) * graph->num_nodes);
            
    // setup frontier with the root node    
    // just like put the root into queue
    frontier->vertices[frontier->count++] = 1;

    // set the root distance with 0
    // sol->distances[ROOT_NODE_ID] = 0;
    for (int i=0; i<graph->num_nodes; i++)
        sol->distances[i] = 0;

    
    // printf("!!!!!!!!!!!!!!!!!!!!fro2: %-10d\n", frontier->count);
    // just like pop the queue
    while (frontier->count != 0) {
        
        frontier->count = 0;
        // double start_time = CycleTimer::currentSeconds();
        

        bottom_up_step(graph, frontier, sol->distances, iteration);

        // double end_time = CycleTimer::currentSeconds();
        // printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);

        iteration++;

    }

    for (int i=1; i<graph->num_nodes; i++)
        if (sol->distances[i] == 0)
            sol->distances[i]=-1;


}

void bfs_hybrid(Graph graph, solution *sol)
{
    // For PP students:
    //
    // You will need to implement the "hybrid" BFS here as
    // described in the handout.
    
    vertex_set list1;
    
    vertex_set_init(&list1, graph->num_nodes);
    
    int iteration = 1;

    vertex_set* frontier = &list1; 
    
    memset(frontier->vertices, 0, sizeof(int) * graph->num_nodes);
            
    // setup frontier with the root node    
    // just like put the root into queue
    frontier->vertices[frontier->count++] = 1;

    // set the root distance with 0
    // sol->distances[ROOT_NODE_ID] = 0;
    for (int i=0; i<graph->num_nodes; i++)
        sol->distances[i] = 0;

    
    // printf("!!!!!!!!!!!!!!!!!!!!fro2: %-10d\n", frontier->count);
    // just like pop the queue
    while (frontier->count != 0) {
        
        frontier->count = 0;
        // double start_time = CycleTimer::currentSeconds();
        

        bottom_up_step(graph, frontier, sol->distances, iteration);

        // double end_time = CycleTimer::currentSeconds();
        // printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);

        iteration++;

    }

    for (int i=1; i<graph->num_nodes; i++)
        if (sol->distances[i] == 0)
            sol->distances[i]=-1;
}
