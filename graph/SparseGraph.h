#ifndef SPARSEGRAPH_H
#define SPARSEGRAPH_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  name:   SparseGraph.h
//
//  Desc:   Graph class using the adjacency list representation.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <list>
#include <cassert>
#include <string>
#include <iostream>


#include "Vector2D.h"
#include "utils.h"
#include "NodeTypeEnumerations.h"



template <class nodeType, class edgeType>
class SparseGraph
{
public:

  //enable easy client access to the edge and node types used in the graph
  typedef edgeType                EdgeType;
  typedef nodeType                NodeType;

  //a couple more typedefs to save my fingers and to help with the formatting
  //of the code on the printed page
  typedef std::vector<nodeType>   NodeVector;
  typedef std::list<edgeType>     EdgeList;
  typedef std::vector<EdgeList>    EdgeListVector;


private:

  //the nodes that comprise this graph
  NodeVector      mNodes;

  //a vector of adjacency edge lists. (each node index keys into the
  //list of edges associated with that node)
  EdgeListVector  mEdges;

  //is this a directed graph?
  bool            mDigraph;

  //the index of the next node to be added
  int             mNextNodeIndex;


  //returns true if an edge is not already present in the graph. Used
  //when adding edges to make sure no duplicates are created.
  bool  uniqueEdge(int from, int to)const;

  //iterates through all the edges in the graph and removes any that point
  //to an invalidated node
  void  cullInvalidEdges();

public:

  //ctor
  SparseGraph(bool digraph): mNextNodeIndex(0), mDigraph(digraph){}

  //returns the node at the given index
  const NodeType&  getNode(int idx)const;

  //non const version
  NodeType&  getNode(int idx);

  //const method for obtaining a reference to an edge
  const EdgeType& getEdge(int from, int to)const;

  //non const version
  EdgeType& getEdge(int from, int to);


  //retrieves the next free node index
  int   getNextFreeNodeIndex()const{return mNextNodeIndex;}

  //adds a node to the graph and returns its index
  int   addNode(nodeType node);

  //removes a node by setting its index to invalidNodeIndex
  void  removeNode(int node);

  //Use this to add an edge to the graph. The method will ensure that the
  //edge passed as a parameter is valid before adding it to the graph. If the
  //graph is a digraph then a similar edge connecting the nodes in the opposite
  //direction will be automatically added.
  void  addEdge(EdgeType edge);

  //removes the edge connecting from and to from the graph (if present). If
  //a digraph then the edge connecting the nodes in the opposite direction
  //will also be removed.
  void  removeEdge(int from, int to);

  //sets the cost of an edge
  void  setEdgeCost(int from, int to, double cost);

  //returns the number of active + inactive nodes present in the graph
  int   numNodes()const{return mNodes.size();}

  //returns the number of active nodes present in the graph (this method's
  //performance can be improved greatly by caching the value)
  int   numActiveNodes()const
  {
    int count = 0;

    for (unsigned int n=0; n<mNodes.size(); ++n) if (mNodes[n].index() != invalidNodeIndex) ++count;

    return count;
  }

  //returns the total number of edges present in the graph
  int   numEdges()const
  {
    int tot = 0;

    for (typename EdgeListVector::const_iterator curEdge = mEdges.begin();
         curEdge != mEdges.end();
         ++curEdge)
    {
      tot += curEdge->size();
    }

    return tot;
  }

  //returns true if the graph is directed
  bool  isDigraph()const{return mDigraph;}

  //returns true if the graph contains no nodes
  bool	isEmpty()const{return mNodes.empty();}

  //returns true if a node with the given index is present in the graph
  bool isNodePresent(int nd)const;

  //returns true if an edge connecting the nodes 'to' and 'from'
  //is present in the graph
  bool isEdgePresent(int from, int to)const;

  //methods for loading and saving graphs from an open file stream or from
  //a file name
  bool  save(const char* fileName)const;
  bool  save(std::ofstream& stream)const;

  bool  load(const char* fileName);
  bool  load(std::ifstream& stream);

  //clears the graph ready for new node insertions
  void clear(){mNextNodeIndex = 0; mNodes.clear(); mEdges.clear();}

  void removeEdges()
  {
    for (typename EdgeListVector::iterator it = mEdges.begin(); it != mEdges.end(); ++it)
    {
      it->clear();
    }
  }


