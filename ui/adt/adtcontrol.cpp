#include "adtcontrol.h"

// 初始化控制卡 第一次调用才初始化, 第二次调用时返回第一次初始化的结果
int index = 1;
int ret = 0;
int init_card()
{
	if (index == 1)
	{
		index = 0;

		ret = adt8949_initial();
		
		if (ret < 0)
		{
			QMessageBox::about(NULL, NULL, QStringLiteral("控制卡初始化失败"));
			return ret;
		}
		else if (ret == 0)
		{
			QMessageBox::about(NULL, NULL, QStringLiteral("控制卡未安装"));
			return ret;
		}
		else
		{
			qDebug() << ret;
			QMessageBox::about(NULL, NULL, QStringLiteral("控制卡初始化成功"));
			
			// 载入卡配置
			load_card();

			return ret;
		}
	}
	else
	{
		return ret;
	}
}


void load_card()
{
	// 【1】 设置急停模式
	adt8949_set_emergency_stop_mode(0, 16, 1);

	// 【2】 设置回原模式, 速度
	set_home_mode();
	set_home_speed();

	// 【3】 设置 X, Y, Z, A 轴
	adt8949_set_pulse_mode(0, 1, 1, 0, 0);
	adt8949_set_pulse_mode(0, 2, 1, 0, 0);
	adt8949_set_pulse_mode(0, 3, 1, 0, 0);
	adt8949_set_pulse_mode(0, 4, 1, 0, 0);

	adt8949_set_gear(0, 1, 1000);
	adt8949_set_gear(0, 2, 1000);
	adt8949_set_gear(0, 3, 1000);
	adt8949_set_gear(0, 4, 3200);

	// 【4】 设置限位锁模式
	// adt8949_set_limit_lock(0, 1);
}


// 停止轴, 减速, by axis
void stop_axis_dec(int axis)
{
	adt8949_dec_stop(0, axis);
}

// 停止轴, by axis
void stop_axis(int axis)
{
	adt8949_sudden_stop(0, axis);
}

// 停止所有轴
void stop_allaxis()
{
	adt8949_sudden_stop(0, 3);
	adt8949_sudden_stop(0, 1);
	adt8949_sudden_stop(0, 2);
}

// 急停
void estop()
{
	adt8949_close_card();
}


// 设置回原模式
void set_home_mode()
{
	QFile file("../config/motor.json");
	if (!file.exists())
	{
		return;
	}

	file.open((QIODevice::ReadOnly));
	QByteArray date = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(date);
	QJsonObject obj = doc.object();

	// 回原模式设置
	int m_nHomeDir[3], m_nStop0Active[3], m_nLimitActive[3], m_nStop1Active[3];
	float m_fBackRange[3], m_fEncoderZRange[3], m_fOffset[3];

	QJsonObject x_obj = obj.value("HomeMode").toObject().value("X_axis").toObject();
	m_nHomeDir[0] = x_obj.value("m_nHomeDir").toInt();
	m_nStop0Active[0] = x_obj.value("m_nStop0Active").toInt();
	m_nLimitActive[0] = x_obj.value("m_nLimitActive").toInt();
	m_nStop1Active[0] = x_obj.value("m_nStop1Active").toInt();
	m_fBackRange[0] = x_obj.value("m_fBackRange").toDouble();
	m_fEncoderZRange[0] = x_obj.value("m_fEncoderZRange").toDouble();
	m_fOffset[0] = x_obj.value("m_fOffset").toDouble();
	adt8949_SetHomeMode_Ex(0, AXISNUM::X,
		m_nHomeDir[0], m_nStop0Active[0], m_nLimitActive[0], m_nStop1Active[0],
		m_fBackRange[0], m_fEncoderZRange[0], m_fOffset[0]);

	QJsonObject y_obj = obj.value("HomeMode").toObject().value("Y_axis").toObject();
	m_nHomeDir[1] = y_obj.value("m_nHomeDir").toInt();
	m_nStop0Active[1] = y_obj.value("m_nStop0Active").toInt();
	m_nLimitActive[1] = y_obj.value("m_nLimitActive").toInt();
	m_nStop1Active[1] = y_obj.value("m_nStop1Active").toInt();
	m_fBackRange[1] = y_obj.value("m_fBackRange").toDouble();
	m_fEncoderZRange[1] = y_obj.value("m_fEncoderZRange").toDouble();
	m_fOffset[1] = y_obj.value("m_fOffset").toDouble();
	adt8949_SetHomeMode_Ex(0, AXISNUM::Y,
		m_nHomeDir[1], m_nStop0Active[1], m_nLimitActive[1], m_nStop1Active[1],
		m_fBackRange[1], m_fEncoderZRange[1], m_fOffset[1]);

	QJsonObject z_obj = obj.value("HomeMode").toObject().value("Z_axis").toObject();
	m_nHomeDir[2] = z_obj.value("m_nHomeDir").toInt();
	m_nStop0Active[2] = z_obj.value("m_nStop0Active").toInt();
	m_nLimitActive[2] = z_obj.value("m_nLimitActive").toInt();
	m_nStop1Active[2] = z_obj.value("m_nStop1Active").toInt();
	m_fBackRange[2] = z_obj.value("m_fBackRange").toDouble();
	m_fEncoderZRange[2] = z_obj.value("m_fEncoderZRange").toDouble();
	m_fOffset[2] = z_obj.value("m_fOffset").toDouble();
	adt8949_SetHomeMode_Ex(0, AXISNUM::Z,
		m_nHomeDir[2], m_nStop0Active[2], m_nLimitActive[2], m_nStop1Active[2],
		m_fBackRange[2], m_fEncoderZRange[2], m_fOffset[2]);

	file.close();

	QFile file2("../config/motor2.json");
	file2.open((QIODevice::WriteOnly));
	QJsonDocument doc_save(obj);
	file2.write(doc_save.toJson());
}

