
5

isTrainingPlaceholder*
shape:*
dtype0

!
inputPlaceholder*
dtype0
j
conv2d/kernelConst*E
value<B:"$???Kd>?X?@?`>???ܾ???Z?<?bw=*
dtype0
D
conv2d/biasConst*
dtype0*!
valueB"            
?
conv2d/convolutionConv2Dinputconv2d/kernel*
paddingVALID*
T0*
data_formatNHWC*
strides
*
use_cudnn_on_gpu(
Z
conv2d/BiasAddBiasAddconv2d/convolutionconv2d/bias*
T0*
data_formatNHWC
.
DropoutDropoutconv2d/BiasAdd
isTraining

ReluReluDropout*
T0