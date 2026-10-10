#include "Pathfinder.h"
#include "HandyGraphFunctions.h"
#include "Cgdi.h"
#include "PrecisionTimer.h"
#include "constants.h"
#include "AStarHeuristicPolicies.h"
#include "Stream_Utility_Functions.h"


#include <iostream>
using namespace std;

extern HWND gToolbar;
extern const wchar_t* gApplicationName;
extern const wchar_t* gWindowClassName;

//----------------------- createGraph ------------------------------------
//
//------------------------------------------------------------------------
void Pathfinder::createGraph(int cellsUp,
                              int cellsAcross)
{
  //get the height of the toolbar
  RECT rectToolbar;
  GetWindowRect(gToolbar, &rectToolbar);

  //get the dimensions of the client area
  HWND hwndMainWindow = FindWindowW(gWindowClassName, gApplicationName);

  RECT rect;
  GetClientRect(hwndMainWindow, &rect);
  mClientWidth = rect.right;
  mClientHeight = rect.bottom - abs(rectToolbar.bottom - rectToolbar.top) - infoWindowHeight;

  //initialize the terrain vector with normal terrain
  mTerrainType.assign(cellsUp * cellsAcross, normal);

  mCellsX     = cellsAcross;
  mCellsY     = cellsUp;
  mCellWidth  = (double)mClientWidth / (double)cellsAcross;
  mCellHeight = (double)mClientHeight / (double)cellsUp;

  //delete any old graph
  delete mGraph;

  //create the graph
  mGraph = new NavGraph(false);//not a digraph

  graphHelperCreateGrid(*mGraph, mClientWidth, mClientHeight, cellsUp, cellsAcross);

  //initialize source and target indexes to mid top and bottom of grid
  pointToIndex(vectorToPoints(Vector2D(mClientWidth/2, mCellHeight*2)), mTargetCell);
  pointToIndex(vectorToPoints(Vector2D(mClientWidth/2, mClientHeight -mCellHeight*2)), mSourceCell);

  mPath.clear();
  mSubTree.clear();

  mCurrentAlgorithm = non;
  mTimeTaken = 0;
}

//--------------------- pointToIndex -------------------------------------
//
//  converts a POINTS into an index into the graph
//------------------------------------------------------------------------
bool Pathfinder::pointToIndex(POINTS p, int& nodeIndex)
{
  //convert p to an index into the graph
  int x = (int)((double)(p.x)/mCellWidth);
  int y = (int)((double)(p.y)/mCellHeight);

  //make sure the values are legal
  if ( (x>mCellsX) || (y>mCellsY) )
  {
    nodeIndex = -1;

    return false;
  }

  nodeIndex = y*mCellsX+x;

  return true;
}

//----------------- getTerrainCost ---------------------------------------
//
//  returns the cost of the terrain represented by the current brush type
//------------------------------------------------------------------------
double Pathfinder::getTerrainCost(const BrushType brush)
{
  const double costNormal = 1.0;
  const double costWater  = 2.0;
  const double costMud    = 1.5;

  switch (brush)
  {
    case normal: return costNormal;
    case water:  return costWater;
    case mud:    return costMud;
    default:     return maxDouble;
  };
}

//----------------------- paintTerrain -----------------------------------
//
//  this either changes the terrain at position p to whatever the current
//  terrain brush is set to, or it adjusts the source/target cell
//------------------------------------------------------------------------
void Pathfinder::paintTerrain(POINTS p)
{
  //convert p to an index into the graph
  int x = (int)((double)(p.x)/mCellWidth);
  int y = (int)((double)(p.y)/mCellHeight);

  //make sure the values are legal
  if ( (x>mCellsX) || (y>(mCellsY-1)) ) return;

  //reset path and tree records
  mSubTree.clear();
  mPath.clear();

  //if the current terrain brush is set to either source or target we
  //should change the appropriate node
  if ( (mCurrentTerrainBrush == source) || (mCurrentTerrainBrush == target) )
  {
    switch (mCurrentTerrainBrush)
    {
    case source:

      mSourceCell = y*mCellsX+x; break;

    case target:

      mTargetCell = y*mCellsX+x; break;

    }//end switch
  }

  //otherwise, change the terrain at the current mouse position
  else
  {
    updateGraphFromBrush(mCurrentTerrainBrush, y*mCellsX+x);
  }

  //update any currently selected algorithm
  updateAlgorithm();
}

//--------------------------- updateGraphFromBrush ----------------------------
//
//  given a brush and a node index, this method updates the graph appropriately
//  (by removing/adding nodes or changing the costs of the node's edges)
//-----------------------------------------------------------------------------
void Pathfinder::updateGraphFromBrush(int brush, int cellIndex)
{
  //set the terrain type in the terrain index
  mTerrainType[cellIndex] = brush;

  //if current brush is an obstacle then this node must be removed
  //from the graph
  if (brush == 1)
  {
    mGraph->removeNode(cellIndex);
  }

  else
  {
    //make the node active again if it is currently inactive
    if (!mGraph->isNodePresent(cellIndex))
    {
      int y = cellIndex / mCellsY;
      int x = cellIndex - (y*mCellsY);

      mGraph->addNode(NavGraph::NodeType(cellIndex, Vector2D(x*mCellWidth + mCellWidth/2.0,
                                                               y*mCellHeight+mCellHeight/2.0)));

      graphHelperAddAllNeighboursToGridNode(*mGraph, y, x, mCellsX, mCellsY);
    }

    //set the edge costs in the graph
    weightNavGraphNodeEdges(*mGraph, cellIndex, getTerrainCost((BrushType)brush));
  }
}

