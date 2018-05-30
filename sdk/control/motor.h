#ifndef MOTOR_H
#define MOTOR_H


#include "control_global.h"
#include "qmath.h"
#include "math.h"
#include <QMessageBox>

static char s_data_path[50] = ".\\data\\mtr";


struct Mtr_Data_Blk // 马达参数，可修改保存
{
    double MotorSpeed[3][2]; //[pf][0=vel,1=acc]
    long   LimitP;
    long   LimitN;
    int    StartPos;
    int    EndPos;
    int    CurSpeed;
};

struct Mtr_MotorAxis_Cfg // 轴配置
{
    int pulseperround  ;
    int mmperround     ;
    int home_logic     ;
    int limit_logic    ;
    int alm_logic      ;
    int enable_limit   ;
    int enable_alm     ;
    int pulse_inmode   ;
    int pulse_outmode  ;

    Mtr_MotorAxis_Cfg()
    {
        pulseperround  = 0;
        mmperround     = 0;
        home_logic     = 0;
        limit_logic    = 0;
        alm_logic      = 0;
        enable_limit   = 0;
        enable_alm     = 0;
        pulse_inmode   = 0;
        pulse_outmode  = 0;
    }

    Mtr_MotorAxis_Cfg& operator=(const Mtr_MotorAxis_Cfg &other)
    {
        pulseperround  = other.pulseperround ;
        mmperround     = other.mmperround    ;
        home_logic     = other.home_logic    ;
        limit_logic    = other.limit_logic   ;
        alm_logic      = other.alm_logic     ;
        enable_limit   = other.enable_limit  ;
        enable_alm     = other.enable_alm    ;
        pulse_inmode   = other.pulse_inmode  ;
        pulse_outmode  = other.pulse_outmode ;
        return *this;
    }
};

struct Mtr_Cfg_Blk // 马达配置，每次固定
{
    double  HomeSpeed[2][2];    //[pf] 0:low 1:fast
    int		HomeMode;           //1:h 2:h+i-----改为是否找限位 1：找限位 0：不找限位
    int		HomeTwice;          //1:二次回原点 0:一次回原点;
    int     HomeTwiceRetVal;    //二次回原点时回走微动量，用来避开原点传感器，正值（防止写错）
    int     HomeOffset;         //回原点后微动量
    int     HomeRange;
    int		PulsePerR;
    int		EncOn;		        // 0d010000(配置伺服使能电平) 0d1000（原点有效电平高）0d0100（限位有效电平高）0d0010（私服步进）0d0001（ALM有效电平高）
    int     HomeLimitDir;       //回原点使用此变量相反数
    int     HomeLimitZero;      //找原点信号时的范围限制，正值
    Mtr_MotorAxis_Cfg MotorAxisCfg;
};

struct Mtr_State_Blk // 马达状态
{
    unsigned short	InitOk;
    int		HomeAlready;
    int		HomeState;
    long	HomePos;
    int		LoopEn;
};

class CONTROLSHARED_EXPORT CMotor
{
public:
    CMotor(int card,int axis);

public:
    int GetId();

public:
    int P01_InitCfg(Mtr_Cfg_Blk& para);
    int P01_Init();
    int P01_MotorExit();
    int P01_SetAxisOnOff(int onoff);
    int P01_GetAxisOnOff();
    int P01_SetZeroPos();
    int P01_SetCmdPos(int pos);
    int P01_GetCmdPos();
    int P01_GetCmdUnitPos(double *pos);
    int P01_SetAtlPos(int pos);
    int P01_GetAtlPos();
    int P01_GetAtlUnitPos(double *pos);
    int P01_GetPrfPos();
    int P01_GetCaptPos();
    int P01_IsMoving();
    int P01_GetHomeStatus();
    int P01_SmoothStop();
    int P01_AbptStop();
    int P01_Wait();
    int P01_JogMove(int ex_dir,double ex_vel, double ex_acc,double ex_dcc, BOOL bELU_Ena = TRUE, BOOL bELD_Ena = TRUE);
    int P01_AbsMoveTo(int pos,double ex_vel, double ex_acc, BOOL bELU_Ena = TRUE, BOOL bELD_Ena = TRUE);
    int P01_RelMoveTo(int dist,double ex_vel ,double ex_acc, BOOL bELU_Ena = TRUE, BOOL bELD_Ena = TRUE);
    int P01_SetHomeSpeedEx(int sp, double vel, double acc);
    int P01_GetHomeSpeed(int sp, double& vel, double& acc);
    int P01_Home();
    int P01_StartHome();
    int P01_Homing();
    int P01_RunLimit();
    int P01_StartRunLimit();
    int P01_RunningLimit();
    int P01_SetLimit(long limitp, long limitn);
    int P01_GetLimit(long& limitp, long& limitn);
    int P01_Load();
    int P01_Save();
    int P01_GetAlarmStatus();
    int P01_GetZhengLimitStatus();
    int P01_GetFuLimitStatus();
    int P01_ChangeSpeed(int ex_vel);
    int P01_SetDecStopTime(double stop_time);
    int P01_ClearPos();
    int P01_GetAlmMode(int &enable,int &alm_logic);
    int P01_GetHomePinLogic(int &org_logic);
    int P01_GetLimitPinLogic(int &el_enable,int &el_logic);

public:
    Mtr_Data_Blk  d_MtrData;
    Mtr_Cfg_Blk   d_MtrCfg;
    Mtr_State_Blk d_MtrState;

protected:
    int g_card;
    int g_axis;

protected:
    int m_MotorHomeStep;
public:
    long  CaptPos;
    short int Status;
};

#endif // MOTOR_H
