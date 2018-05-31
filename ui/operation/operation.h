#ifndef OPERATION_H
#define OPERATION_H

// 基础库
#include <QWidget>
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

// 运动控制卡
#include "../adt/adtcontrol.h"
#include "../io/io.h"
#include "../motor/point/pointdebug.h"

class Operation : public QWidget
{
    Q_OBJECT
public:
    explicit Operation(QWidget *parent = nullptr);

private:
    void setupUi();
	void setConnect();
	void setThread();

public:
	void setGroupWarn();
	void setGroupGlue();
	void setGroupDebug();
	void setGroupChart();
	void setGroupConfig();

	QGroupBox *group_warn;
	QGroupBox *group_glue;
	QGroupBox *group_debug;
	QGroupBox *group_chart;
	QGroupBox *group_config;

public slots:
	void on_changedRundataLabel(QString str);
	void on_changedRundataText(QString str);
	void on_changedOffsetChart(float x, float y, float A);

public:
    void setChart_offset();
	int index_offset;
	QChartView  *chartView_offset;
	QChart		*chart_offset;
	QLineSeries *lseries_offset_x;
	QLineSeries *lseries_offset_y;
	QLineSeries *lseries_offset_A;

	void setChart_pass();
	int index_pass;
	QChartView  *chartView_quality;
	QChart		*chart_quality;
	QPieSeries  *pseries_quality;
	QPieSlice   *slice_pass;
	QPieSlice   *slice_fail;

    QChart *setChart_2();
    QChart *setChart_3();
    QChart *setChart_4();

    QLineSeries *lseries_1;
    QLineSeries *lseries_2;
    QLineSeries *lseries_3_1;
    QLineSeries *lseries_3_2;
    QPieSeries  *pseries_4;

    
    QChart *chart_2;
    QChart *chart_3;
    QChart *chart_4;

public:
	QLabel       *label_rundata;
	QTextBrowser *text_rundata;

public:
	void on_btn_ccd_calibration();
	void on_btn_ccd_runEmpty();
	void on_btn_ccd_laser();
	void on_btn_pin_calibration();
	void on_btn_runEmpty();

	void on_btn_glueAxisHome();
	void on_btn_glueClear();
	void on_btn_glueAutoClear();

	QPushButton *btn_ccd_calibration;
	QPushButton *btn_ccd_laser;
	QPushButton *btn_ccd_runEmpty;
	QPushButton *btn_pin_calibration;
	
	QPushButton *btn_glueAxisHome;
	QPushButton *btn_glueClear;
	QPushButton *btn_glueAutoClear;
	QPushButton *btn_runEmpty;

public:
	void       on_check_glue();
	QCheckBox *check_glue1;
	QCheckBox *check_glue2;
	QCheckBox *check_glue3;

signals:
	void changedConfigGlue(bool glue1, bool glue2, bool glue3);

public:
	// 获取当前时间
	QString getCurrentTime();
};

#endif // OPERATION_H
