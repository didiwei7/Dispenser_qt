#ifndef ADTCONTROL_H
#define ADTCONTROL_H

#include <Windows.h>
#include <QtWidgets>
#include <QDebug>
#include <Eigen/Dense>
#include <iostream>
#include <stdio.h>
#include <adt8949.h>

using namespace std;
using namespace Eigen;

// CCD点胶点(描点)
typedef struct _CCDGlue
{
	QString          name;                    // 点位名字
	QString          description;             // 点位描述
	float            X;					      // X
	float            Y;						  // Y
	float            Z;                       // Z
	float            center_X;				  // 圆心X
	float            center_Y;				  // 圆心Y
	float            speed;					  // 速度
	float            extra_offset_z;	      // 额外偏移Z
	bool             open;					  // 是否开胶
	int              openAdvance;			  // 提前开胶时间
	int              openDelay;               // 延迟开胶时间
	bool             close;                   // 是否关胶
	int              closeAdvance;            // 提前关胶时间
	int              closeDelay;              // 延后关胶时间
	QString			 type;					  // 类型

	_CCDGlue()
	{
		name = "";                    // 点位名字
		description = "";             // 点位描述
		X = 0.000;					  // X
		Y = 0.000;					  // Y
		Z = 0.000;                    // Z
		center_X = 0.000;			  // 圆心X
		center_Y = 0.000;			  // 圆心Y
		speed = 0.000;				  // 是否开启镭射
		extra_offset_z = 0.000;		  // 额外偏移Z		
		open = false;				  // 是否开胶
		openAdvance = 0;			  // 提前开胶时间
		openDelay = 0;                // 延迟开胶时间
		close = false;                // 是否关胶
		closeAdvance = 0;             // 提前关胶时间
		closeDelay = 0;               // 延后关胶时间
		type = "null";				  // 类型
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
		center_X = other.center_X;
		center_Y = other.center_Y;
		speed = other.speed;
		extra_offset_z = other.extra_offset_z;
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
	Z = 3,
	A = 4
};

enum ADMODE
{
	S = 0,
	T = 1,
	INDEX = 2,
	TRIAN = 3
};

// 初始化控制卡 第一次调用才初始化, 第二次调用时返回第一次初始化的结果
int init_card();

// 载入控制卡配置
void load_card();


// 设置回原模式
void set_home_mode();

// 设置回原速度
void set_home_speed();

// 轴回原
void home_axis(int axis);

// 等待回原完成
void wait_axis_homeOk(int axis);



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



// 获取单轴绝对位置
float get_current_pos_axis(int axis);


// 设置所有轴速度, 带起始速度, 带加减速, 带模式
void set_speed_mode(float startv, float speed, float acc, unsigned short mode);

// 设置单轴速度, 带轴号, 带起始速度, 带加减速, 带模式
void set_axis_speed_mode(int axis, float startv, float speed, float acc, unsigned short mode);

// 单轴绝对运动, 不带速度设置, 需提前设置速度, 模式
void move_axis_abs(int axis, float pos);

// 单轴相对运动, 不带速度设置, 需提前设置速度, 模式
void move_axis_offset(int axis, float distance);

// 单轴连续运动, 带方向(0+, 1-), 不带速度设置, 需提前设置速度, 模式
void move_axis_continue(int axis, int dir);


// 单轴绝对运动, 带速度, 带加减速, 带正负限位
void move_axis_abs(int axis, float pos, float speed, float acc, float dec);

// 单轴相对运动, 带速度, 带加减速, 带正负限位
void move_axis_offset(int axis, float distance, float speed, float acc, float dec);

// 单轴连续运动, 带方向(0+, 1-), 带速度, 带加减速, 带正负限位
void move_axis_continue(int axis, int dir, float speed, float acc, float dec);


// 设置插补运动, 带起始速度, 带加减速, 带模式
void set_inp_speed_mode(float startv, float speed, float acc, unsigned short mode);

// 设置匀速插补运动速度, 带速度
void set_inp_speed(float speed);

// X, Y, Z三轴直线插补, by x_pos, y_pos, z_pos
void move_inp_abs_line3(float x_pos, float y_pos, float z_pos);

// X, Y两轴直线插补
void move_inp_abs_line2(float x_pos, float y_pos);

// X, Y圆弧插补, Z轴直线插补 by pos_x, pos_y, pos_z, center_x, center_y
void move_inp_abs_helix2(float pos_x, float pos_y, float pos_z, float center_x, float center_y);

// X, Y圆弧插补 by pos_x, pos_y, center_x, center_y
void move_inp_abs_arc2(float pos_x, float pos_y, float center_x, float center_y);

// 单轴是否正在运动 返回 0停止, 1运动
bool axis_isMoving(int axis);

// 是否正在运动 返回 0停止, 1运动
bool card_isMoving();

// 等待单轴停止
void wait_axis_stop(int axis);

// 等待所有轴停止
void wait_allaxis_stop();

// 等待插补完成
void wait_inp_finish();

// 设置步进轴速度
void set_stepAxis_speed(float speed);

// 等待步进轴停止
void wait_stepAxis_stop();


void asyncSleep(unsigned int msec);



#endif // ADTCONTROL_H