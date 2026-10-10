#include "Transformations.h"

//--------------------------- worldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position, orientation and scale,
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                    const Vector2D   &pos,
                                    const Vector2D   &forward,
                                    const Vector2D   &side,
                                    const Vector2D   &scale)
{
  //copy the original vertices into the buffer about to be transformed
  std::vector<Vector2D> tranVector2Ds = points;

  //create a transformation matrix
  C2DMatrix matTransform;

  //scale
  if ( (scale.x != 1.0) || (scale.y != 1.0) )
  {
    matTransform.scale(scale.x, scale.y);
  }

  //rotate
  matTransform.rotate(forward, side);

  //and translate
  matTransform.translate(pos.x, pos.y);

  //now transform the object's vertices
  matTransform.transformVector2Ds(tranVector2Ds);

  return tranVector2Ds;
}

//--------------------------- worldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position and  orientation
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                     const Vector2D   &pos,
                                     const Vector2D   &forward,
                                     const Vector2D   &side)
{
  //copy the original vertices into the buffer about to be transformed
  std::vector<Vector2D> tranVector2Ds = points;

  //create a transformation matrix
  C2DMatrix matTransform;

  //rotate
  matTransform.rotate(forward, side);

  //and translate
  matTransform.translate(pos.x, pos.y);

  //now transform the object's vertices
  matTransform.transformVector2Ds(tranVector2Ds);

  return tranVector2Ds;
}

//--------------------- pointToWorldSpace --------------------------------
//
//  Transforms a point from the agent's local space into world space
//------------------------------------------------------------------------
Vector2D pointToWorldSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition)
{
  //make a copy of the point
  Vector2D transPoint = point;

  //create a transformation matrix
  C2DMatrix matTransform;

  //rotate
  matTransform.rotate(agentHeading, agentSide);

  //and translate
  matTransform.translate(agentPosition.x, agentPosition.y);

  //now transform the vertices
  matTransform.transformVector2Ds(transPoint);

  return transPoint;
}

//--------------------- vectorToWorldSpace --------------------------------
//
//  Transforms a vector from the agent's local space into world space
//------------------------------------------------------------------------
Vector2D vectorToWorldSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide)
{
  //make a copy of the point
  Vector2D transVec = vec;

  //create a transformation matrix
  C2DMatrix matTransform;

  //rotate
  matTransform.rotate(agentHeading, agentSide);

  //now transform the vertices
  matTransform.transformVector2Ds(transVec);

  return transVec;
}


//--------------------- pointToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
Vector2D pointToLocalSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition)
{

  //make a copy of the point
  Vector2D transPoint = point;

  //create a transformation matrix
  C2DMatrix matTransform;

  double tx = -agentPosition.dot(agentHeading);
  double ty = -agentPosition.dot(agentSide);

  //create the transformation matrix
  matTransform.element11(agentHeading.x); matTransform.element12(agentSide.x);
  matTransform.element21(agentHeading.y); matTransform.element22(agentSide.y);
  matTransform.element31(tx);             matTransform.element32(ty);

  //now transform the vertices
  matTransform.transformVector2Ds(transPoint);

  return transPoint;
}

//--------------------- vectorToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
Vector2D vectorToLocalSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide)
{

  //make a copy of the point
  Vector2D transPoint = vec;

  //create a transformation matrix
  C2DMatrix matTransform;

  //create the transformation matrix
  matTransform.element11(agentHeading.x); matTransform.element12(agentSide.x);
  matTransform.element21(agentHeading.y); matTransform.element22(agentSide.y);

  //now transform the vertices
  matTransform.transformVector2Ds(transPoint);

  return transPoint;
}

//-------------------------- vec2DRotateAroundOrigin --------------------------
//
//  rotates a vector ang rads around the origin
//-----------------------------------------------------------------------------
void vec2DRotateAroundOrigin(Vector2D& v, double ang)
{
  //create a transformation matrix
  C2DMatrix mat;

  //rotate
  mat.rotate(ang);

  //now transform the object's vertices
  mat.transformVector2Ds(v);
}

//------------------------ createWhiskers ------------------------------------
//
//  given an origin, a facing direction, a 'field of view' describing the
//  limit of the outer whiskers, a whisker length and the number of whiskers
//  this method returns a vector containing the end positions of a series
//  of whiskers radiating away from the origin and with equal distance between
//  them. (like the spokes of a wheel clipped to a specific segment size)
//----------------------------------------------------------------------------
std::vector<Vector2D> createWhiskers(unsigned int  numWhiskers,
                                     double        whiskerLength,
                                     double        fov,
                                     Vector2D      facing,
                                     Vector2D      origin)
{
  //this is the magnitude of the angle separating each whisker
  double sectorSize = fov/(double)(numWhiskers-1);

  std::vector<Vector2D> whiskers;
  Vector2D temp;
  double angle = -fov*0.5;

  for (unsigned int w=0; w<numWhiskers; ++w)
  {
    //create the whisker extending outwards at this angle
    temp = facing;
    vec2DRotateAroundOrigin(temp, angle);
    whiskers.push_back(origin + whiskerLength * temp);

    angle+=sectorSize;
  }

  return whiskers;
}
