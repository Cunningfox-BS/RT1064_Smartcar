#include "utils/FuncThread.hpp"
//

#include "devices.hpp"
#include "pose_kalman/params.hpp"

namespace pose_kalman {

static inline void setupSystemCovariance() {
    T sysCov[6][6]{0};
    sysCov[0][0] = sys_xy_sigma2;
    sysCov[1][1] = sys_xy_sigma2;
    sysCov[2][2] = sys_yaw_sigma2;
    sysCov[3][3] = sys_v_xy_sigma2;
    sysCov[4][4] = sys_v_xy_sigma2;
    sysCov[5][5] = sys_v_yaw_sigma2;
    kf.setSystemCovariance(sysCov[0]);
}

static inline void setupSetStateCovariance() {
    T setStateCov[6][6]{0};
    for (int i = 0; i < 6; ++i) setStateCov[i][i] = 1e-6;
    kf.setMeasurementCovariance(MeasurementType::SetState, setStateCov[0]);
}

static inline void setupOdomCovariance() {
    T odomCov[3][3]{0};
    odomCov[0][0] = odom_v_xy_sigma2;
    odomCov[1][1] = odom_v_xy_sigma2;
    odomCov[2][2] = odom_v_yaw_sigma2;
    kf.setMeasurementCovariance(MeasurementType::Odom, odomCov[0]);
}

static inline void setupGyroCovariance() {
    T gyroCov[1][1]{0};
    gyroCov[0][0] = gyro_v_yaw_sigma2;
    kf.setMeasurementCovariance(MeasurementType::Gyro, gyroCov[0]);
}

static inline void setupYawCovariance() {
    T yawCov[1][1]{0};
    yawCov[0][0] = yaw_yaw_sigma2;
    kf.setMeasurementCovariance(MeasurementType::Yaw, yawCov[0]);
}

static inline void setupRectCovariance() {
    T rectCov[2][2]{0};
    rectCov[0][0] = rect_xy_sigma2;
    rectCov[1][1] = rect_xy_sigma2;
    kf.setMeasurementCovariance(MeasurementType::Rect, rectCov[0]);
}

static inline void setupPredictCovariance() {
    T predictCov[6][6]{0};
    predictCov[0][0] = 0.1;
    predictCov[1][1] = 0.1;
    predictCov[2][2] = 0.1;
    predictCov[3][3] = 0.1;
    predictCov[4][4] = 0.1;
    predictCov[5][5] = 0.1;
    kf.setPredictionCovariance(predictCov[0]);
}

static inline void setInitialState() {
    T state[6]{0};
    kf.setState(state);
}

static void runLocalPlanner(uint64_t timestamp_us, const T state[6]) {
    static SerialIO::TxUtil<float, 6, true> goal_tx("goal", 31);
    static SerialIO::TxUtil<float, 3, true> cmd_vel_tx("cmd_vel", 22);
    if (!moveBase.get_enabled()) return;
    auto& goal = moveBase.get_goal();
    if (goal.reached) return;

    static bool has_reached = false;
    static uint64_t last_reached_timestamp_us;
    static T cmd_vel[3]{0};

    if (moveBase.new_goal()) {
        has_reached = false;
        if (goal_tx.txFinished()) {
            goal_tx.setAll(goal.x, goal.y, goal.yaw, 0, 0, 0);
            wireless.send(goal_tx);
        }
    }

    if (localPlanner.getControlCmd(state, state + 3, goal, cmd_vel)) {
        if (!has_reached) {
            has_reached = true, last_reached_timestamp_us = timestamp_us;
        } else if (systick.get_diff_us(last_reached_timestamp_us, timestamp_us) > goal.time_tolerance_us) {
            moveBase.set_reached(true);
        }
    } else {
        has_reached = false;
    }
    baseDriver.cmd_vel(cmd_vel[0], cmd_vel[1], cmd_vel[2]);

    if (cmd_vel_tx.txFinished()) {
        cmd_vel_tx.setArr(cmd_vel);
        wireless.send(cmd_vel_tx);
    }
}

static void poseKalmanEntry() {
    static SerialIO::TxUtil<float, 6, true> pose_tx("pose", 30);
    static SerialIO::TxUtil<float, 1, true> timestamp_tx("timestamp", 23);

    setupSystemCovariance();

    setupOdomCovariance();
    setupGyroCovariance();
    setupYawCovariance();
    setupRectCovariance();
    setupSetStateCovariance();

    setupPredictCovariance();

    setInitialState();
    rt_thread_mdelay(100);
    kf.setEnabled(true);
    for (;;) {
        MoveBase::State set_state;
        if (moveBase.get_set_state(set_state))
            kf.enqueMeasurement(MeasurementType::SetState, set_state.state, set_state.timestamp_us);

        uint64_t timestamp_us = systick.get_us();
        kf.update(timestamp_us);
        const T* state = kf.getState();
        moveBase.send_state(timestamp_us, state);
        runLocalPlanner(timestamp_us, state);

        if (pose_tx.txFinished()) {
            pose_tx.setArr(state);
            wireless.send(pose_tx);
        }

        if (timestamp_tx.txFinished()) {
            timestamp_tx.setAll(timestamp_us);
            wireless.send(timestamp_tx);
        }

        int64_t delay_time = predict_period_us - systick.get_delta_us(timestamp_us);
        delay_time = (delay_time + 500) / 1000;
        if (delay_time > 0) rt_thread_mdelay(delay_time);
    }
}

}  // namespace pose_kalman

bool poseKalmanNode() { return FuncThread(pose_kalman::poseKalmanEntry, "poseKalman", 15360); }