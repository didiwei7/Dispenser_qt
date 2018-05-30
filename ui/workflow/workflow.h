#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QObject>
// 基础库
#include <QtWidgets>
#include <Windows.h>
#include <QDebug>

// 线程库
#include <QtConcurrent>

// 通讯库
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <QtSerialPort/qtserialportversion.h>
#include <QtNetwork>

// 绘图库
#include <QtCharts>

// 数学库
#include <QtMath>

// 运动控制
#include "../adt/adtcontrol.h"
#include "../io/io.h"

class Workflow : public QObject
{
    Q_OBJECT
public:
    explicit Workflow(QObject *parent = nullptr);

private:
	void setConfig();
	void setIOStatus();
	void setThread();
	void setConnect();

public:
	QThreadPool thread_pool;
	
public:		// 复位
	bool is_reset_ok;
	bool start_thread_watch_reset;
	bool close_thread_watch_reset;
	QFuture<void> future_thread_watch_reset;
	void thread_watch_reset();
	
public:		// 开始
	bool is_start_ok;
	bool start_thread_watch_start;
	bool close_thread_watch_start;
	QFuture<void> future_thread_watch_start;	
	void thread_watch_start();

public:		// 停止
	bool is_stop_ok;
	bool start_thread_watch_stop;
	bool close_thread_watch_stop;
	QFuture<void> future_thread_watch_stop;
	void thread_watch_stop();

public:		// 急停
	bool is_estop_ok;
	bool start_thread_watch_estop;
	bool close_thread_watch_estop;
	QFuture<void> future_thread_watch_estop;	
	void thread_watch_estop();

public:		// 工作流程
	bool is_workflow_ok;
	bool start_thread_workflow;
	bool close_thread_workflow;
	QFuture<void> future_thread_workflow;		
	void thread_workflow();

public:		// 换料盘
	bool is_exchangeTrays_ok;
	bool start_thread_exchangeTrays;
	bool close_thread_exchangeTrays;
	QFuture<void> future_thread_exchangeTrays;
	void thread_exchangeTrays();


public:
	bool is_config_gluel;
	bool is_config_glue2;
	bool is_config_glue3;
	void thread_glue_1();
	void thread_glue_2();
	void thread_glue_3();

public:


signals:	// 自定义信号
	void changedRundataLabel(QString str);
	void changedRundataText(QString str);
	void changedOffsetChart(float x, float y, float A);
	
public slots:	// 连接外部信号
	void on_changedConfigGlue();


public:		
	void    writRunningLog(QString str);
	QString getCurrentTime();
};

#endif // WORKFLOW_H