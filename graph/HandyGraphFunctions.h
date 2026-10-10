#ifndef GRAPH_FUNCS
#define GRAPH_FUNCS
//-----------------------------------------------------------------------------
//
//  name:   HandyGraphFunctions.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:   As the name implies, some useful functions you can use with your
//          graphs.

//          For the function templates, make sure your graph interface complies
//          with the SparseGraph class
//-----------------------------------------------------------------------------
#include <iostream>

#include "Cgdi.h"
#include "utils.h"
#include "Stream_Utility_Functions.h"
#include "GraphAlgorithms.h"
#include "AStarHeuristicPolicies.h"





//--------------------------- validNeighbour -----------------------------
//
//  returns true if x,y is a valid position in the map
//------------------------------------------------------------------------
bool validNeighbour(int x, int y, int numCellsX, int numCellsY)
{
  return !((x < 0) || (x >= numCellsX) || (y < 0) || (y >= numCellsY));
}

//------------ graphHelperAddAllNeighboursToGridNode ------------------
//
//  use to add he eight neighboring edges of a graph node that
//  is positioned in a grid layout
//------------------------------------------------------------------------
template <class graphType>
void graphHelperAddAllNeighboursToGridNode(graphType& graph,
                                            int         row,
                                            int         col,
                                            int         numCellsX,
                                            int         numCellsY)
{
  for (int i=-1; i<2; ++i)
  {
    for (int j=-1; j<2; ++j)
    {
      int nodeX = col+j;
      int nodeY = row+i;

      //skip if equal to this node
      if ( (i == 0) && (j==0) ) continue;

      //check to see if this is a valid neighbour
      if (validNeighbour(nodeX, nodeY, numCellsX, numCellsY))
      {
        //calculate the distance to this node
        Vector2D posNode      = graph.getNode(row*numCellsX+col).pos();
        Vector2D posNeighbour = graph.getNode(nodeY*numCellsX+nodeX).pos();

        double dist = posNode.distance(posNeighbour);

        //this neighbour is okay so it can be added
        typename graphType::EdgeType newEdge(row*numCellsX+col,
                                     nodeY*numCellsX+nodeX,
                                     dist);
        graph.addEdge(newEdge);

        //if graph is not a diagraph then an edge needs to be added going
        //in the other direction
        if (!graph.isDigraph())
        {
          typename graphType::EdgeType newEdge(nodeY*numCellsX+nodeX,
                                       row*numCellsX+col,
                                       dist);
          graph.addEdge(newEdge);
        }
      }
    }
  }
}


//--------------------------- graphHelperCreateGrid --------------------------
//
//  creates a graph based on a grid layout. This function requires the
//  dimensions of the environment and the number of cells required horizontally
//  and vertically
//-----------------------------------------------------------------------------
template <class graphType>
void graphHelperCreateGrid(graphType& graph,
                             int cySize,
                             int cxSize,
                             int numCellsY,
                             int numCellsX)
{
  //need some temporaries to help calculate each node center
  double cellWidth  = (double)cySize / (double)numCellsX;
  double cellHeight = (double)cxSize / (double)numCellsY;

  double midX = cellWidth/2;
  double midY = cellHeight/2;


  //first create all the nodes
  for (int row=0; row<numCellsY; ++row)
  {
    for (int col=0; col<numCellsX; ++col)
    {
      graph.addNode(NavGraphNode<>(graph.getNextFreeNodeIndex(),
                                   Vector2D(midX + (col*cellWidth),
                                   midY + (row*cellHeight))));

    }
  }
  //now to calculate the edges. (A position in a 2d array [x][y] is the
  //same as [y*numCellsX + x] in a 1d array). Each cell has up to eight
  //neighbours.
  for (int row=0; row<numCellsY; ++row)
  {
    for (int col=0; col<numCellsX; ++col)
    {
      graphHelperAddAllNeighboursToGridNode(graph, row, col, numCellsX, numCellsY);
    }
  }
}


//--------------------------- graphHelperDrawUsingGdi ------------------------
//
//  draws a graph using the GDI
//-----------------------------------------------------------------------------
template <class graphType>
void graphHelperDrawUsingGdi(const graphType& graph, int color, bool drawNodeIds = false)
{

  //just return if the graph has no nodes
  if (graph.numNodes() == 0) return;

  gdi->setPenColor(color);

  //draw the nodes
  typename graphType::ConstNodeIterator nodeItr(graph);
  for (const typename graphType::NodeType* pN=nodeItr.begin();
      !nodeItr.end();
       pN=nodeItr.next())
  {
    gdi->circle(pN->pos(), 2);

    if (drawNodeIds)
    {
      gdi->textColor(200,200,200);
      gdi->textAtPos((int)pN->pos().x+5, (int)pN->pos().y-5, ttos(pN->index()));
    }

    typename graphType::ConstEdgeIterator edgeItr(graph, pN->index());
    for (const typename graphType::EdgeType* pE=edgeItr.begin();
        !edgeItr.end();
        pE=edgeItr.next())
    {
      gdi->line(pN->pos(), graph.getNode(pE->to()).pos());
    }
  }
}


