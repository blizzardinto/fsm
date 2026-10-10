/*
 * 阅读提示：通用数值工具，提供随机数、范围限制和浮点比较。
 * 无须维护对象状态的操作用普通函数即可；调用者通过参数传入数据、通过返回值得到结果。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef UTILS_H
#define UTILS_H
#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cassert>
#include <iomanip>



// 常用数学常量。
const int     maxInt    = (std::numeric_limits<int>::max)();
const double  maxDouble = (std::numeric_limits<double>::max)();
const double  minDouble = (std::numeric_limits<double>::min)();
const float   maxFloat  = (std::numeric_limits<float>::max)();
const float   minFloat  = (std::numeric_limits<float>::min)();

const double   pi        = 3.14159;
const double   twoPi     = pi * 2;
const double   halfPi    = pi / 2;
const double   quarterPi = pi / 4;

// 判断数值是否不是有效数字。
template <typename T>
inline bool isNaN(T val)
{
  return val != val;
}

inline double degsToRads(double degs)
{
  return twoPi * (degs/360.0);
}



// 在容差范围内判断数值是否为零。
inline bool isZero(double val)
{
  return ( (-minDouble < val) && (val < minDouble) );
}

// 判断第三个参数是否位于前两个参数给出的区间。
inline bool inRange(double start, double end, double val)
{
  if (start < end)
  {
    if ( (val > start) && (val < end) ) return true;
    else return false;
  }

  else
  {
    if ( (val < start) && (val > end) ) return true;
    else return false;
  }
}

template <class T>
T maximum(const T& v1, const T& v2)
{
  return v1 > v2 ? v1 : v2;
}



// 随机数辅助函数：返回指定整数范围内的随机值。
inline int   randInt(int x,int y) {return rand()%(y-x+1)+x;}

// 返回零到一之间的随机小数。
inline double randFloat()      {return ((rand())/(RAND_MAX+1.0));}

inline double randInRange(double x, double y)
{
  return x + randFloat()*(y-x);
}

// 返回随机布尔值。
inline bool   randBool()
{
  if (randInt(0,1)) return true;

  else return false;
}

// 返回负一到一之间的随机小数。
inline double randomClamped()    {return randFloat() - randFloat();}


// 生成正态分布随机数。算法参考：http://www.taygeta.com/random/gaussian.html。
inline double randGaussian(double mean = 0.0, double standardDeviation = 1.0)
{
  double x1, x2, w, y1;
  static double y2;
  static int useLast = 0;

  if (useLast)		        /* 使用上一次调用缓存的另一个随机值。 */
  {
    y1 = y2;
    useLast = 0;
  }
  else
  {
    do
    {
      x1 = 2.0 * randFloat() - 1.0;
      x2 = 2.0 * randFloat() - 1.0;
      w = x1 * x1 + x2 * x2;
    }
    while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    useLast = 1;
  }

  return( mean + y1 * standardDeviation );
}



// 常用数值辅助函数。


inline double sigmoid(double input, double response = 1.0)
{
  return ( 1.0 / ( 1.0 + exp(-input / response)));
}


// 返回两个值中较大的值。
template <class T>
inline T maxOf(const T& a, const T& b)
{
  if (a>b) return a; return b;
}

// 返回两个值中较小的值。
template <class T>
inline T minOf(const T& a, const T& b)
{
  if (a<b) return a; return b;
}


// 把第一个参数限制在后两个参数给出的范围内。
template <class T, class U, class V>
inline void clamp(T& arg, const U& minVal, const V& maxVal)
{
  assert ( (minVal < maxVal) && "<Clamp>MaxVal < MinVal!");

  if (arg < (T)minVal)
  {
    arg = (T)minVal;
  }

  if (arg > (T)maxVal)
  {
    arg = (T)maxVal;
  }
}


// 对小数进行四舍五入。
inline int rounded(double val)
{
  int    integral = (int)val;
  double mantissa = val - integral;

  if (mantissa < 0.5)
  {
    return integral;
  }

  else
  {
    return integral + 1;
  }
}

// 根据指定的小数部分阈值决定向上还是向下取整。
inline int roundUnderOffset(double val, double offset)
{
  int    integral = (int)val;
  double mantissa = val - integral;

  if (mantissa < offset)
  {
    return integral;
  }

  else
  {
    return integral + 1;
  }
}

// 在容差范围内比较两个实数，避免直接相等比较受浮点误差影响。
inline bool isEqual(float a, float b)
{
  if (fabs(a-b) < 1E-12)
  {
    return true;
  }

  return false;
}

inline bool isEqual(double a, double b)
{
  if (fabs(a-b) < 1E-12)
  {
    return true;
  }

  return false;
}


template <class T>
inline double average(const std::vector<T>& v)
{
  double average = 0.0;

  for (unsigned int i=0; i < v.size(); ++i)
  {
    average += (double)v[i];
  }

  return average / (double)v.size();
}


inline double standardDeviation(const std::vector<double>& v)
{
  double sd      = 0.0;
  double mean = average(v);

  for (unsigned int i=0; i<v.size(); ++i)
  {
    sd += (v[i] - mean) * (v[i] - mean);
  }

  sd = sd / v.size();

  return sqrt(sd);
}


template <class container>
inline void deleteStlContainer(container& c)
{
  for (typename container::iterator it = c.begin(); it!=c.end(); ++it)
  {
    delete *it;
    *it = NULL;
  }
}

template <class map>
inline void deleteStlMap(map& m)
{
  for (typename map::iterator it = m.begin(); it!=m.end(); ++it)
  {
    delete it->second;
    it->second = NULL;
  }
}





#endif