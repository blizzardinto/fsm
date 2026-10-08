#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H
#include <vector>

#include "Vector2D.h"
#include "C2DMatrix.h"


//--------------------------- WorldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position, orientation and scale,
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
std::vector<Vector2D> WorldTransform(std::vector<Vector2D> &points,
                                    const Vector2D   &pos,
                                    const Vector2D   &forward,
                                    const Vector2D   &side,
                                    const Vector2D   &scale);

//--------------------------- WorldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position and  orientation
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
std::vector<Vector2D> WorldTransform(std::vector<Vector2D> &points,
                                     const Vector2D   &pos,
                                     const Vector2D   &forward,
                                     const Vector2D   &side);

//--------------------- PointToWorldSpace --------------------------------
//
//  Transforms a point from the agent's local space into world space
//------------------------------------------------------------------------
Vector2D PointToWorldSpace(const Vector2D &point,
                           const Vector2D &AgentHeading,
                           const Vector2D &AgentSide,
                           const Vector2D &AgentPosition);

//--------------------- VectorToWorldSpace --------------------------------
//
//  Transforms a vector from the agent's local space into world space
//------------------------------------------------------------------------
Vector2D VectorToWorldSpace(const Vector2D &vec,
                            const Vector2D &AgentHeading,
                            const Vector2D &AgentSide);


//--------------------- PointToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
Vector2D PointToLocalSpace(const Vector2D &point,
                           const Vector2D &AgentHeading,
                           const Vector2D &AgentSide,
                           const Vector2D &AgentPosition);

//--------------------- VectorToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
Vector2D VectorToLocalSpace(const Vector2D &vec,
                            const Vector2D &AgentHeading,
                            const Vector2D &AgentSide);

//-------------------------- Vec2DRotateAroundOrigin --------------------------
//
//  rotates a vector ang rads around the origin
//-----------------------------------------------------------------------------
void Vec2DRotateAroundOrigin(Vector2D& v, double ang);

//------------------------ CreateWhiskers ------------------------------------
//
//  given an origin, a facing direction, a 'field of view' describing the 
//  limit of the outer whiskers, a whisker length and the number of whiskers
//  this method returns a vector containing the end positions of a series
//  of whiskers radiating away from the origin and with equal distance between
//  them. (like the spokes of a wheel clipped to a specific segment size)
//----------------------------------------------------------------------------
std::vector<Vector2D> CreateWhiskers(unsigned int  NumWhiskers,
                                     double        WhiskerLength,
                                     double        fov,
                                     Vector2D      facing,
                                     Vector2D      origin);


#endif