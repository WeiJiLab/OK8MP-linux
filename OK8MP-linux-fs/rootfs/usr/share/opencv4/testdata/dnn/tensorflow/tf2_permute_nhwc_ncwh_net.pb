
Y
average_pooling2d_inputPlaceholder*
dtype0*$
shape:?????????
K
Func/PartitionedCall/input/_0Identityaverage_pooling2d_input*
T0
a
-Func/PartitionedCall/PartitionedCall/input/_2IdentityFunc/PartitionedCall/input/_0*
T0
?
DPartitionedCall/PartitionedCall/sequential/average_pooling2d/AvgPoolAvgPool-Func/PartitionedCall/PartitionedCall/input/_2*
data_formatNHWC*
strides
*
ksize
*
paddingVALID*
T0
~
APartitionedCall/PartitionedCall/sequential/permute/transpose/permConst*
dtype0*%
valueB"             
?
<PartitionedCall/PartitionedCall/sequential/permute/transpose	TransposeDPartitionedCall/PartitionedCall/sequential/average_pooling2d/AvgPoolAPartitionedCall/PartitionedCall/sequential/permute/transpose/perm*
T0*
Tperm0
m
8PartitionedCall/PartitionedCall/sequential/flatten/ConstConst*
dtype0*
valueB"????   
?
:PartitionedCall/PartitionedCall/sequential/flatten/ReshapeReshape<PartitionedCall/PartitionedCall/sequential/permute/transpose8PartitionedCall/PartitionedCall/sequential/flatten/Const*
T0*
Tshape0
y
(PartitionedCall/PartitionedCall/IdentityIdentity:PartitionedCall/PartitionedCall/sequential/flatten/Reshape*
T0
m
.Func/PartitionedCall/PartitionedCall/output/_3Identity(PartitionedCall/PartitionedCall/Identity*
T0
]
PartitionedCall/IdentityIdentity.Func/PartitionedCall/PartitionedCall/output/_3*
T0
M
Func/PartitionedCall/output/_1IdentityPartitionedCall/Identity*
T0
=
IdentityIdentityFunc/PartitionedCall/output/_1*
T0"?