#ifndef GMM_GM_HPP_
#define GMM_GM_HPP_
#endif
#include <cmath>
#include <iostream>

class GM {
private:
    float weight_ = 0;
    float mean_ = 0;
    float init_std_ = 2;
    float variance_ = std::pow(init_std_, 2);
    float std_deviation_ = init_std_;

public:
    GM() {}
    GM(const float weight, const float mean, const float variance) {
        weight_ = std::move(weight);
        mean_ = std::move(mean);
        variance_ = std::move(variance);
        std_deviation_ = std::sqrt(variance_);
    }
    void test() {
        std::cout << weight_ << " " << mean_ << " " << variance_ << std::endl;
    }
    void SetWeight(const float weight) {
        weight_ = std::move(weight);
    }
    float GetWeight() {
        return weight_;
    }
    void SetVariance(const float variance) {
        variance_ = std::move(variance);
        std_deviation_ = std::sqrt(variance_);
    }
    float GetStdDeviation() {
        return std_deviation_;
    }
    float GetVariance() {
        return variance_;
    }
    void SetMean(const float mean) {
        mean_ = std::move(mean);
    }
    float GetMean() {
        return mean_;
    }
};