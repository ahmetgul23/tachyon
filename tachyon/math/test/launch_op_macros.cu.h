#ifndef TACHYON_MATH_FINITE_FIELDS_KERNELS_TEST_LAUNCH_OP_MACROS_H_
#define TACHYON_MATH_FINITE_FIELDS_KERNELS_TEST_LAUNCH_OP_MACROS_H_

#include "tachyon/device/gpu/gpu_device_functions.h"
#include "tachyon/device/gpu/gpu_logging.h"

#define DEFINE_LAUNCH_UNARY_OP(thread_num, method, type, result_type)   \
  gpuError_t Launch##method(const type* x, result_type* result,         \
                            size_t count) {                             \
    ::tachyon::math::kernels::                                          \
        method<<<(count - 1) / thread_num + 1, thread_num>>>(x, result, \
                                                             count);    \
    gpuError_t error = gpuGetLastError();                               \
    GPU_LOG_IF(ERROR, error != gpuSuccess, error)                       \
        << "Failed to " #method "()";                                   \
    error = gpuDeviceSynchronize();                                     \
    GPU_LOG_IF(ERROR, error != gpuSuccess, error)                       \
        << "Failed to gpuDeviceSynchronize()";                          \
    return error;                                                       \
  }

#define DEFINE_LAUNCH_BINARY_OP(thread_num, method, type, result_type)         \
  gpuError_t Launch##method(const type* x, const type* y, result_type* result, \
                            size_t count) {                                    \
    ::tachyon::math::kernels::                                                 \
        method<<<(count - 1) / thread_num + 1, thread_num>>>(x, y, result,     \
                                                             count);           \
    gpuError_t error = gpuGetLastError();                                      \
    GPU_LOG_IF(ERROR, error != gpuSuccess, error)                              \
        << "Failed to " #method "()";                                          \
    error = gpuDeviceSynchronize();                                            \
    GPU_LOG_IF(ERROR, error != gpuSuccess, error)                              \
        << "Failed to gpuDeviceSynchronize()";                                 \
    return error;                                                              \
  }

#endif  // TACHYON_MATH_FINITE_FIELDS_KERNELS_TEST_LAUNCH_OP_MACROS_H_