// 设置回原速度
void set_home_speed()
{
	QFile file("../config/motor.json");
	if (!file.exists())
	{
		return;
	}

	file.open((QIODevice::ReadOnly));
	QByteArray date = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(date);
	QJsonObject obj = doc.object();

	// 回原速度设置
	float m_fStartSpeed[3], m_fSearchSpeed[3], m_fHomeSpeed[3], m_fAcc[3], m_fZPhaseSpeed[3];
	QJsonObject xs_obj = obj.value("HomeSpeed").toObject().value("X_axis").toObject();
	m_fStartSpeed[0] = xs_obj.value("m_fStartSpeed").toDouble();
	m_fSearchSpeed[0] = xs_obj.value("m_fSearchSpeed").toDouble();
	m_fHomeSpeed[0] = xs_obj.value("m_fHomeSpeed").toDouble();
	m_fAcc[0] = xs_obj.value("m_fAcc").toDouble();
	m_fZPhaseSpeed[0] = xs_obj.value("m_fZPhaseSpeed").toDouble();
	adt8949_SetHomeSpeed_Ex(0, AXISNUM::X, m_fStartSpeed[0], m_fSearchSpeed[0], m_fHomeSpeed[0], m_fAcc[0], m_fZPhaseSpeed[0]);

	QJsonObject ys_obj = obj.value("HomeSpeed").toObject().value("Y_axis").toObject();
	m_fStartSpeed[1] = ys_obj.value("m_fStartSpeed").toDouble();
	m_fSearchSpeed[1] = ys_obj.value("m_fSearchSpeed").toDouble();
	m_fHomeSpeed[1] = ys_obj.value("m_fHomeSpeed").toDouble();
	m_fAcc[1] = ys_obj.value("m_fAcc").toDouble();
	m_fZPhaseSpeed[1] = ys_obj.value("m_fZPhaseSpeed").toDouble();
	adt8949_SetHomeSpeed_Ex(0, AXISNUM::Y, m_fStartSpeed[1], m_fSearchSpeed[1], m_fHomeSpeed[1], m_fAcc[1], m_fZPhaseSpeed[1]);

	QJsonObject zs_obj = obj.value("HomeSpeed").toObject().value("Z_axis").toObject();
	m_fStartSpeed[2] = zs_obj.value("m_fStartSpeed").toDouble();
	m_fSearchSpeed[2] = zs_obj.value("m_fSearchSpeed").toDouble();
	m_fHomeSpeed[2] = zs_obj.value("m_fHomeSpeed").toDouble();
	m_fAcc[2] = zs_obj.value("m_fAcc").toDouble();
	m_fZPhaseSpeed[2] = zs_obj.value("m_fZPhaseSpeed").toDouble();
	adt8949_SetHomeSpeed_Ex(0, AXISNUM::Z, m_fStartSpeed[2], m_fSearchSpeed[2], m_fHomeSpeed[2], m_fAcc[2], m_fZPhaseSpeed[2]);

	file.close();
}

// 轴回原
void home_axis(int axis)
{
	adt8949_HomeProcess_Ex(0, axis);
}

// 等待回原完成
void wait_axis_homeOk(int axis)
{
	int state = 0;
	while (true)
	{
		Sleep(1);
		state = adt8949_GetHomeStatus_Ex(0, axis);

		if (state == 0)
		{
			break;
		}

		// 这才是标准的做法
		//if (m_stopFlag == 1)
		//{
		//	break;
		//}
	}
}




