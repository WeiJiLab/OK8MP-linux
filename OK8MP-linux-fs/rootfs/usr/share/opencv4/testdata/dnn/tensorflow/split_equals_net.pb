
!
inputPlaceholder*
dtype0
9
split/split_dimConst*
value	B :*
dtype0
@
splitSplitsplit/split_diminput*
	num_split*
T0
5
concat/axisConst*
dtype0*
value	B :
V
concatConcatV2split:2split:1splitconcat/axis*
N*
T0*

Tidx0
;
split_1/split_dimConst*
value	B : *
dtype0
E
split_1Splitsplit_1/split_dimconcat*
	num_split*
T0
R
conv2d/kernelConst*-
value$B""??8<Rh[?<?6?*
dtype0
?
conv2d/convolutionConv2Dsplit_1conv2d/kernel*
paddingVALID*
T0*
data_formatNHWC*
strides
*
use_cudnn_on_gpu(
<
conv2d/biasConst*
valueB*    *
dtype0
Z
conv2d/BiasAddBiasAddconv2d/convolutionconv2d/bias*
data_formatNHWC*
T0
T
conv2d_1/kernelConst*-
value$B""??????????*
dtype0
?
conv2d_2/convolutionConv2D	split_1:1conv2d_1/kernel*
use_cudnn_on_gpu(*
T0*
data_formatNHWC*
strides
*
paddingVALID
>
conv2d_1/biasConst*
valueB*    *
dtype0
`
conv2d_2/BiasAddBiasAddconv2d_2/convolutionconv2d_1/bias*
data_formatNHWC*
T0
5
addAddconv2d/BiasAddconv2d_2/BiasAdd*
T0 