//--------------------------- updateAlgorithm ---------------------------------
void Pathfinder::updateAlgorithm()
{
  //update any current algorithm
  switch(mCurrentAlgorithm)
  {
  case non:

    break;

  case searchDfs:

    createPathDfs(); break;

  case searchBfs:

    createPathBfs(); break;

  case searchDijkstra:

    createPathDijkstra(); break;

  case searchAstar:

    createPathAStar(); break;

  default: break;
  }
}

//------------------------- createPathDfs --------------------------------
//
//  uses dfs to find a path between the start and target cells.
//  Stores the path as a series of node indexes in mPath.
//------------------------------------------------------------------------
void Pathfinder::createPathDfs()
{
  //set current algorithm
  mCurrentAlgorithm = searchDfs;

  //clear any existing path
  mPath.clear();
  mSubTree.clear();

  //create and start a timer
  PrecisionTimer timer; timer.start();

  //do the search
  GraphSearchDfs<NavGraph> dfs(*mGraph, mSourceCell, mTargetCell);

  //record the time taken
  mTimeTaken = timer.timeElapsed();

  //now grab the path (if one has been found)
  if (dfs.found())
  {
    mPath = dfs.getPathToTarget();
  }

  mSubTree = dfs.getSearchTree();

  mCostToTarget = 0.0;
}


//------------------------- createPathBfs --------------------------------
//
//  uses bfs to find a path between the start and target cells.
//  Stores the path as a series of node indexes in mPath.
//------------------------------------------------------------------------
void Pathfinder::createPathBfs()
{
  //set current algorithm
  mCurrentAlgorithm = searchBfs;

  //clear any existing path
  mPath.clear();
  mSubTree.clear();

  //create and start a timer
  PrecisionTimer timer; timer.start();

  //do the search
  GraphSearchBfs<NavGraph> bfs(*mGraph, mSourceCell, mTargetCell);

    //record the time taken
  mTimeTaken = timer.timeElapsed();

  //now grab the path (if one has been found)
  if (bfs.found())
  {
    mPath = bfs.getPathToTarget();
  }

  mSubTree = bfs.getSearchTree();

  mCostToTarget = 0.0;
}

//-------------------------- createPathDijkstra --------------------------
//
//  creates a path from mSourceCell to mTargetCell using Dijkstra's algorithm
//------------------------------------------------------------------------
void Pathfinder::createPathDijkstra()
{
  //set current algorithm
  mCurrentAlgorithm = searchDijkstra;

  //create and start a timer
  PrecisionTimer timer; timer.start();

  GraphSearchDijkstra<NavGraph> djk(*mGraph, mSourceCell, mTargetCell);

  //record the time taken
  mTimeTaken = timer.timeElapsed();

  mPath = djk.getPathToTarget();

  mSubTree = djk.getSpt();

  mCostToTarget = djk.getCostToTarget();
}

//--------------------------- createPathAStar ---------------------------
//------------------------------------------------------------------------
void Pathfinder::createPathAStar()
{
  //set current algorithm
  mCurrentAlgorithm = searchAstar;

  //create and start a timer
  PrecisionTimer timer; timer.start();

  //create a couple of typedefs so the code will sit comfortably on the page
  typedef GraphSearchAStar<NavGraph, HeuristicEuclid> AStarSearch;

  //create an instance of the A* search using the Euclidean heuristic
  AStarSearch aStar(*mGraph, mSourceCell, mTargetCell);


  //record the time taken
  mTimeTaken = timer.timeElapsed();

  mPath = aStar.getPathToTarget();

  mSubTree = aStar.getSpt();

  mCostToTarget = aStar.getCostToTarget();

}

//---------------------------load n save methods ------------------------------
//-----------------------------------------------------------------------------
void Pathfinder::save( char* fileName)
{
  ofstream save(fileName);
  assert (save && "Pathfinder::Save< bad file >");

  //save the size of the grid
  save << mCellsX << endl;
  save << mCellsY << endl;

  //save the terrain
  for (unsigned int t=0; t<mTerrainType.size(); ++t)
  {
    if (t==mSourceCell)
    {
      save << source << endl;
    }
    else if (t==mTargetCell)
    {
      save << target << endl;
    }
    else
    {
      save << mTerrainType[t] << endl;
    }
  }
}

