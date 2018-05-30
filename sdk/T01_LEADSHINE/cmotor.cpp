/************************************************************
Copyright (C), 2011, 深圳市图德电子有限公司
FileName:      cmotor.cpp
Author:        范勇
Version :      1.00.01
Date:          2011.7
Description:
Function List:

History:
<author>  <time>   <version >   <desc>

修改记录：1 碰到限位会停止但不会断伺服

***********************************************************/

#include "cmotor.h"
#include "math.h"

#include <direct.h>
#include "io.h"

BOOL g_bIniMotorOK   = FALSE;
BOOL g_bIniIOOK      = FALSE;
BOOL g_bConfigLoaded = FALSE;


#define   Find_P_Goto_Org      0
#define   Find_N_Goto_Org      1
#define   P_GoTo_Org           2
#define   N_GoTo_Org           3
#define   Signal_To_Home       4

//-----------------------------------------------------------
int AFX_API_EXPORT OpenCard_leaser(int mnums,int ionums)
{
    int rtn;
    if (0 != mnums)
    {
        rtn = dmc_board_init();
        if(mnums != rtn)
        {
            return 1;
        }

        for (int i = 0;i<ionums;i++)
        {
            rtn = dmc_write_outport(i,0,0XFFFFFFF);
        }
        g_bIniMotorOK = TRUE;
    }
    if (0 != ionums)
    {
        rtn = ioc_board_init();
        if(ionums != rtn)//20140717 防止卡0打开卡1未打开
        {
            return 2;
        }
        for (int i = 0;i<ionums;i++)
        {
            //ioc_write_outport(i,0,0);
            //ioc_write_outport(i,0,0X0);
        }
        g_bIniIOOK = TRUE;
    }
    return 0;
}

/********************************************************************************
** 函数名称: P01_LoadConfigFile(double stop_time)
** 功能描述: 设置减速停止时间
** 输　  入:
** stop_time 减速时间 s
** 返 回 值: 0：设置失败； 1：ok
**
*********************************************************************************/
int AFX_API_EXPORT LoadConfigFile(int cardindex ,char* path)
{

    int error_num = dmc_download_configfile(cardindex,path);
    if (0 != error_num)
    {
        return 22;
    }
    g_bConfigLoaded = true;
    return 0;
}
int AFX_API_EXPORT CloseIO_leaser(int mnums,int ionums)
{

    for (int i = 0;i<ionums;i++)
    {
        ioc_write_outport(i,0,0XFFFFFFFF);
    }
    return 0;
}
int AFX_API_EXPORT CloseCard_leaser(int mnums,int ionums)
{
    if (0 != mnums)
    {
        if (g_bIniMotorOK)
        {
            dmc_board_close();
        }
        g_bIniMotorOK = FALSE;
    }
    if (0 != ionums)
    {
        if (g_bIniIOOK)
        {
            ioc_board_close();
        }
        g_bIniIOOK = FALSE;
    }
    return 0;
}
//-----------------------------------------------------------
CMotor::CMotor(int card,int axis)
{
    g_card = card;
    g_axis = axis;

}
CMotor::~CMotor()
{
}

