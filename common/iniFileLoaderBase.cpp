#include "iniFileLoaderBase.h"
using std::string;


//removes any commenting from a line of text
void removeCommentingFromLine(string& line)
{
   //search for any comment and remove
   string::size_type idx = line.find('//');

   if (idx != string::npos)
   {
     //cut out the comment
     line = line.substr(0, idx);
   }
}
//----------------------- getNextParameter ------------------------------------
//
//  searches the text file for the next valid parameter. Discards any comments
//  and returns the value as a string
//-----------------------------------------------------------------------------
string IniFileLoaderBase::getNextParameter()
{

  //this will be the string that holds the bext parameter
  std::string line;

  std::getline(file, line);

  removeCommentingFromLine(line);

  //if the line is of zero length, get the next line from
  //the file
  if (line.length() == 0)
  {
    return getNextParameter();
  }

  getParameterValueAsString(line);

  return line;
}


//-------------------------- getParameterValueAsString ------------------------
//
// given a line of text this function removes the parameter description
// and returns just the parameter as a std::string
//-----------------------------------------------------------------------------
void IniFileLoaderBase::getParameterValueAsString(string& line)
{
  //find beginning of parameter description
  string::size_type begIdx;
  string::size_type endIdx;

  //define some delimiters
  const string delims(" \;=,");

  begIdx = line.find_first_not_of(delims);

  //find the end of the parameter description
  if (begIdx != string::npos)
  {
    endIdx = line.find_first_of(delims, begIdx);

    //end of word is the end of the line
    if (endIdx == string::npos)
    {
      endIdx = line.length();
    }
  }

  //find the beginning of the parameter value
  begIdx = line.find_first_not_of(delims, endIdx);
  //find the end of the parameter value
  if(begIdx != string::npos)
  {
    endIdx = line.find_first_of(delims, begIdx);

    //end of word is the end of the line
    if (endIdx == string::npos)
    {
      endIdx = line.length();
    }
  }

  line = line.substr(begIdx, endIdx);
}

//--------------------------- getNextToken ------------------------------------
//
//  ignores any commenting and gets the next string
//-----------------------------------------------------------------------------
std::string IniFileLoaderBase::getNextToken()
{
  //strip the line of any commenting
  while (currentLine.length() == 0)
  {
    std::getline(file, currentLine);

    removeCommentingFromLine(currentLine);
  }

   //find beginning of parameter description
  string::size_type begIdx;
  string::size_type endIdx;

  //define some delimiters
  const string delims(" \;=,");

  begIdx = currentLine.find_first_not_of(delims);

  //find the end of the parameter description
  if (begIdx != string::npos)
  {
    endIdx = currentLine.find_first_of(delims, begIdx);

    //end of word is the end of the line
    if (endIdx == string::npos)
    {
      endIdx = currentLine.length();
    }
  }

  string s = currentLine.substr(begIdx, endIdx);

  if (endIdx != currentLine.length())
  {
    //strip the token from the line
    currentLine = currentLine.substr(endIdx+1, currentLine.length());
  }

  else { currentLine = "";}

  return s;

}

