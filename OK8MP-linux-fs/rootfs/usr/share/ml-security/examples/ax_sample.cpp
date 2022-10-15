/** \file ax_sample.cpp
 *  \brief Example of using multiple inference and voting to harden against adversarial examples.
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
 *  This file is an example of hardening against adversarial examples by multiple inference
 *  on transformed inputs and voting for the result. Here we apply two transformations to the
 *  inputs: rotation by 3 degrees and blurring. The original image and the transformed
 *  images are classified by the network (in this example the inception v3 model), followed
 *  by a voting of the most occuring class. In case of a tie the class of the first (the
 *  original) image is selected.
 *  This procedure can easily be extended to more transformations followed by a voting.
 *  The classification of the original image is even not obligatory. In case of a tie by a
 *  large number of transformed images, the first classifications in the code get more weight.
 */

#include <fstream>
#include <iostream>
#include <sys/time.h>

#include <opencv2/opencv.hpp>

#include "ml-security/input_transformations.hpp"
#include "ml-security/voting.hpp"

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

/*! \brief Apply hardening against adversarial examples
 *
 *  This function classifies an image, a rotated version of the image, and a blurred version
 *  of the image. From the three resulting classes it selects the most frequent one.
 *  In case of a tie (i.e. all three classifications result in a different class) it selects
 *  the first one, which is the classification if the original image.
 *
 *  This procedure can easily be extended to more transformations on the original image,
 *  followed by a voting. In case of a tie, the earlier classifications get more weight.
 *  Possible other input transformations are adding noise and jpeg compression followed
 *  by decompression.
 *
 *  \param image the original image 
 *
 */
void test_voting(cv::Mat image) {
    std::vector<cv::Mat> results;

    // classify the original image
    results.push_back(single_inference(image));
    std::cout << "Top 3 for original image" << std::endl;
    showTop(results.back(), 3);

    // classify the image rotated over 3 degrees
    cv::Mat rotated = rotateImage(image, 3);
    results.push_back(single_inference(rotated));
    std::cout << "Top 3 for rotated image" << std::endl;
    showTop(results.back(), 3);

    // classify the blurred image
    cv::Mat blurred2 = blurImage(image, BlurType::BILATERAL_BLUR);
    results.push_back(single_inference(blurred2));
    std::cout << "Top 3 for blurred (bilateral) image" << std::endl;
    showTop(results.back(), 3);

    // do the voting
    cv::Mat voted = vote(results);
 
    // and show the resulting class
    cv::Point p;
    cv::minMaxLoc(voted, NULL, NULL, NULL, &p);
    std::cout << "Classification by votes: " << p.x << std::endl;
}

int main() {
    // apply the hardening step to two images.
    // firstly a normal problem domain image.
    // in this case the original classification was fine and it can be seen that
    // the hardening process has no influence on the final class 
    cv::Mat image = cv::imread("test_images/hedgehog1small.png", cv::IMREAD_COLOR);
    std::cout << "Using hedgehog image" << std::endl;
    test_voting(image);

    // secondly an adversarial example
    // here the original classification fails, because the image is designed as an
    // adversarial example for this model.
    // The two classifications of the transformed image are correct and change the
    // final class to the correct class. 
    image = cv::imread("test_images/hedgehog2banana_adv.png", cv::IMREAD_COLOR);
    std::cout << "Using adversarial example" << std::endl;
    test_voting(image);

	return 0;
}