/********************************************************************************
** 函数名称: P01_InitCfg
** 功能描述: 初始化回原点参数
** 输　  入: para  参数结构体
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_InitCfg(Mtr_Cfg_Blk& para)
{
    d_MtrCfg.HomeTwice          = para.HomeTwice;       //回原次数
    d_MtrCfg.HomeTwiceRetVal    = para.HomeTwiceRetVal; //回原速度
    d_MtrCfg.HomeMode           = para.HomeMode;        //回原模式
    d_MtrCfg.HomeOffset         = para.HomeOffset;      //二次回原点时回走微动量回原后的偏移量
    d_MtrCfg.HomeRange          = para.HomeRange;       //回原后的偏移量
    d_MtrCfg.PulsePerR          = para.PulsePerR;       //多少脉冲每转
    d_MtrCfg.EncOn              = para.EncOn;
    d_MtrCfg.HomeLimitDir       = para.HomeLimitDir;    //回原方向
    d_MtrCfg.HomeLimitZero      = para.HomeLimitZero;
    d_MtrCfg.MotorAxisCfg       = para.MotorAxisCfg;

    d_MtrState.HomeAlready   = 0;
    d_MtrState.HomePos       = 0;
    d_MtrState.HomeState     = 0;


    if(g_bConfigLoaded)
    {
        d_MtrState.InitOk = 1;
    }
    else
    {
        d_MtrState.InitOk = 0;
    }

    return 0;
}

/********************************************************************************
** 函数名称: P01_Init
** 功能描述: 初始化电机设置
** 输　  入: 无
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_Init()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }

    if (d_MtrCfg.EncOn)//配置伺服使能电平
    {
        dmc_write_sevon_pin(g_card,g_axis, 0);  //0：低电平伺服使能  1：高电平伺服使能
    }

    dmc_set_home_pin_logic(g_card,g_axis,d_MtrCfg.MotorAxisCfg.home_logic,0);

    dmc_set_el_mode(g_card,g_axis,d_MtrCfg.MotorAxisCfg.enable_limit,d_MtrCfg.MotorAxisCfg.limit_logic,1);

    dmc_set_alm_mode(g_card,g_axis,d_MtrCfg.MotorAxisCfg.enable_alm,d_MtrCfg.MotorAxisCfg.alm_logic,0);

    dmc_set_pulse_outmode(g_card,g_axis,d_MtrCfg.MotorAxisCfg.pulse_inmode);       //设置脉冲模式//
    dmc_set_counter_inmode(g_card,g_axis,d_MtrCfg.MotorAxisCfg.pulse_outmode);       //设置编码器计数方式//

    int error;
    error = dmc_set_equiv(0, 0, 1667); //设置X 号轴脉冲当量为1000pulse/unit，每个unit位1mm//
    if (error != 0)
    {
        QMessageBox::about(NULL, NULL, QStringLiteral("卡0轴0脉冲当量设置失败!"));
    }

    error = dmc_set_equiv(0, 1, 1000); //设置Y 号轴脉冲当量为1667pulse/unit，每个unit位1mm//
    if (error != 0)
    {
        QMessageBox::about(NULL, NULL, QStringLiteral("卡0轴1脉冲当量设置失败!"));
    }

    error = dmc_set_equiv(0, 2, 1667); //设置Z 号轴脉冲当量为1000pulse/unit，每个unit位1mm//
    if (error != 0)
    {
        QMessageBox::about(NULL, NULL, QStringLiteral("卡0轴2脉冲当量设置失败!"));
    }

    error = dmc_set_equiv(1, 0, 1667); //设置X 号轴脉冲当量为1000pulse/unit，每个unit位1mm//
    if (error != 0)
    {
        QMessageBox::about(NULL, NULL, QStringLiteral("卡1轴0脉冲当量设置失败!"));
    }

    error = dmc_set_equiv(1, 1, 1000); //设置Y 号轴脉冲当量为1667pulse/unit，每个unit位1mm//
    if (error != 0)
    {
        QMessageBox::about(NULL, NULL, QStringLiteral("卡1轴1脉冲当量设置失败!"));
    }

    error = dmc_set_equiv(1, 2, 1667); //设置Z 号轴脉冲当量为1000pulse/unit，每个unit位1mm//
    if (error != 0)
    {
        QMessageBox::about(NULL, NULL, QStringLiteral("卡1轴2脉冲当量设置失败!"));
    }

    d_MtrState.InitOk = 1;

    return 0;
}

/********************************************************************************
** 函数名称: P01_MotorExit
** 功能描述:
** 输　  入: 无
** 返 回 值: 0：。。； 1：。。；  2：。。
**
*********************************************************************************/
int CMotor::P01_MotorExit()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;


    d_MtrState.InitOk = 0;
    return 0;
}

