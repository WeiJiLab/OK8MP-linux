
!
inputPlaceholder*
dtype0
^
)Flatten_1/flatten/Reshape/shape/_1__cf__1Const*
valueB"   ????*
dtype0
\
'Flatten/flatten/Reshape/shape/_0__cf__0Const*
valueB"   ????*
dtype0
?
conv2d/kernelConst*
dtype0*a
valueXBV"@???,?E>?f?d?B>??s.???S]?@9<?=V=jL?S?1?T#Ͼ??w?*?>@L?? ?=
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
H
conv2d/biasConst*%
valueB"                *
dtype0
U
conv2d/BiasAddBiasAddconv2d/Conv2Dconv2d/bias*
T0*
data_formatNHWC
r
Flatten/flatten/ReshapeReshapeconv2d/BiasAdd'Flatten/flatten/Reshape/shape/_0__cf__0*
T0*
Tshape0
?
conv2d_1/kernelConst*a
valueXBV"@`ŵ?????;<?T?j>0? ??<???>?&i>?	?>???
W??Ǿ,?V?Q??N?(?*
dtype0
?
conv2d_1/Conv2DConv2Dinput:0conv2d_1/kernel*
strides
*
data_formatNHWC*
use_cudnn_on_gpu(*
paddingVALID*
	dilations
*
T0
J
conv2d_1/biasConst*%
valueB"                *
dtype0
[
conv2d_1/BiasAddBiasAddconv2d_1/Conv2Dconv2d_1/bias*
T0*
data_formatNHWC
x
Flatten_1/flatten/ReshapeReshapeconv2d_1/BiasAdd)Flatten_1/flatten/Reshape/shape/_1__cf__1*
Tshape0*
T0
5
concat/axisConst*
value	B :*
dtype0
q
concatConcatV2Flatten/flatten/ReshapeFlatten_1/flatten/Reshapeconcat/axis*
T0*
N*

Tidx0
?
BiasAdd/biasesConst*?
value?B?0"?                                                                                                                                                                                                *
dtype0
R
BiasAdd/BiasAddBiasAddconcatBiasAdd/biases*
data_formatNHWC*
T0 