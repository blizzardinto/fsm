#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H
#pragma warning (disable:4786)

//------------------------------------------------------------------------
//
//  name:   GraphSearches.h
//
//  Desc:   classes to implement graph algorithms, including dfs, bfs,
//          Dijkstra's, A*, Prims etc. (based upon the code created
//          by Robert Sedgewick in his book "Algorithms in C++")
//
//          Any graphs passed to these functions must conform to the
//          same interface used by the SparseGraph
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <list>
#include <queue>
#include <stack>

#include "SparseGraph.h"
#include "PriorityQueue.h"


//----------------------------- GraphSearchDfs -------------------------------
//
//  class to implement a depth first search.
//-----------------------------------------------------------------------------
template<class graphType>
class GraphSearchDfs
{
private:

  //to aid legibility
  enum {visited, unvisited, noParentAssigned};

  //create a typedef for the edge and node types used by the graph
  typedef typename graphType::EdgeType Edge;
  typedef typename graphType::NodeType Node;

private:

  //a reference to the graph to be searched
  const graphType& mGraph;

  //this records the indexes of all the nodes that are visited as the
  //search progresses
  std::vector<int>  mVisited;

  //this holds the route taken to the target. Given a node index, the value
  //at that index is the node's parent. ie if the path to the target is
  //3-8-27, then mRoute[8] will hold 3 and mRoute[27] will hold 8.
  std::vector<int>  mRoute;

  //As the search progresses, this will hold all the edges the algorithm has
  //examined. THIS IS NOT NECESSARY FOR THE SEARCH, IT IS HERE PURELY
  //TO PROVIDE THE USER WITH SOME VISUAL FEEDBACK
  std::vector<const Edge*>  mSpanningTree;

  //the source and target node indices
  int               mSource,
                    mTarget;

  //true if a path to the target has been found
  bool              mFound;


  //this method performs the dfs search
  bool search();

public:

  GraphSearchDfs(const graphType& graph,
                  int          source,
                  int          target = -1 ):

                                      mGraph(graph),
                                      mSource(source),
                                      mTarget(target),
                                      mFound(false),
                                      mVisited(mGraph.numNodes(), unvisited),
                                      mRoute(mGraph.numNodes(), noParentAssigned)

  {
    mFound = search();
  }


  //returns a vector containing pointers to all the edges the search has examined
  std::vector<const Edge*> getSearchTree()const{return mSpanningTree;}

  //returns true if the target node has been located
  bool   found()const{return mFound;}

  //returns a vector of node indexes that comprise the shortest path
  //from the source to the target
  std::list<int> getPathToTarget()const;
};

//-----------------------------------------------------------------------------
template <class graphType>
bool GraphSearchDfs<graphType>::search()
{
  //create a std stack of edges
  std::stack<const Edge*> stack;

  //create a dummy edge and put on the stack
  Edge dummy(mSource, mSource, 0);

  stack.push(&dummy);

  //while there are edges in the stack keep searching
  while (!stack.empty())
  {
    //grab the next edge
    const Edge* next = stack.top();

    //remove the edge from the stack
    stack.pop();

    //make a note of the parent of the node this edge points to
    mRoute[next->to()] = next->from();

    //put it on the tree. (making sure the dummy edge is not placed on the tree)
    if (next != &dummy)
    {
      mSpanningTree.push_back(next);
    }

    //and mark it visited
    mVisited[next->to()] = visited;

    //if the target has been found the method can return success
    if (next->to() == mTarget)
    {
      return true;
    }

    //push the edges leading from the node this edge points to onto
    //the stack (provided the edge does not point to a previously
    //visited node)
    typename graphType::ConstEdgeIterator constEdgeItr(mGraph, next->to());

    for (const Edge* pE=constEdgeItr.begin();
        !constEdgeItr.end();
         pE=constEdgeItr.next())
    {
      if (mVisited[pE->to()] == unvisited)
      {
        stack.push(pE);
      }
    }
  }

  //no path to target
  return false;
}