//-------------------------------- load ---------------------------------------
//-----------------------------------------------------------------------------
void Pathfinder::load( char* fileName)
{
  ifstream load(fileName);
  assert (load && "Pathfinder::Save< bad file >");

  //load the size of the grid
  load >> mCellsX;
  load >> mCellsY;

  //create a graph of the correct size
  createGraph(mCellsY, mCellsX);

  int terrain;

  //save the terrain
  for (int t=0; t<mCellsX*mCellsY; ++t)
  {
    load >> terrain;

    if (terrain == source)
    {
      mSourceCell = t;
    }

    else if (terrain == target)
    {
      mTargetCell = t;
    }

    else
    {
      mTerrainType[t] = terrain;

      updateGraphFromBrush(terrain, t);
    }
  }
}

//------------------------ getNameOfCurrentSearchAlgorithm --------------------
//-----------------------------------------------------------------------------
std::string Pathfinder::getNameOfCurrentSearchAlgorithm()const
{
  switch(mCurrentAlgorithm)
  {
  case non: return "";
  case searchAstar: return "A Star";
  case searchBfs: return "Breadth First";
  case searchDfs: return "Depth First";
  case searchDijkstra: return "Dijkstras";
  }
}

//---------------------------- render ------------------------------------
//
//------------------------------------------------------------------------
void Pathfinder::render()
{
  gdi->transparentText();

  //render all the cells
  for (int nd=0; nd<mGraph->numNodes(); ++nd)
  {
    int left   = (int)(mGraph->getNode(nd).pos().x - mCellWidth/2.0);
    int top    = (int)(mGraph->getNode(nd).pos().y - mCellHeight/2.0);
    int right  = (int)(1+mGraph->getNode(nd).pos().x + mCellWidth/2.0);
    int bottom = (int)(1+mGraph->getNode(nd).pos().y + mCellHeight/2.0);

    gdi->greyPen();

    switch (mTerrainType[nd])
    {
    case 0:
      gdi->whiteBrush();
      if (!mShowTiles)gdi->whitePen();
      break;

    case 1:
      gdi->blackBrush();
      if (!mShowTiles)gdi->blackPen();
      break;

    case 2:
      gdi->lightBlueBrush();
      if (!mShowTiles)gdi->lightBluePen();
      break;

    case 3:
      gdi->brownBrush();
      if (!mShowTiles)gdi->brownPen();
      break;

    default:
      gdi->whiteBrush();
      if (!mShowTiles)gdi->whitePen();
      break;

    }//end switch


    if (nd == mTargetCell)
    {
      gdi->redBrush();
      if (!mShowTiles)gdi->redPen();
    }

    if (nd == mSourceCell)
    {
      gdi->greenBrush();
      if (!mShowTiles)gdi->greenPen();
    }

    gdi->rect(left, top, right, bottom);

    if (nd == mTargetCell)
    {
      gdi->thickBlackPen();
      gdi->cross(Vector2D(mGraph->getNode(nd).pos().x-1, mGraph->getNode(nd).pos().y-1),
                (int)((mCellWidth*0.6)/2.0));
    }

    if (nd == mSourceCell)
    {
      gdi->thickBlackPen();
      gdi->hollowBrush();
      gdi->rect(left+7,top+7,right-6,bottom-6);
    }

    //render dots at the corners of the cells
    gdi->drawDot(left, top, RGB(0,0,0));
    gdi->drawDot(right-1, top, RGB(0,0,0));
    gdi->drawDot(left, bottom-1, RGB(0,0,0));
    gdi->drawDot(right-1, bottom-1, RGB(0,0,0));
  }
  //draw the graph nodes and edges if rqd
  if (mShowGraph)
  {
    graphHelperDrawUsingGdi<NavGraph>(*mGraph, Cgdi::lightGrey, false);  //false = don't draw node IDs
  }

  //draw any tree retrieved from the algorithms
  gdi->redPen();

  for (unsigned int e=0; e<mSubTree.size(); ++e)
  {
    if (mSubTree[e])
    {
      Vector2D from = mGraph->getNode(mSubTree[e]->from()).pos();
      Vector2D to   = mGraph->getNode(mSubTree[e]->to()).pos();

      gdi->line(from, to);
    }
  }

  //draw the path (if any)
  if (mPath.size() > 0)
  {
    gdi->thickBluePen();

    std::list<int>::iterator it = mPath.begin();
    std::list<int>::iterator nxt = it; ++nxt;

    for (it; nxt != mPath.end(); ++it, ++nxt)
    {
      gdi->line(mGraph->getNode(*it).pos(), mGraph->getNode(*nxt).pos());
    }
  }

  if (mTimeTaken)
  {
    //draw time taken to complete algorithm
    string time = ttos(mTimeTaken, 8);
    string s = "Time Elapsed for " + getNameOfCurrentSearchAlgorithm() + " is " + time;
    gdi->textAtPos(1,mClientHeight + 3,s);
  }

  //display the total path cost if appropriate
  if (mCurrentAlgorithm == searchAstar || mCurrentAlgorithm == searchDijkstra)
  {
    gdi->textAtPos(mClientWidth-110, mClientHeight + 3, "Cost is " + ttos(mCostToTarget));
  }
}
