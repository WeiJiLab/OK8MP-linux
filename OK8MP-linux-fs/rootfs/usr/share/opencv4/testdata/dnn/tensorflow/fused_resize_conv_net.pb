
>
inputPlaceholder*
shape:*
dtype0
m
conv2d/Conv2D_dummy_paddingsConst*9
value0B."                                 *
dtype0
H
conv2d/biasConst*
dtype0*%
valueB"                
?
conv2d/kernelConst*a
valueXBV"@?o?>?o9?q?3?@7H=O???ï?? ????j?8] ?Q?;?Y?7?Q>e@?3y??ɢR?W
8?*
dtype0
I
resize_bilinear/sizeConst*
valueB"      *
dtype0
?
conv2d/Conv2DFusedResizeAndPadConv2Dinput:0resize_bilinear/sizeconv2d/Conv2D_dummy_paddingsconv2d/kernel*
strides
*
mode	REFLECT*
resize_align_corners(*
paddingVALID*
T0
U
conv2d/BiasAddBiasAddconv2d/Conv2Dconv2d/bias*
T0*
data_formatNHWC 