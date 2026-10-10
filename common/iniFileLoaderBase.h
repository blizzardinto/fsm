/*
 * 阅读提示：配置读取基类，封装文件流、去注释和数值转换。
 * ParamLoader 复用读取能力，再定义足球需要哪些参数，展示了基类能力与业务派生类的分工。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef INIFILELOADERBASE
#define INIFILELOADERBASE
#pragma warning(disable:4800)

#include <fstream>
#include <string>
#include <cassert>


class IniFileLoaderBase
{

private:

  // 输入文件流保存配置文件的读取状态。
  std::ifstream file;

  std::string   currentLine;

  void        getParameterValueAsString(std::string& line);

  std::string getNextParameter();

  // 忽略注释，查找下一个字符串片段。
  std::string getNextToken();

  // 标记配置文件是否有效。
  bool        mGoodFile;

public:

  // 辅助方法把顺序读出的参数转换成对应的数值或布尔类型。
  double      getNextParameterDouble(){if (mGoodFile) return atof(getNextParameter().c_str());throw std::runtime_error("bad file");}
  float       getNextParameterFloat(){if (mGoodFile) return (float)atof(getNextParameter().c_str());throw std::runtime_error("bad file");}
  int         getNextParameterInt(){if (mGoodFile) return atoi(getNextParameter().c_str());throw std::runtime_error("bad file");}
  bool        getNextParameterBool(){return (bool)(atoi(getNextParameter().c_str()));throw std::runtime_error("bad file");}

  double      getNextTokenAsDouble(){if (mGoodFile) return atof(getNextToken().c_str()); throw std::runtime_error("bad file");}
  float       getNextTokenAsFloat(){if (mGoodFile) return (float)atof(getNextToken().c_str()); throw std::runtime_error("bad file");}
  int         getNextTokenAsInt(){if (mGoodFile) return atoi(getNextToken().c_str()); throw std::runtime_error("bad file");}
  std::string getNextTokenAsString(){if (mGoodFile) return getNextToken(); throw std::runtime_error("bad file");}

  bool        eof()const{if (mGoodFile) return file.eof(); throw std::runtime_error("bad file");}
  bool        fileIsGood()const{return mGoodFile;}

  IniFileLoaderBase(char* filename):currentLine(""), mGoodFile(true)
  {
    file.open(filename);

    if (!file){mGoodFile = false;}
  }

};






#endif



