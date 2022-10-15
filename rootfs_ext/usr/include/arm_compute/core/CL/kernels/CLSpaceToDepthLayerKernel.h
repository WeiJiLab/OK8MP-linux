/*
 * Copyright (c) 2019-2020 ARM Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ARM_COMPUTE_CLSPACETODEPTHLAYERKERNEL_H
#define ARM_COMPUTE_CLSPACETODEPTHLAYERKERNEL_H

#include "arm_compute/core/CL/ICLKernel.h"
#include "arm_compute/core/Types.h"

namespace arm_compute
{
class ICLTensor;

/** Interface for the space to depth kernel */
class CLSpaceToDepthLayerKernel : public ICLKernel
{
public:
    /** Default constructor */
    CLSpaceToDepthLayerKernel();
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    CLSpaceToDepthLayerKernel(const CLSpaceToDepthLayerKernel &) = delete;
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    CLSpaceToDepthLayerKernel &operator=(const CLSpaceToDepthLayerKernel &) = delete;
    /** Allow instances of this class to be moved */
    CLSpaceToDepthLayerKernel(CLSpaceToDepthLayerKernel &&) = default;
    /** Allow instances of this class to be moved */
    CLSpaceToDepthLayerKernel &operator=(CLSpaceToDepthLayerKernel &&) = default;
    /** Default destructor */
    ~CLSpaceToDepthLayerKernel() = default;
    /** Initialise the kernel's inputs and output.
     *
     * @param[in]  input       Tensor input. Supported tensor rank: 4. Data types supported: All.
     * @param[out] output      Tensor output. Data types supported: same as @p input
     * @param[in]  block_shape Block shape value.
     */
    void configure(const ICLTensor *input, ICLTensor *output, int32_t block_shape);
    /** Static function to check if given info will lead to a valid configuration of @ref CLSpaceToDepthLayerKernel.
     *
     * @param[in] input       Tensor input info. Supported tensor rank: 4. Data types supported: All.
     * @param[in] output      Tensor output info. Data types supported: same as @p input
     * @param[in] block_shape Block shape value.
     *
     * @return a status
     */
    static Status validate(const ITensorInfo *input, const ITensorInfo *output, int32_t block_shape);

    // Inherited methods overridden:
    void run(const Window &window, cl::CommandQueue &queue) override;

private:
    const ICLTensor *_input;       /**< Source tensor */
    ICLTensor       *_output;      /**< Destination tensor */
    int32_t          _block_shape; /**< Block shape */
};
} // namespace arm_compute
#endif /* ARM_COMPUTE_CLSPACETODEPTHLAYERKERNEL_H */
