
4
keras_upsampling2d_inputPlaceholder*
dtype0
V
keras_upsampling2d/ShapeShapekeras_upsampling2d_input:0*
T0*
out_type0
T
&keras_upsampling2d/strided_slice/stackConst*
valueB:*
dtype0
V
(keras_upsampling2d/strided_slice/stack_1Const*
valueB:*
dtype0
V
(keras_upsampling2d/strided_slice/stack_2Const*
valueB:*
dtype0
?
 keras_upsampling2d/strided_sliceStridedSlicekeras_upsampling2d/Shape&keras_upsampling2d/strided_slice/stack(keras_upsampling2d/strided_slice/stack_1(keras_upsampling2d/strided_slice/stack_2*
shrink_axis_mask *
ellipsis_mask *

begin_mask *
new_axis_mask *
end_mask *
Index0*
T0
M
keras_upsampling2d/ConstConst*
valueB"      *
dtype0
b
keras_upsampling2d/mulMul keras_upsampling2d/strided_slicekeras_upsampling2d/Const*
T0
?
(keras_upsampling2d/ResizeNearestNeighborResizeNearestNeighborkeras_upsampling2d_input:0keras_upsampling2d/mul*
align_corners( *
T0 