    //non const class used to iterate through all the edges connected to a specific node.
      class EdgeIterator
      {
      private:

        typename EdgeList::iterator         curEdge;

        SparseGraph<nodeType, edgeType>&  g;

        const int                           nodeIndex;

      public:

        EdgeIterator(SparseGraph<nodeType, edgeType>& graph,
                     int                                node): g(graph),
                                                               nodeIndex(node)
        {
          /* we don't need to check for an invalid node index since if the node is
             invalid there will be no associated edges
         */

          curEdge = g.mEdges[nodeIndex].begin();
        }

        EdgeType*  begin()
        {
          curEdge = g.mEdges[nodeIndex].begin();

          return &(*curEdge);
        }

        EdgeType*  next()
        {
          ++curEdge;

          return &(*curEdge);

        }

        //return true if we are at the end of the edge list
        bool end()
        {
          return (curEdge == g.mEdges[nodeIndex].end());
        }
      };

  friend class EdgeIterator;

  //const class used to iterate through all the edges connected to a specific node.
      class ConstEdgeIterator
      {
      private:

        typename EdgeList::const_iterator        curEdge;

        const SparseGraph<nodeType, edgeType>& g;

        const int                                nodeIndex;

      public:

        ConstEdgeIterator(const SparseGraph<nodeType, edgeType>& graph,
                          int                           node): g(graph),
                                                               nodeIndex(node)
        {
          /* we don't need to check for an invalid node index since if the node is
             invalid there will be no associated edges
         */

          curEdge = g.mEdges[nodeIndex].begin();
        }

        const EdgeType*  begin()
        {
          curEdge = g.mEdges[nodeIndex].begin();

          return &(*curEdge);
        }

        const EdgeType*  next()
        {
          ++curEdge;

          return &(*curEdge);

        }

        //return true if we are at the end of the edge list
        bool end()
        {
          return (curEdge == g.mEdges[nodeIndex].end());
        }
      };

  friend class ConstEdgeIterator;

  //non const class used to iterate through the nodes in the graph
    class NodeIterator
    {
    private:

      typename NodeVector::iterator         curNode;

      SparseGraph<nodeType, edgeType>&    g;

      //if a graph node is removed, it is not removed from the
      //vector of nodes (because that would mean changing all the indices of
      //all the nodes that have a higher index). This method takes a node
      //iterator as a parameter and assigns the next valid element to it.
      void getNextValidNode(typename NodeVector::iterator& it)
      {
        if ( curNode == g.mNodes.end() || it->index() != invalidNodeIndex) return;

        while ( (it->index() == invalidNodeIndex) )
        {
          ++it;

          if (curNode == g.mNodes.end()) break;
        }
      }

    public:

      NodeIterator(SparseGraph<nodeType, edgeType> &graph):g(graph)
      {
        curNode = g.mNodes.begin();
      }


      nodeType* begin()
      {
        curNode = g.mNodes.begin();

        getNextValidNode(curNode);

        return &(*curNode);
      }

      nodeType* next()
      {
        ++curNode;

        getNextValidNode(curNode);

        return &(*curNode);
      }

      bool end()
      {
        return (curNode == g.mNodes.end());
      }
    };


  friend class NodeIterator;

    //const class used to iterate through the nodes in the graph
    class ConstNodeIterator
    {
    private:

      typename NodeVector::const_iterator			curNode;

      const SparseGraph<nodeType, edgeType>&      g;

      //if a graph node is removed or switched off, it is not removed from the
      //vector of nodes (because that would mean changing all the indices of
      //all the nodes that have a higher index. This method takes a node
      //iterator as a parameter and assigns the next valid element to it.
      void getNextValidNode(typename NodeVector::const_iterator& it)
      {
        if ( curNode == g.mNodes.end() || it->index() != invalidNodeIndex) return;

        while ( (it->index() == invalidNodeIndex) )
        {
          ++it;

          if (curNode == g.mNodes.end()) break;
        }
      }

    public:

      ConstNodeIterator(const SparseGraph<nodeType, edgeType> &graph):g(graph)
      {
        curNode = g.mNodes.begin();
      }


      const nodeType* begin()
      {
        curNode = g.mNodes.begin();

        getNextValidNode(curNode);

        return &(*curNode);
      }

