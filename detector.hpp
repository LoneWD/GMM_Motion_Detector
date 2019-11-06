#ifndef GMM_DETECTOR_HPP_
#define GMM_DETECTOR_HPP_
#endif
#include <opencv2/opencv.hpp>
#include <vector>
#include "gm.hpp"
#include <thread>
using std::vector;
typedef vector<vector<vector<vector<GM>>>> FourDimsVector;

class Detector {
   public:
    Detector(int history) {
        history_ = history;
    }
    cv::Mat apply(const cv::Mat &frame) {
        int height = frame.size().height, width = frame.size().width;
        if (current_frame_id_ == 0) {
            InitGms(frame);
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    assignment_list_.emplace_back(i, j);
        }
        current_frame_id_++;
        result_ = cv::Mat::zeros(height, width, CV_8UC1);
        for (auto p : assignment_list_) {
            Process(p, frame);
        }
        /*for (int i = 0; i < assignment_list_.size(); i += 4) {
            auto &v = assignment_list_;
            std::thread thread1(test, v[i], frame);
            std::thread thread2(test, v[i], frame);
            std::thread thread3(test, v[i], frame);
            std::thread thread4(test, v[i], frame);
            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
        }*/
        return result_;
    }

   private:
    vector<std::pair<int, int>> assignment_list_;
    float alpha_ = 0.1;
    float lambda_ = 3;
    unsigned int current_frame_id_ = 0;
    unsigned int history_ = 10;
    unsigned int n_mixtures_ = 5;
    cv::Mat result_;
    FourDimsVector gaussian_models_;
    void InitGms(const cv::Mat &frame) {
        int height = frame.size().height, width = frame.size().width;
        gaussian_models_ = FourDimsVector(
            height, vector<vector<vector<GM>>>(
                        width, vector<vector<GM>>(n_mixtures_, vector<GM>(3))));
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                for (int k = 0; k < 3; k++)
                    gaussian_models_[i][j][0][k].SetWeight(1);
    }

    static void test(const std::pair<int, int> &P, const cv::Mat &frame) {
        std::cout << P.first << ' ' << P.second << std::endl;
    }

    void Process(const std::pair<int, int> &P, const cv::Mat &frame) {
        int x = P.first, y = P.second;
        if (current_frame_id_ <= history_) {
            bool have_matched = false;
            for (int k = 0; k < n_mixtures_; k++) {
                bool is_matched = true;
                for (int c = 0; c < 3; c++) {
                    int pixel = frame.at<cv::Vec3b>(x, y)[c];
                    auto &gm_now = gaussian_models_[x][y][k][c];
                    if (!if_match(pixel, gm_now)) {
                        is_matched = false;
                        break;
                    }
                }
                if (is_matched) {
                    have_matched = true;
                    for (int c = 0; c < 3; c++) {
                        int pixel = frame.at<cv::Vec3b>(x, y)[c];
                        auto &gm_now = gaussian_models_[x][y][k][c];
                        auto new_weight = gm_now.GetWeight() + alpha_;
                        gm_now.SetWeight(new_weight);
                        auto learning_rate = alpha_ / new_weight;
                        auto new_mean = gm_now.GetMean() * (1 - learning_rate) +
                                        learning_rate * pixel;
                        gm_now.SetMean(new_mean);
                        auto new_variance = gm_now.GetVariance() * (1 - learning_rate) +
                                            learning_rate * pow(pixel - gm_now.GetMean(), 2);
                        gm_now.SetVariance(new_variance);
                    }
                } else {
                    float min_important = GetSortKey(x, y, 0);
                    int to_delete = 0;
                    for (int i = 1; i < n_mixtures_; i++) {
                        auto sort_key = GetSortKey(x, y, i);
                        if (sort_key < min_important) {
                            min_important = sort_key;
                            to_delete = i;
                        }
                        gaussian_models_[x][y].erase(gaussian_models_[x][y].begin() + i);
                        vector<GM> new_gm;
                        for (int c = 0; c < 3; c++) {
                            int pixel = frame.at<cv::Vec3b>(x, y)[c];
                            new_gm.push_back(GM(0.1, pixel, 450));
                        }
                        gaussian_models_[x][y].push_back(std::move(new_gm));
                    }
                }
                float sum = 0;
                for (int k = 0; k < n_mixtures_; k++) {
                    sum += gaussian_models_[x][y][k][0].GetWeight();
                }
                for (int k = 0; k < n_mixtures_; k++) {
                    float weight = gaussian_models_[x][y][k][0].GetWeight();
                    for (int c = 0; c < 3; c++) {
                        gaussian_models_[x][y][k][c].SetWeight(weight / sum);
                    }
                }
            }
        } else {
            for (int k = 0; k < n_mixtures_; k++) {
                bool is_matched = true;
                for (int c = 0; c < 3; c++) {
                    auto &gm_now = gaussian_models_[x][y][k][c];
                    int pixel = frame.at<cv::Vec3b>(x, y)[c];
                    if (!if_match(pixel, gm_now)) {
                        is_matched = false;
                        break;
                    }
                }
                if (is_matched) {
                    result_.at<uchar>(x, y) = 0;
                } else {
                    result_.at<uchar>(x, y) = 255;
                }
            }
        }
    }

    bool if_match(const int &pixel, GM &gm_now) {
        return fabs(pixel - gm_now.GetMean()) <=
               lambda_ * gm_now.GetStdDeviation();
    }
    
    float GetSortKey(int x, int y, int k) {
        float result = 0;
        for (int i = 0; i < 3; i++) {
            result += gaussian_models_[x][y][k][i].GetWeight()
                    / gaussian_models_[x][y][k][i].GetStdDeviation();
        }
        return result;
    }
};