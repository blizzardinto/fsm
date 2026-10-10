#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H
#include <vector>

#include "Vector2D.h"
#include "C2DMatrix.h"


//--------------------------- worldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position, orientation and scale,
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                    const Vector2D   &pos,
                                    const Vector2D   &forward,
                                    const Vector2D   &side,
                                    const Vector2D   &scale);

//--------------------------- worldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position and  orientation
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                     const Vector2D   &pos,
                                     const Vector2D   &forward,
                                     const Vector2D   &side);

//--------------------- pointToWorldSpace --------------------------------
//
//  Transforms a point from the agent's local space into world space
//------------------------------------------------------------------------
Vector2D pointToWorldSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition);

//--------------------- vectorToWorldSpace --------------------------------
//
//  Transforms a vector from the agent's local space into world space
//------------------------------------------------------------------------
Vector2D vectorToWorldSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide);


//--------------------- pointToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
Vector2D pointToLocalSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition);

//--------------------- vectorToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
Vector2D vectorToLocalSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide);

//-------------------------- vec2DRotateAroundOrigin --------------------------
//
//  rotates a vector ang rads around the origin
//-----------------------------------------------------------------------------
void vec2DRotateAroundOrigin(Vector2D& v, double ang);

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
                                     Vector2D      origin);


#endif