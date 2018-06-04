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

// 点位模型
enum MODELPOINT
{
	GENERAL = 0,
	GLUE1   = 1,
	GLUE2   = 2,
	GLUE3   = 3
};

// 点位类型
enum POINTTYPE
{

};

// Z轴优先优先类型
enum ZMOVETYPE
{
	NORMAL = 0,
	BEFORE = 1,
	AFTER = -1
};

// 点胶工位CCD视野中心
enum CCDORG
{
	GLUE1ORG = 0,
	GLUE2ORG = 1,
	GLUE3ORG = 2
};

class Workflow : public QObject
{
    Q_OBJECT
public:
    explicit Workflow(QObject *parent = nullptr);
	~Workflow();

private:
	void setConfig();
	void setPoint();
	void setIOStatus();
	void setSocketMsg();
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
	QString receivedMsg_ccd;

public:		// 点胶1
	bool is_config_gluel;		// 是否配置  glue1
	bool is_gluel_ok;			
	bool start_thread_glue_1;
	bool close_thread_glue_1;
	QFuture<void> future_thread_glue_1;
	void thread_glue_1();


public:
	// 可通过信号槽来刷新点位, 但不要在运行中设置
	QVector<CCDGlue> vec_ccdGlue_1;
	QVector<CCDGlue> vec_ccdGlue_2;
	QVector<CCDGlue> vec_ccdGlue_3;

	// 可通过信号槽来刷新中心点, 但不要在运行中设置
	float org_ccdglue_x[3];
	float org_ccdglue_y[3];


public:
	bool is_config_glue2;
	bool is_config_glue3;
	void thread_glue_2();
	void thread_glue_3();


public:		// 校针
	float glue_offset_x;
	float glue_offset_y;

	bool is_calibNeedle_ok;
	bool start_thread_calibNeedle;
	bool close_thread_calibNeedle;
	QFuture<void> future_thread_calibNeedle;
	void thread_calibNeedle();

public:		// 点位数据
	QSqlTableModel *model_general;
	QSqlTableModel *model_glue1;
	QSqlTableModel *model_glue2;
	QSqlTableModel *model_glue3;

signals:	// 自定义信号
	void changedRundataLabel(QString str);	// To Operation
	void changedRundataText(QString str);	// To Operation
	void changedOffsetChart(float x, float y, float A);	// To Operation
	
public slots:	// 连接外部信号
	void on_changedConfigGlue(bool glue1, bool glue2, bool glue3);			// From Operation
	void on_changedSqlModel(int index);		// From PointDebug

public:
	// 写Log文件
	void writRunningLog(QString str);
	
	// 获取当前事件
	QString getCurrentTime();


public:		// 获取点位
	QMap<QString, PointGeneral> allpoint_general;
	QMap<QString, PointGlue> allpoint_glue1;
	QMap<QString, PointGlue> allpoint_glue2;
	QMap<QString, PointGlue> allpoint_glue3;
	QMap<QString, PointRun> allpoint_pointRun;
	
	QMap<QString, PointGeneral> getAllGeneralPointInfo();
	QMap<QString, PointGlue> getAllGluePointInfo(int index);
	QMap<QString, PointRun> getAllRunPointInfo();
	
	QVector<CCDGlue> getCCDGluePoint2Vector(int index);

	MatrixXf CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y);
	MatrixXf CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y);

	float wSpeed;
	float wAcc;
	float wDec;

	void set_speed(float speed, float acc, float dec);
	bool move_point_name(QString pointname, int type, int z_flag);
	bool move_point_name(QString pointname, int z_flag = 0);		// 移动到点, by point

};

#endif // WORKFLOW_H