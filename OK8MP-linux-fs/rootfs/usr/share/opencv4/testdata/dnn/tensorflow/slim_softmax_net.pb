
!
inputPlaceholder*
dtype0
J
softmax/Reshape/shapeConst*
valueB"????   *
dtype0
D
conv2d/biasConst*!
valueB"            *
dtype0
v
conv2d/kernelConst*Q
valueHBF"0?)???[S>???hP>?	??a̾??l?@l?<?e=R6Z?
B>??pݾ*
dtype0
?
conv2d/Conv2DConv2Dinput:0conv2d/kernel*
	dilations
*
T0*
strides
*
data_formatNHWC*
use_cudnn_on_gpu(*
paddingVALID
U
conv2d/BiasAddBiasAddconv2d/Conv2Dconv2d/bias*
data_formatNHWC*
T0
?
softmax/ShapeShapeconv2d/BiasAdd*
T0*
out_type0
X
softmax/ReshapeReshapeconv2d/BiasAddsoftmax/Reshape/shape*
T0*
Tshape0
4
softmax/SoftmaxSoftmaxsoftmax/Reshape*
T0
S
softmax/Reshape_1Reshapesoftmax/Softmaxsoftmax/Shape*
T0*
Tshape0 