/********************************************************************************
** 函数名称: P01_SetAxisOnOff
** 功能描述: 设置使能信号----一般情况下设置低电平有效
** 输　  入: onoff 1：使能   0：取消使能
** 返 回 值: 0：无； 1：ok；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_SetAxisOnOff(int onoff) //
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;
    if (1 == onoff)
    {
        dmc_write_sevon_pin(g_card,g_axis, 0);
    }
    else
    {
        dmc_write_sevon_pin(g_card,g_axis, 1);
    }
    return 0;
}
/********************************************************************************
** 函数名称: P01_GetAxisOnOff
** 功能描述: 读取使能信号  --- 一般低电平有效
** 输　  入:
** 返 回 值: 0 : 未使能,1 : 使能
**
*********************************************************************************/
int CMotor::P01_GetAxisOnOff()
{
    if (!g_bIniMotorOK)
    {
        return 0;
    }
    if(d_MtrState.InitOk == 0)
        return 2;

    int sevonStatus = dmc_read_sevon_pin(g_card,g_axis);
    if (0 == sevonStatus)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/********************************************************************************
** 函数名称: P01_SetCmdPos
** 功能描述: 设置命令脉冲
** 输　  入: pos 设置值
** 返 回 值: 0：无； 1：完成；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_SetCmdPos(int pos)
{
    if (!g_bIniMotorOK)
    {
        return 0;
    }
    if(d_MtrState.InitOk == 0)
        return 2;

    dmc_set_position(g_card,g_axis ,pos);
    return 1;
}

/********************************************************************************
** 函数名称: P01_GetCmdPos
** 功能描述: 获取命令脉冲
** 输　  入: 无
** 返 回 值: 命令脉冲
**
*********************************************************************************/
int CMotor::P01_GetCmdPos()
{
    if (!g_bIniMotorOK)
    {
        return 0;
    }
    long pos=0;
    pos = dmc_get_position(g_card,g_axis);
    return(pos);
}

/********************************************************************************
** 函数名称: P01_GetCmdUnitPos
** 功能描述: 获取命令脉冲
** 输　  入: 无
** 返 回 值: 命令脉冲
**
*********************************************************************************/
int CMotor::P01_GetCmdUnitPos(double *pos)
{
    if (!g_bIniMotorOK)
    {
        return 0;
    }
    //double *pos;
    dmc_get_position_unit(g_card,g_axis,pos);
    return 1;
}

/********************************************************************************
** 函数名称: P01_SetAtlPos
** 功能描述: 设置编码器值
** 输　  入: pos 设置值
** 返 回 值: 0：无； 1：完成；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_SetAtlPos(int pos)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;

    dmc_set_encoder(g_card,g_axis ,pos);
    dmc_set_position(g_card,g_axis ,pos);
    return 0;
}

/********************************************************************************
** 函数名称: P01_GetAtlPos
** 功能描述: 读取编码器
** 输　  入: 无
** 返 回 值: 编码器值
**
*********************************************************************************/
int CMotor::P01_GetAtlPos()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }

    int pos=0;
    if(d_MtrCfg.EncOn & 2)
    {
        pos = dmc_get_encoder(g_card,g_axis);
    }
    else
    {
        pos = dmc_get_position(g_card,g_axis);
    }


    return pos;
}

/********************************************************************************
** 函数名称: P01_GetAtlUnitPos
** 功能描述: 读取编码器
** 输　  入: 无
** 返 回 值: 编码器值
**
*********************************************************************************/
int CMotor::P01_GetAtlUnitPos(double *pos)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }

    //int pos=0;
    if(d_MtrCfg.EncOn & 2)
    {
        dmc_get_encoder_unit(g_card,g_axis,pos);
    }
    else
    {
        dmc_get_position_unit(g_card,g_axis,pos);
    }

    return 1;
}

/********************************************************************************
** 函数名称: P01_GetPrfPos
** 功能描述: 无
** 输　  入: 无
** 返 回 值: 0：结束； 1：无；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_GetPrfPos()
{
    return 6;
}

/********************************************************************************
** 函数名称: P01_GetCaptPos
** 功能描述: 无
** 输　  入: 无
** 返 回 值: 0：结束；
**
*********************************************************************************/
int CMotor::P01_GetCaptPos()
{
    return 6;
}

/********************************************************************************
** 函数名称: P01_IsMoving
** 功能描述: 判断轴是否正在运动
** 输　  入: 无
** 返 回 值: 0：停止； 1：正在运动
**
*********************************************************************************/
int CMotor::P01_IsMoving() //1=moving,0=stop
{
    if (!g_bIniMotorOK)
    {
        return 0;
    }
    long status = 0;
    status = dmc_check_done(g_card,g_axis) ;
    return !status;
}

