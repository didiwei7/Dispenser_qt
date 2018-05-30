#include "workflow.h"

Workflow::Workflow(QObject *parent) : QObject(parent)
{
	setIOStatus();
	setConfig();

	setThread();
}

void Workflow::setConfig()
{
	// 配置流程
	QFile file("../config/workflow_glue.ini");
	if (!file.exists())
	{
		is_config_gluel = false;
		is_config_glue2 = false;
		is_config_glue3 = false;
	}
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		is_config_gluel = setting.value("workflow_glue/is_config_gluel").toBool();
		is_config_glue2 = setting.value("workflow_glue/is_config_glue2").toBool();
		is_config_glue3 = setting.value("workflow_glue/is_config_glue3").toBool();
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

	thread_pool.setMaxThreadCount(6);

	is_start_ok = false;
	start_thread_watch_start = false;
	close_thread_watch_start = false;

	is_reset_ok = false;
	start_thread_watch_reset = false;
	close_thread_watch_reset = false;

	is_stop_ok = false;
	close_thread_watch_stop = false;
	close_thread_watch_stop = false;

	is_estop_ok = false;
	close_thread_watch_estop = false;
	close_thread_watch_estop = false;

	is_workflow_ok = false;
	start_thread_workflow = false;
	close_thread_workflow = false;
	
	is_exchangeTrays_ok = false;
	start_thread_exchangeTrays = false;
	close_thread_exchangeTrays = false;
	
	future_thread_watch_start = QtConcurrent::run(&thread_pool, [&]() { thread_watch_start(); });
	future_thread_watch_reset = QtConcurrent::run(&thread_pool, [&]() { thread_watch_reset(); });
	future_thread_watch_stop  = QtConcurrent::run(&thread_pool, [&]() { thread_watch_stop(); });
	future_thread_watch_estop = QtConcurrent::run(&thread_pool, [&]() { thread_watch_estop(); });
	
	future_thread_workflow = QtConcurrent::run(&thread_pool, [&]() { thread_workflow(); });
	future_thread_exchangeTrays = QtConcurrent::run(&thread_pool, [&]() { thread_exchangeTrays(); });
}

