
$
input_13Placeholder*
dtype0
g
conv2d_14/kernelConst*?
value6B4j???\???V???p?k?????h???o*
dtype0
?
conv2d_14/biasConst*
valueBj    *
dtype0
?
conv2d_15/convolutionConv2Dinput_13conv2d_14/kernel*
strides
*
data_formatNHWC*
use_cudnn_on_gpu(*
paddingVALID*
T0
c
conv2d_15/BiasAddBiasAddconv2d_15/convolutionconv2d_14/bias*
T0*
data_formatNHWC
_
Pad_1/paddingsConst*9
value0B."                                *
dtype0
I
Pad_1Padconv2d_15/BiasAddPad_1/paddings*
T0*
	Tpaddings0
7
concat_1/axisConst*
value	B :*
dtype0
R
concat_1ConcatV2Pad_1input_13concat_1/axis*
T0*
N*

Tidx0