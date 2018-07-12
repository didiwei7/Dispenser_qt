#include "workflow.h"

Workflow::Workflow(QObject *parent) : QObject(parent)
{
	setIOStatus();
	setConfig();
	setPoint();
	// setCommunication();

	setThread();
}

Workflow::~Workflow()
{
	// 监视IO
	close_thread_watch_estop = true;
	close_thread_watch_reset = true;
	close_thread_watch_start = true;
	close_thread_watch_clearNeedle = true;

	// 点胶流程
	close_thread_glue_teda = true;

	// 信号槽控制流程
	close_thread_ccd_calib = true;

	thread_pool.waitForDone();
	thread_pool.clear();
	thread_pool.destroyed();
}

void Workflow::setConfig()
{
	// 【1】 流程配置
	QFile file("../config/workflow_glue.ini");
	if (!file.exists())
	{
		is_config_gluel = false;
		is_config_glue2 = false;
		is_config_glue3 = false;
	}

	QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
	is_config_gluel = setting.value("workflow_glue/is_config_gluel").toBool();
	is_config_glue2 = setting.value("workflow_glue/is_config_glue2").toBool();
	is_config_glue3 = setting.value("workflow_glue/is_config_glue3").toBool();

	// 【2】 距离偏移配置
	distance_ccd_needle_x = setting.value("ccd_needle_diatance/offset_x").toInt() / 1000.0;
	distance_ccd_neddle_y = setting.value("ccd_needle_diatance/offset_y").toInt() / 1000.0;

	distance_ccd_laser_x = setting.value("ccd_laser_diatance/offset_x").toInt() / 1000.0;
	diatance_ccd_laser_y = setting.value("ccd_laser_diatance/offset_x").toInt() / 1000.0;

	distance_laser_needle_x = setting.value("laser_needle_diatance/offset_x").toInt() / 1000.0;
	distance_laser_needle_y = setting.value("laser_needle_diatance/offset_y").toInt() / 1000.0;
	distance_laser_needle_z = setting.value("laser_needle_diatance/offset_z").toInt() / 1000.0;

	calib_offset_x = setting.value("calib_needle_optical/calib_offset_x").toInt() / 1000.0;
	calib_offset_y = setting.value("calib_needle_optical/calib_offset_y").toInt() / 1000.0;
	calib_offset_z = setting.value("calib_needle_attach/calib_offset_z").toInt() / 1000.0;
	file.close();


	// 【2】 点位配置
	model_general = new QSqlTableModel(this);
	// 使用 submit 时,数据库才会更改,否则做出的更改存储在缓存中
	model_general->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_general->setTable("point_main");
	// 设置按第0列升序排列
	model_general->setSort(0, Qt::AscendingOrder);
	model_general->select();

	model_glue1 = new QSqlTableModel(this);
	model_glue1->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue1->setTable("point_glue1");
	model_glue1->setSort(0, Qt::AscendingOrder);
	model_glue1->select();

	model_glue2 = new QSqlTableModel(this);
	model_glue2->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue2->setTable("point_glue2");
	model_glue2->setSort(0, Qt::AscendingOrder);
	model_glue2->select();

	model_glue3 = new QSqlTableModel(this);
	model_glue3->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue3->setTable("point_glue3");
	model_glue3->setSort(0, Qt::AscendingOrder);
	model_glue3->select();
}

void Workflow::setPoint()
{
	allpoint_pointRun = getAllRunPointInfo();

	vec_ccdGlue_1 = getCCDGluePoint2Vector(1);
	vec_ccdGlue_2 = getCCDGluePoint2Vector(2);
	vec_ccdGlue_3 = getCCDGluePoint2Vector(3);
}

void Workflow::setCommunication()
{
	// TCP CCD
	receivedMsg_ccd = "";
	socket_ccd = new QTcpSocket(this);
	socket_ccd->connectToHost("127.0.0.1", 7777);
	if (!socket_ccd->waitForConnected(10000))
	{
		QMessageBox::about(NULL, "Warning", QStringLiteral("连接服务器失败"));
	}
	else
	{
		connect(socket_ccd, &QTcpSocket::readyRead, this, &Workflow::socket_ccd_receive);
	}

	// Serial Laser
	receivedMsg_laser = "";
	serial_laser = new QSerialPort(this);
	serial_laser->setPortName("COM1");
	if (!serial_laser->open(QIODevice::ReadWrite))
	{
		QMessageBox::about(NULL, "Warning", QStringLiteral("连接COM1失败"));
	}
	else
	{
		serial_laser->setBaudRate(9600);
		serial_laser->setDataBits(QSerialPort::Data8);
		serial_laser->setParity(QSerialPort::NoParity);
		serial_laser->setStopBits(QSerialPort::OneStop);
		serial_laser->setFlowControl(QSerialPort::NoFlowControl);

		QtConcurrent::run(&thread_pool, [&]() { thread_serialLaserReceive(); });
	}

}

void Workflow::setIOStatus()
{
	if (!(init_card() == 1)) return;

	// 【1】 初始化输出状态

	// 【2】 检测输入状态
}

void Workflow::setThread()
{
	// qDebug() << QThreadPool::globalInstance()->maxThreadCount();
	thread_pool.setMaxThreadCount(20);

	// 【1】 监视 input
	is_estop_ok = false;
	start_thread_watch_estop = true;
	close_thread_watch_estop = false;

	is_reset_ok = false;
	start_thread_watch_reset = true;
	close_thread_watch_reset = false;

	is_start_ok = false;
	start_thread_watch_start = true;
	close_thread_watch_start = false;

	is_clearNeedle_ok = false;
	start_thread_watch_clearNeedle = true;
	close_thread_watch_clearNeedle = false;

	// 【2】 点胶流程  
	is_glue_teda_ok = false;
	start_thread_glue_teda = false;
	close_thread_glue_teda = false;

	// 【3】 信号槽启动
	is_ccd_calib_ing = false;
	start_thread_ccd_calib = false;
	close_thread_ccd_calib = false;

    future_thread_watch_estop = QtConcurrent::run(&thread_pool, [&]() { thread_watch_estop(); });
	future_thread_watch_reset = QtConcurrent::run(&thread_pool, [&]() { thread_watch_reset(); });
	future_thread_watch_start = QtConcurrent::run(&thread_pool, [&]() { thread_watch_start(); });
	future_thread_watch_clearNeedle = QtConcurrent::run(&thread_pool, [&]() { thread_watch_clearNeedle(); });
	
	future_thread_glue_teda_test = QtConcurrent::run(&thread_pool, [&]() { thread_glue_teda_test(); });
}

// Thread 急停
void Workflow::thread_watch_estop()
{
	if (!(init_card() == 1)) return;

	int step_estop = 0;

	while (close_thread_watch_estop == false)
	{
		switch (step_estop)
		{
		case 0:		// 检测急停
		{
			if ( (true == start_thread_watch_estop) && (1 == read_in_bit(16)))
			{
				step_estop = 10;
			}
			else
			{
				Sleep(5);
				step_estop = 0;
			}
		}
		break;

		case 10:	// 急停操作
		{
			stop_allaxis();
			Sleep(200);
			estop();
			Sleep(200);

			step_estop = 9999;
		}
		break;

		case 8888:	// 线程: 流程执行完毕, 等待下次开始
		{
			emit changedRundataLabel(QStringLiteral("急停已按下"));
			emit changedRundataText(QStringLiteral("Thread_watch_estop 急停流程正常结束, 请松开急停, 重新启动软件"));
			writRunningLog(QStringLiteral("Thread_watch_estop 急停流程正常结束, 请松开急停, 重新启动软件"));

			step_estop = 0;
		}
		break;

		case 9999:	// 线程: 线程退出
		{
			emit changedRundataLabel(QStringLiteral("急停已按下"));
			emit changedRundataText(QStringLiteral("Thread_watch_estop 急停流程正常结束, 线程将关闭. 请松开急停, 重新启动软件"));
			writRunningLog(QStringLiteral("Thread_watch_estop 急停流程正常结束, 线程将关闭. 请松开急停, 重新启动软件"));

			start_thread_watch_estop = false;
			close_thread_watch_estop = true;
		}
		break;

		default:
			break;
		}
	}
}

// Thread 复位
void Workflow::thread_watch_reset()
{
	if (!(init_card() == 1)) return;

	int step_reset = 0;

	while (close_thread_watch_reset == false)
	{
		switch (step_reset)
		{
		case 0:		// 检测复位信号
		{
			if ((true == start_thread_watch_reset) && (0 == read_in_bit(20)))
			{
				Sleep(500);
				step_reset = 10;
			}
			else
			{
				Sleep(5);
				step_reset = 0;
			}
		}
		break;

		case 10:	// 消息更新
		{
			emit changedRundataLabel(QStringLiteral("复位开始"));
			emit changedRundataText(QStringLiteral("Thread_watch_reset 复位开始"));
			writRunningLog(QStringLiteral("Thread_watch_reset 复位开始"));
			step_reset = 20;
		}
		break;

		case 20:	// 工站复位
		{
			home_axis(AXISNUM::Z);		
			wait_axis_homeOk(AXISNUM::Z);

			home_axis(AXISNUM::X);
			wait_axis_homeOk(AXISNUM::X);

			home_axis(AXISNUM::Y);
			wait_axis_homeOk(AXISNUM::Y);

			step_reset = 8888;
		}
		break;

		case 8888:	// 流程正常结束, 等待下次开始
		{
			emit changedRundataLabel(QStringLiteral("复位完成, 已就绪"));
			emit changedRundataText(QStringLiteral("Thread_watch_reset 复位流程正常结束"));
			writRunningLog(QStringLiteral("Thread_watch_reset 复位流程正常结束"));

			is_reset_ok = true;
			step_reset = 0;
		}
		break;

		case 9999:	// 流程异常结束, 线程关闭
		{
			emit changedRundataLabel(QStringLiteral("复位失败, 请检查"));
			emit changedRundataText(QStringLiteral("Thread_watch_reset 复位流程正常结束, 线程关闭"));
			writRunningLog(QStringLiteral("Thread_watch_reset 复位流程异常结束, 线程关闭"));
			
			start_thread_watch_reset = false;
			close_thread_watch_reset = true;
		}
		break;

		default:
			break;
		}

	}
}

