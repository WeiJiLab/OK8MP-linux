
!
inputPlaceholder*
dtype0
>
concat/axisConst*
valueB :
?????????*
dtype0
@
conv3d/biasConst*
dtype0*
valueB"        
b
conv3d/kernelConst*=
value4B2"O??(z>-E+???v>a@#?????*
dtype0
?
conv3d/Conv3DConv3Dinput:0conv3d/kernel*
strides	
*
data_formatNDHWC*
paddingSAME*
	dilations	
*
T0
U
conv3d/BiasAddBiasAddconv3d/Conv3Dconv3d/bias*
T0*
data_formatNHWC
V
concatConcatV2input:0conv3d/BiasAddconcat/axis*
T0*
N*

Tidx0 