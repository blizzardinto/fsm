#include "C2DMatrix.h"

//multiply two matrices together
void C2DMatrix::matrixMultiply(Matrix &mIn)
{
  C2DMatrix::Matrix matTemp;

  //first row
  matTemp.element11 = (mMatrix.element11*mIn.element11) + (mMatrix.element12*mIn.element21) + (mMatrix.element13*mIn.element31);
  matTemp.element12 = (mMatrix.element11*mIn.element12) + (mMatrix.element12*mIn.element22) + (mMatrix.element13*mIn.element32);
  matTemp.element13 = (mMatrix.element11*mIn.element13) + (mMatrix.element12*mIn.element23) + (mMatrix.element13*mIn.element33);

  //second
  matTemp.element21 = (mMatrix.element21*mIn.element11) + (mMatrix.element22*mIn.element21) + (mMatrix.element23*mIn.element31);
  matTemp.element22 = (mMatrix.element21*mIn.element12) + (mMatrix.element22*mIn.element22) + (mMatrix.element23*mIn.element32);
  matTemp.element23 = (mMatrix.element21*mIn.element13) + (mMatrix.element22*mIn.element23) + (mMatrix.element23*mIn.element33);

  //third
  matTemp.element31 = (mMatrix.element31*mIn.element11) + (mMatrix.element32*mIn.element21) + (mMatrix.element33*mIn.element31);
  matTemp.element32 = (mMatrix.element31*mIn.element12) + (mMatrix.element32*mIn.element22) + (mMatrix.element33*mIn.element32);
  matTemp.element33 = (mMatrix.element31*mIn.element13) + (mMatrix.element32*mIn.element23) + (mMatrix.element33*mIn.element33);

  mMatrix = matTemp;
}

//applies a 2D transformation matrix to a std::vector of Vector2Ds
void C2DMatrix::transformVector2Ds(std::vector<Vector2D> &vPoint)
{
  for (unsigned int i=0; i<vPoint.size(); ++i)
  {
    double tempX =(mMatrix.element11*vPoint[i].x) + (mMatrix.element21*vPoint[i].y) + (mMatrix.element31);

    double tempY = (mMatrix.element12*vPoint[i].x) + (mMatrix.element22*vPoint[i].y) + (mMatrix.element32);

    vPoint[i].x = tempX;

    vPoint[i].y = tempY;
  }
}

//applies a 2D transformation matrix to a single Vector2D
void C2DMatrix::transformVector2Ds(Vector2D &vPoint)
{
  double tempX =(mMatrix.element11*vPoint.x) + (mMatrix.element21*vPoint.y) + (mMatrix.element31);

  double tempY = (mMatrix.element12*vPoint.x) + (mMatrix.element22*vPoint.y) + (mMatrix.element32);

  vPoint.x = tempX;

  vPoint.y = tempY;
}

//create an identity matrix
void C2DMatrix::identity()
{
  mMatrix.element11 = 1; mMatrix.element12 = 0; mMatrix.element13 = 0;

  mMatrix.element21 = 0; mMatrix.element22 = 1; mMatrix.element23 = 0;

  mMatrix.element31 = 0; mMatrix.element32 = 0; mMatrix.element33 = 1;
}

//create a transformation matrix
void C2DMatrix::translate(double x, double y)
{
  Matrix mat;

  mat.element11 = 1; mat.element12 = 0; mat.element13 = 0;

  mat.element21 = 0; mat.element22 = 1; mat.element23 = 0;

  mat.element31 = x;    mat.element32 = y;    mat.element33 = 1;

  //and multiply
  matrixMultiply(mat);
}

//create a scale matrix
void C2DMatrix::scale(double xScale, double yScale)
{
  C2DMatrix::Matrix mat;

  mat.element11 = xScale; mat.element12 = 0; mat.element13 = 0;

  mat.element21 = 0; mat.element22 = yScale; mat.element23 = 0;

  mat.element31 = 0; mat.element32 = 0; mat.element33 = 1;

  //and multiply
  matrixMultiply(mat);
}

//create a rotation matrix
void C2DMatrix::rotate(double rot)
{
  C2DMatrix::Matrix mat;

  double rotationSin = sin(rot);
  double rotationCos = cos(rot);

  mat.element11 = rotationCos;  mat.element12 = rotationSin; mat.element13 = 0;

  mat.element21 = -rotationSin; mat.element22 = rotationCos; mat.element23 = 0;

  mat.element31 = 0; mat.element32 = 0;mat.element33 = 1;

  //and multiply
  matrixMultiply(mat);
}

//create a rotation matrix from a 2D vector
void C2DMatrix::rotate(const Vector2D &fwd, const Vector2D &side)
{
  C2DMatrix::Matrix mat;

  mat.element11 = fwd.x;  mat.element12 = fwd.y; mat.element13 = 0;

  mat.element21 = side.x; mat.element22 = side.y; mat.element23 = 0;

  mat.element31 = 0; mat.element32 = 0;mat.element33 = 1;

  //and multiply
  matrixMultiply(mat);
}