// Thread 监视擦胶
void Workflow::thread_watch_clearNeedle()
{
	if (!(init_card() == 1)) return;

	int step_clearNeedle = 0;

	while (close_thread_watch_clearNeedle == false)
	{
		switch (step_clearNeedle)
		{
		case 0:		// 等待触发
		{
			if ( (true == start_thread_watch_clearNeedle) && (0 == read_in_bit(19)) ) //  
			{
				Sleep(1000);
				is_clearNeedle_ok = false;
				step_clearNeedle = 10;
			}
			else
			{
				Sleep(5);
				step_clearNeedle = 0;
			}
		}
		break;

		case 10:	// 判断是否可以执行清胶
		{
			if (is_reset_ok == false)
			{
				// 调用系统提示框, 阻塞当前线程
				MessageBox(NULL, TEXT("未复位, 无法执行擦胶"), TEXT("Warning"), MB_OK);
				
				emit changedRundataText(QStringLiteral("Thread_watch_clearNeedle 未复位, 无法执行擦胶"));
				writRunningLog(QStringLiteral("Thread_watch_clearNeedle 未复位, 无法执行擦胶"));
				step_clearNeedle = 0;
			}
			else if (card_isMoving() == true)
			{
				// 调用系统提示框, 阻塞当前线程
				MessageBox(NULL, TEXT("控制卡正在运行, 无法执行擦胶"), TEXT("Warning"), MB_OK);
				
				emit changedRundataText(QStringLiteral("Thread_watch_clearNeedle 控制卡正在运行, 无法执行擦胶"));
				writRunningLog(QStringLiteral("Thread_watch_clearNeedle 控制卡正在运行, 无法执行擦胶"));
				step_clearNeedle = 0;
			}
			else
			{
				// 换料盘开始
				emit changedRundataText(QStringLiteral("Thread_watch_clearNeedle 擦胶开始"));
				writRunningLog(QStringLiteral("Thread_watch_clearNeedle 擦胶开始"));
				step_clearNeedle = 20;
			}
		}

		case 20:	// 清胶
		{
			// 【1】 Z轴到安全点
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 到清胶安全点
			move_point_name("clear_glue_safe");
			wait_allaxis_stop();

			// 【3】 清胶气缸松开

			Sleep(1000);

			// 【4】 判断清胶气缸是否松开
			if (true)
			{
				step_clearNeedle = 30;
			}
			else
			{
				QMessageBox::about(NULL, "Warning", QStringLiteral("擦胶气缸松开失败"));
				step_clearNeedle = 9999;
			}
		}
		break;

		case 30:
		{
			// 【5】 到清胶点
			move_point_name("clear_glue");
			wait_allaxis_stop();

			// 【6】 清胶气缸夹紧

			Sleep(500);

			// 【7】 Z轴到安全点
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【8】 清胶气缸松开


			step_clearNeedle = 8888;
		}
		break;

		case 8888:	// 流程正常结束, 等待下次开始
		{
			emit changedRundataText(QStringLiteral("Thread_watch_clearNeedle 清胶流程正常结束, 等待下次开始"));
			writRunningLog(QStringLiteral("Thread_watch_clearNeedle 清胶流程正常结束, 等待下次开始"));

			is_clearNeedle_ok = true;
			step_clearNeedle = 0;
		}
		break;

		case 9999:	// 流程异常结束, 线程关闭
		{
			emit changedRundataText(QStringLiteral("Thread_watch_clearNeedle 清胶流程异常结束, 线程关闭"));
			writRunningLog(QStringLiteral("Thread_watch_clearNeedle 清胶流程异常结束, 线程关闭"));

			start_thread_watch_clearNeedle = false;
			close_thread_watch_clearNeedle = true;
			step_clearNeedle = 0;
		}
		break;

		default:
			break;
		}
	}
}

// Thread 启动
void Workflow::thread_watch_start()
{
	if (!(init_card() == 1)) return;

	int step_start = 0;

	while (close_thread_watch_start == false)
	{
		switch (step_start)
		{
		case 0:		// 等待触发
		{
			if ( (true == start_thread_watch_start) && (0 == read_in_bit(17)) )
			{
				if (true == is_reset_ok)
				{
					start_thread_watch_start = false;
					step_start = 10;
				}
				else
				{
					emit changedRundataLabel(QStringLiteral("请先复位"));
					emit changedRundataText(QStringLiteral("Thread_watch_start 未复位, 无法启动"));
					writRunningLog(QStringLiteral("Thread_watch_start 未复位, 无法启动"));

					Sleep(1000);
					step_start = 0;
				}

			}
			else
			{
				Sleep(5);
				step_start = 0;
			}
		}
		break;

		case 10:	// 消息更新
		{
			emit changedRundataLabel(QStringLiteral("运行中..."));
			emit changedRundataText(QStringLiteral("Thread_watch_start 点胶流程开始"));
			writRunningLog(QStringLiteral("Thread_watch_start 点胶流程开始"));

			// 开绿灯
			write_out_bit(14, 0);
			write_out_bit(15, 0);
			write_out_bit(16, 1);

			step_start = 20;
		}
		break;

		case 20:	// 点胶泰达开始
		{
			start_thread_glue_teda = true;
			Sleep(1000);
			step_start = 30;
		}
		break;

		case 30:	// 等待点胶1完成
		{
			// 判断点胶1是否完成
			if (is_glue_teda_ok == false)
			{
				Sleep(5);
				step_start = 30;
			}
			else
			{
				step_start = 80;
			}
		}
		break;

		case 80:	// 回清胶安全点
		{
			// 【1】 到原点
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			move_axis_abs(AXISNUM::X, 0);
			move_axis_abs(AXISNUM::Y, 0);

			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);

			// 【2】 等待下次开始
			step_start = 8888;
		}

		case 8888:	// 流程正常结束, 等待下次开始
		{
			// 【1】 消息更新
			emit changedRundataLabel(QStringLiteral("运行完毕"));
			emit changedRundataText(QStringLiteral("Thread_watch_start 所有流程正常结束"));
			writRunningLog(QStringLiteral("Thread_watch_start 所有流程正常结束"));

			// 【2】 开黄灯
			write_out_bit(14, 0);
			write_out_bit(15, 1);
			write_out_bit(16, 0);

			// 【3】 刷新线程标记位
			start_thread_watch_start = true;
			step_start = 0;
		}
		break;

		case 9999:	// 流程异常结束, 线程退出
		{
			// 【1】 消息更新
			emit changedRundataLabel(QStringLiteral("运行完毕"));
			emit changedRundataText(QStringLiteral("Thread_watch_start 流程异常结束"));
			writRunningLog(QStringLiteral("Thread_watch_start 流程异常结束"));

			// 【2】 开红灯
			write_out_bit(14, 1);
			write_out_bit(15, 0);
			write_out_bit(16, 0);

			// 【3】 刷新线程标记位
			close_thread_watch_start = true;
		}
		break;

		default:
			break;
		}
	}
}


