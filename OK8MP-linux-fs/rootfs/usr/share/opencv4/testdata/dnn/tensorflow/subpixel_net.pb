
M
input_imagePlaceholder*
data_formatNHWC*
dtype0*
shape:
W
&SUBPIXEL/SUBPIXEL/subpixel_image/ShapeShapeinput_image:0*
out_type0*
T0
Z
0SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape/3Const*
value	B :*
dtype0
Z
0SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape/2Const*
value	B :*
dtype0
Z
0SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape/1Const*
value	B :*
dtype0
V
,SUBPIXEL/SUBPIXEL/subpixel_image/concat/axisConst*
value	B :*
dtype0
Z
0SUBPIXEL/SUBPIXEL/subpixel_image/split/split_dimConst*
value	B :*
dtype0
?
&SUBPIXEL/SUBPIXEL/subpixel_image/splitSplit0SUBPIXEL/SUBPIXEL/subpixel_image/split/split_diminput_image:0*
T0*
	num_split
?
'SUBPIXEL/SUBPIXEL/subpixel_image/concatConcatV2&SUBPIXEL/SUBPIXEL/subpixel_image/split(SUBPIXEL/SUBPIXEL/subpixel_image/split:1,SUBPIXEL/SUBPIXEL/subpixel_image/concat/axis*

Tidx0*
T0*
N
d
6SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice/stack_2Const*
valueB:*
dtype0
d
6SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice/stack_1Const*
valueB:*
dtype0
b
4SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice/stackConst*
valueB: *
dtype0
?
.SUBPIXEL/SUBPIXEL/subpixel_image/strided_sliceStridedSlice&SUBPIXEL/SUBPIXEL/subpixel_image/Shape4SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice/stack6SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice/stack_16SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice/stack_2*
shrink_axis_mask*
ellipsis_mask *

begin_mask *
new_axis_mask *
end_mask *
Index0*
T0
?
.SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shapePack.SUBPIXEL/SUBPIXEL/subpixel_image/strided_slice0SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape/10SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape/20SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape/3*

axis *
N*
T0
?
(SUBPIXEL/SUBPIXEL/subpixel_image/ReshapeReshape'SUBPIXEL/SUBPIXEL/subpixel_image/concat.SUBPIXEL/SUBPIXEL/subpixel_image/Reshape/shape*
T0*
Tshape0
h
)SUBPIXEL/SUBPIXEL/subpixel_image/IdentityIdentity(SUBPIXEL/SUBPIXEL/subpixel_image/Reshape*
T0 