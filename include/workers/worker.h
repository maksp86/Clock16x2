#ifndef WORKERS_WORKER_H_INCLUDED
#define WORKERS_WORKER_H_INCLUDED

enum InternetNeedState
{
  NO_NEED,
  NEED_WIFI,
  NEED_INTERNET
};

class Worker
{

public:
  virtual InternetNeedState InternetNeed() = 0;
  virtual char* Name() = 0;
  virtual void Setup() = 0;
  virtual void Update() = 0;
  virtual void OnConfigStart() = 0;
  virtual void OnConfigSave() = 0;
  virtual ~Worker() = default;
};

#endif