
:
inputPlaceholder*
dtype0*
shape:
N
Flatten/flatten/ShapeConst*
dtype0*!
valueB"         
Q
#Flatten/flatten/strided_slice/stackConst*
valueB: *
dtype0
S
%Flatten/flatten/strided_slice/stack_1Const*
valueB:*
dtype0
S
%Flatten/flatten/strided_slice/stack_2Const*
valueB:*
dtype0
?
Flatten/flatten/strided_sliceStridedSliceFlatten/flatten/Shape#Flatten/flatten/strided_slice/stack%Flatten/flatten/strided_slice/stack_1%Flatten/flatten/strided_slice/stack_2*
Index0*
T0*
shrink_axis_mask*

begin_mask *
ellipsis_mask *
new_axis_mask *
end_mask 
R
Flatten/flatten/Reshape/shape/1Const*
valueB :
?????????*
dtype0
?
Flatten/flatten/Reshape/shapePackFlatten/flatten/strided_sliceFlatten/flatten/Reshape/shape/1*
T0*

axis *
N
_
Flatten/flatten/ReshapeReshapeinputFlatten/flatten/Reshape/shape*
T0*
Tshape0 