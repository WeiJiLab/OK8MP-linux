
E
input_1Placeholder* 
shape:?????????*
dtype0
B
Flatten_1/flatten/ShapeShapeinput_1*
T0*
out_type0
S
%Flatten_1/flatten/strided_slice/stackConst*
valueB: *
dtype0
U
'Flatten_1/flatten/strided_slice/stack_1Const*
valueB:*
dtype0
U
'Flatten_1/flatten/strided_slice/stack_2Const*
valueB:*
dtype0
?
Flatten_1/flatten/strided_sliceStridedSliceFlatten_1/flatten/Shape%Flatten_1/flatten/strided_slice/stack'Flatten_1/flatten/strided_slice/stack_1'Flatten_1/flatten/strided_slice/stack_2*
end_mask *
Index0*
T0*
shrink_axis_mask*

begin_mask *
ellipsis_mask *
new_axis_mask 
T
!Flatten_1/flatten/Reshape/shape/1Const*
valueB :
?????????*
dtype0
?
Flatten_1/flatten/Reshape/shapePackFlatten_1/flatten/strided_slice!Flatten_1/flatten/Reshape/shape/1*
T0*

axis *
N
e
Flatten_1/flatten/ReshapeReshapeinput_1Flatten_1/flatten/Reshape/shape*
Tshape0*
T0 