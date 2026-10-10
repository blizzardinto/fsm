#ifndef GRAPH_EDGE_TYPES_H
#define GRAPH_EDGE_TYPES_H
//-----------------------------------------------------------------------------
//
//  name:   GraphEdgeTypes.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:   Class to define an edge connecting two nodes.
//
//          An edge has an associated cost.
//-----------------------------------------------------------------------------
#include <ostream>
#include <fstream>

#include "NodeTypeEnumerations.h"


class GraphEdge
{
protected:

  //An edge connects two nodes. Valid node indices are always positive.
  int     mFrom;
  int     mTo;

  //the cost of traversing the edge
  double  mCost;

public:

  //ctors
  GraphEdge(int from, int to, double cost):mCost(cost),
                                           mFrom(from),
                                           mTo(to)
  {}

  GraphEdge(int from, int  to):mCost(1.0),
                               mFrom(from),
                               mTo(to)
  {}

  GraphEdge():mCost(1.0),
              mFrom(invalidNodeIndex),
              mTo(invalidNodeIndex)
  {}

  //stream function Object() { [native code] }
  GraphEdge(std::ifstream& stream)
  {
    char buffer[50];
    stream  >> buffer >> mFrom >> buffer >> mTo >> buffer >> mCost;
  }

  virtual ~GraphEdge(){}

  int   from()const{return mFrom;}
  void  setFrom(int newIndex){mFrom = newIndex;}

  int   to()const{return mTo;}
  void  setTo(int newIndex){mTo = newIndex;}

  double cost()const{return mCost;}
  void  setCost(double newCost){mCost = newCost;}


  //these two operators are required
  bool operator==(const GraphEdge& rhs)
  {
    return rhs.mFrom == this->mFrom &&
           rhs.mTo   == this->mTo   &&
           rhs.mCost == this->mCost;
  }

  bool operator!=(const GraphEdge& rhs)
  {
    return !(*this == rhs);
  }

  //for reading and writing to streams.
  friend std::ostream& operator<<(std::ostream& os, const GraphEdge& e)
  {
    os << "m_iFrom: " << e.mFrom << " m_iTo: " << e.mTo
       << " m_dCost: " << e.mCost << std::endl;

    return os;
  }

};


class NavGraphEdge : public GraphEdge
{
public:

  //examples of typical flags
  enum
  {
    normal            = 0,
    swim              = 1 << 0,
    crawl             = 1 << 1,
    creep             = 1 << 3,
    jump              = 1 << 3,
    fly               = 1 << 4,
    grapple           = 1 << 5,
    goesThroughDoor = 1 << 6
  };

protected:

  int   mFlags;

  //if this edge intersects with an object (such as a door or lift), then
  //this is that object's id.
  int  mIdOfIntersectingEntity;

public:


  NavGraphEdge(int    from,
               int    to,
               double cost,
               int    flags = 0,
               int    id = -1):GraphEdge(from,to,cost),
                               mFlags(flags),
                               mIdOfIntersectingEntity(id)

  {}


  //stream function Object() { [native code] }
  NavGraphEdge(std::ifstream& stream)
  {
    char buffer[50];
    stream  >> buffer >> mFrom >> buffer >> mTo >> buffer >> mCost;
    stream >> buffer >> mFlags >> buffer >> mIdOfIntersectingEntity;
  }

  int  flags()const{return mFlags;}
  void setFlags(int flags){mFlags = flags;}

  int  iDofIntersectingEntity()const{return mIdOfIntersectingEntity;}
  void setIdOfIntersectingEntity(int id){mIdOfIntersectingEntity = id;}


  friend std::ostream& operator<<(std::ostream& os, const NavGraphEdge& e)
  {
    os << "m_iFrom: " << e.mFrom << " m_iTo: " << e.mTo
       << " m_dCost: " << e.mCost << " m_iFlags: " << e.mFlags
       << " ID: " << e.mIdOfIntersectingEntity << std::endl;

    return os;
  }
};


#endif