//-----------------------------------------------------------------------------
template <class graphType>
std::list<int> GraphSearchDfs<graphType>::getPathToTarget()const
{
  std::list<int> path;

  //just return an empty path if no path to target found or if
  //no target has been specified
  if (!mFound || mTarget<0) return path;

  int nd = mTarget;

  path.push_front(nd);

  while (nd != mSource)
  {
    nd = mRoute[nd];

    path.push_front(nd);
  }

  return path;
}



//----------------------------- GraphSearchBfs -------------------------------
//
//-----------------------------------------------------------------------------
template<class graphType>
class GraphSearchBfs
{
private:

  //to aid legibility
  enum {visited, unvisited, noParentAssigned};

  //create a typedef for the edge type used by the graph
  typedef typename graphType::EdgeType Edge;

private:

  //a reference to the graph to be searched
  const graphType&      mGraph;

  //this records the indexes of all the nodes that are visited as the
  //search progresses
  std::vector<int>  mVisited;

  //this holds the route taken to the target. Given a node index, the value
  //at that index is the node's parent. ie if the path to the target is
  //3-8-27, then mRoute[8] will hold 3 and mRoute[27] will hold 8.
  std::vector<int>  mRoute;

  //the source and target node indices
  int               mSource,
                    mTarget;

  //true if a path to the target has been found
  bool              mFound;

  //As the search progresses, this will hold all the edges the algorithm has
  //examined. THIS IS NOT NECESSARY FOR THE SEARCH, IT IS HERE PURELY
  //TO PROVIDE THE USER WITH SOME VISUAL FEEDBACK
  std::vector<const Edge*>  mSpanningTree;


  //the bfs algorithm is very similar to the dfs except that it uses a
  //FIFO queue instead of a stack.
  bool search();


public:

  GraphSearchBfs(const graphType& graph,
             int          source,
             int          target = -1 ):mGraph(graph),
                                        mSource(source),
                                        mTarget(target),
                                        mFound(false),
                                        mVisited(mGraph.numNodes(), unvisited),
                                        mRoute(mGraph.numNodes(), noParentAssigned)

  {
    mFound = search();
  }

  bool   found()const{return mFound;}

  //returns a vector containing pointers to all the edges the search has examined
  std::vector<const Edge*> getSearchTree()const{return mSpanningTree;}

  //returns a vector of node indexes that comprise the shortest path
  //from the source to the target
  std::list<int> getPathToTarget()const;
};

//-----------------------------------------------------------------------------

template <class graphType>
bool GraphSearchBfs<graphType>::search()
{
  //create a std queue of edges
  std::queue<const Edge*> queue;

  const Edge dummy(mSource, mSource, 0);

  //create a dummy edge and put on the queue
  queue.push(&dummy);

  //mark the source node as visited
  mVisited[mSource] = visited;

  //while there are edges in the queue keep searching
  while (!queue.empty())
  {
    //grab the next edge
    const Edge* next = queue.front();

    queue.pop();

    //mark the parent of this node
    mRoute[next->to()] = next->from();

    //put it on the tree. (making sure the dummy edge is not placed on the tree)
    if (next != &dummy)
    {
      mSpanningTree.push_back(next);
    }

    //exit if the target has been found
    if (next->to() == mTarget)
    {
      return true;
    }

    //push the edges leading from the node at the end of this edge
    //onto the queue
    typename graphType::ConstEdgeIterator constEdgeItr(mGraph, next->to());

    for (const Edge* pE=constEdgeItr.begin();
        !constEdgeItr.end();
         pE=constEdgeItr.next())
    {
      //if the node hasn't already been visited we can push the
      //edge onto the queue
      if (mVisited[pE->to()] == unvisited)
      {
        queue.push(pE);

        //and mark it visited
        mVisited[pE->to()] = visited;
      }
    }
  }

  //no path to target
  return false;
}


//-----------------------------------------------------------------------------
template <class graphType>
std::list<int> GraphSearchBfs<graphType>::getPathToTarget()const
{
  std::list<int> path;

  //just return an empty path if no path to target found or if
  //no target has been specified
  if (!mFound || mTarget<0) return path;

  int nd = mTarget;

  path.push_front(nd);

  while (nd != mSource)
  {
    nd = mRoute[nd];

    path.push_front(nd);
  }

  return path;
}



