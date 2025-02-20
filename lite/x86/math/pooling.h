/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include <vector>
#include "lite/core/context.h"
#include "lite/core/tensor.h"
#include "lite/fluid/eigen.h"
#include "lite/utils/macros.h"

namespace paddle {
namespace lite {
namespace x86 {
namespace math {

/*
 * \brief Extracting simple operations from pooling.
 *        Both MaxPool and AvgPool need "initial", "compute" and "finalize"
 * operation.
 *        MaxPool initializes temp variable to the negative maximum to find the
 * maximum value in the pooling field.
 *        AvgPool initializes temp variable to the zero to accumulate all values
 * in pool pooling, and finally takes the average.
 *        MaxPoolGrad and AvgPoolGrad are gradient operations respectively.
 */
template <class T>
class MaxPool {
 public:
  DEVICE inline T initial() { return static_cast<T>(-FLT_MAX); }
  DEVICE inline void compute(const T& x, T* y) { *y = *y > x ? *y : x; }
  DEVICE inline void finalize(const T& pool_field, T* y) {}
};

template <class T>
class AvgPool {
 public:
  DEVICE inline T initial() { return static_cast<T>(0); }
  DEVICE inline void compute(const T& x, T* y) { *y += x; }
  DEVICE inline void finalize(const T& pool_field, T* y) { *y /= pool_field; }
};

template <class T>
class MaxPoolGrad {
 public:
  DEVICE inline void compute(
      const T& x, const T& y, const T& dy, T scale, T* dx) {
    *dx += dy * (x == y);
  }
};

template <class T>
class AvgPoolGrad {
 public:
  DEVICE inline void compute(
      const T& x, const T& y, const T& dy, T scale, T* dx) {
    *dx += (scale * dy);
  }
};

/* used for adaptive pool to calculate start and end index of each divided grid
 */
HOSTDEVICE inline int AdaptStartIndex(int ph, int input_size, int output_size) {
  return static_cast<int>(
      floor(static_cast<double>(ph * input_size) / output_size));
}

HOSTDEVICE inline int AdaptEndIndex(int ph, int input_size, int output_size) {
  return static_cast<int>(
      ceil(static_cast<double>((ph + 1) * input_size) / output_size));
}

/*
 * \brief Getting pooling results, and calculating gradient.
 *
 * In pool2d, all tensors are in NCHW format. Where N is batch size, C is the
 * number of channels, H and W is the height and width of feature.
 * In pool3d, all tensors are in NCDHW format. Where N is batch size, C is the
 * number of channels, D, H and W is the depth, height and width of feature.
 *
 * In max pooling, it is possible that the pooling region has multiple maximum
 * elements. In this case, we should compute the gradient of the first maximum
 * element.
 * This is different from average pooling. So we rewrite the max_pool_grad:
 * MaxPool2dGradFunctor, MaxPool3dGradFunctor.
 */
//#ifdef PADDLE_WITH_CUDA
// template <typename PoolProcess, typename T>
// class Pool2dDirectCUDAFunctor {
// public:
//  void operator()(const T* input, const std::vector<int>& input_shape,
//                  const std::vector<int>& output_shape,
//                  const std::vector<int>& ksize,
//                  const std::vector<int>& strides,
//                  const std::vector<int>& paddings, PoolProcess pool_compute,
//                  bool exclusive, T* output, cudaStream_t stream);
//};
//#endif

template <lite::TargetType Target, typename PoolProcess, typename T>
class Pool2dFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  PoolProcess pool_compute,
                  bool exclusive,
                  bool adaptive,
                  lite::Tensor* output);
};

template <lite::TargetType Target, typename PoolProcess, typename T>
class Pool2dGradFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const lite::Tensor& output,
                  const lite::Tensor& output_grad,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  PoolProcess pool_compute,
                  bool exclusive,
                  bool adaptive,
                  lite::Tensor* input_grad);
};

template <lite::TargetType Target, class T>
class MaxPool2dGradFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const lite::Tensor& output,
                  const lite::Tensor& output_grad,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  lite::Tensor* input_grad);
};

template <lite::TargetType Target, typename PoolProcess, typename T>
class Pool3dFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  PoolProcess pool_compute,
                  bool exclusive,
                  bool adaptive,
                  lite::Tensor* output);
};

template <lite::TargetType Target, typename PoolProcess, typename T>
class Pool3dGradFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const lite::Tensor& output,
                  const lite::Tensor& output_grad,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  PoolProcess pool_compute,
                  bool exclusive,
                  bool adaptive,
                  lite::Tensor* input_grad);
};

template <lite::TargetType Target, class T>
class MaxPool3dGradFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const lite::Tensor& output,
                  const lite::Tensor& output_grad,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  lite::Tensor* input_grad);
};

/*
 * \brief Getting max pooling results and corresponding max index, and
 * calculating gradient.
 * In up-sampling-pooling, it is necessary to know max element index.
 * In pool2d, all tensors are in NCHW format. In pool3d, all tensors are in
 * NCDHW format.
 */
template <lite::TargetType Target, typename T1, typename T2>
class MaxPool2dWithIndexFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  bool adaptive,
                  lite::Tensor* output,
                  lite::Tensor* mask);
};

template <lite::TargetType Target, typename T1, typename T2>
class MaxPool2dWithIndexGradFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& output_grad,
                  const lite::Tensor& mask,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  bool adaptive,
                  lite::Tensor* input_grad);
};

template <lite::TargetType Target, typename T1, typename T2>
class MaxPool3dWithIndexFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& input,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  bool adaptive,
                  lite::Tensor* output,
                  lite::Tensor* mask);
};

template <lite::TargetType Target, typename T1, typename T2>
class MaxPool3dWithIndexGradFunctor {
 public:
  void operator()(const lite::Context<Target>& context,
                  const lite::Tensor& output_grad,
                  const lite::Tensor& mask,
                  const std::vector<int>& ksize,
                  const std::vector<int>& strides,
                  const std::vector<int>& paddings,
                  bool adaptive,
                  lite::Tensor* input_grad);
};

}  // namespace math
}  // namespace x86
}  // namespace lite
}  // namespace paddle
