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

    double element11, element12, element13;
    double element21, element22, element23;
    double element31, element32, element33;

    Matrix()
    {
      element11=0.0; element12=0.0; element13=0.0;
      element21=0.0; element22=0.0; element23=0.0;
      element31=0.0; element32=0.0; element33=0.0;
    }

  };

  Matrix mMatrix;

  //multiplies mMatrix with mIn
  void  matrixMultiply(Matrix &mIn);


public:

  C2DMatrix()
  {
    //initialize the matrix to an identity matrix
    identity();
  }

  //create an identity matrix
  void identity();

  //create a transformation matrix
  void translate(double x, double y);

  //create a scale matrix
  void scale(double xScale, double yScale);

  //create a rotation matrix
  void  rotate(double rotation);

  //create a rotation matrix from a fwd and side 2D vector
  void  rotate(const Vector2D &fwd, const Vector2D &side);

   //applys a transformation matrix to a std::vector of points
  void transformVector2Ds(std::vector<Vector2D> &vPoints);

  //applys a transformation matrix to a point
  void transformVector2Ds(Vector2D &vPoint);

  //accessors to the matrix elements
  void element11(double val){mMatrix.element11 = val;}
  void element12(double val){mMatrix.element12 = val;}
  void element13(double val){mMatrix.element13 = val;}

  void element21(double val){mMatrix.element21 = val;}
  void element22(double val){mMatrix.element22 = val;}
  void element23(double val){mMatrix.element23 = val;}

  void element31(double val){mMatrix.element31 = val;}
  void element32(double val){mMatrix.element32 = val;}
  void element33(double val){mMatrix.element33 = val;}

};

#endif