// Thread 启动
void Workflow::thread_watch_start()
{
	if (!(init_card() == 1)) return;

	while (close_thread_watch_start == false)
	{
		if (adt8949_read_bit(0, 17) == 1)
		{
			// 开始
			emit changedRundataLabel(QStringLiteral("运行中..."));
			emit changedRundataText(QStringLiteral("开始运行"));
			writRunningLog(QStringLiteral("开始运行"));

			// 开启点胶线程

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
		if (adt8949_read_bit(0, 20) == 1)
		{
			switch (step_reset)
			{
			case 0:	// 复位开始
				{
					// 消息更新
					emit changedRundataLabel(QStringLiteral("复位开始..."));
					emit changedRundataText(QStringLiteral("复位开始"));
					writRunningLog(QStringLiteral("复位开始"));

					step_reset = 10;
					break;
				}

			case 10: // 检测输入
				{
					emit changedRundataLabel(QStringLiteral("检测输入信号"));
					emit changedRundataText(QStringLiteral("检测输入信号"));
					writRunningLog(QStringLiteral("检测输入信号"));
					step_reset = 20;
					break;
				}

			case 20: // 初始化输出
				{
					// 初始化输出

					emit changedRundataLabel(QStringLiteral("初始化输出"));
					emit changedRundataText(QStringLiteral("初始化输出"));
					writRunningLog(QStringLiteral("初始化输出"));
					step_reset = 30;
					break;
				}

			case 30: // 检测输入, 输出
				{
					// 检测输入, 输出
					emit changedRundataLabel(QStringLiteral("检测输入输出"));
					emit changedRundataText(QStringLiteral("检测输入输出"));
					writRunningLog(QStringLiteral("检测输入输出"));
				
					step_reset = 40;
					break;
				}

			case 40: // 工站复位
				{ 
					emit changedRundataLabel(QStringLiteral("工站复位"));
					emit changedRundataText(QStringLiteral("工站复位"));
					writRunningLog(QStringLiteral("工站复位"));

					// 【1】 工站复位
					adt8949_HomeProcess_Ex(0, 3);	// Z轴先复位
					wait_finish(3);

					adt8949_HomeProcess_Ex(0, 1);
					adt8949_HomeProcess_Ex(0, 2);
					wait_finish(1);
					wait_finish(2);

					// 【2】 判断复位状态
					emit changedRundataLabel(QStringLiteral("工站复位完成"));
					emit changedRundataText(QStringLiteral("工站复位完成"));
					writRunningLog(QStringLiteral("工站复位完成"));

					step_reset = 50;
					break;
				}

			case 50: // 复位完成
				{
					emit changedRundataLabel(QStringLiteral("复位完成, 已就绪"));
					emit changedRundataText(QStringLiteral("复位完成"));
					writRunningLog(QStringLiteral("复位完成"));

					break;
				}
			
			default:
				break;
			}

			break; 
		}
	}
}

// Thread 停止
void Workflow::thread_watch_stop()
{
	
}

// Thread 急停
void Workflow::thread_watch_estop()
{
	if (!(init_card() == 1)) return;

	while (close_thread_watch_estop == false)
	{
		if (adt8949_read_bit(0, 16) == 1)
		{
			// 急停
			stop_allaxis();
			estop();
			break;
		}
	}
}


// Thread 工作流程
void Workflow::thread_workflow()
{
	if (!(init_card() == 1)) return;

	int step_workflow = 0;

	while (close_thread_workflow == false)
	{
		switch (step_workflow)
		{
		case 0: // 流程开始
		{
			emit changedRundataText(QStringLiteral("工作流程开始"));
			writRunningLog(QStringLiteral("工作流程开始"));

			step_workflow = 10;
			break;
		}

		case 10: // 
		{
			break;
		}

		case 20:
		{
			break;
		}

		case 30:
		{
			break;
		}

		default:
			break;
		}
	}
	
}


// Thread 换料盘
void Workflow::thread_exchangeTrays()
{
	if (!(init_card() == 1)) return;

	int step_tray;

	while (close_thread_exchangeTrays == false)
	{
		switch (step_tray)
		{
		case 0:		// 等待触发
			{
				if (start_thread_exchangeTrays == false)
				{
					step_tray = 0;
				}
				else
				{
					// 换料盘开始
					emit changedRundataText(QStringLiteral("换料盘开始"));
					writRunningLog(QStringLiteral("换料盘开始"));

					step_tray = 10;
				}
			}
			break;

		case 10:	// 料盘退出
			{
				if (1 == read_in_bit(33))	// 料盘到位感应
				{
					write_out_bit(15, 0);	// 关气缸推出
					Sleep(2000);

					step_tray = 20;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("料盘到位感应异常"));

					step_tray = 9999;
				}
			}
			break;

		case 20:	// 感应料盘是否退出成功
			{
				if (1 == read_in_bit(34))	// 料盘退出到位感应
				{
					step_tray = 30;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("料盘到位感应异常"));

					step_tray = 9999;
				}
			}

		case 30:    // 等待物料取下, 
			{
				if (1 == read_in_bit(35))	// 物料未取下
				{
					step_tray == 30;
				}
				else
				{
					Sleep(2000);	// 防止取下后, 又放回
					step_tray = 40;
				}
				break;
			}

		case 40:	// 等待物料重新装填
			{
				if (1 == read_in_bit(35))
				{
					Sleep(3000);	// 等待手离开物料
					step_tray = 50;
				}
				else
				{
					step_tray = 40;
				}
			}
			break;

		case 50:	// 开气缸推过去
			{
				write_out_bit(15, 1);
				Sleep(1000);

				step_tray = 8888;
			}
			break;

		case 8888:
			{
				emit changedRundataText(QStringLiteral("换料盘结束"));
				writRunningLog(QStringLiteral("换料盘结束"));
				start_thread_exchangeTrays = false;
				step_tray = 0;
			}
			break;

		case 9999:
			{
				// 安全退出该线程
				close_thread_exchangeTrays = true;

				// 触发停止信号
				
			}
			break;

		default:
			break;
		}
	}
}

void Workflow::thread_glue_1()
{

}

void Workflow::thread_glue_2()
{

}

void Workflow::thread_glue_3()
{

}


// 自定义槽, 连接外部信号
void Workflow::on_changedConfigGlue()
{
	QFile file("../config/workflow_glue.ini");
	if (!file.exists()) return;
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		is_config_gluel = setting.value("workflow_glue/is_config_gluel").toBool();
		is_config_glue2 = setting.value("workflow_glue/is_config_glue2").toBool();
		is_config_glue3 = setting.value("workflow_glue/is_config_glue3").toBool();
	}
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