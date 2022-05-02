#include "utils/FuncThread.hpp"
//

#include "Eigen/Eigen"
#include "devices.hpp"
#include "pose_kalman/LocalPlanner.hpp"
#include "pose_kalman/NoiseGenerator.hpp"
#include "pose_kalman/params.hpp"
#include "pose_kalman/utils.hpp"
namespace pose_kalman {

static constexpr T xy_sigma2 = 1;
static constexpr T yaw_sigma2 = 3;
using Vector3 = Eigen::Matrix<T, 3, 1>;
using Matrix2 = Eigen::Matrix<T, 2, 2>;
static void testLocalPlannerEntry() {
    static SerialIO::TxUtil<float, 6, true> pose_tx("pose", 30);
    static LocalPlanner lp;
    static NoiseGenerator xy_noise(xy_sigma2);
    static NoiseGenerator yaw_noise(yaw_sigma2);

    lp.params.acc_lim_xy = 1;
    lp.params.acc_lim_yaw = 1;
    lp.params.vel_lim_xy = 2;
    lp.params.vel_lim_yaw = 3;
    lp.params.xy_goal_tolerance = 5 * 1e-3;
    lp.params.yaw_goal_tolerance = 0.1;

    T state[6]{0};

    Eigen::Map<Vector3> pose(state);
    Eigen::Map<Vector3> vel(state + 3);
    Vector3 target{10, 3, 3.14};
    Vector3 cmd_acc{0, 0, 0};

    for (;;) {
        if (lp.getControlCmd(pose.data(), vel.data(), target.data(), cmd_acc.data())) {}
        vel += cmd_acc * predict_period_s;
        // transform to global coordinate
        const T cy = std::cos(pose[2]), sy = std::sin(pose[2]);
        Matrix2 rot{{cy, -sy}, {sy, cy}};
        Vector3 global_vel = vel;
        global_vel.segment(0, 2).noalias() = rot * vel.segment(0, 2);
        pose += global_vel * predict_period_s;
        pose[2] = wrapAngle(pose[2]);
        if (pose_tx.txFinished()) {
            pose_tx.setArr(state);
            wireless.send(pose_tx);
        }
        rt_thread_mdelay(predict_period_ms);
    }
}

}  // namespace pose_kalman

bool testLocalPlannerNode() { return FuncThread(pose_kalman::testLocalPlannerEntry, "testLocalPlanner"); }