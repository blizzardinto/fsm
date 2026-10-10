/*
 * 阅读提示：二维变换矩阵对象，封装平移、缩放和旋转的组合。
 * 类内部保存矩阵元素，对外提供变换方法；调用者无需手动修改每个元素。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "C2DMatrix.h"

// 矩阵相乘，把多个二维变换组合成一个变换。
void C2DMatrix::matrixMultiply(Matrix &mIn)
{
  C2DMatrix::Matrix matTemp;

  // 计算结果矩阵的第一行。
  matTemp.element11 = (mMatrix.element11*mIn.element11) + (mMatrix.element12*mIn.element21) + (mMatrix.element13*mIn.element31);
  matTemp.element12 = (mMatrix.element11*mIn.element12) + (mMatrix.element12*mIn.element22) + (mMatrix.element13*mIn.element32);
  matTemp.element13 = (mMatrix.element11*mIn.element13) + (mMatrix.element12*mIn.element23) + (mMatrix.element13*mIn.element33);

  // 计算结果矩阵的第二行。
  matTemp.element21 = (mMatrix.element21*mIn.element11) + (mMatrix.element22*mIn.element21) + (mMatrix.element23*mIn.element31);
  matTemp.element22 = (mMatrix.element21*mIn.element12) + (mMatrix.element22*mIn.element22) + (mMatrix.element23*mIn.element32);
  matTemp.element23 = (mMatrix.element21*mIn.element13) + (mMatrix.element22*mIn.element23) + (mMatrix.element23*mIn.element33);

  // 计算结果矩阵的第三行。
  matTemp.element31 = (mMatrix.element31*mIn.element11) + (mMatrix.element32*mIn.element21) + (mMatrix.element33*mIn.element31);
  matTemp.element32 = (mMatrix.element31*mIn.element12) + (mMatrix.element32*mIn.element22) + (mMatrix.element33*mIn.element32);
  matTemp.element33 = (mMatrix.element31*mIn.element13) + (mMatrix.element32*mIn.element23) + (mMatrix.element33*mIn.element33);

  mMatrix = matTemp;
}

// 用当前矩阵逐个变换顶点容器中的坐标。
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

// 用当前矩阵变换一个二维坐标。
void C2DMatrix::transformVector2Ds(Vector2D &vPoint)
{
  double tempX =(mMatrix.element11*vPoint.x) + (mMatrix.element21*vPoint.y) + (mMatrix.element31);

  double tempY = (mMatrix.element12*vPoint.x) + (mMatrix.element22*vPoint.y) + (mMatrix.element32);

  vPoint.x = tempX;

  vPoint.y = tempY;
}

// 单位矩阵表示不改变原坐标。
void C2DMatrix::identity()
{
  mMatrix.element11 = 1; mMatrix.element12 = 0; mMatrix.element13 = 0;

  mMatrix.element21 = 0; mMatrix.element22 = 1; mMatrix.element23 = 0;

  mMatrix.element31 = 0; mMatrix.element32 = 0; mMatrix.element33 = 1;
}

// 构造平移矩阵。
void C2DMatrix::translate(double x, double y)
{
  Matrix mat;

  mat.element11 = 1; mat.element12 = 0; mat.element13 = 0;

  mat.element21 = 0; mat.element22 = 1; mat.element23 = 0;

  mat.element31 = x;    mat.element32 = y;    mat.element33 = 1;

  // 与已有矩阵相乘，叠加本次变换；变换顺序会影响结果。
  matrixMultiply(mat);
}

// 构造缩放矩阵。
void C2DMatrix::scale(double xScale, double yScale)
{
  C2DMatrix::Matrix mat;

  mat.element11 = xScale; mat.element12 = 0; mat.element13 = 0;

  mat.element21 = 0; mat.element22 = yScale; mat.element23 = 0;

  mat.element31 = 0; mat.element32 = 0; mat.element33 = 1;

  // 与已有矩阵相乘，叠加缩放。
  matrixMultiply(mat);
}

// 根据角度构造旋转矩阵。
void C2DMatrix::rotate(double rot)
{
  C2DMatrix::Matrix mat;

  double rotationSin = sin(rot);
  double rotationCos = cos(rot);

  mat.element11 = rotationCos;  mat.element12 = rotationSin; mat.element13 = 0;

  mat.element21 = -rotationSin; mat.element22 = rotationCos; mat.element23 = 0;

  mat.element31 = 0; mat.element32 = 0;mat.element33 = 1;

  // 与已有矩阵相乘，叠加旋转。
  matrixMultiply(mat);
}

// 根据朝向和侧向量构造旋转矩阵。
void C2DMatrix::rotate(const Vector2D &fwd, const Vector2D &side)
{
  C2DMatrix::Matrix mat;

  mat.element11 = fwd.x;  mat.element12 = fwd.y; mat.element13 = 0;

  mat.element21 = side.x; mat.element22 = side.y; mat.element23 = 0;

  mat.element31 = 0; mat.element32 = 0;mat.element33 = 1;

  // 与已有矩阵相乘，叠加方向变换。
  matrixMultiply(mat);
}
