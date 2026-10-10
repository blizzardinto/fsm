#ifndef Pathfinder_H
#define Pathfinder_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  name:   Pathfinder.h
//
//  Desc:   class enabling users to create simple environments consisting
//          of different terrain types and then to use various search algorithms
//          to find paths through them
//
//  Author: Mat Buckland  (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <windows.h>
#include <vector>
#include <fstream>
#include <string>
#include <list>


#include "Vector2d.h"
#include "SparseGraph.h"
#include "GraphAlgorithms.h"
#include "utils.h"
#include "GraphEdgeTypes.h"
#include "GraphNodeTypes.h"



class Pathfinder
{
public:

  enum BrushType
  {
    normal   = 0,
    obstacle = 1,
    water    = 2,
    mud      = 3,
    source   = 4,
    target   = 5
  };

  enum AlgorithmType
  {
    non,
    searchAstar,
    searchBfs,
    searchDfs,
    searchDijkstra
  };

private:

  //the terrain type of each cell
  std::vector<int>              mTerrainType;

  //this vector will store any path returned from a graph search
  std::list<int>                mPath;

  //create a typedef for the graph type
  typedef SparseGraph<NavGraphNode<void*>, GraphEdge> NavGraph;

  NavGraph*                     mGraph;

  //this vector of edges is used to store any subtree returned from
  //any of the graph algorithms (such as an SPT)
  std::vector<const GraphEdge*> mSubTree;

  //the total cost of the path from target to source
  double                         mCostToTarget;

  //the currently selected algorithm
  AlgorithmType                mCurrentAlgorithm;

  //the current terrain brush
  BrushType                    mCurrentTerrainBrush;

  //the dimensions of the cells
  double                        mCellWidth;
  double                        mCellHeight;

  //number of cells vertically and horizontally
  int                           mCellsX,
                                mCellsY;

  //local record of the client area
  int                           mClientWidth,
                                mClientHeight;

  //the indices of the source and target cells
  int                           mSourceCell,
                                mTargetCell;

  //flags to indicate if the start and finish points have been added
  bool                          mStart,
                                mFinish;

  //should the graph (nodes and GraphEdges) be rendered?
  bool                          mShowGraph;

  //should the tile outlines be rendered
  bool                          mShowTiles;

  //holds the time taken for the most currently used algorithm to
  //complete
  double                        mTimeTaken;

  //this calls the appropriate algorithm
  void  updateAlgorithm();

  //helper function for paintTerrain (see below)
  void  updateGraphFromBrush(int brush, int cellIndex);

 std::string getNameOfCurrentSearchAlgorithm()const;

public:

  Pathfinder():mStart(false),
                mFinish(false),
                mShowGraph(false),
                mShowTiles(true),
                mCellWidth(0),
                mCellHeight(0),
                mCellsX(0),
                mCellsY(0),
                mTimeTaken(0.0),
                mCurrentTerrainBrush(normal),
                mSourceCell(0),
                mTargetCell(0),
                mClientWidth(0),
                mClientHeight(0),
                mCostToTarget(0.0),
                mGraph(NULL)
  {}

  ~Pathfinder(){delete mGraph;}

  void createGraph(int cellsUp, int cellsAcross);

  void render();

  //this will paint whatever cell the cursor is currently over in the
  //currently selected terrain brush
  void paintTerrain(POINTS p);

  //the algorithms
  void createPathDfs();
  void createPathBfs();
  void createPathDijkstra();
  void createPathAStar();
  void minSpanningTree();

  //if mShowGraph is true the graph will be rendered
  void toggleShowGraph(){mShowGraph = !mShowGraph;}
  void switchGraphOn(){mShowGraph = true;}
  void switchGraphOff(){mShowGraph = false;}
  bool isShowGraphOn()const{return mShowGraph;}

  void toggleShowTiles(){mShowTiles = !mShowTiles;}
  void switchTilesOn(){mShowTiles = true;}
  void switchTilesOff(){mShowTiles = false;}
  bool isShowTilesOn()const{return mShowTiles;}

  void changeBrush(const BrushType newBrush){mCurrentTerrainBrush = newBrush;}

  void changeSource(const int cell){mSourceCell = cell;}
  void changeTarget(const int cell){mTargetCell = cell;}

  //converts a POINTS to an index into the graph. Returns false if p
  //is invalid
  bool pointToIndex(POINTS p, int& nodeIndex);

  //returns the terrain cost of the brush type
  double getTerrainCost(BrushType brush);

  void save( char* fileName);
  void load( char* fileName);

};


#endif