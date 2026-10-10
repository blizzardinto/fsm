/*
 * 阅读提示：流转换辅助函数，使用模板复用不同数值类型的读取与输出。
 * 模板参数在编译时决定类型；与虚函数的运行时多态是两种不同机制。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef STREAM_UTILITY_FUNCTIONS
#define STREAM_UTILITY_FUNCTIONS
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <stdexcept>


// 模板函数把支持流输出的任意类型转换成字符串。
template <class T>
inline std::string ttos(const T& t, int precision = 2)
{
  std::ostringstream buffer;

  buffer << std::fixed << std::setprecision(precision) << t;

  return buffer.str();
}

// 把布尔值转换成表示真或假的字符串。
inline std::string btos(bool b)
{
  if (b) return "true";
  return "false";
}

// 从输入文件流中读取一个指定类型的值。
template <typename T>
inline T getValueFromStream(std::ifstream& stream)
{
  T val;

  stream >> val;

  // 检查提取是否成功；类型不匹配或流错误时抛出异常。
  if (!stream)
  {
    throw std::runtime_error("Attempting to retrieve wrong type from stream");
  }

  return val;
}


#endif