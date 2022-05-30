#include "GlobalVars.hpp"

#include "edge_detect/A4Detect.hpp"
#include "utils/InterruptGuard.hpp"

using namespace imgProc::edge_detect;

GlobalVars::GlobalVars() { rt_event_init(&coord_recv_event, "coord_recv_event", RT_IPC_FLAG_PRIO); }

bool GlobalVars::wait_for_coord_recv(rt_int32_t timeout) {
    return rt_event_recv(&coord_recv_event, 1, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, timeout, RT_NULL) == RT_EOK;
}

void GlobalVars::send_coord_recv(int cnt, const float* coords) {
    {
        ScheduleGuard guard;
        target_coords_cnt = cnt;
        rt_memcpy(target_coords_corr, coords, sizeof(target_coords_corr[0]) * cnt);
    }
    rt_event_send(&coord_recv_event, 1);
}

void GlobalVars::get_coord_recv(int& cnt, float* coords) {
    ScheduleGuard guard;
    cnt = target_coords_cnt;
    rt_memcpy(coords, target_coords_corr, sizeof(target_coords_corr[0]) * cnt);
}

GlobalVars globalVars;