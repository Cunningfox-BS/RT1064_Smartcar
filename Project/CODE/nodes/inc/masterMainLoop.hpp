#ifndef _nodes_masterMainLoop_hpp
#define _nodes_masterMainLoop_hpp

#include "utils/FuncThread.hpp"
//
extern "C" {
#include "SEEKFREE_IPS114_SPI.h"
}
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "MasterGlobalVars.hpp"
#include "RectConfig.hpp"
#include "SlaveGlobalVars.hpp"
#include "TSP/TSP.hpp"
#include "bresenham.hpp"
#include "devices.hpp"
#include "edge_detect/A4Detect.hpp"
#include "pose_kalman/magnetAlign.hpp"
#include "pose_kalman/utils.hpp"
//
#include "fieldParam.hpp"
#include "masterConfig.hpp"
#include "naviParam.hpp"

using namespace imgProc;
using namespace imgProc::apriltag;
using namespace imgProc::edge_detect;

constexpr float tsp_k = std::min((M / 4) / borderWidth, (N / 4) / borderHeight);

static int& coords_cnt = masterGlobalVars.coords_cnt;
static auto& coords = masterGlobalVars.coords;
static TSP::TSP_Solver tsp;

constexpr int key_cnt = sizeof(master_key) / sizeof(master_key[0]), switch_cnt = sizeof(master_switch) / sizeof(master_switch[0]);
static bool key_pressing[key_cnt];
// static bool switch_on[switch_cnt];
static inline void keyScan() {
    for (int i = 0; i < key_cnt; ++i) key_pressing[i] = master_key[i].pressing();
    // for (int i = 0; i < switch_cnt; ++i) switch_on[i] = master_switch[i].get();
}

// clang-format off
#define Task_t static inline bool
#define RUN_TASK(entry) { if (masterGlobalVars.reset_requested() || !(entry)) return false; }
#define GUARD_COND(cond) { if (!(cond)) return false; }
#define CHECK_RESET_SIGNAL() { if (masterGlobalVars.reset_requested()) return false; }
#define CHECK_KEY_RESET() { keyScan(); if (key_pressing[0]) return false; }
#define WAIT_FOR(cond) {      \
    while (!(cond)) {         \
        CHECK_KEY_RESET();    \
        CHECK_RESET_SIGNAL(); \
    }                         \
}
#define SHOW_STATE(s) ips114_showstr(188, 4, s);
// clang-format on

namespace utils {
static inline void clear_screen() { ips114_clear(WHITE); }
static inline void sendSlaveTask(SlaveGlobalVars::State task, bool blocking = false) {
    static SerialIO::TxFlag<true> task_tx("task_tx", 255);
    if (task_tx.txFinished(blocking ? -1 : 0)) {
        task_tx.set(task);
        slave_uart.send(task_tx);
    }
}
static inline void sendCoords() {
    static SerialIO::TxArr<float, target_coords_maxn * 2, true> a4_tx(32, "a4_tx");
    a4_tx.txFinished(-1);
    a4_tx.setArr(coords[1], coords_cnt * 2);
    wireless.send(a4_tx);
}
static inline void sendArtSnapshotTask() {
    static uint8_t cmd_id = 0xA5;
    static SerialIO::TxXfer art_xfer(&cmd_id, 1, "art_snapshot_tx");
    art_xfer.txFinished(-1);
    art_uart.send(art_xfer);
}
static inline void sendArtBorderTask() {
    static uint8_t cmd_id = 0x5A;
    static SerialIO::TxXfer art_xfer(&cmd_id, 1, "art_border_tx");
    art_xfer.txFinished(-1);
    art_uart.send(art_xfer);
}
static inline float calcDist2(const float a[2], const float b[2]) {
    float dx = a[0] - b[0], dy = a[1] - b[1];
    return dx * dx + dy * dy;
}
static inline float calcDist(const float a[2], const float b[2]) { return calcDist2(a, b); }

// �ٶ�x��atan2������ģ���[-pi, pi]֮��
constexpr pose_kalman::T wrapAngleNear(pose_kalman::T x, pose_kalman::T cur) {
    using pose_kalman::wrapAngle;
    if (std::abs(wrapAngle(x - cur)) > PI_2) x = wrapAngle(x + PI);
    return x;
}
Task_t moveBaseReachedCheck() {
    for (;;) {
        CHECK_KEY_RESET();
        CHECK_RESET_SIGNAL();
        switch (moveBase.wait_for_result(mainloop_timeout)) {
        case MoveBase::GoalEventFlag::disabled: return false;
        case MoveBase::GoalEventFlag::reached: return true;
        case MoveBase::GoalEventFlag::timeout: break;
        }
    }
}
#define WAIT_MOVE_BASE_GOAL_REACHED GUARD_COND(utils::moveBaseReachedCheck());
Task_t waitArtSnapshot() {
    rt_tick_t start_ms = rt_tick_get_millisecond();
    for (;;) {
        CHECK_KEY_RESET();
        CHECK_RESET_SIGNAL();
        if (masterGlobalVars.wait_art_snapshot(mainloop_timeout)) break;
        if (rt_tick_get_millisecond() - start_ms > art_snapshot_timeout_ms) {
            // TODO
            // masterGlobalVars.send_art_snapshot();
            // masterGlobalVars.send_art_result(ResultCatgory::Major(rand() % 3));
        }
    }
    return true;
}

static inline int find_idle_magnet_index() {
    for (int i = 0; i < magnet::cnt; ++i)
        if (masterGlobalVars.art_results[i] == ResultCatgory::Major::None) return i;
    return -1;
}

static inline int dropCatgory(ResultCatgory::Major catgory) {
    int res = 0;
    for (int i = 0; i < magnet::cnt; ++i)
        if (masterGlobalVars.art_results[i] == catgory) {
            magnets[i].set(0);
            ++res;
            masterGlobalVars.art_results[i] = ResultCatgory::Major::None;
        }
    masterGlobalVars.send_drop_rect();
    return res;
}
static void dropCatgoryEntry() {
    for (;;) {
        masterGlobalVars.wait_drop_rect();
        srv_l.max(), srv_r.max();
        rt_thread_mdelay(magnet_drop_delay_ms);
        srv_l.min(), srv_r.min();
        rt_thread_mdelay(magnet_drop_wait_ms);
        for (auto& mag : magnets) mag.set(1);
    }
}

}  // namespace utils

#endif  // _nodes_masterMainLoop_hpp