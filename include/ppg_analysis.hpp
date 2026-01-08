#ifndef PPG_ANALYSIS_HPP
#define PPG_ANALYSIS_HPP

#include <vector>

namespace ppg
{

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
        const std::vector<float> &filtered_signal,
        double sample_rate,
        double min_time_interval,
        std::vector<int> &peaks,
        std::vector<int> &valleys,
        float &ac_component);

    /**
     * @brief 基于双通道（红光+红外光）AC/DC比率估算SpO2
     * @param red_input 红光原始信号
     * @param red_filtered 红光滤波后信号
     * @param red_ac 红光AC分量
     * @param ir_input 红外光原始信号
     * @param ir_filtered 红外光滤波后信号
     * @param ir_ac 红外光AC分量
     * @param spo2 输出：估算的SpO2值 (%)
     * @param ratio 输出：R值 (红光AC/DC) / (红外光AC/DC)
     * @return true表示计算成功
     */
    bool calculate_spo2_dual_channel(
        const std::vector<float> &red_input,
        const std::vector<float> &red_filtered,
        float red_ac,
        const std::vector<float> &ir_input,
        const std::vector<float> &ir_filtered,
        float ir_ac,
        float &spo2,
        float &ratio);

    /**
     * @brief 基于峰值间隔计算心率
     * @param peaks 峰值索引数组
     * @param sample_rate 采样率 (Hz)
     * @param heart_rate 输出：估算的心率 (BPM)
     * @param hrv 输出：心率变异性 (标准差，单位ms)
     * @return true表示计算成功
     */
    bool calculate_heart_rate(
        const std::vector<int> &peaks,
        double sample_rate,
        float &heart_rate,
        float &hrv);

} // namespace ppg

#endif // PPG_ANALYSIS_HPP