// Thread 点胶
void Workflow::thread_glue_1()
{
	if (!(init_card() == 1)) return;

	// 从CCD获取到的偏移 X, Y, A
	float ccd_offset_x, ccd_offset_y, ccd_offset_A;

	// 镭射获取的高度
	float index_laser = 0;
	float laser_z[4];
	float laser_z_average = 0;

	// 4段圆弧的圆心坐标
	float center_result_x[4];
	float center_result_y[4];

	MatrixXf work_matrix;					// CCD点位旋转平移后的矩阵
	QVector<CCDGlue>::iterator iter_cmd;	// Vector的头尾
	CCDGlue current_cmd;					// 当前点位
	int     index_current_cmd;				// 当前点位索引
	
	int step_glue1 = 0;

	while (close_thread_glue_1 == false)
	{
		switch (step_glue1)
		{
		case 0:		// 等待触发
		{
			if (start_thread_glue_1 == false)
			{
				Sleep(2);
				step_glue1 = 0;
			}
			else
			{
				step_glue1 = 10;
			}
		}
		break;

		case 10:	// 检查是否配置了该流程
		{
			if (is_config_gluel == false)
			{
				step_glue1 = 7777;
			}
			else
			{
				step_glue1 = 20;
			}
		}
		break;

		case 20:	// 设置速度, 模式
		{
			// 设置所有轴速度, 模式
			// set_speed_mode()
			step_glue1 = 30;
		}
		break;

		case 30:	// Z轴到安全位
		{
			move_axis_abs(AXISNUM::Z, 0);
			step_glue1 = 40;
		}
		break;

		case 40:	// 提前切换图像程序
		{
			// 发消息
			step_glue1 = 50;
		}
		break;

		case 50:	// 点前清胶
		{
			// 【1】 到清胶安全点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【2】 判断清胶气缸
			if (true)
			{
				QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("清胶气缸状态错误, 请检查"));
				step_glue1 = 8888;
			}

			// 【4】 到清胶点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【5】 清胶气缸夹紧


			// 【6】 到清胶安全点, 此点位安全高度为 0
			move_point_name("xx");
			wait_allaxis_stop();

			// 【7】 清胶气缸松开

			step_glue1 = 60;
		}
		break;

		case 60:	// 到 "点胶1" 拍照点
		{
			// 到点胶1拍照点
			move_point_name("xx");
			wait_allaxis_stop();

			step_glue1 = 70;
		}
		break;

		case 70:	// 拍照
		{
			// 发消息拍照

			step_glue1 = 80;
		}
		break;

		case 80:	// 等待获取偏移 ccd_offset_x, ccd_offset_y, ccd_offset_z
		{
			if (receivedMsg_ccd == "" || receivedMsg_ccd.length() < 5)
			{
				step_glue1 = 80;
			}
			else
			{
				if (receivedMsg_ccd.split("]").last() == "-1")
				{
					step_glue1 = 7777;
				}
				else
				{
					ccd_offset_x = receivedMsg_ccd.split("]").at(1).toFloat();
					ccd_offset_y = receivedMsg_ccd.split("]").at(2).toFloat();
					ccd_offset_A = receivedMsg_ccd.split("]").at(3).toFloat();

					receivedMsg_ccd = "";

					step_glue1 = 90;
				}
			}
		}
		break;

		case 90:	// 计算矩阵偏移
		{
			work_matrix = CalCCDGluePoint(vec_ccdGlue_1, ccd_offset_x, ccd_offset_y, ccd_offset_A, org_ccdglue_x[CCDORG::GLUE1ORG], org_ccdglue_y[CCDORG::GLUE1ORG]);
		}
		break;

		case 100:	// 初始化所有标志位, 开启速度前瞻, 设置插补速度
		{
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			
			adt8949_set_speed_pretreat_mode(0, 1);
			set_inp_speed(1);

			step_glue1 = 110;

		}
		break;

		case 110:	// 判断 "结束" 和 "CCDGlue.type"
		{
			if ( iter_cmd == vec_ccdGlue_1.end())
			{
				step_glue1 = 500;	
			}
			else
			{
				if (vec_ccdGlue_1.at(index_current_cmd).type == QString("null"))
				{
					step_glue1 = 500;
				}
				else if (current_cmd.type == QString("line"))
				{
					current_cmd = vec_ccdGlue_1.at(index_current_cmd);
					step_glue1 = 200;
				}
				else if (current_cmd.type == QString("circle"))
				{
					current_cmd = vec_ccdGlue_1.at(index_current_cmd);
					step_glue1 = 400;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("点位解析错误"));
					step_glue1 = 9999;
				}
			}
		}
		break;


		/********************** 直线插补 **********************/
		case 200:	// 直线插补
		{
			// 判断 "镭射"
			if (current_cmd.laser)
			{
				step_glue1 = 300;
			}
			else
			{
				step_glue1 = 210;
			}
		}
		break;

		case 210:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				adt8949_set_fifo_io(0, 15, 1, -1);
			}
			else
			{

				adt8949_set_fifo_io(0, 15, 0, -1);
			}

			step_glue1 = 230;
		}
		break;

		/*case 210:	// 是否开胶
		{
			if (current_cmd.open)
			{
				if (current_cmd.openAdvance > 0 || current_cmd.openDelay <= 0)
				{
					// 提前开胶
				}
				else if (current_cmd.openAdvance <= 0 || current_cmd.openDelay > 0)
				{
					// 滞后开胶
				}
				else
				{
					// 立即开胶
					adt8949_set_fifo_io(0, 18, 1, -1);
				}
			}

			step_glue1 = 220;
		}
		break;

		case 220:	// 是否提前滞后关胶
		{
			if (current_cmd.closeAdvance > 0 || current_cmd.closeDelay <= 0)
			{
				// 提前关胶
			}
			else if (current_cmd.closeAdvance <= 0 || current_cmd.closeDelay > 0)
			{
				// 滞后关胶
			}
			step_glue1 = 230;
		}
		break;*/

		case 230:	// 直线插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(index_current_cmd, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(index_current_cmd, 2) + distance_ccd_needle_x + calib_offset_z; // +"镭射高度";

			// 直线插补
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue1 = 240;
		}
		break;

		/*case 240:	// 是否立即关胶
		{
			if (current_cmd.close)
			{
				// 关胶
				write_out_bit(0, 0);
			}

			step_glue1 = 250;
		}
		break;*/

		case 250:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;
			step_glue1 = 110;
		}
		break;


		/********************** 镭射测量 **********************/
		case 300:	// 镭射测量
		{
			float move_pos[3];

			// move_pos[0] = work_matrix[index_current_cmd, 0] + ;
		}
		break;

		
		/********************** 圆弧插补 **********************/
		case 400:	// 圆弧插补
		{
			step_glue1 = 410;
		}
		break;

		case 410:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				adt8949_set_fifo_io(0, 15, 1, -1);
			}
			else
			{

				adt8949_set_fifo_io(0, 15, 0, -1);
			}

			step_glue1 = 430;
		}
		break;

		/*case 410:	// 是否开胶
		{
			if (current_cmd.open)
			{
				if (current_cmd.openAdvance > 0 || current_cmd.openDelay <= 0)
				{
					// 提前开胶
				}
				else if (current_cmd.openAdvance <= 0 || current_cmd.openDelay > 0)
				{
					// 滞后开胶
				}
				else
				{
					// 立即开胶
					write_out_bit(0, 1);
				}
			}

			step_glue1 = 420;
		}
		break;

		case 420:	// 是否 提前, 滞后关胶
		{
			if (current_cmd.closeAdvance > 0 || current_cmd.closeDelay <= 0)
			{
				// 提前关胶
			}
			else if (current_cmd.closeAdvance <= 0 || current_cmd.closeDelay > 0)
			{
				// 滞后关胶
			}
			step_glue1 = 430;
		}
		break;*/


		case 430:	// 圆弧插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(index_current_cmd, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(index_current_cmd, 2) + distance_ccd_needle_x + calib_offset_z; // +"镭射高度";

			// 圆弧插补指令
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue1 = 440;
		}
		break;

		/*case 440:	// 是否立即关胶
		{
			if (current_cmd.close)
			{
				// 关胶
				write_out_bit(0, 0);
			}

			step_glue1 = 450;
		}
		break;*/


		case 450:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue1 = 110;
		}
		break;


		/********************** 点位解析完毕后做的事 **********************/
		case 500:	// 关闭速度前瞻
		{
			// 关闭插补缓存区
			// adt8949_set_speed_pretreat_mode(0, 0);

			step_glue1 = 510;
		}
		break;

		case 510:	// 设置运动速度, Z轴到安全位
		{
			// 【1】 等待插补完成
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			set_speed_mode(1, 1, 1, 1);

			// 【2】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			step_glue1 = 520;
		}
		break;

		case 520:	// 重置计数
		{
			iter_cmd = vec_ccdGlue_1.begin();
			iter_cmd = 0;
			index_current_cmd = 0;

			step_glue1 = 6666;
		}



		case 6666:	// 流程正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【1】 发消息
			emit changedRundataText(QStringLiteral("点胶1已完成"));
			writRunningLog(QStringLiteral("点胶1已完成"));

			start_thread_glue_1 = false;
			step_glue1 = 0;
		}
		break;

		case 7777:	// 流程非正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【0】 速度设置
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			set_speed_mode(1, 1, 1, 1);

			// 【1】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			// 【3】 发消息
			emit changedRundataText(QStringLiteral("点胶1流程执行失败, 正在跳过"));
			writRunningLog(QStringLiteral("点胶1流程执行失败, 正在跳过"));

			start_thread_glue_1 = false;
			step_glue1 = 0;
		}
		break;

		case 8888:	// 线程正常执行完毕, 关闭该线程
		{
			// 【1】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			emit changedRundataText(QStringLiteral("点胶1线程正常结束"));
			writRunningLog(QStringLiteral("点胶1线程正常结束"));

			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			step_glue1 = 0;
		}
		break;

		case 9999:	// 线程非正常执行完毕, 关闭该线程
		{
			// 安全退出该线程
			start_thread_glue_1 = false;
			close_thread_glue_1 = true;

			// 停止所有轴
			stop_allaxis();

			step_glue1 = 0;
		}
		break;

		default:
			break;
		}
	}

}

