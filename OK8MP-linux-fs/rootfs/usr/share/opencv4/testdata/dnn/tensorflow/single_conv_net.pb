
!
inputPlaceholder*
dtype0
j
conv2d/kernelConst*E
value<B:"$@???@Y?\X?ܫy???=???<?H?T??G?????*
dtype0
D
conv2d/biasConst*!
valueB"]e???????>*
dtype0
?
conv2d/convolutionConv2Dinputconv2d/kernel*
use_cudnn_on_gpu(*
T0*
strides
*
data_formatNHWC*
paddingVALID
Z
conv2d/BiasAddBiasAddconv2d/convolutionconv2d/bias*
data_formatNHWC*
T0
,
conv2d/ReluReluconv2d/BiasAdd*
T0