
#
input_3Placeholder*
dtype0
?
conv2d_3/kernelConst*
dtype0*a
valueXBV"@?;????>?ZL?r? ? 
???^?>,?T???Z>|??~>??>??*??˽??A?A'G?????
J
conv2d_3/biasConst*%
valueB"PT?=??9? ?l>f?s?*
dtype0
?
conv2d_4/convolutionConv2Dinput_3conv2d_3/kernel*
use_cudnn_on_gpu(*
strides
*
data_formatNHWC*
T0*
paddingVALID
`
conv2d_4/BiasAddBiasAddconv2d_4/convolutionconv2d_3/bias*
T0*
data_formatNHWC
0
conv2d_4/TanhTanhconv2d_4/BiasAdd*
T0
?
conv2d_4/kernelConst*
dtype0*a
valueXBV"@?+?>d|%??ϯ?*X?>?????c???>?X?>?=Ѿ?ߵI????]??%.;?V?>??=
J
conv2d_4/biasConst*%
valueB"???j???}9P???*
dtype0
?
conv2d_5/convolutionConv2Dinput_3conv2d_4/kernel*
use_cudnn_on_gpu(*
strides
*
data_formatNHWC*
T0*
paddingVALID
`
conv2d_5/BiasAddBiasAddconv2d_5/convolutionconv2d_4/bias*
data_formatNHWC*
T0
6
conv2d_5/SigmoidSigmoidconv2d_5/BiasAdd*
T0
2
mul/yConst*
valueB
 *R??>*
dtype0
#
mulMulinput_3mul/y*
T0
4
mul_1/xConst*
dtype0*
valueB
 *   @
-
mul_1Mulmul_1/xconv2d_4/Tanh*
T0

addAddmulmul_1*
T0
,
mul_2Muladdconv2d_5/Sigmoid*
T0