// Thread 点胶泰达
void Workflow::thread_glue_teda()
{
	if (!(init_card() == 1)) return;

	// 从CCD获取到的偏移 X, Y, A
	float ccd_offset_x, ccd_offset_y, ccd_offset_A;

	// 镭射获取的高度
	float index_laser = 0;
	float laser_z[4];
	float laser_z_average = 0;

	// 4段圆弧的圆心坐标
	float center_result_x[4];
	float center_result_y[4];

	MatrixXf work_matrix;					// CCD点位旋转平移后的矩阵
	QVector<CCDGlue>::iterator iter_cmd;	// Vector的头尾
	CCDGlue current_cmd;					// 当前点位
	int     index_current_cmd;				// 当前点位索引

	int step_glue_teda = 0;

	while (close_thread_glue_teda == false)
	{
		switch (step_glue_teda)
		{
		case 0:		// 等待触发
		{
			if (start_thread_glue_teda == false)
			{
				Sleep(5);
				step_glue_teda = 0;
			}
			else
			{
				start_thread_glue_teda = false;
				step_glue_teda = 10;
			}
		}
		break;

		case 10:	// 检查是否配置了该流程
		{
			if (is_config_glue_teda == false)
			{
				step_glue_teda = 7777;
			}
			else
			{
				step_glue_teda = 20;
			}
		}
		break;

		case 20:	// 设置速度, 模式
		{
			// 设置运动速度
			set_speed_mode(0.1, 5, 5, ADMODE::T);
			
			// 设置插补速度
			set_inp_speed_mode(0.1, 5, 5, ADMODE::T);

			step_glue_teda = 30;
		}
		break;

		case 30:	// Z轴到安全位
		{
			move_axis_abs(AXISNUM::Z, 0);
			step_glue_teda = 90;
		}
		break;

		case 40:	// 提前切换图像程序
		{
			// 发消息

			step_glue_teda = 50;
		}
		break;

		case 50:	// 点前清胶
		{
			// 【1】 到清胶安全点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【2】 判断清胶气缸
			if (true)
			{
				QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("清胶气缸状态错误, 请检查"));
				step_glue_teda = 8888;
			}

			// 【4】 到清胶点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【5】 清胶气缸夹紧


			// 【6】 到清胶安全点, 此点位安全高度为 0
			move_point_name("xx");
			wait_allaxis_stop();

			// 【7】 清胶气缸松开

			step_glue_teda = 60;
		}
		break;

		case 60:	// 到 "点胶1" 拍照点
		{
			// 到点胶1拍照点
			move_point_name("xx");
			wait_allaxis_stop();

			step_glue_teda = 70;
		}
		break;

		case 70:	// 拍照
		{
			// 发消息拍照

			step_glue_teda = 80;
		}
		break;

		case 80:	// 等待获取偏移 ccd_offset_x, ccd_offset_y, ccd_offset_z
		{
			if (receivedMsg_ccd == "" || receivedMsg_ccd.length() < 5)
			{
				step_glue_teda = 80;
			}
			else
			{
				if (receivedMsg_ccd.split("]").last() == "-1")
				{
					step_glue_teda = 7777;
				}
				else
				{
					ccd_offset_x = receivedMsg_ccd.split("]").at(0).toFloat();
					ccd_offset_y = receivedMsg_ccd.split("]").at(1).toFloat();
					ccd_offset_A = receivedMsg_ccd.split("]").at(2).toFloat();

					receivedMsg_ccd = "";

					step_glue_teda = 90;
				}
			}
		}
		break;

		case 90:	// 计算矩阵偏移
		{
			// work_matrix = CalCCDGluePoint(vec_ccdGlue_1, ccd_offset_x, ccd_offset_y, ccd_offset_A, org_ccdglue_x[CCDORG::GLUE1ORG], org_ccdglue_y[CCDORG::GLUE1ORG]);
			work_matrix = CalCCDGluePoint(vec_ccdGlue_1, 0, 0, 0, 0, 0);
			step_glue_teda = 100;
		}
		break;

		case 100:	// 初始化所有标志位, 开启速度前瞻, 设置插补速度
		{
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			adt8949_set_speed_pretreat_mode(0, 1);
			set_inp_speed(1);

			step_glue_teda = 110;

		}
		break;

		case 110:	// 判断 "结束" 和 "CCDGlue.type"
		{
			if (iter_cmd == vec_ccdGlue_1.end())
			{
				step_glue_teda = 7777;
			}
			else
			{
				current_cmd = vec_ccdGlue_1.at(index_current_cmd);

				if (current_cmd.type == QString("null"))
				{
					step_glue_teda = 7777;
				}
				else if (current_cmd.type == QString("line"))	// 直线插补
				{
					step_glue_teda = 200;
				}
				else if (current_cmd.type == QString("circle"))	// 圆弧插补
				{
					step_glue_teda = 400;
				}
				else if (current_cmd.type == QString("up_abs"))		// Z轴向上定位
				{
					step_glue_teda = 500;
				}
				else if (current_cmd.type == QString("up_offset"))	// Z轴向上偏移
				{
					step_glue_teda = 600;
				}
				else if (current_cmd.type == QString("move_abs"))		// 轴定位指令
				{
					step_glue_teda = 700;
				}
				else if (current_cmd.type == QString("move_offset"))	// 轴偏移指令
				{
					step_glue_teda = 800;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("点位解析错误"));
					step_glue_teda = 9999;
				}
			}
		}
		break;


		/********************** 直线插补 **********************/
		case 200:	// 直线插补
		{
			// 判断 "镭射"
			if (current_cmd.laser)
			{
				step_glue_teda = 300;
			}
			else
			{
				step_glue_teda = 210;
			}
		}
		break;

		case 210:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				adt8949_set_fifo_io(0, 15, 1, -1);
			}
			else
			{

				adt8949_set_fifo_io(0, 15, 0, -1);
			}

			step_glue_teda = 230;
		}
		break;

		case 230:	// 直线插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(index_current_cmd, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(index_current_cmd, 2) + distance_ccd_needle_x + calib_offset_z; // +"镭射高度";

																									// 直线插补
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue_teda = 240;
		}
		break;

		case 250:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;
			step_glue_teda = 110;
		}
		break;



		/********************** 镭射测量 **********************/
		case 300:	// 镭射测量
		{
			// float move_pos[3];

			// move_pos[0] = work_matrix[index_current_cmd, 0] + ;
		}
		break;



		/********************** 圆弧插补 **********************/
		case 400:	// 圆弧插补
		{
			step_glue_teda = 410;
		}
		break;

		case 410:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				adt8949_set_fifo_io(0, 15, 1, -1);
			}
			else if (current_cmd.close)
			{
				adt8949_set_fifo_io(0, 15, 0, -1);
			}

			step_glue_teda = 430;
		}
		break;

		case 430:	// 圆弧插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(index_current_cmd, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(index_current_cmd, 2) + distance_ccd_needle_x + calib_offset_z; // +"镭射高度";

																									// 圆弧插补指令
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue_teda = 440;
		}
		break;

		case 450:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** Z轴向上, 定位运动 **********************/
		case 500:	// Z轴向上, 定位指令
		{
			step_glue_teda = 510;
		}
		break;

		case 510:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 520;
		}
		break;

		case 520:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 530;
		}
		break;

		case 530:	// Z轴向上, 定位运动
		{
			float move_pos_z;
			move_pos_z = work_matrix(index_current_cmd, 2);
			
			move_axis_abs(AXISNUM::Z, move_pos_z, 2, 2, 2);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 540;
		}
		break;

		case 540:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** Z轴向上, 偏移指令 **********************/
		case 600:	// Z轴向上, 定位指令
		{
			step_glue_teda = 610;
		}
		break;

		case 610:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 620;
		}
		break;

		case 620:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 630;
		}
		break;

		case 630:	// Z轴向上, 偏移运动
		{
			float move_pos_z;
			move_pos_z = work_matrix(index_current_cmd, 2);

			move_axis_offset(AXISNUM::Z, move_pos_z, 2, 2, 2);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 640;
		}
		break;

		case 640:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 轴定位指令 **********************/
		case 700:	// Z轴向上, 定位指令
		{
			step_glue_teda = 710;
		}
		break;

		case 710:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 720;
		}
		break;

		case 720:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 630;
		}
		break;

		case 730:	// Z轴向上, 偏移运动
		{
			float move_pos[3];
			move_pos[0] = current_cmd.X;
			move_pos[1] = current_cmd.Y;
			move_pos[2] = current_cmd.Z;
			
			move_axis_abs(AXISNUM::X, move_pos[0], 2, 2, 2);
			move_axis_abs(AXISNUM::Y, move_pos[1], 2, 2, 2);
			move_axis_abs(AXISNUM::Z, move_pos[2], 2, 2, 2);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 640;
		}
		break;

		case 740:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 轴偏移指令 **********************/
		case 800:	// Z轴向上, 定位指令
		{
			step_glue_teda = 810;
		}
		break;

		case 810:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 820;
		}
		break;

		case 820:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 830;
		}
		break;

		case 830:	// Z轴向上, 偏移运动
		{
			float move_pos[3];
			move_pos[0] = current_cmd.X;
			move_pos[1] = current_cmd.Y;
			move_pos[2] = current_cmd.Z;

			move_axis_offset(AXISNUM::X, move_pos[0], 2, 2, 2);
			move_axis_offset(AXISNUM::Y, move_pos[1], 2, 2, 2);
			move_axis_offset(AXISNUM::Z, move_pos[2], 2, 2, 2);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 840;
		}
		break;

		case 840:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 点位解析完毕后做的事 **********************/
		case 1000:	// 关闭速度前瞻
		{
			// 关闭插补缓存区
			// adt8949_set_speed_pretreat_mode(0, 0);

			step_glue_teda = 510;
		}
		break;

		case 1010:	// 设置运动速度, Z轴到安全位
		{
			// 【1】 等待插补完成
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			set_speed_mode(1, 1, 1, 1);

			// 【2】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 520;
		}
		break;

		case 1020:	// 重置计数
		{
			iter_cmd = vec_ccdGlue_1.begin();
			iter_cmd = 0;
			index_current_cmd = 0;

			step_glue_teda = 6666;
		}



		case 6666:	// 流程正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【1】 发消息
			emit changedRundataText(QStringLiteral("点胶1已完成"));
			writRunningLog(QStringLiteral("点胶1已完成"));

			start_thread_glue_1 = false;
			step_glue_teda = 0;
		}
		break;

		case 7777:	// 流程非正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【0】 速度设置
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			set_speed_mode(1, 1, 1, 1);

			// 【1】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			// 【3】 发消息
			emit changedRundataText(QStringLiteral("点胶1流程执行失败, 正在跳过"));
			writRunningLog(QStringLiteral("点胶1流程执行失败, 正在跳过"));

			start_thread_glue_1 = false;
			step_glue_teda = 0;
		}
		break;

		case 8888:	// 线程正常执行完毕, 关闭该线程
		{
			// 【1】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			emit changedRundataText(QStringLiteral("点胶1线程正常结束"));
			writRunningLog(QStringLiteral("点胶1线程正常结束"));

			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			step_glue_teda = 0;
		}
		break;

		case 9999:	// 线程非正常执行完毕, 关闭该线程
		{
			// 安全退出该线程
			start_thread_glue_1 = false;
			close_thread_glue_1 = true;

			// 停止所有轴
			stop_allaxis();

			step_glue_teda = 0;
		}
		break;

		default:
			break;
		}
	}
}

