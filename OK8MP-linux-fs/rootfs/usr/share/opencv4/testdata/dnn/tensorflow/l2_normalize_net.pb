
!
inputPlaceholder*
dtype0
?
average_pooling2d/AvgPoolAvgPoolinput:0*
T0*
data_formatNHWC*
strides
*
ksize
*
paddingSAME
A
l2_normalize/SquareSquareaverage_pooling2d/AvgPool*
T0
U
"l2_normalize/Sum/reduction_indicesConst*
valueB :
?????????*
dtype0
v
l2_normalize/SumSuml2_normalize/Square"l2_normalize/Sum/reduction_indices*
T0*

Tidx0*
	keep_dims(
C
l2_normalize/Maximum/yConst*
valueB
 *̼?+*
dtype0
R
l2_normalize/MaximumMaximuml2_normalize/Suml2_normalize/Maximum/y*
T0
:
l2_normalize/RsqrtRsqrtl2_normalize/Maximum*
T0
K
l2_normalizeMulaverage_pooling2d/AvgPooll2_normalize/Rsqrt*
T0
6
l2_normalize_1/SquareSquarel2_normalize*
T0
]
$l2_normalize_1/Sum/reduction_indicesConst*!
valueB"         *
dtype0
|
l2_normalize_1/SumSuml2_normalize_1/Square$l2_normalize_1/Sum/reduction_indices*

Tidx0*
	keep_dims(*
T0
E
l2_normalize_1/Maximum/yConst*
valueB
 *̼?+*
dtype0
X
l2_normalize_1/MaximumMaximuml2_normalize_1/Suml2_normalize_1/Maximum/y*
T0
>
l2_normalize_1/RsqrtRsqrtl2_normalize_1/Maximum*
T0
B
l2_normalize_1Mull2_normalizel2_normalize_1/Rsqrt*
T0 