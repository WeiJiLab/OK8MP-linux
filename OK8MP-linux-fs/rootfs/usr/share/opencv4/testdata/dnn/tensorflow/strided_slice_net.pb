
!
inputPlaceholder*
dtype0
R
strided_slice/stack_2Const*%
valueB"            *
dtype0
R
strided_slice/stack_1Const*%
valueB"              *
dtype0
P
strided_slice/stackConst*%
valueB"              *
dtype0
H
conv2d/biasConst*%
valueB"                *
dtype0
?
conv2d/kernelConst*a
valueXBV"@???,?E>?f?d?B>??s.???S]?@9<?=V=jL?S?1?T#Ͼ??w?*?>@L?? ?=*
dtype0
?
conv2d/Conv2DConv2Dinput:0conv2d/kernel*
use_cudnn_on_gpu(*
paddingVALID*
	dilations
*
T0*
strides
*
data_formatNHWC
U
conv2d/BiasAddBiasAddconv2d/Conv2Dconv2d/bias*
T0*
data_formatNHWC
?
strided_sliceStridedSliceconv2d/BiasAddstrided_slice/stackstrided_slice/stack_1strided_slice/stack_2*
shrink_axis_mask *

begin_mask*
ellipsis_mask *
new_axis_mask *
end_mask*
Index0*
T0 