/*
 * 阅读提示：二维坐标变换辅助函数，连接对象局部坐标与球场世界坐标。
 * 轮廓在局部坐标中定义一次，再按对象位置和朝向变换，便于复用同一绘图形状。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "Transformations.h"

// 根据缩放、朝向和位置，将局部轮廓顶点转换到世界坐标。
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                    const Vector2D   &pos,
                                    const Vector2D   &forward,
                                    const Vector2D   &side,
                                    const Vector2D   &scale)
{
  // 复制原始顶点，保留对象的局部轮廓不变。
  std::vector<Vector2D> tranVector2Ds = points;

  // 创建用于组合变换的矩阵对象。
  C2DMatrix matTransform;

  // 先缩放局部轮廓。
  if ( (scale.x != 1.0) || (scale.y != 1.0) )
  {
    matTransform.scale(scale.x, scale.y);
  }

  // 再根据对象朝向旋转。
  matTransform.rotate(forward, side);

  // 最后平移到世界位置。
  matTransform.translate(pos.x, pos.y);

  // 将组合矩阵作用于所有顶点。
  matTransform.transformVector2Ds(tranVector2Ds);

  return tranVector2Ds;
}

// 根据朝向和位置，将局部顶点转换到世界坐标。
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                     const Vector2D   &pos,
                                     const Vector2D   &forward,
                                     const Vector2D   &side)
{
  // 复制原始顶点，避免修改原始轮廓。
  std::vector<Vector2D> tranVector2Ds = points;

  // 创建变换矩阵。
  C2DMatrix matTransform;

  // 根据对象朝向旋转。
  matTransform.rotate(forward, side);

  // 平移到对象的世界位置。
  matTransform.translate(pos.x, pos.y);

  // 变换顶点容器中的所有坐标。
  matTransform.transformVector2Ds(tranVector2Ds);

  return tranVector2Ds;
}

// 将局部坐标点转换为世界坐标，包含旋转和平移。
Vector2D pointToWorldSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition)
{
  // 复制输入坐标，避免修改调用者的数据。
  Vector2D transPoint = point;

  // 创建变换矩阵。
  C2DMatrix matTransform;

  // 根据局部坐标轴旋转。
  matTransform.rotate(agentHeading, agentSide);

  // 加上对象所在的世界位置。
  matTransform.translate(agentPosition.x, agentPosition.y);

  // 变换坐标并返回结果。
  matTransform.transformVector2Ds(transPoint);

  return transPoint;
}

// 将局部方向向量转换为世界方向；方向没有位置，因此不做平移。
Vector2D vectorToWorldSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide)
{
  // 复制输入向量。
  Vector2D transVec = vec;

  // 创建变换矩阵。
  C2DMatrix matTransform;

  // 根据坐标轴旋转方向。
  matTransform.rotate(agentHeading, agentSide);

  // 变换方向向量。
  matTransform.transformVector2Ds(transVec);

  return transVec;
}


// 将世界坐标点转换为对象的局部坐标。
Vector2D pointToLocalSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition)
{

  // 复制输入坐标。
  Vector2D transPoint = point;

  // 创建变换矩阵。
  C2DMatrix matTransform;

  double tx = -agentPosition.dot(agentHeading);
  double ty = -agentPosition.dot(agentSide);

  // 根据局部坐标轴和原点设置逆向坐标变换。
  matTransform.element11(agentHeading.x); matTransform.element12(agentSide.x);
  matTransform.element21(agentHeading.y); matTransform.element22(agentSide.y);
  matTransform.element31(tx);             matTransform.element32(ty);

  // 变换坐标并返回结果。
  matTransform.transformVector2Ds(transPoint);

  return transPoint;
}

// 将世界方向向量转换为对象的局部方向。
Vector2D vectorToLocalSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide)
{

  // 复制输入向量。
  Vector2D transPoint = vec;

  // 创建变换矩阵。
  C2DMatrix matTransform;

  // 根据局部坐标轴构造方向变换，不包含平移。
  matTransform.element11(agentHeading.x); matTransform.element12(agentSide.x);
  matTransform.element21(agentHeading.y); matTransform.element22(agentSide.y);

  // 变换向量并返回结果。
  matTransform.transformVector2Ds(transPoint);

  return transPoint;
}

// 围绕原点将向量旋转指定弧度；弧度是代码中使用的角度单位。
void vec2DRotateAroundOrigin(Vector2D& v, double ang)
{
  // 创建旋转矩阵。
  C2DMatrix mat;

  // 设置旋转角度。
  mat.rotate(ang);

  // 将矩阵作用于输入向量。
  mat.transformVector2Ds(v);
}

// 在给定视角范围内生成等角度间隔的探测线，返回各探测线的终点。
std::vector<Vector2D> createWhiskers(unsigned int  numWhiskers,
                                     double        whiskerLength,
                                     double        fov,
                                     Vector2D      facing,
                                     Vector2D      origin)
{
  // 计算相邻两条探测线之间的角度。
  double sectorSize = fov/(double)(numWhiskers-1);

  std::vector<Vector2D> whiskers;
  Vector2D temp;
  double angle = -fov*0.5;

  for (unsigned int w=0; w<numWhiskers; ++w)
  {
    // 根据当前角度生成一条探测线终点。
    temp = facing;
    vec2DRotateAroundOrigin(temp, angle);
    whiskers.push_back(origin + whiskerLength * temp);

    angle+=sectorSize;
  }

  return whiskers;
}