//----------------------- GraphSearchDijkstra --------------------------------
//
//  Given a graph, source and optional target this class solves for
//  single source shortest paths (without a target being specified) or
//  shortest path from source to target.
//
//  The algorithm used is a priority queue implementation of Dijkstra's.
//  note how similar this is to the algorithm used in GraphMinSpanningTree.
//  The main difference is in the calculation of the priority in the line:
//
//  double newCost = mCostToThisNode[best] + pE->cost;
//------------------------------------------------------------------------
template <class graphType>
class GraphSearchDijkstra
{
private:

  //create a typedef for the edge type used by the graph
  typedef typename graphType::EdgeType Edge;

private:

  const graphType&             mGraph;

  //this vector contains the edges that comprise the shortest path tree -
  //a directed subtree of the graph that encapsulates the best paths from
  //every node on the SPT to the source node.
  std::vector<const Edge*>      mShortestPathTree;

  //this is indexed into by node index and holds the total cost of the best
  //path found so far to the given node. For example, mCostToThisNode[5]
  //will hold the total cost of all the edges that comprise the best path
  //to node 5, found so far in the search (if node 5 is present and has
  //been visited)
  std::vector<double>            mCostToThisNode;

  //this is an indexed (by node) vector of 'parent' edges leading to nodes
  //connected to the SPT but that have not been added to the SPT yet. This is
  //a little like the stack or queue used in BST and DST searches.
  std::vector<const Edge*>     mSearchFrontier;

  int                           mSource;
  int                           mTarget;

  void search();

public:

  GraphSearchDijkstra(const graphType   &graph,
                       int           source,
                       int           target = -1):mGraph(graph),
                                       mShortestPathTree(graph.numNodes()),
                                       mSearchFrontier(graph.numNodes()),
                                       mCostToThisNode(graph.numNodes()),
                                       mSource(source),
                                       mTarget(target)
  {
    search();
  }

  //returns the vector of edges that defines the SPT. If a target was given
  //in the function Object() { [native code] } then this will be an SPT comprising of all the nodes
  //examined before the target was found, else it will contain all the nodes
  //in the graph.
  std::vector<const Edge*> getSpt()const{return mShortestPathTree;}

  //returns a vector of node indexes that comprise the shortest path
  //from the source to the target. It calculates the path by working
  //backwards through the SPT from the target node.
  std::list<int> getPathToTarget()const;

  //returns the total cost to the target
  double getCostToTarget()const{return mCostToThisNode[mTarget];}

  //returns the total cost to the given node
  double getCostToNode(unsigned int nd)const{return mCostToThisNode[nd];}
};


//-----------------------------------------------------------------------------
template <class graphType>
void GraphSearchDijkstra<graphType>::search()
{
  //create an indexed priority queue that sorts smallest to largest
  //(front to back).Note that the maximum number of elements the iPQ
  //may contain is N. This is because no node can be represented on the
  //queue more than once.
  IndexedPriorityQLow<double> pq(mCostToThisNode, mGraph.numNodes());

  //put the source node on the queue
  pq.insert(mSource);

  //while the queue is not empty
  while(!pq.empty())
  {
    //get lowest cost node from the queue. Don't forget, the return value
    //is a *node index*, not the node itself. This node is the node not already
    //on the SPT that is the closest to the source node
    int nextClosestNode = pq.pop();

    //move this edge from the frontier to the shortest path tree
    mShortestPathTree[nextClosestNode] = mSearchFrontier[nextClosestNode];

    //if the target has been found exit
    if (nextClosestNode == mTarget) return;

    //now to relax the edges.
    typename graphType::ConstEdgeIterator constEdgeItr(mGraph, nextClosestNode);

    //for each edge connected to the next closest node
    for (const Edge* pE=constEdgeItr.begin();
        !constEdgeItr.end();
        pE=constEdgeItr.next())
    {
      //the total cost to the node this edge points to is the cost to the
      //current node plus the cost of the edge connecting them.
      double newCost = mCostToThisNode[nextClosestNode] + pE->cost();

      //if this edge has never been on the frontier make a note of the cost
      //to get to the node it points to, then add the edge to the frontier
      //and the destination node to the PQ.
      if (mSearchFrontier[pE->to()] == 0)
      {
        mCostToThisNode[pE->to()] = newCost;

        pq.insert(pE->to());

        mSearchFrontier[pE->to()] = pE;
      }

      //else test to see if the cost to reach the destination node via the
      //current node is cheaper than the cheapest cost found so far. If
      //this path is cheaper, we assign the new cost to the destination
      //node, update its entry in the PQ to reflect the change and add the
      //edge to the frontier
      else if ( (newCost < mCostToThisNode[pE->to()]) &&
                (mShortestPathTree[pE->to()] == 0) )
      {
        mCostToThisNode[pE->to()] = newCost;

        //because the cost is less than it was previously, the PQ must be
        //re-sorted to account for this.
        pq.changePriority(pE->to());

        mSearchFrontier[pE->to()] = pE;
      }
    }
  }
}

