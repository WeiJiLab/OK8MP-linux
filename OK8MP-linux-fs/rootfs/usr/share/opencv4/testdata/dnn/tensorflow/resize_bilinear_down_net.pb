
P
inputPlaceholder*
dtype0*-
shape$:"??????????????????
0
ShapeShapeinput:0*
T0*
out_type0
K
truediv_1/Cast_1/_1__cf__1Const*
valueB 2       @*
dtype0
I
truediv/Cast_1/_0__cf__0Const*
valueB 2      @*
dtype0
E
strided_slice_1/stack_2Const*
dtype0*
valueB:
E
strided_slice_1/stack_1Const*
valueB:*
dtype0
C
strided_slice_1/stackConst*
valueB:*
dtype0
?
strided_slice_1StridedSliceShapestrided_slice_1/stackstrided_slice_1/stack_1strided_slice_1/stack_2*
shrink_axis_mask*

begin_mask *
ellipsis_mask *
new_axis_mask *
end_mask *
Index0*
T0
O
truediv_1/CastCaststrided_slice_1*

SrcT0*
Truncate( *

DstT0
I
	truediv_1RealDivtruediv_1/Casttruediv_1/Cast_1/_1__cf__1*
T0
M
resize_down/Cast_1Cast	truediv_1*

DstT0*

SrcT0*
Truncate( 
C
strided_slice/stack_2Const*
valueB:*
dtype0
C
strided_slice/stack_1Const*
dtype0*
valueB:
A
strided_slice/stackConst*
dtype0*
valueB:
?
strided_sliceStridedSliceShapestrided_slice/stackstrided_slice/stack_1strided_slice/stack_2*
T0*
Index0*
shrink_axis_mask*
ellipsis_mask *

begin_mask *
new_axis_mask *
end_mask 
K
truediv/CastCaststrided_slice*
Truncate( *

DstT0*

SrcT0
C
truedivRealDivtruediv/Casttruediv/Cast_1/_0__cf__0*
T0
I
resize_down/CastCasttruediv*

SrcT0*
Truncate( *

DstT0
\
resize_down/sizePackresize_down/Castresize_down/Cast_1*
T0*

axis *
N
@
conv2d/biasConst*
valueB"        *
dtype0
V
conv2d/kernelConst*
dtype0*1
value(B&"N ľ?̋>f|??X??>
?
conv2d/Conv2DConv2Dinput:0conv2d/kernel*
	dilations
*
T0*
strides
*
data_formatNHWC*
use_cudnn_on_gpu(*
explicit_paddings
 *
paddingVALID
U
conv2d/BiasAddBiasAddconv2d/Conv2Dconv2d/bias*
data_formatNHWC*
T0
?
resize_down/ResizeBilinearResizeBilinearconv2d/BiasAddresize_down/size*
T0*
align_corners( *
half_pixel_centers(  