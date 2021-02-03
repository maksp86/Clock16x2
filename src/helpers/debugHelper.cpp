#include "helpers/debugHelper.h"

#include "Arduino.h"

void DebugHelper::Log(char* topic, char* msg)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);
}

void DebugHelper::Log(char* topic, const char* msg)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);
}

void DebugHelper::Log(Worker* worker, char* msg)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(worker->Name());
  Serial.print(": ");
  Serial.println(msg);
}

void DebugHelper::Log(Worker* worker, const char* msg)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(worker->Name());
  Serial.print(": ");
  Serial.println(msg);
}


void DebugHelper::Log(char* topic, int val)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(val);
}


void DebugHelper::Log(Worker* worker, int val)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(worker->Name());
  Serial.print(": ");
  Serial.println(val);
}
