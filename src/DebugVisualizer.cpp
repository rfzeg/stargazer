//
// Created by bandera on 28.03.16.
//

#include "DebugVisualizer.h"
#include "CoordinateTransformations.h"

using namespace stargazer;

DebugVisualizer::DebugVisualizer() : m_window_mode(CV_WINDOW_NORMAL), m_wait_time(1) {
}

void DebugVisualizer::prepareImg(cv::Mat& img) {
    if (img.type() == CV_8UC1) {
        // input image is grayscale
        cvtColor(img, img, CV_GRAY2RGB);
    } else {
    }
}
void DebugVisualizer::ShowImage(cv::Mat& img, std::string name) {
    cv::namedWindow(name, m_window_mode);
    cv::imshow(name, img);
    cv::waitKey(m_wait_time);
}

void DebugVisualizer::ShowPoints(const cv::Mat& img, const std::vector<cv::Point> points) {
    cv::Mat temp = img.clone();
    prepareImg(temp);
    for (auto& point : points)
        circle(temp, point, 1, cv::Scalar(255, 0, 0), 2); // Blue
    ShowImage(temp, "Points");
}

void DebugVisualizer::ShowClusters(const cv::Mat& img, const std::vector<std::vector<cv::Point>> points) {
    cv::Mat temp = img.clone();
    prepareImg(temp);
    for (auto& group : points) {
        cv::Point median(0, 0);
        for (auto& point : group) {
            median += point;
            circle(temp, point, 1, cv::Scalar(255, 0, 0), 2); // Blue
        }
        median *= 1.0 / group.size();
        circle(temp, median, 30, cv::Scalar(0, 255, 0), 2); // Green
    }
    ShowImage(temp, "Clusters");
}

void DebugVisualizer::DrawLandmarks(cv::Mat& img, const std::vector<ImgLandmark>& landmarks) {

    for (auto& lm : landmarks) {
        for (auto& imgPoint : lm.voCorners) {
            circle(img, imgPoint, 2, cv::Scalar(255, 0, 0), 2); // Blue
        }
        for (auto& imgPoint : lm.voIDPoints) {
            circle(img, imgPoint, 2, cv::Scalar(255, 128, 0), 2); // Magenta
        }

        std::string text = "ID: ";
        text += std::to_string(lm.nID);
        cv::Point imgPoint = lm.voCorners.front();
        imgPoint.x += 25;
        imgPoint.y += 25;
        putText(img, text, imgPoint, 2, 0.4, cvScalar(255, 255, 0));
    }
}

void DebugVisualizer::DrawLandmarks(cv::Mat& img, const landmark_map_t& landmarks,
                                    const camera_params_t& camera_intrinsics, const pose_t& ego_pose) {
    cv::Point imgPoint;

    for (auto& lm : landmarks) {
        for (size_t i = 0; i < lm.second.points.size(); i++) {
            auto& pt = lm.second.points[i];

            // Convert point into camera frame
            double x = 0.0;
            double y = 0.0;

            transformWorld2Img(&pt[(int)POINT::X], &pt[(int)POINT::Y], &pt[(int)POINT::Z], ego_pose.data(),
                               camera_intrinsics.data(), &x, &y);
            imgPoint.x = static_cast<int>(x);
            imgPoint.y = static_cast<int>(y);

            /// Corner Points
            if (i > 3)
                circle(img, imgPoint, 4, cv::Scalar(255, 0, 255), 2); // Orange
            else
                circle(img, imgPoint, 4, cv::Scalar(0, 0, 255), 2); // Red
        }

        std::string text = "ID: ";
        text += std::to_string(lm.second.id);
        imgPoint.x += 25;
        imgPoint.y += 25;
        putText(img, text, imgPoint, 2, 0.4, cvScalar(255, 255, 0)); // Türkis
    }
}