      const nodeType* next()
      {
        ++curNode;

        getNextValidNode(curNode);

        return &(*curNode);
      }

      bool end()
      {
        return (curNode == g.mNodes.end());
      }
    };

  friend class ConstNodeIterator;
};


//--------------------------- isNodePresent --------------------------------
//
//  returns true if a node with the given index is present in the graph
//--------------------------------------------------------------------------
template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::isNodePresent(int nd)const
{
    if ( (mNodes[nd].index() == invalidNodeIndex) || (nd >= mNodes.size()))
    {
      return false;
    }
    else return true;
}

//--------------------------- isEdgePresent --------------------------------
//
//  returns true if an edge with the given from/to is present in the graph
//--------------------------------------------------------------------------
template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::isEdgePresent(int from, int to)const
{
    if (isNodePresent(from) && isNodePresent(from))
    {
       for (typename EdgeList::const_iterator curEdge = mEdges[from].begin();
            curEdge != mEdges[from].end();
            ++curEdge)
        {
          if (curEdge->to() == to) return true;
        }

        return false;
    }
    else return false;
}
//------------------------------ getNode -------------------------------------
//
//  const and non const methods for obtaining a reference to a specific node
//----------------------------------------------------------------------------
template <class nodeType, class edgeType>
const nodeType&  SparseGraph<nodeType, edgeType>::getNode(int idx)const
{
    assert( (idx < (int)mNodes.size()) &&
            (idx >=0)              &&
           "<SparseGraph::GetNode>: invalid index");

    return mNodes[idx];
}

  //non const version
template <class nodeType, class edgeType>
nodeType&  SparseGraph<nodeType, edgeType>::getNode(int idx)
{
    assert( (idx < (int)mNodes.size()) &&
            (idx >=0)             &&
          "<SparseGraph::GetNode>: invalid index");

    return mNodes[idx];
}

//------------------------------ getEdge -------------------------------------
//
//  const and non const methods for obtaining a reference to a specific edge
//----------------------------------------------------------------------------
template <class nodeType, class edgeType>
const edgeType& SparseGraph<nodeType, edgeType>::getEdge(int from, int to)const
{
  assert( (from < mNodes.size()) &&
          (from >=0)              &&
           mNodes[from].index() != invalidNodeIndex &&
          "<SparseGraph::GetEdge>: invalid 'from' index");

  assert( (to < mNodes.size()) &&
          (to >=0)              &&
          mNodes[to].index() != invalidNodeIndex &&
          "<SparseGraph::GetEdge>: invalid 'to' index");

  for (typename EdgeList::const_iterator curEdge = mEdges[from].begin();
       curEdge != mEdges[from].end();
       ++curEdge)
  {
    if (curEdge->to() == to) return *curEdge;
  }

  assert (0 && "<SparseGraph::GetEdge>: edge does not exist");
}

//non const version
template <class nodeType, class edgeType>
edgeType& SparseGraph<nodeType, edgeType>::getEdge(int from, int to)
{
  assert( (from < mNodes.size()) &&
          (from >=0)              &&
           mNodes[from].index() != invalidNodeIndex &&
          "<SparseGraph::GetEdge>: invalid 'from' index");

  assert( (to < mNodes.size()) &&
          (to >=0)              &&
          mNodes[to].index() != invalidNodeIndex &&
          "<SparseGraph::GetEdge>: invalid 'to' index");

  for (typename EdgeList::iterator curEdge = mEdges[from].begin();
       curEdge != mEdges[from].end();
       ++curEdge)
  {
    if (curEdge->to() == to) return *curEdge;
  }

  assert (0 && "<SparseGraph::GetEdge>: edge does not exist");
}

