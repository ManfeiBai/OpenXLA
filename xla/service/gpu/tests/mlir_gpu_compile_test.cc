/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/service/gpu/tests/mlir_gpu_test_base.h"
#include "tsl/lib/core/status_test_util.h"
#include "tsl/platform/test.h"

namespace xla {
namespace gpu {

// Tests XLA GPU compilation using MLIR LMHLO dialect as the input.
class CompileTest : public MlirGpuTestBase {};

TEST_F(CompileTest, InvalidCollectivePermuteOp) {
  const char* mlir_text = R"(
      func.func @main(
         %arg0: memref<4xf32> {lmhlo.params = 0 : index},
          %arg1: memref<4xf32> {lmhlo.output_index = dense<[0]> : tensor<1xindex>}) -> () {
          %token = "lmhlo_gpu.collective_permute_start"(%arg0, %arg1) {
             source_target_pairs = dense<[[0, 1, 2]]> : tensor<1x3xi64>,
             is_sync = true
             } : (memref<4xf32>, memref<4xf32>) -> (!mhlo.token)
          "func.return" () : () -> ()
      })";
  auto executable = CompileMlirText(mlir_text);
  ASSERT_FALSE(executable.ok());
  EXPECT_THAT(executable.status().message(),
              ::testing::HasSubstr("expect source_target_pairs attribute of "
                                   "shape (N, 2), but got (1, 3)"));
}

}  // namespace gpu
}  // namespace xla
