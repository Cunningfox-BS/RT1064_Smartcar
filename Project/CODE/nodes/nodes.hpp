#ifndef _nodes_hpp
#define _nodes_hpp

#include "Thread.h"
#include "utils/FuncThread.hpp"

// 无线串口接收进程, 用于遥控
bool wirelessNode();

// apriltag检测, 需要初始化mt9v03x
bool apriltagDetectNode();

// 上位机传图, 需要初始化usb_cdc和mt9v03x
bool imgUSBXferNode();

// 用按键和ips屏幕测试电机
bool testMotorNode();

#endif  // _nodes_hpp