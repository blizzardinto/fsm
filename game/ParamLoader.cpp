/*
 * 阅读提示：参数加载对象，复用配置读取基类，将按顺序读取的值保存为比赛参数。
 * 当前使用单例共享配置；文件顺序必须与读取顺序一致，参数名称本身不参与查找。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "ParamLoader.h"

ParamLoader* ParamLoader::instance()
{
  static ParamLoader instance;

  return &instance;
}