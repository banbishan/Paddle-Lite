/* Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.

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
#include <stdio.h>

#include "lite/fpga/KD/llapi/zynqmp_api.h"

namespace paddle {
namespace zynqmp {

inline int align_image(int wc) { return align_to_x(wc, IMAGE_ALIGNMENT); }

}  // namespace zynqmp
}  // namespace paddle
