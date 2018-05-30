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

// 数据库
#include <QtSql>

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


public:		// 配置数据
	bool is_config_gluel;
	bool is_config_glue2;
	bool is_config_glue3;
	void thread_glue_1();
	void thread_glue_2();
	void thread_glue_3();

public:		// 点位数据
	QSqlTableModel *model_main;
	QSqlTableModel *model_glue1;
	QSqlTableModel *model_glue2;
	QSqlTableModel *model_glue3;

signals:	// 自定义信号
	void changedRundataLabel(QString str);	// To Operation
	void changedRundataText(QString str);	// To Operation
	void changedOffsetChart(float x, float y, float A);	// To Operation
	
public slots:	// 连接外部信号
	void on_changedConfigGlue();			// From Operation
	void on_changedSqlModel(int index);		// From PointDebug

public:
	// 写Log文件
	void writRunningLog(QString str);
	
	// 获取当前事件
	QString getCurrentTime();

public:		// 获取点位
	int index_model;
	QSqlTableModel *getCurrentModel();
	QMap<QString, PointGlue> getPointInfo();	// 通过结构体存储

};

#endif // WORKFLOW_H