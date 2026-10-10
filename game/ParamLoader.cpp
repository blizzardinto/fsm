#include "ParamLoader.h"

ParamLoader* ParamLoader::instance()
{
  static ParamLoader instance;

  return &instance;
}