//-----------------------------------------------------------------------------
template <class graphType>
std::list<int> GraphSearchDijkstra<graphType>::getPathToTarget()const
{
  std::list<int> path;

  //just return an empty path if no target or no path found
  if (mTarget < 0)  return path;

  int nd = mTarget;

  path.push_front(nd);

  while ((nd != mSource) && (mShortestPathTree[nd] != 0))
  {
    nd = mShortestPathTree[nd]->from();

    path.push_front(nd);
  }

  return path;
}

//------------------------------- GraphSearchAStar --------------------------
//
//  this searchs a graph using the distance between the target node and the
//  currently considered node as a heuristic.
//
//  This search is more commonly known as A* (pronounced Ay-Star)
//-----------------------------------------------------------------------------
template <class graphType, class heuristic>
class GraphSearchAStar
{
private:

  //create a typedef for the edge type used by the graph
  typedef typename graphType::EdgeType Edge;

private:

  const graphType&              mGraph;

  //indexed into my node. Contains the 'real' accumulative cost to that node
  std::vector<double>             mGCosts;

  //indexed into by node. Contains the cost from adding mGCosts[n] to
  //the heuristic cost from n to the target node. This is the vector the
  //iPQ indexes into.
  std::vector<double>             mFCosts;

  std::vector<const Edge*>       mShortestPathTree;
  std::vector<const Edge*>       mSearchFrontier;

  int                            mSource;
  int                            mTarget;

  //the A* search algorithm
  void search();

public:

  GraphSearchAStar(graphType &graph,
                    int   source,
                    int   target):mGraph(graph),
                                  mShortestPathTree(graph.numNodes()),
                                  mSearchFrontier(graph.numNodes()),
                                  mGCosts(graph.numNodes(), 0.0),
                                  mFCosts(graph.numNodes(), 0.0),
                                  mSource(source),
                                  mTarget(target)
  {
    search();
  }

  //returns the vector of edges that the algorithm has examined
  std::vector<const Edge*> getSpt()const{return mShortestPathTree;}

  //returns a vector of node indexes that comprise the shortest path
  //from the source to the target
  std::list<int> getPathToTarget()const;

  //returns the total cost to the target
  double getCostToTarget()const{return mGCosts[mTarget];}
};

