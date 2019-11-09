#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.hpp"

int main() {
    std::cout << "Please input the path of Video." << std::endl;
    std::string s;
    std::cin >> s;
    auto cap = cv::VideoCapture(s);
    int history = 10;
    cv::VideoWriter writer;
    auto fgbg = Detector(history);
    cv::Mat frame;
    int width, height;
    int cnt = 0;
    while (true) {
        if (!cap.read(frame)) {
            if (cnt == 0) {
                std::cout << "Can not open this video." << std::endl;
                system("pause");
            }
            break;
        }
		if (cnt == 0) {
			width = frame.size().width;
			height = frame.size().height;
			std::cout << width << " " << height << std::endl;
			cv::VideoWriter tmp("result.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 5, cv::Size(width * 2, height));
			writer = tmp;
		}
        if (height > 400) {
            cv::resize(frame, frame, cv::Size(width / 10, height / 10));
        }
        ++ cnt;
        cv::Mat fgmask;
        
        fgmask = fgbg.apply(frame);
        cv::erode(fgmask, fgmask, cv::Mat::ones(5, 5, CV_8UC1));
        cv::erode(fgmask, fgmask, cv::Mat::ones(2, 2, CV_8UC1));
        cv::dilate(fgmask, fgmask, cv::Mat::ones(7, 5, CV_8UC1));
        auto font = cv::FONT_HERSHEY_SIMPLEX;
        cv::putText(fgmask, std::to_string(cnt), {3, 18}, font, 0.6, (255), 2);
        cv::Mat result;
        cv::cvtColor(fgmask, fgmask, cv::COLOR_GRAY2RGB);
        cv::resize(frame, frame, cv::Size(width, height));
        cv::resize(fgmask, fgmask, cv::Size(width, height));
        cv::hconcat(fgmask, frame, result);
        cv::imshow("figure", result);
        writer.write(result);
        int k;
        k = cv::waitKey(1) & 0xff;
        if (k == 27) {
            break;
        } else if(k == 32) {
            cv::waitKey(0);
        }
    }
    writer.release();
    cap.release();
    cv::destroyAllWindows();
    std::cout << "Video have been output as result.avi" << std::endl;
    system("pause");
    return 0;
}