// Thread 点胶泰达测试
void Workflow::thread_glue_teda_test()
{
	if (!(init_card() == 1)) return;

	int index_laser = 0;					// Laser索引

	MatrixXf work_matrix;					// CCD点位旋转平移后的矩阵
	QVector<CCDGlue>::iterator iter_cmd;	// Vector的头尾
	CCDGlue current_cmd;					// 当前点位
	int     index_current_cmd;				// 当前点位索引

	int step_glue_teda = 0;
	while (close_thread_glue_teda == false)
	{
		switch (step_glue_teda)
		{
		case 0:		// 等待触发
		{
			if (start_thread_glue_teda == false)
			{
				Sleep(5);
				step_glue_teda = 0;
			}
			else
			{
				// 刷新标志位
				start_thread_glue_teda = false;
				is_glue_teda_ok = false;
				
				step_glue_teda = 10;
			}
		}
		break;

		case 10:	// 检查是否配置了该流程
		{
			if (is_config_glue_teda == false)
			{
				step_glue_teda = 7777;
			}
			else
			{
				step_glue_teda = 20;
			}
		}
		break;

		case 20:	// 设置速度, 模式, 开启速度前瞻
		{
			// 设置运动速度
			set_speed_mode(0.1, 20, 20, ADMODE::T);

			// 设置插补速度
			set_inp_speed_mode(0.1, 20, 20, ADMODE::T);

			// 开启速度前瞻
			adt8949_set_speed_pretreat_mode(0, 1);

			// 设置插补过程中线段角点最大速度
			adt8949_set_speed_constraint(0, 1, 20);	// 此速度可以比插补速度稍大
			adt8949_set_speed_constraint(0, 2, 20);

			// 设置插补过程中线段角点加加速度
			adt8949_set_acc_constraint(0, 1, 80000);
			adt8949_set_acc_constraint(0, 2, 80000);

			step_glue_teda = 30;
		}
		break;

		case 30:	// Z轴到安全位
		{
			move_axis_abs(AXISNUM::Z, 0);
			step_glue_teda = 90;
		}
		break;

		case 90:	// 计算矩阵偏移
		{
			// work_matrix = CalCCDGluePoint(vec_ccdGlue_1, ccd_offset_x, ccd_offset_y, ccd_offset_A, org_ccdglue_x[CCDORG::GLUE1ORG], org_ccdglue_y[CCDORG::GLUE1ORG]);
			work_matrix = CalCCDGluePoint(vec_ccdGlue_1, 0, 0, 0, 0, 0);
			cout << work_matrix << endl << endl;
			step_glue_teda = 100;
		}
		break;

		case 100:	// 初始化所有标志位
		{
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			step_glue_teda = 110;
		}
		break;

		case 110:	// 判断 "结束" 和 "CCDGlue.type"
		{
			if (iter_cmd == vec_ccdGlue_1.end())
			{
				step_glue_teda = 6666;
			}
			else
			{
				current_cmd = vec_ccdGlue_1.at(index_current_cmd);

				if (current_cmd.type == QString("finish"))
				{
					step_glue_teda = 6666;
				}
				else if (current_cmd.type == QString("line"))		// 直线插补
				{
					step_glue_teda = 200;
				}
				else if (current_cmd.type == QString("circle"))		// 圆弧插补
				{
					step_glue_teda = 300;
				}
				else if (current_cmd.type == QString("laser"))		// 镭射测量
				{
					step_glue_teda = 400;
				}
				else if (current_cmd.type == QString("up_abs"))		// Z轴定位运动
				{
					step_glue_teda = 500;
				}
				else if (current_cmd.type == QString("up_offset"))	// Z轴偏移运动
				{
					step_glue_teda = 600;
				}
				else if (current_cmd.type == QString("move_abs"))	// 轴定位指令
				{
					step_glue_teda = 700;
				}
				else if (current_cmd.type == QString("move_offset"))// 轴偏移指令
				{
					step_glue_teda = 800;
				}
				else if (current_cmd.type == QString("open_close"))
				{
					step_glue_teda = 900;
				}
				else if (current_cmd.type == QString("continue"))	// 继续指令
				{
					step_glue_teda = 1000;
				}
				else if (current_cmd.type == QString("clear_needle"))
				{
					step_glue_teda = 1100;
				}
				else
				{
					MessageBox(NULL, TEXT("点位解析错误"), TEXT("警告"), MB_OK);
					step_glue_teda = 9999;
				}

				cout << step_glue_teda << endl;
			}
		}
		break;



		/********************** 直线插补 **********************/
		case 200:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				adt8949_set_fifo_io(0, 15, 1, -1);
			}
			else
			{
				adt8949_set_fifo_io(0, 15, 0, -1);
			}

			step_glue_teda = 210;
		}
		break;

		case 210:	// 直线插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(index_current_cmd, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(index_current_cmd, 2) + distance_ccd_needle_x + calib_offset_z; 

																									// 直线插补
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue_teda = 220;
		}
		break;

		case 220:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;


		/********************** 圆弧插补 **********************/
		case 300:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				adt8949_set_fifo_io(0, 15, 1, -1);
			}
			else if (current_cmd.close)
			{
				adt8949_set_fifo_io(0, 15, 0, -1);
			}
			step_glue_teda = 310;
		}
		break;

		case 310:	// 圆弧插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(index_current_cmd, 1) + distance_ccd_neddle_y + calib_offset_y;

			float center_pos[2];
			CalCCDGlueCenterPoint(center_pos, current_cmd.center_X, current_cmd.center_Y, 0, 0, 0, 0, 0);

			move_inp_abs_arc2(move_pos[0], move_pos[1], center_pos[0], center_pos[1]);

			step_glue_teda = 320;
		}
		break;

		case 320:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 镭射测量 **********************/
		case 400:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 410;
		}
		break;

		case 410:	// 
		{
			step_glue_teda = 420;
		}
		break;

		case 420:	// 
		{
			step_glue_teda = 430;
		}
		break;

		case 430:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;


		/********************** Z轴定位运动 **********************/
		case 500:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 510;
		}
		break;

		case 510:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 520;
		}
		break;

		case 520:	// Z轴定位运动
		{
			float move_pos_z = current_cmd.Z;

			move_axis_abs(AXISNUM::Z, move_pos_z);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 530;
		}
		break;

		case 530:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;


		/********************** Z轴偏移指令 **********************/
		case 600:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 610;
		}
		break;

		case 610:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 620;
		}
		break;

		case 620:	// Z轴偏移运动
		{
			float move_pos_z = current_cmd.Z;

			move_axis_offset(AXISNUM::Z, move_pos_z);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 630;
		}
		break;

		case 630:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;


		/********************** 轴定位指令 **********************/
		case 700:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 710;
		}
		break;

		case 710:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 720;
		}
		break;

		case 720:	// 轴绝对运动
		{
			float move_pos[3];
			move_pos[0] = current_cmd.X;
			move_pos[1] = current_cmd.Y;
			move_pos[2] = current_cmd.Z;

			move_axis_abs(AXISNUM::X, move_pos[0]);
			move_axis_abs(AXISNUM::Y, move_pos[1]);
			move_axis_abs(AXISNUM::Z, move_pos[2]);

			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 730;
		}
		break;

		case 730:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 轴偏移指令 **********************/
		case 800:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 810;
		}
		break;

		case 810:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 820;
		}
		break;

		case 820:	// 轴偏移运动
		{
			float move_pos[3];
			move_pos[0] = current_cmd.X;
			move_pos[1] = current_cmd.Y;
			move_pos[2] = current_cmd.Z;

			move_axis_offset(AXISNUM::X, move_pos[0]);
			move_axis_offset(AXISNUM::Y, move_pos[1]);
			move_axis_offset(AXISNUM::Z, move_pos[2]);

			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_glue_teda = 830;
		}
		break;

		case 830:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 开胶/关胶 **********************/	
		case 900:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 910;
		}
		break;

		case 910:	// 开胶, 关胶
		{
			if (current_cmd.open)
			{
				write_out_bit(15, 1);
			}
			else if (current_cmd.close)
			{
				write_out_bit(15, 0);
			}

			step_glue_teda = 920;
		}
		break;

		case 920:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;



		/********************** 继续指令 **********************/
		case 1000:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 1010;
		}
		break;

		case 1010:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;


		/********************** 清胶指令 **********************/
		case 1100:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 1110;
		}
		break;

		case 1110:	// 清胶
		{
			// 【1】 Z轴到安全点
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 到清胶安全点
			move_point_name("clear_glue_safe");
			wait_allaxis_stop();

			// 【3】 清胶气缸松开


			// 【4】 判断清胶气缸是否松开
			if (true)
			{
				step_glue_teda = 1120;
			}
			else
			{
				QMessageBox::about(NULL, "Warning", QStringLiteral("擦胶气缸松开失败"));
				step_glue_teda = 9999;
			}
		}
		break;

		case 1120:	
		{
			// 【5】 到清胶点
			move_point_name("clear_glue");
			wait_allaxis_stop();

			// 【6】 清胶气缸夹紧

			
			// 【7】 Z轴到安全点
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【8】 清胶气缸松开


			step_glue_teda = 1130;
		}
		break;

		case 1130:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;


		/********************** 拍照指令 **********************/
		case 1200:	// 等待插补结束
		{
			wait_inp_finish();
			step_glue_teda = 1210;
		}
		break;

		case 1210:	// 拍照
		{
			step_glue_teda = 1220;
		}
		break;

		case 1220:
		{
			step_glue_teda = 1230;
		}
		break;

		case 1230:	// 置标志位跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue_teda = 110;
		}
		break;




		case 6666:	// 流程正常结束, 跳回0, 等待下次触发流程
		{
			// 【1】 等待插补结束
			wait_inp_finish();

			// 【2】 关闭插补速度预处理
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【3】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【4】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			// 【5】 消息更新
			emit changedRundataText(QStringLiteral("Thread_glue_teda_test 点胶泰达测试流程正常结束, 等待下次触发流程"));
			writRunningLog(QStringLiteral("Thread_glue_teda_test 点胶泰达测试流程正常结束, 等待下次触发流程"));

			// 【6】 刷新线程标记位
			start_thread_glue_1 = false;
			is_glue_teda_ok = true;
			step_glue_teda = 0;
		}
		break;

		case 7777:	// 流程异常结束, 跳回0, 等待下次触发流程
		{
			// 【1】 等待插补结束
			wait_inp_finish();

			// 【2】 关闭插补速度预处理
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【3】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【4】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			// 【5】 消息更新
			emit changedRundataText(QStringLiteral("Thread_glue_teda_test 点胶泰达测试流程异常结束, 等待下次开始"));
			writRunningLog(QStringLiteral("Thread_glue_teda_test 点胶泰达测试流程异常结束, 等待下次开始"));

			// 【6】 刷新线程标记位
			start_thread_glue_1 = false;
			is_glue_teda_ok = true;
			step_glue_teda = 0;
		}
		break;

		case 8888:	// 线程正常结束, 线程关闭
		{
			// 【1】 等待插补结束
			wait_inp_finish();

			// 【2】 关闭插补速度预处理
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【3】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【4】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			// 【5】 消息更新
			emit changedRundataText(QStringLiteral("Thread_glue_teda_test 点胶泰达测试 线程正常结束, 线程关闭"));
			writRunningLog(QStringLiteral("Thread_glue_teda_test 点胶泰达测试 线程正常结束, 线程关闭"));

			// 【6】 刷新线程标记位
			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			step_glue_teda = 0;
		}
		break;

		case 9999:	// 线程异常结束, 线程关闭
		{
			// 【1】 等待插补结束
			wait_inp_finish();

			// 【2】 关闭插补速度预处理
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【3】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【4】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;
			index_laser = 0;

			// 【5】 消息更新
			emit changedRundataText(QStringLiteral("Thread_glue_teda_test 点胶泰达测试 线程异常结束, 线程关闭"));
			writRunningLog(QStringLiteral("Thread_glue_teda_test 点胶泰达测试 线程异常结束, 线程关闭"));
			
			// 【6】 刷新线程标记位
			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			step_glue_teda = 0;
		}
		break;

		default:
			break;

		}
	}
}



// Thread 校针 
void Workflow::thread_calibNeedle()
{
	if (!(init_card() == 1)) return;

	float x_start_pos, x_end_pos;
	float y_start_pos, y_end_pos;

	int step_calibNeedle = 0;

	while (close_thread_calibNeedle == false)
	{
		switch (step_calibNeedle)
		{
		case 0:		// 等待触发
		{
			if (start_thread_calibNeedle == false)
			{
				step_calibNeedle = 0;
			}
			else
			{
				step_calibNeedle = 10;
			}
		}
		break;

		case 10:	// 到校针安全点
		{
			// 换料盘开始
			emit changedRundataText(QStringLiteral("校针开始"));
			writRunningLog(QStringLiteral("校针开始"));

			// 移动到校针安全点
			move_point_name("calib_needle_safe");
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_calibNeedle = 20;
		}
		break;

		case 20:	// 到校针点, X-3, Y-3
		{
			// 移动到校针安全点
			move_point_name("calib_needle");
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			move_axis_offset(AXISNUM::X, -3);
			move_axis_offset(AXISNUM::Y, -3);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);

			step_calibNeedle = 30;
		}
		break;

		case 30:    // 向 X+ 走6mm 
		{
			move_axis_offset(AXISNUM::X, 6);

			step_calibNeedle = 40;
		}
		break;

		case 40:	// 等待触发 "对射X" == 1
		{
			if (1 == read_in_bit(35))	// 此处写对射X
			{
				stop_axis(AXISNUM::X);
				Sleep(100);
				x_start_pos = get_current_pos_axis(AXISNUM::X);

				step_calibNeedle = 50;
			}
			else
			{
				step_calibNeedle = 40;
			}
		}
		break;

		case 50:	// 等待触发 "对射X" == 0
		{
			if (0 == read_in_bit(35))	// 此处写 对射X
			{
				stop_axis(AXISNUM::X);
				Sleep(100);
				x_end_pos = get_current_pos_axis(AXISNUM::X);

				step_calibNeedle = 60;
			}
			else
			{
				step_calibNeedle = 50;
			}
		}
		break;

		case 60:	// 到对射X中间, 计算偏差
		{
			float x_mid_pos = (x_end_pos - x_start_pos) / 2;
			// move_axis_abs(AXISNUM::X, x_mid_pos, wSpeed, wAcc, wDec);
			calib_offset_x = x_mid_pos - allpoint_pointRun["calib_needle"].X;

			step_calibNeedle = 100;
		}
		break;

		case 100:	// 向 Y+ 走6mm 
		{
			move_axis_offset(AXISNUM::X, 6);
			step_calibNeedle = 110;
		}
		break;

		case 110:	// 等待触发 "对射Y" == 1
		{
			if (1 == read_in_bit(35))	// 此处写 "对射Y"
			{
				stop_axis(AXISNUM::Y);
				Sleep(100);
				y_start_pos = get_current_pos_axis(AXISNUM::Y);

				step_calibNeedle = 120;
			}
			else
			{
				step_calibNeedle = 110;
			}
		}
		break;

		case 120:   // 继续向 Y+ 走6mm
		{
			move_axis_offset(AXISNUM::X, 6);

			step_calibNeedle = 130;
		}
		break;

		case 130:	// 等待触发 "对射Y" == 0
		{
			if (0 == read_in_bit(35))	// 此处写 对射Y
			{
				stop_axis(AXISNUM::Y);
				Sleep(100);
				y_end_pos = get_current_pos_axis(AXISNUM::Y);

				step_calibNeedle = 140;
			}
			else
			{
				step_calibNeedle = 130;
			}
		}
		break;

		case 140:	// 到 "对射Y" 中间, 计算偏差
		{
			float y_mid_pos = (x_end_pos - x_start_pos) / 2;
			// move_axis_abs(AXISNUM::X, x_mid_pos, wSpeed, wAcc, wDec);
			calib_offset_y = y_mid_pos - allpoint_pointRun["calib_needle"].Y;

			step_calibNeedle = 200;
		}
		break;

		case 200:	// Z回安全高度 0
		{
			move_axis_abs(AXISNUM::Z, 0.000);
			wait_axis_stop(AXISNUM::Z);

			step_calibNeedle = 300;
		}
		break;

		case 300:
		{
			move_point_name("calib_needle_attach_safe");
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_calibNeedle = 310;
		}
		break;

		case 310:
		{
			move_point_name("calib_needle_attach");
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);

			step_calibNeedle = 320;
		}
		break;


		case 320:
		{
			move_axis_offset(AXISNUM::X, calib_offset_x);
			move_axis_offset(AXISNUM::Y, calib_offset_y);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);

			step_calibNeedle = 330;
		}
		break;

		case 330:
		{
			move_axis_offset(AXISNUM::X, calib_offset_x);
			move_axis_offset(AXISNUM::Y, calib_offset_y);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);

			step_calibNeedle = 8888;
		}
		break;


		case 8888:
		{
			emit changedRundataText(QStringLiteral("针头校准已结束"));
			writRunningLog(QStringLiteral("针头校准已结束"));

			start_thread_calibNeedle = false;
			close_thread_calibNeedle = true;
			step_calibNeedle = 0;
		}
		break;

		case 9999:
		{
			emit changedRundataText(QStringLiteral("针头校准失败, 已结束"));
			writRunningLog(QStringLiteral("针头校准失败, 已结束"));

			start_thread_calibNeedle = false;
			close_thread_calibNeedle = true;
			step_calibNeedle = 0;
		}
		break;

		default:
			break;
		}
	}
}

