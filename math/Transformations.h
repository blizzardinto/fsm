/*
 * 阅读提示：二维坐标变换辅助函数，连接对象局部坐标与球场世界坐标。
 * 轮廓在局部坐标中定义一次，再按对象位置和朝向变换，便于复用同一绘图形状。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H
#include <vector>

#include "Vector2D.h"
#include "C2DMatrix.h"


// 根据位置、朝向和缩放，把局部顶点变换为世界坐标。
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                    const Vector2D   &pos,
                                    const Vector2D   &forward,
                                    const Vector2D   &side,
                                    const Vector2D   &scale);

// 根据位置和朝向，把局部顶点变换为世界坐标。
std::vector<Vector2D> worldTransform(std::vector<Vector2D> &points,
                                     const Vector2D   &pos,
                                     const Vector2D   &forward,
                                     const Vector2D   &side);

// 把局部位置变换为世界位置。
Vector2D pointToWorldSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition);

// 把局部方向变换为世界方向，不包含平移。
Vector2D vectorToWorldSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide);


// 把世界位置变换为局部位置。
Vector2D pointToLocalSpace(const Vector2D &point,
                           const Vector2D &agentHeading,
                           const Vector2D &agentSide,
                           const Vector2D &agentPosition);

// 把世界方向变换为局部方向。
Vector2D vectorToLocalSpace(const Vector2D &vec,
                            const Vector2D &agentHeading,
                            const Vector2D &agentSide);

// 围绕原点旋转向量，角度使用弧度。
void vec2DRotateAroundOrigin(Vector2D& v, double ang);

// 在给定视角中生成等角度间隔的探测线终点。
std::vector<Vector2D> createWhiskers(unsigned int  numWhiskers,
                                     double        whiskerLength,
                                     double        fov,
                                     Vector2D      facing,
                                     Vector2D      origin);


#endif