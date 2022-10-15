/** \file cloning_sample.cpp
 *  \brief Example of using noise adding to make model cloning more difficult.
 *  \copyright
 *  Copyright 2019 NXP 
 *  \copyright
 *  This software is owned or controlled by NXP and may only be used strictly in
 *  accordance with the applicable license terms.  By expressly accepting such
 *  terms or by downloading, installing, activating and/or otherwise using the
 *  software, you are agreeing that you have read, and that you agree to comply 
 *  with and are bound by, such license terms.  If you do not agree to be bound
 *  by the applicable license terms, then you may not retain, install, activate
 *  or otherwise use the software.
 *  \copyright
 *  License: NXP Proprietary. See the file "COPYING" for the license of this file.
 *
 *  This file is an example of hardening against model cloning by adding pseudo-random noise to the output.
 *  A similar strategy would be adding noise or rounding the confidence levels to obfuscate the details of the result.
 *  Both of these methods are also supported.
 */

#include <fstream>
#include <iostream>
#include <sys/time.h>

#include <opencv2/opencv.hpp>

#include "ml-security/output_transformations.hpp"

static const int imgWidth = 299;
static const int imgHeight = 299;

/*! \brief Make a classification of an image with the inception v3 model.
 *  This function reads the inception v3 model from file and classifies the
 *  input image. It returns a vector with probabilities for all classes.
 *
 *  \param image is an image that has to be classified
 *  \return a vector with probabilities
 */
cv::Mat single_inference(cv::Mat image) {
    cv::Size imgSize = cv::Size(imgWidth, imgHeight);

	cv::dnn::Net nn = cv::dnn::readNetFromTensorflow("test_model/inception_v3.pb");

    cv::Mat blob;
    cv::dnn::blobFromImage(image, blob, 1.0/127.5, cv::Size(imgWidth, imgHeight),
                           cv::Scalar(127.5, 127.5, 127.5), true, false, CV_32F);

	nn.setInput(blob);
    cv::Mat output = nn.forward();

    bool applyNormalization = true;
    output = addPseudoNoiseSin(output, applyNormalization);

    return output;
}

/*! \brief Print the top three classes with their probabilities
 *  This function finds the three indices with the highest probabilities and prints those
 *  indices together with their probabilities
 *
 *  \param confidences is a vector with probabilities 
 */
void showTop(cv::Mat confidences, unsigned int n) {
    cv::Mat idxs;
    cv::sortIdx(confidences, idxs, cv::SORT_EVERY_ROW+cv::SORT_DESCENDING);
    for(size_t i=0; i<n; i++) {
        int cls = idxs.at<int>(0, i);
        std::cout << i+1 << ": Class " << cls << ", Confidence: " << confidences.at<float>(0, cls) << std::endl;
    }
}

/*! \brief Apply hardening against cloning
 *
 *  This function classifies an image and adds noise to the result.
 *
 *  \param image the image to be classified
 *
 */
void test_noise(cv::Mat image) {
    cv::Mat result = single_inference(image);
    std::cout << "Top 3" << std::endl;
    showTop(result, 3);
}

int main() {
    cv::Mat image = cv::imread("test_images/hedgehog1small.png", cv::IMREAD_COLOR);
    std::cout << "Using hedgehog image" << std::endl;
    test_noise(image);

	return 0;
}
