
#
input_4Placeholder*
dtype0
x
conv2d_5/kernelConst*Q
valueHBF"06??>``=`1ݼ??? ??< ??=?K???>2<???l?D?ξ??>*
dtype0
J
conv2d_5/biasConst*
dtype0*%
valueB"                
?
conv2d_6/convolutionConv2Dinput_4conv2d_5/kernel*
use_cudnn_on_gpu(*
strides
*
data_formatNHWC*
T0*
paddingVALID
`
conv2d_6/BiasAddBiasAddconv2d_6/convolutionconv2d_5/bias*
data_formatNHWC*
T0
]
Pad/paddingsConst*
dtype0*9
value0B."                                
D
PadPadconv2d_6/BiasAddPad/paddings*
	Tpaddings0*
T0
5
concat/axisConst*
dtype0*
value	B :
K
concatConcatV2Padinput_4concat/axis*
N*
T0*

Tidx0