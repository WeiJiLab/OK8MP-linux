
G
inputPlaceholder*$
shape:?????????*
dtype0
.
ShapeShapeinput*
T0*
out_type0
A
strided_slice/stackConst*
valueB:*
dtype0
C
strided_slice/stack_1Const*
valueB:*
dtype0
C
strided_slice/stack_2Const*
valueB:*
dtype0
?
strided_sliceStridedSliceShapestrided_slice/stackstrided_slice/stack_1strided_slice/stack_2*
shrink_axis_mask*
ellipsis_mask *

begin_mask *
new_axis_mask *
end_mask *
Index0*
T0
/
mul/yConst*
value	B :*
dtype0
)
mulMulstrided_slicemul/y*
T0
0
Shape_1Shapeinput*
T0*
out_type0
C
strided_slice_1/stackConst*
valueB:*
dtype0
E
strided_slice_1/stack_1Const*
valueB:*
dtype0
E
strided_slice_1/stack_2Const*
valueB:*
dtype0
?
strided_slice_1StridedSliceShape_1strided_slice_1/stackstrided_slice_1/stack_1strided_slice_1/stack_2*
shrink_axis_mask*

begin_mask *
ellipsis_mask *
new_axis_mask *
end_mask *
T0*
Index0
1
mul_1/yConst*
value	B :*
dtype0
/
mul_1Mulstrided_slice_1mul_1/y*
T0
M
resize_bilinear_factor/sizePackmulmul_1*
T0*

axis *
N
j
resize_bilinear_factorResizeBilinearinputresize_bilinear_factor/size*
T0*
align_corners( 
2
sub/yConst*
valueB
 *???>*
dtype0
2
subSubresize_bilinear_factorsub/y*
T0
2
add/yConst*
valueB
 *???>*
dtype0

addAddsubadd/y*
T0 