#include "nodes.hpp"
//
#include "devices.hpp"

static SerialIO::TxUtil<float, 4, true> encoderXfer("encoder", 20);
static SerialIO::TxUtil<float, 4, true> motorOutputXfer("motorOutput", 21);
static MoveBase::ControlState state;

inline void readEncoder() { encoderL1.update(), encoderL2.update(), encoderR1.update(), encoderR2.update(); }

inline void updateControlState() {
    const MoveBase::ControlState &cur_state = moveBase.loadControlState();
    state ^= cur_state;
    if (state.L1()) motorCtrlL1.handleStateChange(cur_state.L1());
    if (state.L2()) motorCtrlL2.handleStateChange(cur_state.L2());
    if (state.R1()) motorCtrlR1.handleStateChange(cur_state.R1());
    if (state.R2()) motorCtrlR2.handleStateChange(cur_state.R2());
    state = cur_state;
}

inline void applyMotorCtrl() {
    if (state.L1()) motorCtrlL1.update();
    if (state.L2()) motorCtrlL2.update();
    if (state.R1()) motorCtrlR1.update();
    if (state.R2()) motorCtrlR2.update();
}

inline void uploadDebugData() {
    if (encoderXfer.txFinished()) {
        encoderXfer.setAll(encoderL1.get(), encoderL2.get(), encoderR1.get(), encoderR2.get());
        wireless.send(encoderXfer);
    }
    if (motorOutputXfer.txFinished()) {
        motorOutputXfer.setAll(motorCtrlL1.getOutput(), motorCtrlL2.getOutput(), motorCtrlR1.getOutput(),
                               motorCtrlR2.getOutput());
        wireless.send(motorOutputXfer);
    }
}

static void motorControlEntry() {
    readEncoder();
    updateControlState();
    applyMotorCtrl();
    uploadDebugData();
}

bool motorControlNode() { return FuncTimer(motorControlEntry, "motorControl", Param::MotorControlPeriod); }