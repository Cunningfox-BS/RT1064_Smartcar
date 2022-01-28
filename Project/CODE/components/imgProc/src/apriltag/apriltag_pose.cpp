#include "apriltag/apriltag_pose.hpp"

#include "apriltag/internal/homography.hpp"
#include "apriltag/internal/utility.hpp"

namespace imgProc {
namespace apriltag {

void estimate_pose_for_tag_homography(apriltag_detection_info& info, apriltag_pose& solution) {
    auto& R = solution.R;
    auto& t = solution.t;
    homography_to_pose(info.det->H, info.fx, info.fy, info.cx, info.cy, R, t);

    double scale = info.tagsize / 2;
    rep(i, 0, 3) t[i] *= scale;
    req(i, 1, 2) {
        t[i] = -t[i];
        rep(j, 0, 3) R[i][j] = -R[i][j];
    }
}

}  // namespace apriltag
}  // namespace imgProc