/********************************************************************************
** 函数名称: P01_GetHomeStatus
** 功能描述: 获取原点信号状态
** 输　  入: 无
** 返 回 值: 0：OFF； 1：ON；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_GetHomeStatus()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;

    long home;
    unsigned short h = 0x0001;

    home = dmc_axis_io_status(g_card,g_axis);

    h = h << 4;

    if(home & h)
        return 1;
    else
        return 0;
}

/********************************************************************************
** 函数名称: P01_GetAlarmStatus
** 功能描述: 获取伺服报警信号
** 输　  入: 无
** 返 回 值: 0：OFF； 1：ON；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_GetAlarmStatus()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;

    long alarm;
    unsigned short h = 0x0001;
    alarm = dmc_axis_io_status(g_card,g_axis);
    h = h << 0;

    if(alarm & h)
        return 1;
    else
        return 0;
}

/********************************************************************************
** 函数名称: P01_GetZhengLimitStatus
** 功能描述: 获取正限位状态
** 输　  入: 无
** 返 回 值: 0：OFF； 1：ON；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_GetZhengLimitStatus()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;

    long zhengLimit;
    unsigned short h = 0x0001;

    zhengLimit = dmc_axis_io_status(g_card,g_axis);

    h = h << 1; //正限位

    if(zhengLimit & h)
        return 1;
    else
        return 0;




}

/********************************************************************************
** 函数名称: P01_GetFuLimitStatus
** 功能描述: 获取负限位状态
** 输　  入: 无
** 返 回 值: 0：OFF； 1：ON；  2：电机未初始化
**
*********************************************************************************/
int CMotor::P01_GetFuLimitStatus()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;

    long fuLimit;
    unsigned short h = 0x0001;

    fuLimit = dmc_axis_io_status(g_card,g_axis);

    h = h << 2; //负限位

    if(fuLimit & h)
        return 1;
    else
        return 0;
}

/********************************************************************************
** 函数名称: P01_SmoothStop
** 功能描述: 减速停
** 输　  入: 无
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_SmoothStop()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    dmc_set_dec_stop_time(g_card,g_axis,0.05);
    dmc_stop(g_card,g_axis,0);
    return 0;
}

/********************************************************************************
** 函数名称: P01_AbptStop
** 功能描述: 急停
** 输　  入: 无
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_AbptStop()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    dmc_stop(g_card,g_axis,1);     //1:紧急停止;
    return 0;
}

/********************************************************************************
** 函数名称: P01_Wait
** 功能描述: 等待轴运动停止
** 输　  入: 无
** 返 回 值: 1：停止运动
**
*********************************************************************************/
int CMotor::P01_Wait()
{
    while (P01_IsMoving())
    {
        Sleep(1);
    }
    return(1);
}

/********************************************************************************
** 函数名称: P01_JogMove
** 功能描述: 连续运动
** 输　  入:
** ex_dir 方向 (1:正方向,0:负方向)
** ex_vel 速度 r/min
** ex_acc 加速时间
** ex_dcc 减速时间
** bELU_Ena 是否判断正限位信号（默认判断）
** bELD_Ena 是否判断负限位信号
** 返 回 值: 0：运动失败； 1：ok
**
*********************************************************************************/
int CMotor::P01_JogMove(int ex_dir,double ex_vel, double ex_acc,double ex_dcc, BOOL bELU_Ena, BOOL bELD_Ena)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)// 未初始化
    {
        return 8;
    }

    if(ex_vel == 0 || ex_acc == 0 || ex_dcc == 0)
    {
        return 10;
    }

    double vel,acc,dcc;
    vel = ex_vel;
    acc = ex_acc;
    dcc = ex_dcc;

    if (ex_dir <= 0)
    {
        ex_dir = 0;
    }
    else
    {
        ex_dir = 1;
    }

    dmc_set_profile(g_card, g_axis, vel/2.0, vel, acc, dcc, 0.0);
    dmc_set_s_profile(g_card,g_axis,0,0.05);
    dmc_vmove(g_card,g_axis,ex_dir);
    return 0;
}

/********************************************************************************
** 函数名称: P01_AbsMoveTo
** 功能描述: 绝对运动
** 输　  入: pos  位移，单位：脉冲
ex_vel 速度 r/min
ex_acc 加减速时间
bELU_Ena 是否判断正限位信号（默认判断）
bELD_Ena 是否判断负限位信号
** 返 回 值: 0：运动失败； 1：ok
**
*********************************************************************************/
int CMotor::P01_AbsMoveTo(int pos, double ex_vel, double ex_acc, BOOL bELU_Ena, BOOL bELD_Ena)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }//正向移动判断提示有无接触正向限位，便于分析问题；负向同理

    long lp, ln;
    P01_GetLimit(lp, ln);
    if(d_MtrState.InitOk == 0)// 未初始化
    {
        return 8;
    }
    if(d_MtrState.HomeAlready == 0 )  //没有归位
    {
        return 11;
    }

    if(( pos > lp ) || ( pos < ln ))//超限位
    {
        return 12;
    }

    if(ex_vel == 0 || ex_acc == 0)
    {
        return 10;
    }

    double vel,acc;
    vel = ex_vel;
    acc = ex_acc;


    dmc_set_profile(g_card,g_axis,vel/2.0,vel,acc,acc,vel/2.0);
    dmc_set_s_profile(g_card,g_axis,0,0.05);
    dmc_pmove(g_card, g_axis, pos, 1);   //0:相对运动  1:绝对运动

    return 0;
}

