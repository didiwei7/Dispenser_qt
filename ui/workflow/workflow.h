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
#include <math.h>

// 数据库
#include <QtSql>

// 运动控制
#include "../adt/adtcontrol.h"

// 点位模型
enum MODELPOINT
{
	GENERAL = 0,
	GLUE1   = 1,
	GLUE2   = 2,
	GLUE3   = 3
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
	void setThread();
	void setConnect();
	void setCommunication();

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

public:		// 通讯
	QTcpSocket  *socket_ccd;
	QString receivedMsg_ccd;
	void socket_ccd_receive();


	QSerialPort *serial_laser;
	QString receivedMsg_laser;

	bool close_thread_serialLaserReceive;
	void thread_serialLaserReceive();

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

public:		// 点胶1
	bool is_config_gluel;		// 是否配置  glue1
	bool is_gluel_ok;			
	bool start_thread_glue_1;
	bool close_thread_glue_1;
	QFuture<void> future_thread_glue_1;
	void thread_glue_1();

public:		// 点胶2
	bool is_config_glue2;		// 是否配置  glue1
	bool is_glue2_ok;
	bool start_thread_glue_2;
	bool close_thread_glue_2;
	QFuture<void> future_thread_glue_2;
	void thread_glue_2();

public:		// 点胶3
	bool is_config_glue3;		// 是否配置  glue1
	bool is_glue3_ok;
	bool start_thread_glue_3;
	bool close_thread_glue_3;
	QFuture<void> future_thread_glue_3;
	void thread_glue_3();

public:		// 点胶泰达
	bool is_config_glue_teda = true;
	bool is_glue_teda_ok;
	bool start_thread_glue_teda;
	bool close_thread_glue_teda;
	QFuture<void> future_thread_glue_teda;
	void thread_glue_teda();

public:		// 点胶泰达
	QFuture<void> future_thread_glue_teda_test;
	void thread_glue_teda_test();

public:		// 空跑点胶轨迹
	bool is_ccdGlue3_ok;
	bool start_thread_ccd_glue_1;
	bool close_thread_ccd_glue_1;
	QFuture<void> future_ccd_glue_1;
	void thread_ccd_glue_1();

public:
	// 可通过信号槽来刷新点位, 但不要在运行中设置
	QVector<CCDGlue> vec_ccdGlue_1;
	QVector<CCDGlue> vec_ccdGlue_2;
	QVector<CCDGlue> vec_ccdGlue_3;

	// 可通过信号槽来刷新中心点, 但不要在运行中设置
	float org_ccdglue_x[3];
	float org_ccdglue_y[3];

	// 可通过信号槽来刷新偏移量, 但不要在运行中设置
	float calib_offset_x;
	float calib_offset_y;
	float calib_offset_z;

public:
	float distance_ccd_needle_x;
	float distance_ccd_neddle_y;

	float distance_ccd_laser_x;
	float diatance_ccd_laser_y;

	float distance_laser_needle_x;
	float distance_laser_needle_y;
	float distance_laser_needle_z;

public:		// 校针
	bool is_calibNeedle_ok;
	bool start_thread_calibNeedle;
	bool close_thread_calibNeedle;
	QFuture<void> future_thread_calibNeedle;
	void thread_calibNeedle();

public:		// 清胶
	bool is_clearNeedle_ok;
	bool start_thread_clearNeedle;
	bool close_thread_clearNeedle;
	QFuture<void> future_thread_clearNeedle;
	void thread_clearNeedle();


public:		// 点位数据
	QSqlTableModel *model_general;
	QSqlTableModel *model_glue1;
	QSqlTableModel *model_glue2;
	QSqlTableModel *model_glue3;

signals:	// 自定义信号
	// To Operation
	void changedRundataLabel(QString str);	
	void changedRundataText(QString str);	
	void changedOffsetChart(float x, float y, float A);	
	void changedOffset(float offset_x, float offset_y, float offset_z);
	
public slots:	// 连接外部信号
	// 来自 Operation
	void on_changedConfigGlue(bool glue1, bool glue2, bool glue3);			
	void on_changedConfigGlueOffset(float offset_x, float offset_y, float offset_z);
	
	// 来自 PointDebug
	void on_changedSqlModel(int index);		


public:
	// 写Log文件
	void writRunningLog(QString str);
	QString getCurrentTime();


public:		// 获取点位
	QMap<QString, PointGeneral> allpoint_general;
	QMap<QString, PointGlue> allpoint_glue1;
	QMap<QString, PointGlue> allpoint_glue2;
	QMap<QString, PointGlue> allpoint_glue3;
	QMap<QString, PointRun>  allpoint_pointRun;
	
	QMap<QString, PointGeneral> getAllGeneralPointInfo();
	QMap<QString, PointGlue> getAllGluePointInfo(int index);
	QMap<QString, PointRun>  getAllRunPointInfo();
	
	QVector<CCDGlue> getCCDGluePoint2Vector(int index);

	MatrixXf CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y);
	MatrixXf CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y);

	void CalCCDGlueCenterPoint(float center_pos[2], const float center_x, const float center_y, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y);

	float wSpeed;
	float wAcc;
	float wDec;

	void set_speed(float speed, float acc, float admode);
	void set_speed(float speed, float acc);

	bool move_point_name(QString pointname, int type, int z_flag);
	bool move_point_name(QString pointname, int z_flag = 0);		// 移动到点, by point

};

#endif // WORKFLOW_H