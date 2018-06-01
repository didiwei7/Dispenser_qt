#ifndef ADTCONTROL_H
#define ADTCONTROL_H

#include <Windows.h>
#include <QtWidgets>
#include <QDebug>
#include <iostream>
#include <stdio.h>
using namespace std;


#include "adt8949.h"

// CCD点胶点(描点)
typedef struct _CCDGlue
{
	QString          name;                    // 点位名字
	QString          description;             // 点位描述
	float            X;					      // X
	float            Y;						  // Y
	float            Z;                       // Z
	bool             open;					  // 是否开胶
	int              openAdvance;			  // 提前开胶时间
	int              openDelay;               // 延迟开胶时间
	bool             close;                   // 是否关胶
	int              closeAdvance;            // 提前关胶时间
	int              closeDelay;              // 延后关胶时间
	int			     type;					  // 类型

	_CCDGlue()
	{
		name = "";                    // 点位名字
		description = "";             // 点位描述
		X = 0.000;					  // X
		Y = 0.000;					  // Y
		Z = 0.000;                    // Z
		open = false;				  // 是否开胶
		openAdvance = 0;			  // 提前开胶时间
		openDelay = 0;                 // 延迟开胶时间
		close = false;                // 是否关胶
		closeAdvance = 0;             // 提前关胶时间
		closeDelay = 0;                // 延后关胶时
		type = 0;					  // 类型
	}

	_CCDGlue & operator = (const _CCDGlue &other)
	{
		if (this == &other)
		{
			return *this;
		}
		name = other.name;
		description = other.description;
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		open = other.open;
		openAdvance = other.openAdvance;
		openDelay = other.openDelay;
		close = other.close;
		closeAdvance = other.closeAdvance;
		closeDelay = other.closeDelay;
		type = other.type;
		return *this;
	}
}CCDGlue;

// 实际点胶点
typedef struct _PointGlue
{
	QString          name;                    // 点位名字
	QString          description;             // 点位描述
	float            X;					      // X
	float            Y;						  // Y
	float            Z;                       // Z
	bool             open;					  // 是否开胶
	int              openAdvance;			  // 提前开胶时间
	int              openDelay;               // 延迟开胶时间
	bool             close;                   // 是否关胶
	int              closeAdvance;            // 提前关胶时间
	int              closeDelay;              // 延后关胶时间
	int			     type;					  // 类型

	_PointGlue()
	{
		name = "";                    // 点位名字
		description = "";             // 点位描述
		X = 0.000;					  // X
		Y = 0.000;					  // Y
		Z = 0.000;                    // Z
		open = false;				  // 是否开胶
		openAdvance = 0;			  // 提前开胶时间
		openDelay = 0;                 // 延迟开胶时间
		close = false;                // 是否关胶
		closeAdvance = 0;             // 提前关胶时间
		closeDelay = 0;                // 延后关胶时
		type = 0;					  // 类型
	}

	_PointGlue & operator = (const _PointGlue &other)
	{
		if (this == &other)
		{
			return *this;
		}
		name = other.name;
		description = other.description;
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		open = other.open;
		openAdvance = other.openAdvance;
		openDelay = other.openDelay;
		close = other.close;
		closeAdvance = other.closeAdvance;
		closeDelay = other.closeDelay;
		type = other.type;
		return *this;
	}
}PointGlue;

// 普通点位
typedef struct _PointGeneral
{
	QString          name;                    // 点位名字
	QString          description;             // 点位描述
	float            X;					      // X
	float            Y;						  // Y
	float            Z;                       // Z

	_PointGeneral()
	{
		name = "";                    // 点位名字
		description = "";             // 点位描述
		X = 0.000;					  // X
		Y = 0.000;					  // Y
		Z = 0.000;                    // Z
	}

	_PointGeneral & operator = (const _PointGeneral &other)
	{
		if (this == &other)
		{
			return *this;
		}
		name = other.name;
		description = other.description;
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		return *this;
	}
}PointGeneral;

// 所有点位 包含 name desc x y z
typedef struct _PointRun
{
	QString          name;                    // 点位名字
	QString          description;             // 点位描述
	float            X;					      // X
	float            Y;						  // Y
	float            Z;                       // Z

	_PointRun()
	{
		name = "";                    // 点位名字
		description = "";             // 点位描述
		X = 0.000;					  // X
		Y = 0.000;					  // Y
		Z = 0.000;                    // Z
	}

	_PointRun & operator = (const _PointRun &other)
	{
		if (this == &other)
		{
			return *this;
		}
		name = other.name;
		description = other.description;
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		return *this;
	}
}PointRun;

enum AXISNUM
{
	X = 1,
	Y = 2,
	Z = 3
};

// 初始化控制卡 第一次调用才初始化, 第二次调用时返回第一次初始化的结果
int init_card();

// 减速停止
void stop_axis_dec(int axis);


// 停止轴, by axis
void stop_axis(int axis);

// 停止所有轴
void stop_allaxis();

// 急停
void estop();




// 获取输入点状态, by bit
int read_in_bit(int bit);

// 获取输出点状态, by bit
int read_out_bit(int bit);

// 写入输出点状态, by bit
void write_out_bit(int bit, int status);

// 改变输出点状态, by bit
void change_out_bit(int bit);




// 设置回原模式
void set_home_mode();

// 设置回原速度
void set_home_speed();

// 轴回原
void home_axis(int axis);

// 等待回原完成
void wait_axis_homeOk(int axis);



// 速度, 带轴号, 带加减速
void set_axis_speed(int axis, float speed, float acc, float dec);

// 移动, 带速度, 带加减速, 带正负限位
void move_axis_abs(int axis, float pos, float speed, float acc, float dec);

// 移动, 带速度, 带加减速, 带正负限位
void move_axis_offset(int axis, float distance, float speed, float acc, float dec);

// 连续运动, 带方向(0+, 1-), 带速度, 带加减速, 带正负限位
void move_axis_continue(int axis, int dir, float speed, float acc, float dec);


float get_current_pos_axis(int axis);


// 运动, 
bool axis_isMoving(int axis);

// 等待运动结束
void wait_axis_stop(int axis);



#endif // ADTCONTROL_H