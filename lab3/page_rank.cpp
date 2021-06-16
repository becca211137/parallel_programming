#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#include <iostream>
#include <stdio.h>
#include <type_traits>
using namespace std;

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
bool swap_double(double* p, double old, double new_v){
  if (*p != old){
    return false;
  }
  *p = new_v;
  return true;
}
void pageRank(Graph g, double *solution, double damping, double convergence)
{

  // initialize vertex weights to uniform probability. Double
  // precision scores are used to avoid underflow for large graphs
  /*
     For PP students: Implement the page rank algorithm here.  You
     are expected to parallelize the algorithm using openMP.  Your
     solution may need to allocate (and free) temporary arrays.

     Basic page rank pseudocode is provided below to get you started:

     // initialization: see example code above
     score_old[vi] = 1/numNodes;

     while (!converged) {

       // compute score_new[vi] for all nodes vi:
       score_new[vi] = sum over all nodes vj reachable from incoming edges
                          { score_old[vj] / number of edges leaving vj  }
       score_new[vi] = (damping * score_new[vi]) + (1.0-damping) / numNodes;

       score_new[vi] += sum over all nodes v in graph with no outgoing edges
                          { damping * score_old[v] / numNodes }

       // compute how much per-node scores have changed
       // quit once algorithm has converged

       global_diff = sum over all nodes vi { abs(score_new[vi] - score_old[vi]) };
       converged = (global_diff < convergence)
    */
  
  int numNodes = num_nodes(g);
  double *score_old, *score_new, offset, current, next, global_diff=10.f;
  double *diff;
  Graph graph = g;
  bool converge = false;
  double equal_prob = 1.0 / numNodes;
  int numOut = 0;
  int temp;
  int *out_array, *out_size;
  out_array = (int*)malloc(sizeof(int) * numNodes);
  out_size = (int*)malloc(sizeof(int) * numNodes);
  diff = (double*)malloc(sizeof(double) * numNodes);
  score_old = (double*)malloc(sizeof(double) * numNodes);
  score_new = (double*)malloc(sizeof(double) * numNodes);
  //outgoing_size-->out_array
  //#pragma omp parallel for private(temp) reduction(+:numOut)
  for(int i = 0; i < numNodes; ++i){
    solution[i] = equal_prob;
    score_old[i] = equal_prob;
    score_new[i] = 0.0;
    diff[i] = 0.0;
    temp = outgoing_size(g, i); 
    out_size[i] = temp;
    if ( temp == 0){
        out_array[numOut] = i;
        numOut++;
    }
  }
  double total = 10.0;  
  while(global_diff>convergence){
    global_diff = 0.f;
    offset = 0.f;
    // count v in graph with no outgoing edges
    //#pragma omp parallel for reduction(+:offset)
    for(int j = 0; j < numNodes; j++){        
      // offset += score_old[out_array[j]];
      if (outgoing_size(g,j) == 0){
        offset+=score_old[j];
      }
    }
    offset = damping * offset / numNodes;
    //cout<<"offset = "<<offset<<endl;
    
    double old_sum = 0.f;
    //#pragma omp parallel for reduction(+:global_diff) private(old_sum)
    for(int i = 0; i<numNodes; ++i){
      const Vertex* start= incoming_begin(g,i);
      const Vertex* end= incoming_end(g,i);
      old_sum = 0.f;
      // sum over all nodes vj reachable from incoming edges
      //#pragma omp parallel for reduction(+:old_sum)
      for(const Vertex* z = start; z < end; z++){
        old_sum += (double)score_old[*z] / out_size[*z];
      }
      score_new[i] += old_sum;
      score_new[i] = (damping * score_new[i]) + (1-damping) / numNodes;
      score_new[i] += offset;
      global_diff += abs(score_new[i] - score_old[i]);
    }

    //#pragma omp parallel for
    for(int i = 0; i<numNodes; ++i){
      score_old[i] = score_new[i];
      score_new[i] = 0.0;
      diff[i] = 0.0;
    }  
    
  }
  //#pragma omp parallel for
  for(int i = 0; i<numNodes; ++i){
    solution[i] = score_old[i];
  }
}