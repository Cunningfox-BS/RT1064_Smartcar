#ifndef _AprilTagConfig_hpp
#define _AprilTagConfig_hpp

namespace imgProc {
namespace apriltag {

constexpr int maxhamming = 1;               // ����tagʱ����λ
constexpr float_t tagsize = 0.18 * 7 / 11;  // apriltag�ı߳�����λ����
constexpr float_t shift_dist = 0.05;        // 3ά��Ϣת��ά����ʱtagƽ�����ƽ�Ƶľ���

// ����궨����
constexpr float_t fx = 589.70233154;  // In pixels.
constexpr float_t fy = 648.32983398;  // In pixels.
constexpr float_t cx = 376.15877696;  // In pixels.
constexpr float_t cy = 242.75541548;  // In pixels.

}  // namespace apriltag
}  // namespace imgProc

#endif  // _AprilTagConfig_hpp