/********************************************************************************
** 函数名称: P01_RelMoveTo
** 功能描述: 相对运动
** 输　  入: dist  位移，单位：脉冲
ex_vel 速度 r/min
ex_acc 加减速时间
bELU_Ena 是否判断正限位信号（默认判断）
bELD_Ena 是否判断负限位信号
** 返 回 值: 0：运动失败； 1：ok
**
*********************************************************************************/
int CMotor::P01_RelMoveTo(int dist,double ex_vel ,double ex_acc, BOOL bELU_Ena, BOOL bELD_Ena)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }//正向移动判断提示有无接触正向限位，便于分析问题；负向同理

    if(d_MtrState.InitOk == 0)// 未初始化
    {
        return 8;
    }

    if(ex_vel == 0 || ex_acc == 0)
    {
        return 10;
    }


    double vel,acc;
    vel = ex_vel;
    acc = ex_acc;

    dmc_set_profile(g_card,g_axis,vel/2.0,vel,acc/10.0,acc/10.0,vel/2.0);
    dmc_set_s_profile(g_card,g_axis,0,0.05);
    dmc_pmove(g_card,g_axis,dist,0);   //0:相对运动  1:绝对运动

    return 0;

}

/********************************************************************************
** 函数名称: P01_SetHomeSpeedEx
** 功能描述: 设置回原点速度
** 输　  入: sp  速度模式，可设置两级速度
vel 速度  r/min
acc 加减速时间
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_SetHomeSpeedEx(int sp, double vel, double acc)
{
//    QVERIFY(sp>=0 && sp<4);

//    d_MtrCfg.HomeSpeed[sp][0]=vel;
//    d_MtrCfg.HomeSpeed[sp][1]=acc;
    return 0;
}

/********************************************************************************
** 函数名称: P01_SetZeroPos
** 功能描述: 命令脉冲清零
** 输　  入: 无
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_SetZeroPos()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    dmc_set_position(g_card,g_axis,0);
    return 0;
}

/********************************************************************************
** 函数名称: P01_SetLimit
** 功能描述: 设置系统配置的软件上下限位
** 输　  入: limitp  上限设置值
limitn  下限设置值
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_SetLimit(long limitp, long limitn)
{
    if (limitp > limitn)
    {
        d_MtrData.LimitP = limitp;
        d_MtrData.LimitN = limitn;
    }
    else
    {
        d_MtrData.LimitP = limitn;
        d_MtrData.LimitN = limitp;
    }


    return 0;
}

/********************************************************************************
** 函数名称: P01_GetLimit
** 功能描述: 获取系统配置的软件上下限位
** 输　  入: limitp  上限
limitn  下限
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_GetLimit(long& limitp, long& limitn)
{
    limitp = d_MtrData.LimitP;
    limitn = d_MtrData.LimitN;
    return 0;
}

/********************************************************************************
** 函数名称: P01_GetHomeSpeed
** 功能描述: 获取回原点的速度和加减速时间
** 输　  入: sp  速度模式，可设置两级速度
vel 速度
acc 加减速时间
** 返 回 值: 1：ok
**
*********************************************************************************/
int CMotor::P01_GetHomeSpeed(int sp, double& vel, double& acc)
{
//    QVERIFY(sp>=0 && sp<4);

//    vel = d_MtrCfg.HomeSpeed[sp][0];
//    acc = d_MtrCfg.HomeSpeed[sp][1];
    return 0;
}

/********************************************************************************
** 函数名称: P01_Home
** 功能描述: 实现回原点动作（执行完回原点动作才结束）
** 输　  入: 无
** 返 回 值: 1：回原点结束； 2：回原点过程出现异常
**
*********************************************************************************/
int CMotor::P01_Home()
{
     return P01_Homing();
}

