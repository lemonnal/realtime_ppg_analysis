#include "ppg_filters.hpp"
#include "DspFilters/Dsp.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace ppg {

// ===================== 滤波函数（零相位）=====================

std::vector<float> apply_bandpass_zerophase(
    const std::vector<float>& input_signal,
    double low_freq,
    double high_freq,
    double sample_rate,
    int filter_order
) {
    std::cout << "\n【零相位滤波】" << std::endl;
    std::cout << "  方法: filtfilt (正向+反向)" << std::endl;
    
    // 计算中心频率和带宽
    double center_frequency = std::sqrt(low_freq * high_freq);
    double bandwidth = high_freq - low_freq;
    
    // 创建滤波器
    Dsp::SimpleFilter<Dsp::Butterworth::BandPass<5>, 1> filter;
    filter.setup(filter_order, sample_rate, center_frequency, bandwidth);
    
    // 复制输入信号
    std::vector<float> output_signal = input_signal;
    
    // 应用filtfilt
    filtfilt(filter, output_signal.data(), output_signal.size());
    
    std::cout << "  零相位滤波完成！" << std::endl;
    
    return output_signal;
}

// ===================== 滤波函数（单向IIR）=====================

std::vector<float> apply_bandpass_oneway(
    const std::vector<float>& input_signal,
    double low_freq,
    double high_freq,
    double sample_rate,
    int filter_order,
    bool use_warmup
) {
    std::cout << "\n【单向IIR滤波】" << std::endl;
    std::cout << "  方法: 单向正向滤波" << std::endl;
    std::cout << "  预计群延迟: ~" << filter_order / (2 * M_PI * low_freq) * 1000 
              << " ms" << std::endl;
    
    // 计算中心频率和带宽
    double center_frequency = std::sqrt(low_freq * high_freq);
    double bandwidth = high_freq - low_freq;
    
    // 创建滤波器
    Dsp::SimpleFilter<Dsp::Butterworth::BandPass<5>, 1> filter;
    filter.setup(filter_order, sample_rate, center_frequency, bandwidth);
    
    // ========== 均值初始化预热 ==========
    if (use_warmup && input_signal.size() > 100) {
        // 计算前100个样本的均值
        int warmup_samples = std::min(100, (int)input_signal.size());
        float mean = 0.0f;
        for (int i = 0; i < warmup_samples; i++) {
            mean += input_signal[i];
        }
        mean /= warmup_samples;
        
        // 使用均值预热滤波器（约50次迭代）
        int warmup_iterations = 50;
        for (int i = 0; i < warmup_iterations; i++) {
            float temp = mean;
            float* p = &temp;
            filter.process(1, &p);
        }
        
        std::cout << "  滤波器预热: 是 (均值=" << std::fixed << std::setprecision(2) 
                  << mean << ", 迭代次数=" << warmup_iterations << ")" << std::endl;
    } else {
        std::cout << "  滤波器预热: 否" << std::endl;
    }
    
    // ========== 滤波处理 ==========
    std::vector<float> output_signal = input_signal;
    for (size_t i = 0; i < output_signal.size(); i++) {
        float* p = &output_signal[i];
        filter.process(1, &p);
    }
    
    std::cout << "  单向滤波完成！" << std::endl;
    
    return output_signal;
}

} // namespace ppg