//-----------------------------------------------------------------------------
template <class graphType, class heuristic>
void GraphSearchAStar<graphType, heuristic>::search()
{
  //create an indexed priority queue of nodes. The nodes with the
  //lowest overall F cost (G+H) are positioned at the front.
  IndexedPriorityQLow<double> pq(mFCosts, mGraph.numNodes());

  //put the source node on the queue
  pq.insert(mSource);

  //while the queue is not empty
  while(!pq.empty())
  {
    //get lowest cost node from the queue
    int nextClosestNode = pq.pop();

    //move this node from the frontier to the spanning tree
    mShortestPathTree[nextClosestNode] = mSearchFrontier[nextClosestNode];

    //if the target has been found exit
    if (nextClosestNode == mTarget) return;

    //now to test all the edges attached to this node
    typename graphType::ConstEdgeIterator constEdgeItr(mGraph, nextClosestNode);

    for (const Edge* pE=constEdgeItr.begin();
        !constEdgeItr.end();
         pE=constEdgeItr.next())
    {
      //calculate the heuristic cost from this node to the target (H)
      double hCost = heuristic::calculate(mGraph, mTarget, pE->to());

      //calculate the 'real' cost to this node from the source (G)
      double gCost = mGCosts[nextClosestNode] + pE->cost();

      //if the node has not been added to the frontier, add it and update
      //the G and F costs
      if (mSearchFrontier[pE->to()] == NULL)
      {
        mFCosts[pE->to()] = gCost + hCost;
        mGCosts[pE->to()] = gCost;

        pq.insert(pE->to());

        mSearchFrontier[pE->to()] = pE;
      }

      //if this node is already on the frontier but the cost to get here
      //is cheaper than has been found previously, update the node
      //costs and frontier accordingly.
      else if ((gCost < mGCosts[pE->to()]) && (mShortestPathTree[pE->to()]==NULL))
      {
        mFCosts[pE->to()] = gCost + hCost;
        mGCosts[pE->to()] = gCost;

        pq.changePriority(pE->to());

        mSearchFrontier[pE->to()] = pE;
      }
    }
  }
}

//-----------------------------------------------------------------------------
template <class graphType, class heuristic>
std::list<int> GraphSearchAStar<graphType, heuristic>::getPathToTarget()const
{
  std::list<int> path;

  //just return an empty path if no target or no path found
  if (mTarget < 0)  return path;

  int nd = mTarget;

  path.push_front(nd);

  while ((nd != mSource) && (mShortestPathTree[nd] != 0))
  {
    nd = mShortestPathTree[nd]->from();

    path.push_front(nd);
  }

  return path;
}



//---------------------- GraphMinSpanningTree --------------------------------
//
//  given a graph and a source node you can use this class to calculate
//  the minimum spanning tree. If no source node is specified then the
//  algorithm will calculate a spanning forest starting from node 1
//
//  It uses a priority first queue implementation of Prims algorithm
//------------------------------------------------------------------------
template <class graphType>
class GraphMinSpanningTree
{
private:

  //create a typedef for the edge type used by the graph
  typedef typename graphType::EdgeType Edge;

  const graphType&              mGraph;

  std::vector<double>            mCostToThisNode;

  std::vector<const Edge*>  mSpanningTree;
  std::vector<const Edge*>  mFringe;

  void search(const int source)
  {
    //create a priority queue
    IndexedPriorityQLow<double> pq(mCostToThisNode, mGraph.numNodes());

    //put the source node on the queue
    pq.insert(source);

    //while the queue is not empty
    while(!pq.empty())
    {
      //get lowest cost edge from the queue
      int best = pq.pop();

      //move this edge from the fringe to the spanning tree
      mSpanningTree[best] = mFringe[best];

      //now to test the edges attached to this node
      typename graphType::ConstEdgeIterator constEdgeItr(mGraph, best);

      for (const Edge* pE=constEdgeItr.begin(); !constEdgeItr.end(); pE=constEdgeItr.next())
      {
        double priority = pE->cost();

        if (mFringe[pE->to()] == 0)
        {
          mCostToThisNode[pE->to()] = priority;

          pq.insert(pE->to());

          mFringe[pE->to()] = pE;
        }

        else if ((priority<mCostToThisNode[pE->to()]) && (mSpanningTree[pE->to()]==0))
        {
          mCostToThisNode[pE->to()] = priority;

          pq.changePriority(pE->to());

          mFringe[pE->to()] = pE;
        }
      }
    }
  }

public:

  GraphMinSpanningTree(graphType &g,
                   int   source = -1):mGraph(g),
                                   mSpanningTree(g.numNodes()),
                                   mFringe(g.numNodes()),
                                   mCostToThisNode(g.numNodes(), -1)
  {
    if (source < 0)
    {
      for (int nd=0; nd<g.numNodes(); ++nd)
      {
        if (mSpanningTree[nd] == 0)
        {
          search(nd);
        }
      }
    }

    else
    {
      search(source);
    }
  }

  std::vector<const Edge*> getSpanningTree()const{return mSpanningTree;}

};



#endif