/********************************************************************************
** 函数名称: P01_StartHome
** 功能描述: 启动回原点动作
** 输　  入: 无
** 返 回 值: 1：已启动；
**
*********************************************************************************/
int CMotor::P01_StartHome()
{
    d_MtrState.HomeState = 0;
    d_MtrState.HomeAlready = 0;
    m_MotorHomeStep = 1;
    return 0 ;

}

long CaptPos = 0;
short int Status = 0;

/********************************************************************************
** 函数名称: P01_Homing
** 功能描述: 实现回原点动作（顺序执行一次）
** 输　  入: 无
** 返 回 值: 0：回原点结束； 1：正在回原点；  2：回原点过程出现异常
**
*********************************************************************************/
int CMotor::P01_Homing()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    double vel, acc;
    long lmp, lmn;

    if(d_MtrState.InitOk == 0)
    {
        return 8;//lcl 防止电机初始化失败造成复位误判撞机
    }

    if(2 == (d_MtrCfg.EncOn & 2))//判断电机类型
    {
        int CurAlarm  = 0;
        CurAlarm = P01_GetAlarmStatus();
        if (CurAlarm != 0)
        {
            return 13;
        }
    }
    switch(m_MotorHomeStep)
    {
    case 0:
        return 15;//未开启回原功能
    case 1://判断是否处罚正限位或者负限位，如果复位方式是是找到负限位后在正限复位的话，则此时不能有负限位信号
        {
            m_MotorHomeStep = 5;
        }
        break;

    case 2: //等待电机停止后才能复位
        if(!P01_IsMoving())
        {
            m_MotorHomeStep = 5;
        }
        break;

    case 5://判断复位模式
        if(Find_P_Goto_Org == d_MtrCfg.HomeMode)
        {
            d_MtrCfg.HomeLimitDir = 1 ;
            m_MotorHomeStep = 10;
        }
        else if(Find_N_Goto_Org == d_MtrCfg.HomeMode)
        {
            d_MtrCfg.HomeLimitDir = -1 ;
            m_MotorHomeStep = 10;
        }
        else if( P_GoTo_Org == d_MtrCfg.HomeMode)
        {
            d_MtrCfg.HomeLimitDir = -1 ;
            m_MotorHomeStep = 15;
        }
        else if( N_GoTo_Org == d_MtrCfg.HomeMode)
        {
            d_MtrCfg.HomeLimitDir = 1 ;
            m_MotorHomeStep = 15;
        }
        else if(Signal_To_Home == d_MtrCfg.HomeMode)
        {
            m_MotorHomeStep = 15;
        }
        break;

    case 10: //运动到限位上去
        {
            P01_GetHomeSpeed(1, vel, acc);
            dmc_set_profile(g_card,g_axis,vel/2.0,vel,acc,acc,vel/2.0);
            dmc_set_s_profile(g_card,g_axis,0,0.1);
            dmc_pmove(g_card,g_axis,d_MtrCfg.HomeLimitDir * d_MtrCfg.HomeRange,0);
            m_MotorHomeStep = 12;
            break;
        }

        break;
    case 12:
        {
            if(!P01_IsMoving())
            {
                if (1 == d_MtrCfg.HomeLimitDir)
                {
                    if (1 == P01_GetZhengLimitStatus())
                    {
                        m_MotorHomeStep = 20;
                    }
                    else
                    {
                        return 16;
                    }
                }
                else
                {
                    if (1 == P01_GetFuLimitStatus())
                    {
                        m_MotorHomeStep = 20;
                    }
                    else
                    {
                        return 16;
                    }
                }
            }

            break;
        }

        break;
    case 15:  //等待电机停止
        {
            if(!P01_IsMoving())
            {
                if (1 == P01_GetHomeStatus())
                {
                    P01_GetHomeSpeed(1, vel, acc);
                    if (-1 == d_MtrCfg.HomeLimitDir)
                    {
                        P01_JogMove(0,vel,acc,acc);
                    }
                    else
                    {
                        P01_JogMove(1,vel,acc,acc);
                    }
                }
                m_MotorHomeStep = 17;
                break;

            }
        }
        break;
    case 17:  //等待电机停止
        {
            if (0 == P01_GetHomeStatus())
            {
                P01_AbptStop();
                m_MotorHomeStep = 20;
                break;
            }

            if (!P01_IsMoving())
            {
                return 17;
            }

        }
        break;
    case 20:    //开始调用复位函数复位
        {
            if(!P01_IsMoving())
            {
                if (Signal_To_Home == d_MtrCfg.HomeMode)
                {
                    m_MotorHomeStep = 25;
                }
                else
                {
                    P01_SetZeroPos();
                    P01_GetHomeSpeed(1, vel, acc);
                    dmc_set_profile(g_card,g_axis,vel/2.0,vel,acc,acc,vel/2.0);
                    dmc_set_s_profile(g_card,g_axis,0,0.05);
                    if(-1 == d_MtrCfg.HomeLimitDir)  //设置复位模式
                    {

                        if ((g_card == 2) && ((g_axis == 4)||(g_axis == 3)) )
                        {
                            dmc_set_homemode(g_card,g_axis,1,0,3,1);   //2：负向回原点 0:低速回0 3:一次回零再记EZ信号回零？
                            dmc_home_move(g_card,g_axis);  //开始复位
                        }
                        else
                        {
                            dmc_set_homemode(g_card,g_axis,1,0,2,1);
                            //  （1:Home + EZ信号回零	 0: Home信号） 第二参数（1：一次机=记EZ信号）
                            dmc_home_move(g_card,g_axis);  //开始复位    //1：正向回原点 0:低速回0
                        }
                    }
                    else
                    {
                        if ((g_card == 2) && ((g_axis == 4)||(g_axis == 3)) )
                        {
                            dmc_set_homemode(g_card,g_axis,0,0,3,1);   //2：负向回原点 0:低速回0 3:一次回零再记EZ信号回零？
                            dmc_home_move(g_card,g_axis);  //开始复位
                        }
                        else
                        {
                            dmc_set_homemode(g_card,g_axis,0,0,2,1);   //2：负向回原点 0:低速回0 3:一次回零再记EZ信号回零？
                            dmc_home_move(g_card,g_axis);  //开始复位
                        }

                    }
                }

                m_MotorHomeStep = 25;
                break;
            }

            break;
        }
    case 25:
        {
            if (Signal_To_Home == d_MtrCfg.HomeMode)
            {
                if(1 == P01_GetHomeStatus())
                {
                    Sleep(2000);
                    m_MotorHomeStep = 30;
                    break;
                }
                else
                {
                    m_MotorHomeStep = 30;
                    break;
                }
            }
            else
            {
                if(!P01_IsMoving())  //等待复位完成
                {
                    m_MotorHomeStep = 30;
                    break;
                }
                else
                {
                    if(abs(P01_GetCmdPos()) > abs(d_MtrCfg.HomeLimitZero))
                    {
                        P01_AbptStop();//急停
                        return 18;
                    }
                }
            }

            break;
        }
        break;
    case 30: //偏移量调整
        {

            if (Signal_To_Home != d_MtrCfg.HomeMode)
            {

                if (1 != P01_GetHomeStatus())
                {
                    return 6;
                }
                P01_GetHomeSpeed(1, vel, acc);
                dmc_set_profile(g_card,g_axis,vel/2.0,vel,acc,acc,vel/2.0);
                dmc_set_s_profile(g_card,g_axis,0,0.05);
                dmc_pmove(g_card,g_axis,d_MtrCfg.HomeOffset,0);
                m_MotorHomeStep = 12;
            }
            m_MotorHomeStep = 35;

            break;
        }
        break;
    case 35://等待偏移量调整完成
        {
            if(!P01_IsMoving())
            {
                m_MotorHomeStep = 40;
                break;
            }
        }
        break;

    case 40:
        {
            Sleep(1500);
            m_MotorHomeStep = 45;
        }
        break;

    case 45://将编码器 ，输出脉冲清零，表示复位完成
        {
            d_MtrState.HomeState = 1;
            d_MtrState.HomeAlready = 1;
            dmc_set_position(g_card,g_axis,0);
            dmc_set_encoder(g_card,g_axis,0);

            m_MotorHomeStep = 50;
        }
        break;
        //防止一个电机复位完成后一直调用40；
    case 50:
        m_MotorHomeStep = 0;
        return 0;
    default:
        break;
    }
    return 1;
}

