
#
input_1Placeholder*
dtype0
3
l2_normalize_2/SquareSquare	input_1:0*
T0
N
$l2_normalize_2/Sum/reduction_indicesConst*
value	B :*
dtype0
|
l2_normalize_2/SumSuml2_normalize_2/Square$l2_normalize_2/Sum/reduction_indices*

Tidx0*
	keep_dims(*
T0
E
l2_normalize_2/Maximum/yConst*
valueB
 *̼?+*
dtype0
X
l2_normalize_2/MaximumMaximuml2_normalize_2/Suml2_normalize_2/Maximum/y*
T0
>
l2_normalize_2/RsqrtRsqrtl2_normalize_2/Maximum*
T0
?
l2_normalize_2Mul	input_1:0l2_normalize_2/Rsqrt*
T0
8
l2_normalize_3/SquareSquarel2_normalize_2*
T0
W
$l2_normalize_3/Sum/reduction_indicesConst*
dtype0*
valueB :
?????????
|
l2_normalize_3/SumSuml2_normalize_3/Square$l2_normalize_3/Sum/reduction_indices*

Tidx0*
	keep_dims(*
T0
E
l2_normalize_3/Maximum/yConst*
valueB
 *̼?+*
dtype0
X
l2_normalize_3/MaximumMaximuml2_normalize_3/Suml2_normalize_3/Maximum/y*
T0
>
l2_normalize_3/RsqrtRsqrtl2_normalize_3/Maximum*
T0
D
l2_normalize_3Mull2_normalize_2l2_normalize_3/Rsqrt*
T0
8
l2_normalize_4/SquareSquarel2_normalize_3*
T0
Y
$l2_normalize_4/Sum/reduction_indicesConst*
valueB"       *
dtype0
|
l2_normalize_4/SumSuml2_normalize_4/Square$l2_normalize_4/Sum/reduction_indices*

Tidx0*
	keep_dims(*
T0
E
l2_normalize_4/Maximum/yConst*
valueB
 *̼?+*
dtype0
X
l2_normalize_4/MaximumMaximuml2_normalize_4/Suml2_normalize_4/Maximum/y*
T0
>
l2_normalize_4/RsqrtRsqrtl2_normalize_4/Maximum*
T0
D
l2_normalize_4Mull2_normalize_3l2_normalize_4/Rsqrt*
T0 