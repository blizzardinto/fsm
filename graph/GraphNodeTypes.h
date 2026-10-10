#ifndef GRAPH_NODE_TYPES_H
#define GRAPH_NODE_TYPES_H
//-----------------------------------------------------------------------------
//
//  name:   GraphNodeTypes.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:   Node classes to be used with graphs
//-----------------------------------------------------------------------------
#include <list>
#include <ostream>
#include <fstream>
#include "Vector2D.h"
#include "NodeTypeEnumerations.h"




class GraphNode
{
protected:

  //every node has an index. A valid index is >= 0
  int        mIndex;

public:

  GraphNode():mIndex(invalidNodeIndex){}
  GraphNode(int idx):mIndex(idx){}
  GraphNode(std::ifstream& stream){char buffer[50]; stream >> buffer >> mIndex;}

  virtual ~GraphNode(){}

  int  index()const{return mIndex;}
  void setIndex(int newIndex){mIndex = newIndex;}



  //for reading and writing to streams.
  friend std::ostream& operator<<(std::ostream& os, const GraphNode& n)
  {
    os << "Index: " << n.mIndex << std::endl; return os;
  }

};



//-----------------------------------------------------------------------------
//
//  Graph node for use in creating a navigation graph.This node contains
//  the position of the node and a pointer to a EntityBase... useful
//  if you want your nodes to represent health packs, gold mines and the like
//-----------------------------------------------------------------------------
template <class ExtraInfoType = void*>
class NavGraphNode : public GraphNode
{
protected:

  //the node's position
  Vector2D     mPosition;

  //often you will require a navgraph node to contain additional information.
  //For example a node might represent a pickup such as armor in which
  //case mExtraInfo could be an enumerated value denoting the pickup type,
  //thereby enabling a search algorithm to search a graph for specific items.
  //Going one step further, mExtraInfo could be a pointer to the instance of
  //the item type the node is twinned with. This would allow a search algorithm
  //to test the status of the pickup during the search.
  ExtraInfoType  mExtraInfo;

public:

  //ctors
  NavGraphNode():mExtraInfo(ExtraInfoType()){}

  NavGraphNode(int      idx,
               Vector2D pos):GraphNode(idx),
                             mPosition(pos),
                             mExtraInfo(ExtraInfoType())
  {}

  //stream function Object() { [native code] }
  NavGraphNode(std::ifstream& stream):mExtraInfo(ExtraInfoType())
  {
    char buffer[50];
    stream >> buffer >> mIndex >> buffer >> mPosition.x >> buffer >> mPosition.y;
  }


  virtual ~NavGraphNode(){}

  Vector2D   pos()const{return mPosition;}
  void       setPos(Vector2D newPosition){mPosition = newPosition;}

  ExtraInfoType extraInfo()const{return mExtraInfo;}
  void       setExtraInfo(ExtraInfoType info){mExtraInfo = info;}

  //for reading and writing to streams.
  friend std::ostream& operator<<(std::ostream& os, const NavGraphNode& n)
  {
    os << "Index: " << n.mIndex << " PosX: " << n.mPosition.x << " PosY: " << n.mPosition.y << std::endl;

    return os;
  }

};


#endif