//-------------------------- addEdge ------------------------------------------
//
//  Use this to add an edge to the graph. The method will ensure that the
//  edge passed as a parameter is valid before adding it to the graph. If the
//  graph is a digraph then a similar edge connecting the nodes in the opposite
//  direction will be automatically added.
//-----------------------------------------------------------------------------
template <class nodeType, class edgeType>
void SparseGraph<nodeType, edgeType>::addEdge(EdgeType edge)
{
  //first make sure the from and to nodes exist within the graph
  assert( (edge.from() < mNextNodeIndex) && (edge.to() < mNextNodeIndex) &&
          "<SparseGraph::AddEdge>: invalid node index");

  //make sure both nodes are active before adding the edge
  if ( (mNodes[edge.to()].index() != invalidNodeIndex) &&
       (mNodes[edge.from()].index() != invalidNodeIndex))
  {
    //add the edge, first making sure it is unique
    if (uniqueEdge(edge.from(), edge.to()))
    {
      mEdges[edge.from()].push_back(edge);
    }

    //if the graph is undirected we must add another connection in the opposite
    //direction
    if (!mDigraph)
    {
      //check to make sure the edge is unique before adding
      if (uniqueEdge(edge.to(), edge.from()))
      {
        EdgeType newEdge = edge;

        newEdge.setTo(edge.from());
        newEdge.setFrom(edge.to());

        mEdges[edge.to()].push_back(newEdge);
      }
    }
  }
}


//----------------------------- removeEdge ---------------------------------
template <class nodeType, class edgeType>
void SparseGraph<nodeType, edgeType>::removeEdge(int from, int to)
{
  assert ( (from < (int)mNodes.size()) && (to < (int)mNodes.size()) &&
           "<SparseGraph::RemoveEdge>:invalid node index");

  typename EdgeList::iterator curEdge;

  if (!mDigraph)
  {
    for (curEdge = mEdges[to].begin();
         curEdge != mEdges[to].end();
         ++curEdge)
    {
      if (curEdge->to() == from){curEdge = mEdges[to].erase(curEdge);break;}
    }
  }

  for (curEdge = mEdges[from].begin();
       curEdge != mEdges[from].end();
       ++curEdge)
  {
    if (curEdge->to() == to){curEdge = mEdges[from].erase(curEdge);break;}
  }
}

//-------------------------- addNode -------------------------------------
//
//  Given a node this method first checks to see if the node has been added
//  previously but is now innactive. If it is, it is reactivated.
//
//  If the node has not been added previously, it is checked to make sure its
//  index matches the next node index before being added to the graph
//------------------------------------------------------------------------
template <class nodeType, class edgeType>
int SparseGraph<nodeType, edgeType>::addNode(nodeType node)
{
  if (node.index() < (int)mNodes.size())
  {
    //make sure the client is not trying to add a node with the same id as
    //a currently active node
    assert (mNodes[node.index()].index() == invalidNodeIndex &&
      "<SparseGraph::AddNode>: Attempting to add a node with a duplicate ID");

    mNodes[node.index()] = node;

    return mNextNodeIndex;
  }

  else
  {
    //make sure the new node has been indexed correctly
    assert (node.index() == mNextNodeIndex && "<SparseGraph::AddNode>:invalid index");

    mNodes.push_back(node);
    mEdges.push_back(EdgeList());

    return mNextNodeIndex++;
  }
}

//----------------------- cullInvalidEdges ------------------------------------
//
//  iterates through all the edges in the graph and removes any that point
//  to an invalidated node
//-----------------------------------------------------------------------------
template <class nodeType, class edgeType>
void SparseGraph<nodeType, edgeType>::cullInvalidEdges()
{
  for (typename EdgeListVector::iterator curEdgeList = mEdges.begin(); curEdgeList != mEdges.end(); ++curEdgeList)
  {
    for (typename EdgeList::iterator curEdge = (*curEdgeList).begin(); curEdge != (*curEdgeList).end(); ++curEdge)
    {
      if (mNodes[curEdge->to()].index() == invalidNodeIndex ||
          mNodes[curEdge->from()].index() == invalidNodeIndex)
      {
        curEdge = (*curEdgeList).erase(curEdge);
      }
    }
  }
}


//------------------------------- removeNode -----------------------------
//
//  Removes a node from the graph and removes any links to neighbouring
//  nodes
//------------------------------------------------------------------------
template <class nodeType, class edgeType>
void SparseGraph<nodeType, edgeType>::removeNode(int node)
{
  assert(node < (int)mNodes.size() && "<SparseGraph::RemoveNode>: invalid node index");

  //set this node's index to invalidNodeIndex
  mNodes[node].setIndex(invalidNodeIndex);

  //if the graph is not directed remove all edges leading to this node and then
  //clear the edges leading from the node
  if (!mDigraph)
  {
    //visit each neighbour and erase any edges leading to this node
    for (typename EdgeList::iterator curEdge = mEdges[node].begin();
         curEdge != mEdges[node].end();
         ++curEdge)
    {
      for (typename EdgeList::iterator curE = mEdges[curEdge->to()].begin();
           curE != mEdges[curEdge->to()].end();
           ++curE)
      {
         if (curE->to() == node)
         {
           curE = mEdges[curEdge->to()].erase(curE);

           break;
         }
      }
    }

    //finally, clear this node's edges
    mEdges[node].clear();
  }

  //if a digraph remove the edges the slow way
  else
  {
    cullInvalidEdges();
  }
}

