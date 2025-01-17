// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \file Utils.h
/// \brief
///

#ifndef ITSTRACKINGGPU_UTILS_H_
#define ITSTRACKINGGPU_UTILS_H_

#include "GPUCommonDef.h"
#include "Stream.h"

namespace o2
{
namespace its
{
template <typename T1, typename T2>
struct gpuPair {
  T1 first;
  T2 second;
};

namespace gpu
{
// Poor man implementation of a span-like struct. It is very limited.
template <typename T>
struct gpuSpan {
  using value_type = T;
  using ptr = T*;
  using ref = T&;

  GPUd() gpuSpan() : _data(nullptr), _size(0) {}
  GPUd() gpuSpan(ptr data, unsigned int dim) : _data(data), _size(dim) {}
  GPUd() ref operator[](unsigned int idx) const { return _data[idx]; }
  GPUd() unsigned int size() const { return _size; }
  GPUd() bool empty() const { return _size == 0; }
  GPUd() ref front() const { return _data[0]; }
  GPUd() ref back() const { return _data[_size - 1]; }
  GPUd() ptr begin() const { return _data; }
  GPUd() ptr end() const { return _data + _size; }

 protected:
  ptr _data;
  unsigned int _size;
};

template <typename T>
struct gpuSpan<const T> {
  using value_type = T;
  using ptr = const T*;
  using ref = const T&;

  GPUd() gpuSpan() : _data(nullptr), _size(0) {}
  GPUd() gpuSpan(ptr data, unsigned int dim) : _data(data), _size(dim) {}
  GPUd() gpuSpan(const gpuSpan<T>& other) : _data(other._data), _size(other._size) {}
  GPUd() ref operator[](unsigned int idx) const { return _data[idx]; }
  GPUd() unsigned int size() const { return _size; }
  GPUd() bool empty() const { return _size == 0; }
  GPUd() ref front() const { return _data[0]; }
  GPUd() ref back() const { return _data[_size - 1]; }
  GPUd() ptr begin() const { return _data; }
  GPUd() ptr end() const { return _data + _size; }

 protected:
  ptr _data;
  unsigned int _size;
};

enum class Task {
  Tracker = 0,
  Vertexer = 1
};

template <class T>
GPUhd() T* getPtrFromRuler(int index, T* src, const int* ruler, const int stride = 1)
{
  return src + ruler[index] * stride;
}

template <class T>
GPUhd() const T* getPtrFromRuler(int index, const T* src, const int* ruler, const int stride = 1)
{
  return src + ruler[index] * stride;
}

GPUh() void gpuThrowOnError();

namespace utils
{
#ifdef __CUDACC__
void checkGPUError(const cudaError_t error, const char* file = __FILE__, const int line = __LINE__);
#endif
#ifdef __HIPCC__
void checkGPUError(const hipError_t error, const char* file = __FILE__, const int line = __LINE__);
#endif

// Dump device properties
void getDeviceProp(int, bool verbose = true);

dim3 getBlockSize(const int);
dim3 getBlockSize(const int, const int);
dim3 getBlockSize(const int, const int, const int);
dim3 getBlocksGrid(const dim3&, const int);
dim3 getBlocksGrid(const dim3&, const int, const int);

void gpuMalloc(void**, const int);
void gpuFree(void*);
void gpuMemset(void*, int, int);
void gpuMemcpyHostToDevice(void*, const void*, int);
void gpuMemcpyDeviceToHost(void*, const void*, int);
void gpuMemcpyToSymbol(const void* symbol, const void* src, int size);
void gpuMemcpyFromSymbol(void* dst, const void* symbol, int size);

GPUd() int getLaneIndex();
GPUd() int shareToWarp(const int, const int);
} // namespace utils
} // namespace gpu
} // namespace its
} // namespace o2

#endif