// Thread 空跑
void Workflow::thread_ccd_glue_1()
{
	if (!(init_card() == 1)) return;

	// 从CCD获取到的偏移 X, Y, A
	float ccd_offset_x, ccd_offset_y, ccd_offset_A;

	// 4段圆弧的圆心坐标
	float center_result_x[4];
	float center_result_y[4];

	MatrixXf work_matrix;					// CCD点位旋转平移后的矩阵
	QVector<CCDGlue>::iterator iter_cmd;	// Vector的头尾
	CCDGlue current_cmd;					// 当前点位
	int     index_current_cmd;				// 当前点位索引

	int step_glue1 = 0;

	while (close_thread_ccd_glue_1 == false)
	{
		switch (step_glue1)
		{
		case 0:		// 等待触发
		{
			if (start_thread_glue_1 == false)
			{
				Sleep(2);
				step_glue1 = 0;
			}
			else
			{
				step_glue1 = 20;
			}
		}
		break;

		case 20:	// 设置速度, 模式
		{
			// 设置所有轴速度, 模式
			// set_speed_mode()

			step_glue1 = 30;
		}
		break;

		case 30:	// Z轴到安全位
		{
			move_axis_abs(AXISNUM::Z, 0);
			step_glue1 = 40;
		}
		break;

		case 40:	// 提前切换图像程序
		{
			// 发消息
			step_glue1 = 60;
		}
		break;

		case 60:	// 到 "点胶1" 拍照点
		{
			// 到点胶1拍照点
			move_point_name("xx");
			wait_allaxis_stop();

			step_glue1 = 70;
		}
		break;

		case 70:	// 拍照
		{
			// 发消息拍照

			step_glue1 = 80;
		}
		break;

		case 80:	// 等待获取偏移 ccd_offset_x, ccd_offset_y, ccd_offset_z
		{
			if (receivedMsg_ccd == "" || receivedMsg_ccd.length() < 5)
			{
				step_glue1 = 80;
			}
			else
			{
				if (receivedMsg_ccd.split("]").last() == "-1")
				{
					step_glue1 = 7777;
				}
				else
				{
					ccd_offset_x = receivedMsg_ccd.split("]").at(1).toFloat();
					ccd_offset_y = receivedMsg_ccd.split("]").at(2).toFloat();
					ccd_offset_A = receivedMsg_ccd.split("]").at(3).toFloat();

					receivedMsg_ccd = "";

					step_glue1 = 90;
				}
			}
		}
		break;

		case 90:	// 计算矩阵偏移
		{
			work_matrix = CalCCDGluePoint(vec_ccdGlue_1, ccd_offset_x, ccd_offset_y, ccd_offset_A, org_ccdglue_x[CCDORG::GLUE1ORG], org_ccdglue_y[CCDORG::GLUE1ORG]);
		}
		break;

		case 100:	// 初始化所有标志位, 开启速度前瞻, 设置插补速度
		{
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			adt8949_set_speed_pretreat_mode(0, 1);
			set_inp_speed(1);

			step_glue1 = 110;

		}
		break;

		case 110:	// 判断 "结束" 和 "CCDGlue.type"
		{
			if (iter_cmd == vec_ccdGlue_1.end())
			{
				step_glue1 = 500;
			}
			else
			{
				if (vec_ccdGlue_1.at(index_current_cmd).type == QString("null"))
				{
					step_glue1 = 8888;
				}
				else if (current_cmd.type == QString("line"))
				{
					current_cmd = vec_ccdGlue_1.at(index_current_cmd);
					step_glue1 = 200;
				}
				else if (current_cmd.type == QString("circle"))
				{
					current_cmd = vec_ccdGlue_1.at(index_current_cmd);
					step_glue1 = 400;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("点位解析错误"));
					step_glue1 = 9999;
				}
			}
		}
		break;


		/********************** 直线插补 **********************/
		case 200:	// 直线插补
		{
			step_glue1 = 230;
		}
		break;

		case 230:	// 直线插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(index_current_cmd, 0);
			move_pos[1] = work_matrix(index_current_cmd, 1);
			move_pos[2] = work_matrix(index_current_cmd, 2);
			
			// 直线插补
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue1 = 250;
		}
		break;

		case 250:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;
			step_glue1 = 110;
		}
		break;

		/********************** 圆弧插补 **********************/
		case 400:	// 圆弧插补
		{
			step_glue1 = 430;
		}
		break;

		case 430:	// 圆弧插补
		{
			float move_pos[2];
			move_pos[0] = work_matrix(index_current_cmd, 0);
			move_pos[1] = work_matrix(index_current_cmd, 1);

			float center_pos[2];
			CalCCDGlueCenterPoint(center_pos, current_cmd.center_X, current_cmd.center_Y, ccd_offset_x, ccd_offset_y, ccd_offset_A, org_ccdglue_x[CCDORG::GLUE1ORG], org_ccdglue_y[CCDORG::GLUE1ORG]);
			
			move_inp_abs_arc2(move_pos[0], move_pos[1], center_pos[0], center_pos[1]);

			step_glue1 = 450;
		}
		break;

		case 450:	// 置标志位, 跳回
		{
			iter_cmd++;
			index_current_cmd++;

			step_glue1 = 110;
		}
		break;


		/********************** 点位解析完毕后做的事 **********************/
		case 6666:	// 流程正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【1】 等待插补完成
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			// 【3】 消息刷新
			emit changedRundataText(QStringLiteral("CCD点胶1空跑已完成"));
			writRunningLog(QStringLiteral("CCD点胶1空跑已完成"));

			start_thread_glue_1 = false;
			stop_allaxis();
			step_glue1 = 0;
		}
		break;

		case 7777:	// 流程非正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【1】 等待插补完成
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			// 【3】 发消息
			emit changedRundataText(QStringLiteral("CCD点胶1空跑流程执行失败, 正在跳过"));
			writRunningLog(QStringLiteral("CCD点胶1空跑执行失败, 正在跳过"));

			start_thread_glue_1 = false;
			stop_allaxis();
			step_glue1 = 0;
		}
		break;

		case 8888:	// 线程正常执行完毕, 关闭该线程
		{
			// 【1】 等待插补完成
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			// 【3】 发消息
			emit changedRundataText(QStringLiteral("CCD点胶1空跑线程执行完成"));
			writRunningLog(QStringLiteral("CCD点胶1空跑线程执行完成"));

			start_thread_ccd_glue_1 = false;
			close_thread_ccd_glue_1 = true;
			stop_allaxis();
			step_glue1 = 0;
		}
		break;

		case 9999:	// 线程非正常执行完毕, 关闭该线程
		{
			// 【1】 等待插补完成
			wait_inp_finish();
			adt8949_set_speed_pretreat_mode(0, 0);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			index_current_cmd = 0;

			// 【3】 发消息
			emit changedRundataText(QStringLiteral("CCD点胶1空跑线程执行失败, 正在跳过"));
			writRunningLog(QStringLiteral("CCD点胶1空跑线程执行完成"));

			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			stop_allaxis();
			step_glue1 = 0;
		}
		break;

		default:
			break;
		}
	}
}

// Thread 清胶
void Workflow::thread_clearNeedle()
{
	if (!(init_card() == 1)) return;
	if (card_isMoving() == 1)
	{
		MessageBox(NULL, TEXT("控制卡正在运动, 无法执行擦胶"), TEXT("Warning"), MB_OK);
		return;
	}
	

	int step_clearNeedle = 0;

	while (close_thread_clearNeedle == false)
	{
		switch (step_clearNeedle)
		{
		case 0:		// 等待触发
		{
			if (start_thread_clearNeedle == false)
			{
				step_clearNeedle = 0;
			}
			else
			{
				step_clearNeedle = 5;
			}
		}
		break;

		case 5:		// 消息更新
		{
			// 换料盘开始
			emit changedRundataText(QStringLiteral("清针开始"));
			writRunningLog(QStringLiteral("清针开始"));
			step_clearNeedle = 10;
		}

		case 10:	// 清针开始, 抬针头
		{
			move_axis_abs(AXISNUM::Z, 0.000);
			wait_axis_stop(AXISNUM::Z);

			step_clearNeedle = 20;
		}
		break;


		case 20:	// 到清针安全位
		{
			move_point_name("xxx");
			wait_allaxis_stop();

			step_clearNeedle = 30;
		}
		break;

		case 30:	// 判断清胶气缸
		{
			if (true)
			{
				QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("清胶气缸状态错误, 请检查"));
				step_clearNeedle = 8888;
			}
			else
			{
				step_clearNeedle = 40;
			}
		}
		break;

		case 40:	// 清胶
		{
			// 【1】 到清胶点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【2】 清胶气缸夹紧


			// 【3】 到清胶安全点, 此点位安全高度为 0
			move_axis_abs(AXISNUM::Z, 0.000);
			wait_allaxis_stop();

			// 【4】 清胶气缸松开

			step_clearNeedle = 8888;
		}
		break;

		case 8888:	// 线程正常执行完毕, 关闭该线程
		{
			emit changedRundataText(QStringLiteral("清胶正常结束"));
			writRunningLog(QStringLiteral("清胶正常结束"));

			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			step_clearNeedle = 0;
		}
		break;

		case 9999:	// 线程非正常执行完毕, 关闭该线程
		{
			emit changedRundataText(QStringLiteral("清胶执行失败, 已结束"));
			writRunningLog(QStringLiteral("清胶执行失败, 已结束"));

			start_thread_glue_1 = false;
			close_thread_glue_1 = true;
			step_clearNeedle = 0;
		}
		break;

		default:
			break;
		}
	}
}

