#ifndef FIND_PEAKS_HPP
#define FIND_PEAKS_HPP

#include <vector>
#include <limits>

/**
 * @file find_peaks.hpp
 * @brief C++ implementation of scipy.signal.find_peaks
 * 
 * 仿照scipy.signal.find_peaks实现
 * 主要用于PPG信号的峰值检测
 */

// =====================================================================
// 核心数据结构
// =====================================================================

struct PeakProperties {
    std::vector<int> peak_heights;      // 峰值高度
    std::vector<float> prominences;     // 显著性
    std::vector<int> left_bases;        // 左侧基线位置
    std::vector<int> right_bases;       // 右侧基线位置
};

// =====================================================================
// 函数声明
// =====================================================================

/**
 * @brief 找到信号中的所有局部最大值点
 * 
 * 对应scipy中的 _local_maxima_1d(x)
 * 局部最大值定义：x[i-1] < x[i] >= x[i+1]
 * 
 * @param signal 输入信号
 * @return 局部最大值索引数组
 */
std::vector<int> find_local_maxima(const std::vector<float>& signal);

/**
 * @brief 根据distance约束过滤峰值
 * 
 * 对应scipy中的 _select_by_peak_distance(peaks, priority, distance)
 * 
 * 算法：
 * 1. 按优先级（峰值高度）排序
 * 2. 贪心选择：保留优先级高的峰值，删除距离太近的峰值
 * 
 * @param peaks 峰值索引
 * @param signal 原始信号
 * @param distance 最小间距
 * @return 过滤后的峰值索引
 */
std::vector<int> filter_peaks_by_distance(
    const std::vector<int>& peaks,
    const std::vector<float>& signal,
    int distance
);

/**
 * @brief 根据height约束过滤峰值
 * 
 * @param peaks 峰值索引
 * @param signal 原始信号
 * @param min_height 最小高度（默认：负无穷）
 * @param max_height 最大高度（默认：正无穷）
 * @return 过滤后的峰值索引
 */
std::vector<int> filter_peaks_by_height(
    const std::vector<int>& peaks,
    const std::vector<float>& signal,
    float min_height = -std::numeric_limits<float>::infinity(),
    float max_height = std::numeric_limits<float>::infinity()
);

/**
 * @brief 计算峰值的显著性（prominence）
 * 
 * Prominence定义：峰值相对于周围最低点的高度
 * 
 * @param signal 原始信号
 * @param peak_idx 峰值索引
 * @param left_base 左侧基线位置（输出）
 * @param right_base 右侧基线位置（输出）
 * @return prominence值
 */
float calculate_prominence(
    const std::vector<float>& signal,
    int peak_idx,
    int& left_base,
    int& right_base
);

/**
 * @brief 根据prominence约束过滤峰值
 * 
 * @param peaks 峰值索引
 * @param signal 原始信号
 * @param min_prominence 最小显著性（默认：0.0）
 * @return 过滤后的峰值索引
 */
std::vector<int> filter_peaks_by_prominence(
    const std::vector<int>& peaks,
    const std::vector<float>& signal,
    float min_prominence = 0.0f
);

/**
 * @brief C++版本的scipy.signal.find_peaks
 * 
 * 用法示例：
 *   std::vector<float> signal = {1, 3, 5, 4, 2, 3, 6, 4, 1};
 *   auto peaks = find_peaks(signal, 2);  // distance=2
 *   // peaks = {2, 6}
 * 
 * @param signal 输入信号
 * @param distance 峰值最小间距（样本数，默认：0）
 * @param min_height 峰值最小高度（默认：无限制）
 * @param min_prominence 峰值最小显著性（默认：不使用，-1.0表示禁用）
 * @return 峰值索引数组
 */
std::vector<int> find_peaks(
    const std::vector<float>& signal,
    int distance = 0,
    float min_height = -std::numeric_limits<float>::infinity(),
    float min_prominence = -1.0f
);

/**
 * @brief 完整版find_peaks，返回peaks和properties
 * 
 * @param signal 输入信号
 * @param peaks 输出：峰值索引
 * @param properties 输出：峰值属性
 * @param distance 峰值最小间距（默认：0）
 * @param min_height 最小高度（默认：无限制）
 * @param min_prominence 最小显著性（默认：不使用）
 */
void find_peaks_with_properties(
    const std::vector<float>& signal,
    std::vector<int>& peaks,
    PeakProperties& properties,
    int distance = 0,
    float min_height = -std::numeric_limits<float>::infinity(),
    float min_prominence = -1.0f
);

#endif // FIND_PEAKS_HPP

