//
//  SecConvOp.hpp
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//


#include <stdio.h>

#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/kernels/matmul_op.h>

using namespace tensorflow;
using tensorflow::functor::MatMulFunctor;

template <typename Device, typename T>
class SecMatmulOp : public OpKernel {
public:
    explicit SecMatmulOp(OpKernelConstruction* ctx)
                        : OpKernel(ctx) {
    }
    
    void Compute(OpKernelContext* context) override {
        
        const Tensor& input_tensor0 = context->input(0);
        const Tensor& input_tensor1 = context->input(1);

        Tensor* output_tensor = NULL;
        OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor0.shape(),
                                                         &output_tensor));
        
        
        //
        OP_REQUIRES(context, input_tensor0.NumElements() <= tensorflow::kint32max,
                    errors::InvalidArgument("Too many elements in tensor"));
        MatMulFunctor<Device, T>()(context->eigen_device<Device>(),
                                   static_cast<int>(input_tensor0.NumElements()),
                                   input_tensor0.flat<T>().data(),
                                   output_tensor->flat<T>().data());
    }
};
