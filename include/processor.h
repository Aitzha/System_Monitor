#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

 private:
    long pIdle = 0;
    long pNonIdle = 0;
    long Idle;
    long NonIdle;
};

#endif