/********************************************************************************
** 函数名称: P01_RunLimit
** 功能描述: 实现找限位动作（执行完回限位动作结束，找到限位后反向微动一定量）
** 输　  入: 无
** 返 回 值: 1：找限位结束； 2：找限位过程出现异常
**
*********************************************************************************/
int CMotor::P01_RunLimit()
{
    return 6;
}

/********************************************************************************
** 函数名称: P01_StartRunLimit
** 功能描述: 启动找限位动作
** 输　  入: 无
** 返 回 值: 1：已启动
**
*********************************************************************************/
int CMotor::P01_StartRunLimit()
{
    return 6;
}

/********************************************************************************
** 函数名称: P01_RunningLimit
** 功能描述: 实现找限位动作（顺序执行一次，找到限位后反向微动一定量）
** 输　  入: 无
** 返 回 值: 0：找限位结束； 1：正在找限位；  2：找限位过程出现异常
**
*********************************************************************************/
int CMotor::P01_RunningLimit()
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    double vel, acc;

    if(d_MtrState.InitOk == 0)
    {
        return 8;//lcl 防止电机初始化失败造成复位误判撞机
    }

    if(2 == (d_MtrCfg.EncOn & 2))
    {
        if ( P01_GetAlarmStatus())
        {
            return 13;
        }
    }
    return 0;
}

