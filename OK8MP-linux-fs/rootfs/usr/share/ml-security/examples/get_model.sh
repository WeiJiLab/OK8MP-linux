#!/bin/bash

# Copyright 2019 NXP 

# This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms.  By expressly accepting such
# terms or by downloading, installing, activating and/or otherwise using the
# software, you are agreeing that you have read, and that you agree to comply 
# with and are bound by, such license terms.  If you do not agree to be bound
# by the applicable license terms, then you may not retain, install, activate
# or otherwise use the software.

# License: NXP Proprietary. See the file "COPYING" for the license of this file.

#FREEZE_GRAPH_DIR=/home/user/virtualenv/lib/python3.6/site-packages/tensorflow/python/tools

if [ -z ${FREEZE_GRAPH_DIR} ]
then
    echo "Please set FREEZE_GRAPH_DIR to a folder where the freeze_graph.py tool from Tensorflow can be found."
    echo -e "If you have tensorflow installed in\n    /home/user/virtualenv/\nyou might want to try\n    /home/user/virtualenv/lib/python3.6/site-packages/tensorflow/python/tools"
    exit 1
fi

mkdir test_model
cd test_model
curl -O http://download.tensorflow.org/models/inception_v3_2016_08_28.tar.gz
tar xvf inception_v3_2016_08_28.tar.gz
git clone https://github.com/tensorflow/models.git
cd models/research/slim
python export_inference_graph.py --model_name=inception_v3 --output_file=../../../inception_v3_inf_graph.pb
cd ../../..
python ${FREEZE_GRAPH_DIR}/freeze_graph.py --input_graph=inception_v3_inf_graph.pb --input_checkpoint=inception_v3.ckpt --input_binary=true --output_graph=inception_v3.pb --output_node_names=InceptionV3/Predictions/Softmax
