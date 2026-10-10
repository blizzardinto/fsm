/*
 * 阅读提示：二维变换矩阵对象，封装平移、缩放和旋转的组合。
 * 类内部保存矩阵元素，对外提供变换方法；调用者无需手动修改每个元素。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
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

  // 将内部矩阵与传入矩阵相乘。
  void  matrixMultiply(Matrix &mIn);


public:

  C2DMatrix()
  {
    // 构造时设置为单位矩阵。
    identity();
  }

  // 重置为不改变坐标的单位矩阵。
  void identity();

  // 叠加平移变换。
  void translate(double x, double y);

  // 叠加缩放变换。
  void scale(double xScale, double yScale);

  // 根据角度叠加旋转变换。
  void  rotate(double rotation);

  // 根据朝向与侧向量叠加旋转变换。
  void  rotate(const Vector2D &fwd, const Vector2D &side);

   // 变换容器中的所有顶点。
  void transformVector2Ds(std::vector<Vector2D> &vPoints);

  // 变换单个坐标。
  void transformVector2Ds(Vector2D &vPoint);

  // 设置矩阵元素的接口。
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