// 获取输入点状态, by bit
int read_in_bit(int bit)
{
	return adt8949_read_bit(0, bit);
}

// 获取输出点状态, by bit
int read_out_bit(int bit)
{
	return adt8949_get_out(0, bit);
}

// 设置输出点状态, by bit
void write_out_bit(int bit, int status)
{
	adt8949_write_bit(0, bit, status);
}

// 改变输出点状态, by bit
void change_out_bit(int bit)
{
	if (adt8949_get_out(0, bit) == 0)
	{
		adt8949_write_bit(0, bit, 1);
	}
	else
	{
		adt8949_write_bit(0, bit, 0);
	}
}






// 获取单轴绝对位置
float get_current_pos_axis(int axis)
{
	long lPos;
	float fPos;
	adt8949_get_command_pos(0, axis, &lPos);
	fPos = lPos / 1000.0;
	return fPos;
}


// 设置所有轴速度, 带起始速度, 带加减速, 带模式
void set_speed_mode(float startv, float speed, float acc, unsigned short mode)
{
	for ( int axis = 1; axis < 4; axis++)
	{
		adt8949_set_admode(0, axis, mode);
		adt8949_set_startv(0, axis, startv);
		// adt8949_set_endv(0, axis, float(0.1));
		adt8949_set_speed(0, axis, speed);
		adt8949_set_acc(0, axis, acc);		// 加速度 
		// adt8949_set_dec(0, axis, dec);	// 减速度 
		adt8949_set_jcc(0, axis, 3);		// 加加速度, 值越小, 加减速效果越明显
	}
}

// 设置单轴速度, 带轴号, 带起始速度, 带加减速, 带模式
void set_axis_speed_mode(int axis, float startv, float speed, float acc, unsigned short mode)
{
	adt8949_set_admode(0, axis, mode);
	adt8949_set_startv(0, axis, startv);
	// adt8949_set_endv(0, axis, float(0.1));
	adt8949_set_speed(0, axis, speed);
	adt8949_set_acc(0, axis, acc);	// 加速度 
	// adt8949_set_dec(0, axis, dec);	// 减速度 
	adt8949_set_jcc(0, axis, 3);	// 加加速度, 值越小, 加减速效果越明显
}

// 单轴绝对运动, 不带速度设置, 需提前设置速度, 模式
void move_axis_abs(int axis, float pos)
{
	adt8949_abs_pmove(0, axis, pos);
}

// 单轴相对运动, 不带速度设置, 需提前设置速度, 模式
void move_axis_offset(int axis, float distance)
{
	adt8949_abs_pmove(0, axis, distance);
}

// 单轴连续运动, 不带速度设置, 需提前设置速度, 模式
void move_axis_continue(int axis, int dir)
{
	adt8949_continue_move(0, axis, dir);
}



// 单轴绝对运动, 带速度, 带加减速, 带正负限位
void move_axis_abs(int axis, float pos, float speed, float acc, float dec)
{
	adt8949_set_admode(0, axis, 0);
	adt8949_set_startv(0, axis, float(0.1));
	adt8949_set_endv(0, axis, float(0.1));
	adt8949_set_speed (0, axis, speed);
	adt8949_set_acc(0, axis, acc);	// 加速度 
	adt8949_set_dec(0, axis, dec);	// 减速度 
	adt8949_set_jcc(0, axis, 3);	// 加加速度, 值越小, 加减速效果越明显
	adt8949_abs_pmove(0, axis, pos);
}

// 单轴相对运动, 带速度, 带加减速, 带正负限位
void move_axis_offset(int axis, float distance, float speed, float acc, float dec)
{
	adt8949_set_admode(0, axis, 0);
	adt8949_set_startv(0, axis, float(0.1));
	adt8949_set_endv(0, axis, float(0.1));
	adt8949_set_speed(0, axis, speed);
	adt8949_set_acc(0, axis, acc);	// 加速度 
	adt8949_set_dec(0, axis, dec);	// 减速度 
	adt8949_set_jcc(0, axis, 3);	// 加加速度, 值越小, 加减速效果越明显
	adt8949_abs_pmove(0, axis, distance);
}

