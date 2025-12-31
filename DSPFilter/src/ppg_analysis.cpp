#include "ppg_analysis.hpp"
#include "find_peaks.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cmath>

namespace ppg {

// ===================== 峰值检测函数 =====================

void detect_peaks_and_valleys(
    const std::vector<float>& filtered_signal,
    double sample_rate,
    double min_time_interval,
    std::vector<int>& peaks,
    std::vector<int>& valleys,
    float& ac_component
) {
    std::cout << "\n【峰值检测】" << std::endl;
    
    // 计算最小峰值间距
    int min_distance = static_cast<int>(sample_rate * min_time_interval);
    
    std::cout << "  采样率: " << sample_rate << " Hz" << std::endl;
    std::cout << "  最小峰值间距: " << min_distance << " 样本 (" 
              << min_time_interval << " 秒)" << std::endl;
    
    // 找峰值
    peaks = find_peaks(filtered_signal, min_distance);
    std::cout << "  检测到峰值数量: " << peaks.size() << std::endl;
    
    // 找谷值（信号取反）
    std::vector<float> inverted_signal = filtered_signal;
    for (float& val : inverted_signal) {
        val = -val;
    }
    valleys = find_peaks(inverted_signal, min_distance);
    std::cout << "  检测到谷值数量: " << valleys.size() << std::endl;
    
    // 打印前5个峰值
    if (!peaks.empty()) {
        std::cout << "\n  前" << std::min(5, (int)peaks.size()) << "个峰值:" << std::endl;
        for (int i = 0; i < std::min(5, (int)peaks.size()); i++) {
            int idx = peaks[i];
            std::cout << "    峰值 " << (i+1) << ": 位置=" << idx 
                      << " (" << std::fixed << std::setprecision(3) 
                      << (idx / sample_rate) << "s), 幅值=" 
                      << std::setprecision(2) << filtered_signal[idx] << std::endl;
        }
    }
    
    // 打印前5个谷值
    if (!valleys.empty()) {
        std::cout << "\n  前" << std::min(5, (int)valleys.size()) << "个谷值:" << std::endl;
        for (int i = 0; i < std::min(5, (int)valleys.size()); i++) {
            int idx = valleys[i];
            std::cout << "    谷值 " << (i+1) << ": 位置=" << idx 
                      << " (" << std::fixed << std::setprecision(3)
                      << (idx / sample_rate) << "s), 幅值=" 
                      << std::setprecision(2) << filtered_signal[idx] << std::endl;
        }
    }
    
    // 计算平均AC分量（峰峰值）
    ac_component = 0.0f;
    if (!peaks.empty() && !valleys.empty()) {
        float sum_ac = 0;
        int count = 0;
        
        for (int peak_idx : peaks) {
            // 找最近的谷值
            int closest_valley = -1;
            int min_dist = std::numeric_limits<int>::max();
            
            for (int valley_idx : valleys) {
                int dist = std::abs(valley_idx - peak_idx);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_valley = valley_idx;
                }
            }
            
            if (closest_valley >= 0) {
                float ac = std::abs(filtered_signal[peak_idx] - filtered_signal[closest_valley]);
                sum_ac += ac;
                count++;
            }
        }
        
        if (count > 0) {
            ac_component = sum_ac / count;
            std::cout << "\n  平均AC分量（峰峰值）: " << std::fixed 
                      << std::setprecision(2) << ac_component << std::endl;
        }
    }
    
    std::cout << "  峰值检测完成！" << std::endl;
}

// ===================== SpO2计算函数 =====================

bool calculate_spo2_from_ppg(
    const std::vector<float>& input_signal,
    const std::vector<float>& filtered_signal,
    const std::vector<int>& peaks,
    const std::vector<int>& valleys,
    float ac_component,
    float& spo2,
    float& ratio
) {
    std::cout << "\n【SpO2估算】" << std::endl;
    std::cout << "  算法: 基于AC/DC比率的单通道PPG方法" << std::endl;
    
    float dc_component = 0.0f;
    
    if (!peaks.empty() && !valleys.empty() && peaks.size() >= 2 && valleys.size() >= 2) {
        // 方法1: 精确计算（使用峰谷值）
        std::cout << "  方法: 精确峰谷检测" << std::endl;
        
        // DC分量：原始信号的平均值
        for (float val : input_signal) {
            dc_component += val;
        }
        dc_component /= input_signal.size();
        
        std::cout << "\n  AC分量: " << std::fixed << std::setprecision(2) 
                  << ac_component << std::endl;
        std::cout << "  DC分量: " << dc_component << std::endl;
        
        // 计算归一化比率
        if (dc_component != 0) {
            ratio = ac_component / dc_component;
            std::cout << "  AC/DC比率: " << std::setprecision(6) << ratio << std::endl;
            
            // 使用三次多项式计算SpO2
            spo2 = (-3.7465271198e+01f * std::pow(ratio, 3) + 
                    5.8403912586e+01f * std::pow(ratio, 2) + 
                    -3.7079378855e+01f * ratio + 
                    1.0016136403e+02f);
            
            // 限制在合理范围 (90-100%)
            spo2 = std::max(90.0f, std::min(100.0f, spo2));
            
            std::cout << "\n  ┌─────────────────────────────────┐" << std::endl;
            std::cout << "  │  估算SpO2: " << std::fixed << std::setprecision(1) 
                      << spo2 << "%            │" << std::endl;
            std::cout << "  └─────────────────────────────────┘" << std::endl;
            
            // 健康状态评估
            std::cout << "\n  健康评估: ";
            if (spo2 >= 95.0f) {
                std::cout << "正常 ✓ (SpO2 ≥ 95%)" << std::endl;
            } else if (spo2 >= 90.0f) {
                std::cout << "轻度缺氧 ⚠ (90% ≤ SpO2 < 95%)" << std::endl;
            } else {
                std::cout << "低氧血症 ✗ (SpO2 < 90%)" << std::endl;
            }
            
            std::cout << "\n  注意: 此为估算值，精度±5-10%，仅供参考" << std::endl;
            
            return true;
            
        } else {
            std::cout << "  错误: DC分量为0，无法计算比率" << std::endl;
            return false;
        }
        
    } else {
        // 方法2: 降级方案（峰值不足时）
        std::cout << "  警告: 峰值/谷值数量不足，使用简化方法" << std::endl;
        
        // 计算信号的最大最小值差作为AC
        float signal_max = *std::max_element(input_signal.begin(), input_signal.end());
        float signal_min = *std::min_element(input_signal.begin(), input_signal.end());
        ac_component = signal_max - signal_min;
        
        // DC分量：信号均值
        for (float val : input_signal) {
            dc_component += val;
        }
        dc_component /= input_signal.size();
        
        if (dc_component != 0) {
            ratio = ac_component / dc_component;
            
            spo2 = (-3.7465271198e+01f * std::pow(ratio, 3) + 
                    5.8403912586e+01f * std::pow(ratio, 2) + 
                    -3.7079378855e+01f * ratio + 
                    1.0016136403e+02f);
            
            spo2 = std::max(90.0f, std::min(100.0f, spo2));
            
            std::cout << "  估算SpO2 (简化方法): " << std::fixed 
                      << std::setprecision(1) << spo2 << "%" << std::endl;
            std::cout << "  注意: 精度较低，建议增加信号长度" << std::endl;
            
            return true;
        } else {
            std::cout << "  错误: 无法计算SpO2" << std::endl;
            return false;
        }
    }
}

} // namespace ppg

