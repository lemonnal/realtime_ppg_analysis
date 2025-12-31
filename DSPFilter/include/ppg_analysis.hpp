#ifndef PPG_ANALYSIS_HPP
#define PPG_ANALYSIS_HPP

#include <vector>

namespace ppg {

/**
 * @brief 检测PPG信号的峰值和谷值
 * @param filtered_signal 滤波后的信号
 * @param sample_rate 采样率 (Hz)
 * @param min_time_interval 最小峰值时间间隔 (秒)
 * @param peaks 输出：峰值索引数组
 * @param valleys 输出：谷值索引数组
 * @param ac_component 输出：平均AC分量
 */
void detect_peaks_and_valleys(
    const std::vector<float>& filtered_signal,
    double sample_rate,
    double min_time_interval,
    std::vector<int>& peaks,
    std::vector<int>& valleys,
    float& ac_component
);

/**
 * @brief 基于AC/DC比率估算SpO2
 * @param input_signal 原始输入信号（用于计算DC）
 * @param filtered_signal 滤波后信号（用于计算AC）
 * @param peaks 峰值索引数组
 * @param valleys 谷值索引数组
 * @param ac_component AC分量值
 * @param spo2 输出：估算的SpO2值 (%)
 * @param ratio 输出：AC/DC比率
 * @return true表示计算成功
 */
bool calculate_spo2_from_ppg(
    const std::vector<float>& input_signal,
    const std::vector<float>& filtered_signal,
    const std::vector<int>& peaks,
    const std::vector<int>& valleys,
    float ac_component,
    float& spo2,
    float& ratio
);

} // namespace ppg

#endif // PPG_ANALYSIS_HPP