// 单轴连续运动, 带方向(0+, 1-), 带速度, 带加减速, 带正负限位
void move_axis_continue(int axis, int dir, float speed, float acc, float dec)
{
	adt8949_set_admode(0, axis, 0);
	adt8949_set_startv(0, axis, float(0.1));
	adt8949_set_endv(0, axis, float(0.1));
	adt8949_set_speed(0, axis, speed);
	adt8949_set_acc(0, axis, acc);	// 加速度 
	adt8949_set_dec(0, axis, dec);	// 减速度 
	adt8949_set_jcc(0, axis, 3);	// 加加速度, 值越小, 加减速效果越明显
	adt8949_continue_move(0, axis, dir);
}


// 设置插补运动速度, 带起始速度, 带加减速, 带模式
void set_inp_speed_mode(float startv, float speed, float acc, unsigned short mode)
{
	adt8949_set_admode(0, 63, mode);
	adt8949_set_startv(0, 63, startv);
	adt8949_set_speed(0, 63, speed);
	adt8949_set_acc(0, 63, acc);	// 加速度
}

// 设置匀速插补运动速度, 带速度
void set_inp_speed(float speed)
{
	adt8949_set_admode(0, 63, 1);
	adt8949_set_startv(0, 63, speed);
	adt8949_set_speed(0, 63, speed);
}

// X, Y, Z三轴直线插补, by x_pos, y_pos, z_pos
void move_inp_abs_line3(float x_pos, float y_pos, float z_pos)
{
	unsigned char axismap = 7;
	adt8949_set_precount(0, 30);
	adt8949_inp_abs_move4(0, 0, axismap, x_pos, y_pos, z_pos, 0);
}

// X, Y两轴直线插补
void move_inp_abs_line2(float x_pos, float y_pos)
{
	unsigned char axismap = 3;
	adt8949_set_precount(0, 30);
	adt8949_inp_abs_move4(0, 0, axismap, x_pos, y_pos, 0, 0);
}

// X, Y圆弧插补, Z轴直线插补 by pos_x, pos_y, pos_z, center_x, center_y
void move_inp_abs_helix2(float pos_x, float pos_y, float pos_z, float center_x, float center_y)
{
	int AxisList[4] = { 1, 2, 3, 0 };
	float pos[4] = { pos_x, pos_y, pos_z, 0 };
	float center[2] = { center_x, center_y};

	adt8949_set_precount(0, 30);
	adt8949_inp_abs_helix2(0, 0, AxisList, pos, center, 0);
}

// X, Y圆弧插补 by pos_x, pos_y, center_x, center_y
void move_inp_abs_arc2(float pos_x, float pos_y, float center_x, float center_y)
{
	unsigned char axismap = 3;
	float pos[4] = { pos_x, pos_y, 0, 0 };
	float center[2] = { center_x, center_y };

	adt8949_set_precount(0, 30);
	adt8949_inp_abs_arc2(0, 0, axismap, pos, center, 1);
}


// 单轴是否正在运动 返回 0停止, 1运动
bool axis_isMoving(int axis)
{
	int state = 0;
	adt8949_get_status(0, axis, &state);
	return state;
}

// 是否正在运动 返回 0停止, 1运动
bool card_isMoving()
{
	int state = 0;
	if (axis_isMoving(1) || axis_isMoving(1) || axis_isMoving(1) || axis_isMoving(1))
	{
		state = 1;
	}
	return state;
}

// 等待单轴停止
void wait_axis_stop(int axis)
{
	int state = 0;
	while (true)
	{
		Sleep(1);
		adt8949_get_status(0, axis, &state);
		
		if (state==0)
		{
			break;
		}

		// 这才是标准的做法
		//if (m_stopFlag == 1)
		//{
		//	break;
		//}
	} 	
}

// 等待所有轴停止
void wait_allaxis_stop()
{
	int stateX = 0;
	int stateY = 0;
	int stateZ = 0;
	while (true)
	{
		Sleep(1);
		adt8949_get_status(0, 1, &stateX);
		adt8949_get_status(0, 2, &stateY);
		adt8949_get_status(0, 3, &stateZ);
		if (stateX == 0 && stateY == 0 && stateZ == 0)
		{
			break;
		}

		// 这才是标准的做法
		//if (m_stopFlag == 1)
		//{
		//	break;
		//}
	}
}

// 等待插补完成
void wait_inp_finish()
{
	int state = 0;
	while (true)
	{
		Sleep(1);
		adt8949_get_inp_status(0, &state);

		if (state == 0)
		{
			break;
		}

		// 这才是标准的做法
		//if (m_stopFlag == 1)
		//{
		//	break;
		//}
	}
}

// 等待步进轴停止
void wait_stepmotor_stop()
{
	int state = 0;
	while (true)
	{
		Sleep(1);
		adt8949_read_bit(0, 23);

		if (1 == state)
		{
			stop_axis(AXISNUM::A);
			break;
		}
	}
}