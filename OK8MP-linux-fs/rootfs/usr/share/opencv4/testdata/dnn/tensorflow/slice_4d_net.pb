
!
inputPlaceholder*
dtype0
R
conv2d/kernelConst*-
value$B""N ľ?̋>f|??*
dtype0
?
conv2d/Conv2DConv2Dinputconv2d/kernel*
paddingSAME*
T0*
data_formatNHWC*
strides
*
use_cudnn_on_gpu(
D
conv2d/biasConst*!
valueB"            *
dtype0
U
conv2d/BiasAddBiasAddconv2d/Conv2Dconv2d/bias*
T0*
data_formatNHWC
H
Slice/beginConst*
dtype0*%
valueB"              
G

Slice/sizeConst*%
valueB"????         *
dtype0
M
SliceSliceconv2d/BiasAddSlice/begin
Slice/size*
Index0*
T0 