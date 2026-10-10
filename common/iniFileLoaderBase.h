#ifndef INIFILELOADERBASE
#define INIFILELOADERBASE
#pragma warning(disable:4800)

#include <fstream>
#include <string>
#include <cassert>


class IniFileLoaderBase
{

private:

  //the file the parameters are stored in
  std::ifstream file;

  std::string   currentLine;

  void        getParameterValueAsString(std::string& line);

  std::string getNextParameter();

  //this ignores any comments and finds the next delimited string
  std::string getNextToken();

  //this is set to true if the file specified by the user is valid
  bool        mGoodFile;

public:

  //helper methods. They convert the next parameter value found into the
  //relevant type
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



