#ifndef _AprilTagConfig_hpp
#define _AprilTagConfig_hpp

namespace imgProc {
namespace apriltag {

constexpr int maxhamming = 1;              // ����tagʱ����λ
constexpr float_t tagsize = 0.1145454545;  // apriltag�ı߳�����λ����
constexpr float_t shift_dist = 0.05;       // 3ά��Ϣת��ά����ʱtagƽ�����ƽ�Ƶľ���

// ����궨����
constexpr float_t fx = 397.38711548;  // In pixels.
constexpr float_t fy = 399.70840454;  // In pixels.
constexpr float_t cx = 369.06676129;  // In pixels.
constexpr float_t cy = 226.30497101;  // In pixels.

}  // namespace apriltag
}  // namespace imgProc

#endif  // _AprilTagConfig_hpp