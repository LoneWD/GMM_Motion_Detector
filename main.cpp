#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/bgsegm.hpp>
#include "detector.hpp"

int main() {
    auto cap = cv::VideoCapture("../v1.avi");
    //auto fgbg = cv::bgsegm::createBackgroundSubtractorMOG();
    int history = 200;
    auto fgbg = Detector(history);
    cv::Mat frame;
    int cnt = 0;
    while (true) {
        ++ cnt;
        if (!cap.read(frame)) {
            //std::cout << frame.size() << std::endl;
            break;
        }
        //std::cout << frame.size().height << std::endl;
        cv::Mat fgmask;
        
        fgmask = fgbg.apply(frame);
        //std::cout << int(fgmask.at<cv::Vec3b>(180, 120)[0]) << std::endl;
        cv::morphologyEx(fgmask, fgmask, cv::MORPH_OPEN, cv::Mat::ones(5, 3, CV_64F));
        auto font = cv::FONT_HERSHEY_SIMPLEX;
        cv::putText(fgmask, std::to_string(cnt), {3, 18}, font, 0.6, (255), 2);
        cv::Mat result;
        cv::cvtColor(fgmask, fgmask, cv::COLOR_GRAY2RGB);
        cv::hconcat(fgmask, frame, result);
        cv::imshow("figure", result);
        int k;
        if (cnt <= history) {
            k = cv::waitKey(1) & 0xff;
        } else {
            k = cv::waitKey(200) & 0xff;
        }
        if (k == 27) {
            break;
        } else if(k == 32) {
            cv::waitKey(0);
        }
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
