// #pragma once
#ifndef __PERF_COUNTER_H__
#define __PERF_COUNTER_H__

#include <stdint.h>
 
uint64_t perf_counter();
// uint32_t lo, hi;  // 是否要把这个局部变量优化掉

// __inline__ uint64_t perf_counter() // 这里的__inline__在此平台上，会crash，但是加了打印，就🆗，太扯了
uint64_t perf_counter()
{
  uint32_t lo, hi;
  __asm__ __volatile__ (
      "rdtscp" : "=a"(lo), "=d"(hi)
      );
  return ((uint64_t)lo) | (((uint64_t)hi) << 32);
}
#endif