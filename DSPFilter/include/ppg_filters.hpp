#ifndef PPG_FILTERS_HPP
#define PPG_FILTERS_HPP

#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>

namespace ppg {

// ===================== 零相位滤波核心函数 =====================

/**
 * @brief 零相位滤波函数（实现Python的filtfilt功能）
 * @param filter 滤波器对象
 * @param data 输入/输出数据（in-place修改）
 * @param numSamples 样本数量
 * 
 * @note 模板函数必须在头文件中实现
 */
template<typename FilterType>
void filtfilt(FilterType& filter, float* data, int numSamples) {
    std::cout << "执行零相位滤波 (filtfilt)..." << std::endl;
    
    float* temp = new float[numSamples];
    float* temp_ptr = temp;  // 用于process函数的指针
    
    // 第一步：正向滤波
    memcpy(temp, data, numSamples * sizeof(float));
    filter.reset();
    filter.process(numSamples, &temp_ptr);
    std::cout << "  - 正向滤波完成" << std::endl;
    
    // 第二步：反转信号
    std::reverse(temp, temp + numSamples);
    std::cout << "  - 信号反转完成" << std::endl;
    
    // 第三步：反向滤波
    temp_ptr = temp;  // 重置指针
    filter.reset();
    filter.process(numSamples, &temp_ptr);
    std::cout << "  - 反向滤波完成" << std::endl;
    
    // 第四步：再次反转得到最终结果
    std::reverse(temp, temp + numSamples);
    std::cout << "  - 最终反转完成" << std::endl;
    
    // 复制结果到输出
    memcpy(data, temp, numSamples * sizeof(float));
    delete[] temp;
    
    std::cout << "零相位滤波完成！" << std::endl;
}

// ===================== 滤波函数声明 =====================

/**
 * @brief 零相位带通滤波
 * @param input_signal 输入信号
 * @param low_freq 低频截止 (Hz)
 * @param high_freq 高频截止 (Hz)
 * @param sample_rate 采样率 (Hz)
 * @param filter_order 滤波器阶数
 * @return 滤波后的信号
 */
std::vector<float> apply_bandpass_zerophase(
    const std::vector<float>& input_signal,
    double low_freq,
    double high_freq,
    double sample_rate,
    int filter_order = 3
);

/**
 * @brief 单向IIR带通滤波（带均值初始化）
 * @param input_signal 输入信号
 * @param low_freq 低频截止 (Hz)
 * @param high_freq 高频截止 (Hz)
 * @param sample_rate 采样率 (Hz)
 * @param filter_order 滤波器阶数
 * @param use_warmup 是否使用均值初始化预热
 * @return 滤波后的信号
 */
std::vector<float> apply_bandpass_oneway(
    const std::vector<float>& input_signal,
    double low_freq,
    double high_freq,
    double sample_rate,
    int filter_order = 3,
    bool use_warmup = true
);

} // namespace ppg

#endif // PPG_FILTERS_HPP

