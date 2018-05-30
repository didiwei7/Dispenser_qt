#ifndef AXIS_H
#define AXIS_H

#include "control_global.h"

class CONTROLSHARED_EXPORT Axis
{
public:
    Axis();

    // 运动
    int Home(); // 回原点
    int HomeAsCapture();       //捕获回原
    int HomeAsNormal();        //正常回原
    int HomeAsSelf();          //自定义回原
    int HomeFun();             //回原函数
    int DDHome();              //DD马达回原
    // int HomeFinished() { return m_iHomeResult; }                                        // 是否回原结束
    int MovePos(double pos, double vel = 0, double acc = 0, double dec = 0);            // 移动到指定位置, 运动中可更换指令
    int MoveRealOffset(long offset, double vel = 0, double acc = 0, double dec = 0);    // 移动相对位置, 脉冲位置
    int MoveOffset(double offset, double vel = 0, double acc = 0, double dec = 0,int basePos=0);     //移动相对位置
    int MoveJog(int dir, double vel = 0, double acc = 0, double dec = 0);               // 连续移动
    bool MoveToLimitN();        //移动到负限位
    bool MoveToLimitP();        //移动到正限位
    bool WaitExitMoveSign(int iWait);   //等待运动退出信号，若返回true，则说明有退出信号

    // 速度
    int SetSpeed(double vel = 0, double acc = 0, double dec = 0);   //设置速度，若不合法则使用默认速度

    // 点位
    void GetCurPos(double &pos); // 获取当前位置

    // 状态
    bool IsMoving();
    bool Stop(int type = 0);        //停止运动, 包括回原
    bool StopAndWait(int type = 0); //停止并等待停稳, 内部用
    bool eStop();                   // 急停
    bool smoothStop();              // 平滑停止
    bool isHome(unsigned short cardIndex,unsigned short axisIndex); //是否在原点
    bool isHomeP();                 //判断是否处于原点位置
    bool isLimitP();                //判断是否到达正限位
    bool isLimitN();                //判断是否到达负限位
    bool isDrvAlarm();              //判断驱动器报警
    bool isReady();                 //判断轴已经ready
    bool isMoveOK();                //判断已经运动完成
    bool setZeroPos();              //清零位置信息
};



#endif // AXIS_H
