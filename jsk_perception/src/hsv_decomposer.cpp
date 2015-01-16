// -*- mode: c++ -*-
/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2015, JSK Lab
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/o2r other materials provided
 *     with the distribution.
 *   * Neither the name of the JSK Lab nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/


#include "jsk_perception/hsv_decomposer.h"
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

namespace jsk_perception
{
  void HSVDecomposer::onInit()
  {
    DiagnosticNodelet::onInit();
    pub_h_ = advertise<sensor_msgs::Image>(*pnh_, "output/hue", 1);
    pub_s_ = advertise<sensor_msgs::Image>(*pnh_, "output/saturation", 1);
    pub_v_ = advertise<sensor_msgs::Image>(*pnh_, "output/value", 1);
  }

  void HSVDecomposer::subscribe()
  {
    sub_ = pnh_->subscribe("input", 1, &HSVDecomposer::decompose, this);
  }

  void HSVDecomposer::unsubscribe()
  {
    sub_.shutdown();
  }

  void HSVDecomposer::decompose(
    const sensor_msgs::Image::ConstPtr& image_msg)
  {
    cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(
      image_msg, sensor_msgs::image_encodings::BGR8);
    cv::Mat image = cv_ptr->image;
    cv::Mat hsv_image;
    cv::Mat hue = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::Mat saturation = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::Mat value = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::cvtColor(image, hsv_image, CV_BGR2HSV);
    for (size_t j = 0; j < hsv_image.rows; j++) {
      for (size_t i = 0; i < hsv_image.cols; i++) {
        cv::Vec3b hsv = hsv_image.at<cv::Vec3b>(j, i);
        hue.at<uchar>(j, i) = hsv[0] % 180; // up to 180
        saturation.at<uchar>(j, i) = hsv[1];
        value.at<uchar>(j, i) = hsv[2];
      }
    }
    pub_h_.publish(cv_bridge::CvImage(
                     image_msg->header,
                     sensor_msgs::image_encodings::MONO8,
                     hue).toImageMsg());
    pub_s_.publish(cv_bridge::CvImage(
                     image_msg->header,
                     sensor_msgs::image_encodings::MONO8,
                     saturation).toImageMsg());
    pub_v_.publish(cv_bridge::CvImage(
                     image_msg->header,
                     sensor_msgs::image_encodings::MONO8,
                     value).toImageMsg());
  }
}

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS (jsk_perception::HSVDecomposer, nodelet::Nodelet);
