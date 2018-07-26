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

private:
	QThreadPool thread_pool;
	QMutex mutex_serial;
	
private:	// 急停
	bool is_estop;
	bool close_thread_watch_estop;
	QFuture<void> future_thread_watch_estop;
	void thread_watch_estop();

private:	// 复位
	bool is_reset_ok;
	bool close_thread_watch_reset;
	QFuture<void> future_thread_watch_reset;
	void thread_watch_reset();

private:	// 监视点胶
	bool is_glue_ing;
	bool close_thread_watch_glue;
	QFuture<void> future_thread_watch_glue;
	void thread_watch_glue();

private:	// 监视擦胶
	bool is_clearNeedle_ing;
	bool close_thread_watch_clearNeedle;
	QFuture<void> future_thread_watch_clearNeedle;
	void thread_watch_clearNeedle();

private:	// 监视排胶
	bool is_dischargeGlue_ing;
	bool close_thread_watch_dischargeGlue;
	QFuture<void> future_thread_watch_dischargeGlue;
	void thread_watch_dischargeGlue();
	
private:	// 开始
	bool is_start_ok;
	bool close_thread_watch_start;
	QFuture<void> future_thread_watch_start;	
	void thread_watch_start();


private:	// 点胶1,2,3
	bool is_config_gluel;		
	bool is_config_glue2;
	bool is_config_glue3;

private:	// 点胶泰达
	bool is_config_glue_teda = true;
	bool is_glue_teda_ok;
	bool start_thread_glue_teda;
	bool close_thread_glue_teda;
	QFuture<void> future_thread_glue_teda;
	void thread_glue_teda();

private:	// 点胶泰达测试
	QFuture<void> future_thread_glue_teda_test;
	void thread_glue_teda_test();

private:	// BTN CCD标定
	bool is_thread_btn_ccd_calib_ing;
	bool close_thread_btn_ccd_calib;
	QFuture<void> future_thread_btn_ccd_calib;
	void thread_btn_ccd_calib();

private:	// BTN CCD空跑
	bool is_thread_btn_ccd_runEmpty_ing;
	bool close_thread_btn_ccd_runEmpty;
	QFuture<void> future_thread_btn_ccd_runEmpty;
	void thread_btn_ccd_runEmpty();

private:	// BTN 空跑不点胶
	bool is_thread_btn_runEmpty_ing;
	bool close_thread_btn_runEmpty;
	QFuture<void> future_thread_btn_runEmpty;
	void thread_btn_runEmpty();

private:	// BTN 针头清理
	bool is_thread_btn_clearNeedle_ing;
	bool close_thread_btn_clearNeedle;
	QFuture<void> future_thread_btn_clearNeedle;
	void thread_btn_clearNeedle();

private:	// BTN 自动排胶
	bool is_thread_btn_dischargeGlue_ing;
	bool close_thread_btn_dischargeGlue;
	QFuture<void> future_thread_btn_dischargeGlue;
	void thread_btn_dischargeGlue();

private:	// BTN 针头校准1
	bool is_btn_needleCalib1_ing;
	bool close_thread_needleCalib1;
	QFuture<void> future_thread_needleCalib_1;
	void thread_needleCalib_1();

private:	// BTN 针头校准2
	bool is_btn_needleCalib2_ing;
	bool close_thread_needleCalib2;
	QFuture<void> future_thread_needleCalib_2;
	void thread_needleCalib_2();

private:	// 通讯
	QTcpSocket  *socket_ccd;
	QString receivedMsg_ccd;
	void socket_ccd_receive();

	QSerialPort *serial_laser;
	QString receivedMsg_laser;
	bool close_thread_serialLaserReceive;
	void thread_serialLaserReceive();


private:	// 点位
	QSqlTableModel *model_general;	// 点位模型
	QSqlTableModel *model_glue1;
	QSqlTableModel *model_glue2;
	QSqlTableModel *model_glue3;

	// 所有点位 可通过信号槽来刷新点位, 但不要在运行中设置
	QMap<QString, PointRun>  allpoint_pointRun;
	QMap<QString, PointRun>  getAllRunPointInfo();
	
	// 工站点位 可通过信号槽来刷新点位, 但不要在运行中设置
	QVector<CCDGlue> vec_ccdGlue_1;
	QVector<CCDGlue> vec_ccdGlue_2;
	QVector<CCDGlue> vec_ccdGlue_3;
	QVector<CCDGlue> getCCDGluePoint2Vector(int index);

	// 计算平移矩阵
	MatrixXf CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y);
	// 计算平移旋转矩阵
	MatrixXf CalCCDGluePoint(const QVector<CCDGlue> vector_ccdGlue, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y);
	// 计算平移旋转点位
	void CalCCDGlueCenterPoint(float center_pos[2], const float center_x, const float center_y, const float offset_x, const float offset_y, const float offset_angle, const float org_x, const float org_y);

private:	// 偏移
	float distance_ccd_needle_x;
	float distance_ccd_neddle_y;

	float distance_ccd_laser_x;
	float diatance_ccd_laser_y;

	float distance_laser_needle_x;
	float distance_laser_needle_y;
	float distance_laser_needle_z;

	float distance_needle_z;

	// 可通过信号槽来刷新偏移量, 但不要在运行中设置
	float calib_offset_x;
	float calib_offset_y;
	float calib_offset_z;

	// 可通过信号槽来刷新中心点, 但不要在运行中设置
	float org_ccdglue_x[3];
	float org_ccdglue_y[3];


private:
	// 移动到点, by point
	bool move_point_name(QString pointname, int z_flag = 0);


private:
	void writRunningLog(QString str);	
	QString getCurrentTime();

	bool getLaserData(float laser, int limit_ms);
	bool getVisionData(float offset_x, float offset_y, float offset_z, int limit_ms);

public slots:	// 自定义槽 来自 Operation
	void on_clicked_check_flowConfig();
	void on_clicked_btn_saveDistanceOffset();

	void on_clicked_btn_ccd_calib();
	void on_clicked_btn_ccd_runEmpty();
	void on_clicked_btn_runEmpty();
	void on_clicked_btn_clearNeedle();
	void on_clicked_btn_dischargeGlue();
	void on_clicked_btn_needleCalib_1();
	void on_clicked_btn_needleCalib_2();

public slots:	// 自定义槽 来自 PointDebug
	void on_changedSqlModel(int index);

signals:		// 自定义信号 连接 Operation
	void changedRundataLabel(QString str);	
	void changedRundataText(QString str);	
	void changedDistanceOffset();
	void changedOffsetChart(float x, float y, float A);	

	void changedDischargeGlue();
};

#endif // WORKFLOW_H
