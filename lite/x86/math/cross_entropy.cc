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

#include "lite/x86/math/cross_entropy.h"

namespace paddle {
namespace lite {
namespace x86 {
namespace math {

template <typename T,
          int MajorType = Eigen::RowMajor,
          typename IndexType = Eigen::DenseIndex>
using EigenMatrix = lite::fluid::EigenMatrix<T, MajorType, IndexType>;

template <typename T>
class CrossEntropyFunctor<lite::TargetType::kX86, T> {
 public:
  void operator()(const lite::X86Context& ctx,
                  lite::Tensor* out,
                  const lite::Tensor* prob,
                  const lite::Tensor* labels,
                  const bool softLabel,
                  const int ignore_index,
                  const int axis_dim) {
    const int batch_size = prob->dims()[0];
    const int num_classes = prob->dims()[1];
    const int num_remain = num_classes / axis_dim;

    Eigen::DSizes<int, 3> batch_axis_remain(batch_size, axis_dim, num_remain);

    if (softLabel) {
      auto in = EigenMatrix<T>::From(*prob);
      auto lbl = EigenMatrix<T>::From(*labels);
      auto loss = EigenMatrix<T>::From(*out);

      loss.device(lite::fluid::EigenDeviceType<lite::TargetType::kX86>()) =
          -((lbl * in.log().unaryExpr(math::TolerableValue<T>()))
                .reshape(batch_axis_remain)
                .sum(Eigen::DSizes<int, 1>(1)));
    } else {
      const T* prob_data = prob->data<T>();
      T* loss_data = out->mutable_data<T>();

      const int64_t* label_data = labels->data<int64_t>();
      for (int i = 0; i < batch_size; ++i) {
        for (int j = 0; j < num_remain; j++) {
          int lbl = label_data[i * num_remain + j];
          PADDLE_ENFORCE((lbl >= 0 && lbl < axis_dim) || lbl == ignore_index);
          int index = i * num_classes + lbl * num_remain + j;
          int loss_idx = i * num_remain + j;
          loss_data[loss_idx] =
              lbl == ignore_index
                  ? 0
                  : -math::TolerableValue<T>()(std::log(prob_data[index]));
        }
      }
    }
  }
};

template class CrossEntropyFunctor<lite::TargetType::kX86, float>;
template class CrossEntropyFunctor<lite::TargetType::kX86, double>;
}  // namespace math
}  // namespace x86
}  // namespace lite
}  // namespace paddle