// Thread CCD标定
void Workflow::thread_ccd_calib()
{
	int step_nineCalib = 0;

	while (close_thread_ccd_calib == false)
	{
		switch (step_nineCalib)
		{
		case 0:		// 等待触发
		{
			if (start_thread_ccd_calib == true) //  
			{
				Sleep(200);
				is_ccd_calib_ing = true;
				step_nineCalib = 10;
			}
			else
			{
				Sleep(5);
				step_nineCalib = 0;
			}
		}
		break;

		case 10:	// 刷新消息
		{
			// 换料盘开始
			emit changedRundataText(QStringLiteral("Thread_nineCalib 9点标定开始"));
			writRunningLog(QStringLiteral("Thread_nineCalib 点标定开始"));
			step_nineCalib = 20;
		}

		case 20:	// 设置速度, 模式
		{
			// 设置运动速度
			// set_speed_mode(0.1, 20, 20, ADMODE::T);
			step_nineCalib = 30;
		}
		break;

		case 30:	// 到标定拍照点
		{
			step_nineCalib = 8888;
		}
		break;

		case 8888:	// 线程正常结束, 线程关闭
		{
			// 刷新消息
			emit changedRundataText(QStringLiteral("Thread_ccd_calib CCD标定正常结束"));
			writRunningLog(QStringLiteral("Thread_ccd_calib CCD标定正常结束"));

			// 刷新线程标记位
			is_ccd_calib_ing = false;
			start_thread_ccd_calib = false;
			close_thread_ccd_calib = true;
			step_nineCalib = 0;
		}
		break;

		default:
			break;
		}
	}
}


// 通讯 Laser 串口
void Workflow::thread_serialLaserReceive()
{
	while (close_thread_serialLaserReceive == false)
	{
		QByteArray readData = serial_laser->readAll();
		if (readData.size() > 5)
		{
			mutex_serial.lock();
			receivedMsg_laser = QString(readData).remove(QChar(2)).remove(QChar(3));
			mutex_serial.unlock();
		}
		readData.clear();
		Sleep(5);
	}
}

// 通讯 CCD socket
void Workflow::socket_ccd_receive()
{
	QByteArray readData = socket_ccd->read(128);
	if (readData.size() > 5)
	{
		QString str_recerve = QString(readData);
	}
	readData.clear();

}


// 获取所有可运动点位
QMap<QString, PointRun> Workflow::getAllRunPointInfo()
{
	QMap<QString, PointRun> _allPoint;

	for (int index = 0; index < model_general->rowCount(); index++)
	{
		QString name = model_general->record(index).value("name").toString();
		QString description = model_general->record(index).value("description").toString();
		float X = model_general->record(index).value("X").toString().toFloat();
		float Y = model_general->record(index).value("Y").toString().toFloat();
		float Z = model_general->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;
		_allPoint.insert(name, point);
	}
	
	for (int index = 0; index < model_glue1->rowCount(); index++)
	{
		QString name = model_glue1->record(index).value("name").toString();
		QString description = model_glue1->record(index).value("description").toString();
		float X = model_glue1->record(index).value("X").toString().toFloat();
		float Y = model_glue1->record(index).value("Y").toString().toFloat();
		float Z = model_glue1->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;

		_allPoint.insert(name, point);
	}

	for (int index = 0; index < model_glue2->rowCount(); index++)
	{
		QString name = model_glue2->record(index).value("name").toString();
		QString description = model_glue2->record(index).value("description").toString();
		float X = model_glue2->record(index).value("X").toString().toFloat();
		float Y = model_glue2->record(index).value("Y").toString().toFloat();
		float Z = model_glue2->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;

		_allPoint.insert(name, point);
	}

	for (int index = 0; index < model_glue3->rowCount(); index++)
	{
		QString name = model_glue3->record(index).value("name").toString();
		QString description = model_glue3->record(index).value("description").toString();
		float X = model_glue3->record(index).value("X").toString().toFloat();
		float Y = model_glue3->record(index).value("Y").toString().toFloat();
		float Z = model_glue3->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;

		_allPoint.insert(name, point);
	}

	return _allPoint;
}

// 获取点胶工站点位, CCDGlue到QVector
QVector<CCDGlue> Workflow::getCCDGluePoint2Vector(int index)
{
	QSqlTableModel *pointmodel = new QSqlTableModel();
	QVector<CCDGlue> _vector_ccdGlue;

	if (1 == index) pointmodel = model_glue1; // QSqlTableModel *pointmodel = model_glue1;
	else if (2 == index) pointmodel = model_glue2;
	else if (3 == index) pointmodel = model_glue3;
	else
	{
		QMessageBox::warning(NULL, "错误", QStringLiteral("设置数据库模型错误"));
		return _vector_ccdGlue;
	}


	for (int index = 0; index < pointmodel->rowCount(); index++)
	{
		QString name = pointmodel->record(index).value("name").toString();
		QString description = pointmodel->record(index).value("description").toString();
		float X = pointmodel->record(index).value("X").toString().toFloat();
		float Y = pointmodel->record(index).value("Y").toString().toFloat();
		float Z = pointmodel->record(index).value("Z").toString().toFloat();
		float center_X = pointmodel->record(index).value("center_X").toString().toFloat();
		float center_Y = pointmodel->record(index).value("center_Y").toString().toFloat();
		float extra_offset_z = pointmodel->record(index).value("extra_offset_z").toString().toFloat();
		bool laser = pointmodel->record(index).value("laser").toBool();
		bool open = pointmodel->record(index).value("open").toBool();
		int openAdvance = pointmodel->record(index).value("openAdvance").toInt();
		int openDelay = pointmodel->record(index).value("openDelay").toInt();
		bool close = pointmodel->record(index).value("close").toBool();
		int closeAdvance = pointmodel->record(index).value("closeAdvance").toInt();
		int closeDelay = pointmodel->record(index).value("closeDelay").toInt();
		QString type = pointmodel->record(index).value("type").toString();

		CCDGlue point; // = { name, description, X, Y, Z, open, openAdvance, openDelay, close, closeAdvance, closeDelay, type };
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;
		point.center_X = center_X;
		point.center_Y = center_Y;
		point.extra_offset_z = calib_offset_z;
		point.laser = laser;
		point.open = open;
		point.openAdvance = openAdvance;
		point.openDelay = openDelay;
		point.close = close;
		point.closeAdvance = closeAdvance;
		point.closeDelay = closeDelay;
		point.type = type;

		_vector_ccdGlue.append(point);
	}


	return _vector_ccdGlue;
}

// 计算平移矩阵     offset_x, offset_y
MatrixXf Workflow::CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y)
{
	// 【1】 声明矩阵
	MatrixXf m2f_ccdGlue(2, 1);			// CCD矩阵 X, Y
	MatrixXf m2f_offset(2, 1);			// 偏移矩阵 offset_x, offset_y
	MatrixXf m2f_tmp(2, 1);				// m2f_tmp = m2f_offset + m2f_ccdGlue

	MatrixXf m3f_result_ccdGlue(20, 3);		// 返回 3维pointGlue

											// 【2】 初始化矩阵
	m2f_ccdGlue <<
		0,
		0;

	m2f_offset <<
		offset_x,
		offset_y;

	m2f_tmp <<
		0,
		0;

	// 【3】计算结果
	for (int index = 0; index < vector_ccdGlue.size(); index++)
	{
		// 获取CCD矩阵的 X，Y
		m2f_ccdGlue <<
			vector_ccdGlue.at(index).X,
			vector_ccdGlue.at(index).Y;

		// 平移
		m2f_tmp = m2f_ccdGlue + m2f_offset;

		// 获取返回值
		m3f_result_ccdGlue(index, 0) = m2f_tmp(0, 0);
		m3f_result_ccdGlue(index, 1) = m2f_tmp(1, 0);
		m3f_result_ccdGlue(index, 2) = vector_ccdGlue.at(index).Z;
	}

	return  m3f_result_ccdGlue;
}

// 计算平移旋转矩阵  offset_x, offset_y, offset_angle, org_x, org_y
MatrixXf Workflow::CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y)
{
	MatrixXf m3f_ccdGlue(3, 1);				// CCD矩阵
	MatrixXf m3f_offset(3, 3);				// 偏移矩阵

	MatrixXf m3f_angle(3, 3);			    // 角度矩阵
	MatrixXf m3f_move_left(3, 3);			// 左移			
	MatrixXf m3f_move_right(3, 3);			// 右移

	MatrixXf m3f_tmp(3, 1);
	MatrixXf m3f_result_ccdGlue(100, 3);

	m3f_ccdGlue <<
		0,
		0,
		1;

	m3f_offset <<
		1, 0, offset_x,
		0, 1, offset_y,
		0, 0, 1;

	m3f_angle <<
		cos(offset_angle), -sin(offset_angle), 0,
		sin(offset_angle), cos(offset_angle), 0,
		0, 0, 1;

	m3f_move_left <<
		1, 0, -org_x,
		0, 1, -org_y,
		0, 0, 1;

	m3f_move_right <<
		1, 0, org_x,
		0, 1, org_x,
		0, 0, 1;

	m3f_tmp <<
		0,
		0,
		1;


	for (int index = 0; index<vector_ccdGlue.size(); index++)
	{
		// 获取CCD矩阵的 X，Y
		m3f_ccdGlue <<
			vector_ccdGlue.at(index).X,
			vector_ccdGlue.at(index).Y,
			1;

		// 绕原点旋转
		/*// x = r * cos(A),  y = r * sin(A);
		// _x = r * cos(A + B) = r * cos(A) * cos(B) - r * sin(A) * sin(B) = x * cos(B) - y * sin(B)
		// _y = r * sin(A + B) = r * sin(A) * cos(B) + r * cos(A) * sin(B) = x * sin(B) + y * cos(B)
		// MatrixXf m2f_angle(2, 2);
		// m2f_angle << cos(B), -sin(B),
		//				sin(B), cos(B);*/

		// 绕某一点旋转
		/*// 1. 左移 -> (-org_x, -org_y)
		// 2. 旋转
		//		MatrixXf m3f_angle(3, 3);
		//		m2f_angle << cos(B), -sin(B), 0,
		//					 sin(B),  cos(B), 0,
		//						0,      0,    1;
		// 3. 右移 -> (org_x, org_y) */

		/*// 左移
		m3f_tmp = m3f_move_left * m3f_ccdGlue;
		cout << m3f_tmp << endl << endl;
		// 旋转
		m3f_tmp = m3f_angle * m3f_tmp;
		cout << m3f_tmp << endl << endl;
		// 右移
		m3f_tmp = m3f_move_right * m3f_tmp;
		cout << m3f_tmp << endl << endl;*/

		// 旋转
		m3f_tmp = m3f_move_right * m3f_angle * m3f_move_left * m3f_ccdGlue;

		// 平移
		m3f_tmp = m3f_offset * m3f_tmp;

		// 返回
		m3f_result_ccdGlue(index, 0) = m3f_tmp(0, 0);
		m3f_result_ccdGlue(index, 1) = m3f_tmp(1, 0);
		m3f_result_ccdGlue(index, 2) = vector_ccdGlue.at(index).Z;
	}

	return  m3f_result_ccdGlue;
}