//--------------------------- weightNavGraphNodeEdges -------------------------
//
//  Given a cost value and an index to a valid node this function examines
//  all a node's edges, calculates their length, and multiplies
//  the value with the weight. Useful for setting terrain costs.
//------------------------------------------------------------------------
template <class graphType>
void weightNavGraphNodeEdges(graphType& graph, int node, double weight)
{
  //make sure the node is present
  assert(node < graph.numNodes());

  //set the cost for each edge
  typename graphType::ConstEdgeIterator constEdgeItr(graph, node);
  for (const typename graphType::EdgeType* pE=constEdgeItr.begin();
       !constEdgeItr.end();
       pE=constEdgeItr.next())
  {
    //calculate the distance between nodes
    double dist = vec2DDistance(graph.getNode(pE->from()).pos(),
                               graph.getNode(pE->to()).pos());

    //set the cost of this edge
    graph.setEdgeCost(pE->from(), pE->to(), dist * weight);

    //if not a digraph, set the cost of the parallel edge to be the same
    if (!graph.isDigraph())
    {
      graph.setEdgeCost(pE->to(), pE->from(), dist * weight);
    }
  }
}


//----------------------- createAllPairsTable ---------------------------------
//
// creates a lookup table encoding the shortest path info between each node
// in a graph to every other
//-----------------------------------------------------------------------------
template <class graphType>
std::vector<std::vector<int> > createAllPairsTable(const graphType& g)
{
  enum {noPath = -1};

  std::vector<int> row(g.numNodes(), noPath);

  std::vector<std::vector<int> > shortestPaths(g.numNodes(), row);

  for (int source=0; source<g.numNodes(); ++source)
  {
    //calculate the SPT for this node
    GraphSearchDijkstra<graphType> search(g, source);

    std::vector<const typename graphType::EdgeType*> spt = search.getSpt();

    //now we have the SPT it's easy to work backwards through it to find
    //the shortest paths from each node to this source node
    for (int target = 0; target<g.numNodes(); ++target)
    {
      //if the source node is the same as the target just set to target
      if (source == target)
      {
        shortestPaths[source][target] = target;
      }

      else
      {
        int nd = target;

        while ((nd != source) && (spt[nd] != 0))
        {
          shortestPaths[spt[nd]->from()][target]= nd;

          nd = spt[nd]->from();
        }
      }
    }//next target node
  }//next source node

  return shortestPaths;
}


//----------------------- createAllPairsCostsTable -------------------------------
//
//  creates a lookup table of the cost associated from traveling from one
//  node to every other
//-----------------------------------------------------------------------------
template <class graphType>
std::vector<std::vector<double> > createAllPairsCostsTable(const graphType& g)
{
  //create a two dimensional vector
  std::vector<double> row(g.numNodes(), 0.0);
  std::vector<std::vector<double> > pathCosts(g.numNodes(), row);

  for (int source=0; source<g.numNodes(); ++source)
  {
    //do the search
    GraphSearchDijkstra<graphType> search(g, source);

    //iterate through every node in the graph and grab the cost to travel to
    //that node
    for (int target = 0; target<g.numNodes(); ++target)
    {
      if (source != target)
      {
        pathCosts[source][target]= search.getCostToNode(target);
      }
    }//next target node

  }//next source node

  return pathCosts;
}

//---------------------- calculateAverageGraphEdgeLength ----------------------
//
//  determines the average length of the edges in a navgraph (using the
//  distance between the source & target node positions (not the cost of the
//  edge as represented in the graph, which may account for all sorts of
//  other factors such as terrain type, gradients etc)
//------------------------------------------------------------------------------
template <class graphType>
double calculateAverageGraphEdgeLength(const graphType& g)
{
  double totalLength = 0;
  int numEdgesCounted = 0;

  typename graphType::ConstNodeIterator nodeItr(g);
  const typename graphType::NodeType* pN;
  for (pN = nodeItr.begin(); !nodeItr.end(); pN=nodeItr.next())
  {
    typename graphType::ConstEdgeIterator edgeItr(g, pN->index());
    for (const typename graphType::EdgeType* pE = edgeItr.begin(); !edgeItr.end(); pE=edgeItr.next())
    {
      //increment edge counter
      ++numEdgesCounted;

      //add length of edge to total length
      totalLength += vec2DDistance(g.getNode(pE->from()).pos(), g.getNode(pE->to()).pos());
    }
  }

  return totalLength / (double)numEdgesCounted;
}

//----------------------------- getCostliestGraphEdge -------------------
//
//  returns the cost of the costliest edge in the graph
//-----------------------------------------------------------------------------
template <class graphType>
double getCostliestGraphEdge(const graphType& g)
{
  double greatest = minDouble;

  typename graphType::ConstNodeIterator nodeItr(g);
  const typename graphType::NodeType* pN;
  for (pN = nodeItr.begin(); !nodeItr.end(); pN=nodeItr.next())
  {
    typename graphType::ConstEdgeIterator edgeItr(g, pN->index());
    for (const typename graphType::EdgeType* pE = edgeItr.begin(); !edgeItr.end(); pE=edgeItr.next())
    {
      if (pE->cost() > greatest)greatest = pE->cost();
    }
  }

  return greatest;
}

#endif
