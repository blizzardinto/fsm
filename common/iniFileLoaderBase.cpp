/*
 * 阅读提示：配置读取基类，封装文件流、去注释和数值转换。
 * ParamLoader 复用读取能力，再定义足球需要哪些参数，展示了基类能力与业务派生类的分工。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "iniFileLoaderBase.h"
using std::string;


// 从一行配置文字中去掉注释。
void removeCommentingFromLine(string& line)
{
   // 查找注释起点。
   string::size_type idx = line.find('//');

   if (idx != string::npos)
   {
     // 删除注释起点之后的文字。
     line = line.substr(0, idx);
   }
}
// 顺序读取下一个有效参数，忽略空行和注释，并返回参数值字符串。
string IniFileLoaderBase::getNextParameter()
{

  // 保存下一条参数所在的文本行。
  std::string line;

  std::getline(file, line);

  removeCommentingFromLine(line);

  // 空行不包含参数，继续读取下一行。
  if (line.length() == 0)
  {
    return getNextParameter();
  }

  getParameterValueAsString(line);

  return line;
}


// 去掉参数名称，只保留参数值字符串。
void IniFileLoaderBase::getParameterValueAsString(string& line)
{
  // 查找参数名称的起点。
  string::size_type begIdx;
  string::size_type endIdx;

  // 定义用于分隔名称和值的字符。
  const string delims(" \;=,");

  begIdx = line.find_first_not_of(delims);

  // 查找参数名称的结束位置。
  if (begIdx != string::npos)
  {
    endIdx = line.find_first_of(delims, begIdx);

    // 没有分隔符时，以行尾作为结束位置。
    if (endIdx == string::npos)
    {
      endIdx = line.length();
    }
  }

  // 查找参数值的起点。
  begIdx = line.find_first_not_of(delims, endIdx);
  // 查找参数值的结束位置。
  if(begIdx != string::npos)
  {
    endIdx = line.find_first_of(delims, begIdx);

    // 没有分隔符时，以行尾作为结束位置。
    if (endIdx == string::npos)
    {
      endIdx = line.length();
    }
  }

  line = line.substr(begIdx, endIdx);
}

// 忽略注释，提取下一段以分隔符划分的字符串。
std::string IniFileLoaderBase::getNextToken()
{
  // 先从当前行中去掉注释。
  while (currentLine.length() == 0)
  {
    std::getline(file, currentLine);

    removeCommentingFromLine(currentLine);
  }

   // 查找字符串的起点。
  string::size_type begIdx;
  string::size_type endIdx;

  // 定义分隔字符。
  const string delims(" \;=,");

  begIdx = currentLine.find_first_not_of(delims);

  // 查找字符串的结束位置。
  if (begIdx != string::npos)
  {
    endIdx = currentLine.find_first_of(delims, begIdx);

    // 没有分隔符时，以行尾作为结束位置。
    if (endIdx == string::npos)
    {
      endIdx = currentLine.length();
    }
  }

  string s = currentLine.substr(begIdx, endIdx);

  if (endIdx != currentLine.length())
  {
    // 从当前行中移除已经取出的字符串。
    currentLine = currentLine.substr(endIdx+1, currentLine.length());
  }

  else { currentLine = "";}

  return s;

}