//-------------------------- setEdgeCost ---------------------------------
//
//  Sets the cost of a specific edge
//------------------------------------------------------------------------
template <class nodeType, class edgeType>
void SparseGraph<nodeType, edgeType>::setEdgeCost(int from, int to, double newCost)
{
  //make sure the nodes given are valid
  assert( (from < mNodes.size()) && (to < mNodes.size()) &&
        "<SparseGraph::SetEdgeCost>: invalid index");

  //visit each neighbour and erase any edges leading to this node
  for (typename EdgeList::iterator curEdge = mEdges[from].begin();
       curEdge != mEdges[from].end();
       ++curEdge)
  {
    if (curEdge->to() == to)
    {
      curEdge->setCost(newCost);
      break;
    }
  }
}

  //-------------------------------- uniqueEdge ----------------------------
//
//  returns true if the edge is not present in the graph. Used when adding
//  edges to prevent duplication
//------------------------------------------------------------------------
template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::uniqueEdge(int from, int to)const
{
  for (typename EdgeList::const_iterator curEdge = mEdges[from].begin();
       curEdge != mEdges[from].end();
       ++curEdge)
  {
    if (curEdge->to() == to)
    {
      return false;
    }
  }

  return true;
}

//-------------------------------- save ---------------------------------------

template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::save(const char* fileName)const
{
  //open the file and make sure it's valid
  std::ofstream out(fileName);

  if (!out)
  {
    throw std::runtime_error("Cannot open file: " + std::string(fileName));
    return false;
  }

  return save(out);
}

//-------------------------------- save ---------------------------------------
template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::save(std::ofstream& stream)const
{
  //save the number of nodes
  stream << mNodes.size() << std::endl;

  //iterate through the graph nodes and save them
  typename NodeVector::const_iterator curNode = mNodes.begin();
  for (curNode; curNode!=mNodes.end(); ++curNode)
  {
    stream << *curNode;
  }

  //save the number of edges
  stream << numEdges() << std::endl;


  //iterate through the edges and save them
  for (unsigned int nodeIdx = 0; nodeIdx < mNodes.size(); ++nodeIdx)
  {
    for (typename EdgeList::const_iterator curEdge = mEdges[nodeIdx].begin();
         curEdge!=mEdges[nodeIdx].end(); ++curEdge)
    {
      stream << *curEdge;
    }
  }

  return true;
}

//------------------------------- load ----------------------------------------
//-----------------------------------------------------------------------------
template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::load(const char* fileName)
{
  //open file and make sure it's valid
  std::ifstream in(fileName);

  if (!in)
  {
    throw std::runtime_error("Cannot open file: " + std::string(fileName));
    return false;
  }

  return load(in);
}

//------------------------------- load ----------------------------------------
//-----------------------------------------------------------------------------
template <class nodeType, class edgeType>
bool SparseGraph<nodeType, edgeType>::load(std::ifstream& stream)
{
  clear();

  //get the number of nodes and read them in
  int numNodes, numEdges;

  stream >> numNodes;

  for (int n=0; n<numNodes; ++n)
  {
    NodeType newNode(stream);

    //when editing graphs it's possible to end up with a situation where some
    //of the nodes have been invalidated (their id's set to invalidNodeIndex). Therefore
    //when a node of index invalidNodeIndex is encountered, it must still be added.
    if (newNode.index() != invalidNodeIndex)
    {
      addNode(newNode);
    }
    else
    {
      mNodes.push_back(newNode);

      //make sure an edgelist is added for each node
      mEdges.push_back(EdgeList());

      ++mNextNodeIndex;
    }
  }

  //now add the edges
  stream >> numEdges;
  for (int e=0; e<numEdges; ++e)
  {
    EdgeType nextEdge(stream);

    mEdges[nextEdge.from()].push_back(nextEdge);
  }

  return true;
}


#endif