#ifndef REALTIME_FILTER_HPP
#define REALTIME_FILTER_HPP

#include "DspFilters/Dsp.h"
#include <vector>
#include <deque>

namespace ppg {

/**
 * @brief 实时IIR带通滤波器类（逐样本处理）
 * 
 * 该类封装了Butterworth带通滤波器，支持逐样本输入和输出，
 * 适合嵌入式实时系统使用。
 */
class RealtimeFilter {
public:
    /**
     * @brief 构造函数
     * @param low_freq 低频截止频率 (Hz)
     * @param high_freq 高频截止频率 (Hz)
     * @param sample_rate 采样率 (Hz)
     * @param filter_order 滤波器阶数
     */
    RealtimeFilter(double low_freq, double high_freq, 
                   double sample_rate, int filter_order = 3);
    
    /**
     * @brief 处理单个样本
     * @param input 输入样本值
     * @return 滤波后的样本值
     */
    float process_sample(float input);
    
    /**
     * @brief 重置滤波器状态
     */
    void reset();
    
    /**
     * @brief 使用初始值预热滤波器（减少瞬态响应）
     * @param initial_value 初始值（通常使用信号的均值）
     * @param num_samples 预热样本数
     */
    void warmup(float initial_value, int num_samples = 100);
    
private:
    Dsp::SimpleFilter<Dsp::Butterworth::BandPass<6>, 1> filter_;
    double low_freq_;
    double high_freq_;
    double sample_rate_;
    int filter_order_;
};

/**
 * @brief 实时数据缓冲区类（滑动窗口）
 */
class RealtimeBuffer {
public:
    /**
     * @brief 构造函数
     * @param capacity 缓冲区容量（样本数）
     */
    RealtimeBuffer(size_t capacity);
    
    /**
     * @brief 添加新样本到缓冲区
     * @param sample 样本值
     */
    void push(float sample);
    
    /**
     * @brief 获取缓冲区中的所有数据
     * @return 数据向量
     */
    std::vector<float> get_data() const;
    
    /**
     * @brief 获取缓冲区大小
     * @return 当前样本数
     */
    size_t size() const { return buffer_.size(); }
    
    /**
     * @brief 检查缓冲区是否已满
     * @return true表示已满
     */
    bool is_full() const { return buffer_.size() == capacity_; }
    
    /**
     * @brief 清空缓冲区
     */
    void clear() { buffer_.clear(); }
    
    /**
     * @brief 获取最新的样本
     * @return 最新样本值
     */
    float get_latest() const { 
        return buffer_.empty() ? 0.0f : buffer_.back(); 
    }
    
private:
    std::deque<float> buffer_;
    size_t capacity_;
};

} // namespace ppg

#endif // REALTIME_FILTER_HPP