// 计算平移旋转点位
void Workflow::CalCCDGlueCenterPoint(float center_pos[2], const float center_x, const float center_y, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y)
{
	MatrixXf m3f_ccdGlue(3, 1);				// CCD矩阵
	MatrixXf m3f_offset(3, 3);				// 偏移矩阵

	MatrixXf m3f_angle(3, 3);			    // 角度矩阵
	MatrixXf m3f_move_left(3, 3);			// 左移			
	MatrixXf m3f_move_right(3, 3);			// 右移

	MatrixXf m3f_tmp(3, 1);

	m3f_ccdGlue <<
		center_x,
		center_y,
		1;

	m3f_offset <<
		1, 0, offset_x,
		0, 1, offset_y,
		0, 0, 1;

	m3f_angle <<
		cos(offset_angle), -sin(offset_angle), 0,
		sin(offset_angle), cos(offset_angle), 0,
		0, 0, 1;

	m3f_move_left <<
		1, 0, -org_x,
		0, 1, -org_y,
		0, 0, 1;

	m3f_move_right <<
		1, 0, org_x,
		0, 1, org_x,
		0, 0, 1;

	m3f_tmp <<
		0,
		0,
		1;

	// 旋转
	m3f_tmp = m3f_move_right * m3f_angle * m3f_move_left * m3f_ccdGlue;

	// 平移
	m3f_tmp = m3f_offset * m3f_tmp;

	// 返回
	center_pos[0] = m3f_tmp(0, 0);
	center_pos[1] = m3f_tmp(1, 0);
}

// 移动到点, by pointname
bool Workflow::move_point_name(QString pointname, int z_flag)
{
	if (!allpoint_pointRun.contains(pointname))
	{
		QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("找不到点: %1 数据").arg(pointname));
		return false;
	}
	else
	{
		PointRun point = allpoint_pointRun[pointname];

		float x_pos = point.X;
		float y_pos = point.Y;
		float z_pos = point.Z;

		if (ZMOVETYPE::NORMAL == z_flag)
		{
			move_axis_abs(AXISNUM::X, x_pos);
			move_axis_abs(AXISNUM::Y, y_pos);
			move_axis_abs(AXISNUM::Z, z_pos);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
			wait_axis_stop(AXISNUM::Z);
		}
		else if (ZMOVETYPE::BEFORE == z_flag)
		{
			move_axis_abs(AXISNUM::Z, z_pos);
			wait_axis_stop(AXISNUM::Z);

			move_axis_abs(AXISNUM::X, x_pos);
			move_axis_abs(AXISNUM::Y, y_pos);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);
		}
		else if (ZMOVETYPE::AFTER == z_flag)
		{
			move_axis_abs(AXISNUM::X, x_pos);
			move_axis_abs(AXISNUM::Y, y_pos);
			wait_axis_stop(AXISNUM::X);
			wait_axis_stop(AXISNUM::Y);

			move_axis_abs(AXISNUM::Z, z_pos);
			wait_axis_stop(AXISNUM::Z);
		}
		else
		{
			return false;
		}
	}

	return true;
}


// 写log文件
void Workflow::writRunningLog(QString str)
{
	// 【1】 输出信息格式化
	QDateTime currentDate = QDateTime::currentDateTime();
	QString s_currentDate = currentDate.toString(QStringLiteral("yyyy-MM-dd"));
	QString s_currentTime = getCurrentTime();
	QString s_filepath = QString("../data/log/%1.txt").arg(s_currentDate);
	QString s_write = s_currentTime + "   " + str + "\n";

	// 【2】 向文件中写入内容
	QFile file(s_filepath);
	file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	QTextStream in(&file);
	in << s_write;
	file.close();
}

// 获取当前时间
QString Workflow::getCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
	return s_currentTime;
}

// 获取镭射数据
bool Workflow::getLaserData(float laser, int limit_ms)
{
	if (!serial_laser->isOpen())
	{
		return false;
	}

	char data_send[10] = { 0x02, 'M', 'E', 'A', 'S', 'U', 'R', 'E', 0x03 };
	serial_laser->write(data_send);

	int i = 0;
	while (i < limit_ms)
	{
		if (receivedMsg_laser != "")
		{
			float rcv = receivedMsg_laser.toFloat();

			mutex_serial.lock();
			receivedMsg_laser = "";
			mutex_serial.unlock();

			if (rcv > float(45.0) && rcv < float(55.0))
			{
				laser = rcv;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			Sleep(1);
			i++;
		}
	}
	return false;
}

// 获取视觉数据
bool Workflow::getVisionData(float offset_x, float offset_y, float offset_z, int limit_ms)
{
	if (!socket_ccd->isOpen())
	{
		return 0;
	}
	
	char data_send[11] = { '0', ',', '0', ',', '0', ',', '0', ',', '1', ';' };
	socket_ccd->write(data_send);

	int i = 0; 
	while (i < limit_ms)
	{
		if (receivedMsg_ccd != "")
		{
			QStringList ldata = receivedMsg_ccd.split("]");
			if (ldata.last() == "1")
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			Sleep(1);
			i++;
		}
	}

	return false;
}



// 连接 Operation 流程配置修改
void Workflow::on_clicked_check_flowConfig()
{
	// 还是使用读文件, 并不会影响多少效率

	QFile file("../config/workflow_glue.ini");
	if (!file.exists()) 
	{
		QMessageBox::about(NULL, "Warning", QStringLiteral("Workflow 下未找到 workflow_glue.ini 文件"));
		return;
	}
	QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);

	is_config_gluel = setting.value("workflow_glue/is_config_gluel").toBool();
	is_config_glue2 = setting.value("workflow_glue/is_config_glue2").toBool();
	is_config_glue3 = setting.value("workflow_glue/is_config_glue3").toBool();

	file.close();
}

// 连接 Operation 距离,偏移量修改
void Workflow::on_clicked_btn_saveDistanceOffset()
{
	// 还是使用读文件, 并不会影响多少效率
	QFile file("../config/workflow_glue.ini");
	if (!file.exists())
	{
		QMessageBox::about(NULL, "Warning", QStringLiteral("Workflow 下未找到 workflow_glue.ini 文件"));
		return;
	}
	QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);

	distance_ccd_needle_x = setting.value("ccd_needle_diatance/offset_x").toInt() / 1000.0;
	distance_ccd_neddle_y = setting.value("ccd_needle_diatance/offset_y").toInt() / 1000.0;

	distance_ccd_laser_x = setting.value("ccd_laser_diatance/offset_x").toInt() / 1000.0;
	diatance_ccd_laser_y = setting.value("ccd_laser_diatance/offset_x").toInt() / 1000.0;

	distance_laser_needle_x = setting.value("laser_needle_diatance/offset_x").toInt() / 1000.0;
	distance_laser_needle_y = setting.value("laser_needle_diatance/offset_y").toInt() / 1000.0;
	distance_laser_needle_z = setting.value("laser_needle_diatance/offset_z").toInt() / 1000.0;

	calib_offset_x = setting.value("calib_needle_optical/calib_offset_x").toInt() / 1000.0;
	calib_offset_y = setting.value("calib_needle_optical/calib_offset_y").toInt() / 1000.0;
	calib_offset_z = setting.value("calib_needle_attach/calib_offset_z").toInt() / 1000.0;
	file.close();
}

// 连接 Operation CCD标定
void Workflow::on_clicked_btn_ccd_calib()
{
	if (!(init_card() == 1)) return;

	if (is_reset_ok == false)
	{
		QMessageBox::about(NULL, "Warning", QStringLiteral("未复位, 无法进行CCD标定"));
		return;
	}

	if (card_isMoving() == true)
	{
		QMessageBox::about(NULL, "Warning", QStringLiteral("控制卡正在运行, 无法进行CCD标定"));
		return;
	}

	if (is_calibNeedle_ing == true)
	{
		QMessageBox::about(NULL, "Waring", QStringLiteral("正在标定, 请勿重复点击"));
		return;
	}

	start_thread_ccd_calib = true;
	close_thread_ccd_calib = false;

	future_thread_ccd_calib = QtConcurrent::run(&thread_pool, [&]() { thread_ccd_calib(); });
}

// 连接 Operation CCD空跑
void Workflow::on_clicked_btn_ccd_runEmpty()
{

}

// 连接 Operation 空跑不点胶
void Workflow::on_clicked_btn_runEmpty()
{

}

// 连接 Operation 清胶
void Workflow::on_clicked_btn_clearGlue()
{

}

// 连接 Operation 自动排胶
void Workflow::on_clicked_btn_dischargeGlue()
{

}

// 连接 Operation 校针1
void Workflow::on_clicked_btn_needleCalib_1()
{

}

// 连接 Operation 校针2
void Workflow::on_clicked_btn_needleCalib_2()
{

}

// 连接 PointDebug 点位数据修改
void Workflow::on_changedSqlModel(int index)
{
	allpoint_pointRun = getAllRunPointInfo();

	if (0 == index)
	{
		model_general->select();
	}
	else if (1 == index)
	{
		model_glue1->select();
		vec_ccdGlue_1 = getCCDGluePoint2Vector(1);	// 点胶流程点位信息, 用于自动解析指令
	}
	else if (2 == index)
	{
		model_glue2->select();
		vec_ccdGlue_2 = getCCDGluePoint2Vector(2);
	}
	else if (3 == index)
	{
		model_glue3->select();
		vec_ccdGlue_3 = getCCDGluePoint2Vector(3);
	}
	else return;
}
