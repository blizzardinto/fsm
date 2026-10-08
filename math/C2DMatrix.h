#ifndef C2DMATRIX_H
#define C2DMATRIX_H
#include <math.h>
#include <vector>

#include "utils.h"
#include "Vector2D.h"




class C2DMatrix
{
private:
  
  struct Matrix
  {

    double _11, _12, _13;
    double _21, _22, _23;
    double _31, _32, _33;

    Matrix()
    {
      _11=0.0; _12=0.0; _13=0.0;
      _21=0.0; _22=0.0; _23=0.0;
      _31=0.0; _32=0.0; _33=0.0;
    }

  };

  Matrix m_Matrix;

  //multiplies m_Matrix with mIn
  void  MatrixMultiply(Matrix &mIn);


public:

  C2DMatrix()
  {
    //initialize the matrix to an identity matrix
    Identity();
  }

  //create an identity matrix
  void Identity();
  
  //create a transformation matrix
  void Translate(double x, double y);

  //create a scale matrix
  void Scale(double xScale, double yScale);

  //create a rotation matrix
  void  Rotate(double rotation);

  //create a rotation matrix from a fwd and side 2D vector
  void  Rotate(const Vector2D &fwd, const Vector2D &side);

   //applys a transformation matrix to a std::vector of points
  void TransformVector2Ds(std::vector<Vector2D> &vPoints);

  //applys a transformation matrix to a point
  void TransformVector2Ds(Vector2D &vPoint);

  //accessors to the matrix elements
  void _11(double val){m_Matrix._11 = val;}
  void _12(double val){m_Matrix._12 = val;}
  void _13(double val){m_Matrix._13 = val;}

  void _21(double val){m_Matrix._21 = val;}
  void _22(double val){m_Matrix._22 = val;}
  void _23(double val){m_Matrix._23 = val;}

  void _31(double val){m_Matrix._31 = val;}
  void _32(double val){m_Matrix._32 = val;}
  void _33(double val){m_Matrix._33 = val;}

};

#endif