/********************************************************************************
** 函数名称: P01_Save
** 功能描述: 保存电机配置文件
** 输　  入: 无
** 返 回 值: 0：失败； 1：成功；
**
*********************************************************************************/
int CMotor::P01_Save()
{
    if((_access(".\\data", 0 )) != 0)//文件夹不存在则创建
    {
        _mkdir(".\\data");
    }
    if((_access(s_data_path, 0 )) != 0)//文件夹不存在则创建
    {
        _mkdir(s_data_path);
    }

    FILE* pFile = NULL;
    char mtrdata[100];
    sprintf(mtrdata, "%s\\mtr%02d.dat", s_data_path, GetId());
    pFile = fopen(mtrdata, "wb");
    if(pFile)
    {
        fwrite(&d_MtrData, sizeof(Mtr_Data_Blk), 1, pFile);
        fwrite(&d_MtrCfg, sizeof(Mtr_Cfg_Blk), 1, pFile);
        fclose(pFile);
        return 20;
    }
    return 0;
}
/********************************************************************************
** 函数名称: P01_Load
** 功能描述: 加载电机配置文件
** 输　  入: 无
** 返 回 值: 0：重新写入； 1：成功；
**
*********************************************************************************/
int CMotor::P01_Load()
{
    FILE* pFile = NULL;
    char mtrdata[100];
    sprintf(mtrdata, "%s\\mtr%02d.dat", s_data_path, GetId());
    pFile = fopen(mtrdata, "rb");
    if(pFile == NULL)
    {
        P01_Save();
        return 21;
    }
    else
    {
        fread(&d_MtrData, sizeof(Mtr_Data_Blk), 1, pFile);//Mtr_Data_Blk
        fread(&d_MtrCfg, sizeof(Mtr_Cfg_Blk), 1, pFile);
        fclose(pFile);
    }

    return 0;
}

/********************************************************************************
** 函数名称: P01_ChangeSpeed
** 功能描述: 在线改变轴运动速度
** 输　  入:
** ex_vel 速度 r/min
** 返 回 值: 0：设置失败； 1：ok
**
*********************************************************************************/
int CMotor::P01_ChangeSpeed(int ex_vel)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;

    double vel = 0;
    vel = ex_vel;

    dmc_change_speed(g_card,g_axis,vel,0);
    return 1;
}
/********************************************************************************
** 函数名称: P01_SetDecStopTime(double stop_time)
** 功能描述: 设置减速停止时间
** 输　  入:
** stop_time 减速时间 s
** 返 回 值: 0：设置失败； 1：ok
**
*********************************************************************************/
int CMotor::P01_SetDecStopTime(double stop_time)
{
    if (!g_bIniMotorOK)
    {
        return 7;
    }
    if(d_MtrState.InitOk == 0)
        return 8;
    return 0;
}

int CMotor::P01_ClearPos()
{
    dmc_set_position(g_card,g_axis,0);
    dmc_set_encoder(g_card,g_axis,0);
    return 1;
}

int CMotor::P01_GetAlmMode(int &enable,int &alm_logic)
{
    return 6;
}
int CMotor::P01_GetHomePinLogic(int &org_logic)
{
    return 6;


}
int CMotor::P01_GetLimitPinLogic(int &el_enable,int &el_logic)
{
    return 6;

}

