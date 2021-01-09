#ifndef HELPERS_DEBUG_HELPER_H

#define HELPERS_DEBUG_HELPER_H

#include "../workers/worker.h"

class DebugHelper
{
public:
  static void Log(char* topic, char* msg);
  static void Log(char* topic, const char* msg);
  static void Log(Worker* worker, char* msg);
  static void Log(Worker* worker, const char* msg);

private:
  DebugHelper() {};
};

#endif