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
	void setTimer();

private:
	void setGroupLogo();
	void setGroupWarn();
	void setGroupGlue();
	void setGroupDebug();
	void setGroupChart();
	void setGroupConfig();
	void setGroupDistanceOffset();

	QGroupBox *group_logo;
	QGroupBox *group_warn;
	QGroupBox *group_glue;
	QGroupBox *group_debug;
	QGroupBox *group_chart;
	QGroupBox *group_config;
	QGroupBox *group_distance_offset;


private:
    void setChart_offset();
	int  index_offset;
	QChartView  *chartView_offset;
	QChart		*chart_offset;
	QLineSeries *lseries_offset_x;
	QLineSeries *lseries_offset_y;
	QLineSeries *lseries_offset_A;

	void setChart_pass();
	int  index_pass;
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

private:
	QLabel       *label_rundata;
	QTextBrowser *text_rundata;

private:
	void on_btn_station_home();
	void on_btn_ccd_calib();
	void on_btn_ccd_runEmpty();
	void on_btn_runEmpty();

	void on_btn_clearNeedle();
	void on_btn_dischargeGlue();
	void on_btn_needleCalib_1();
	void on_btn_needleCalib_2();


	void on_timer_dischargeGlue();
	QTimer *timer_dischargeGlue;
	bool   is_dischargeGlue_ing;

	QPushButton *btn_station_home;
	QPushButton *btn_ccd_calib;
	QPushButton *btn_ccd_runEmpty;
	QPushButton *btn_runEmpty;
	
	QPushButton *btn_clearNeedle;
	QPushButton *btn_dischargeGlue;
	QPushButton *btn_needleCalib_1;
	QPushButton *btn_needleCalib_2;

private:
	void       on_check_flowConfig();
	QCheckBox *check_glue1;
	QCheckBox *check_glue2;
	QCheckBox *check_glue3;

private:
	void on_btn_saveDistanceOffset();

	QLineEdit *edit_distance_ccd_needle_x;
	QLineEdit *edit_distance_ccd_needle_y;

	QLineEdit *edit_distance_ccd_laser_x;
	QLineEdit *edit_distance_ccd_laser_y;

	QLineEdit *edit_distance_laser_needle_x;
	QLineEdit *edit_distance_laser_needle_y;
	QLineEdit *edit_distance_laser_needle_z;

	QLineEdit *edit_offset_ccd_needle_x;
	QLineEdit *edit_offset_ccd_needle_y;
	QLineEdit *edit_offset_laser_needle_z;

	QLineEdit *edit_distance_needle_z;
	
	QPushButton *btn_saveDistanceOffset;

signals:
	void clicked_check_flowConfig();
	void clicked_btn_saveDistanceOffset();

	void clicked_btn_ccd_calib();
	void clicked_btn_ccd_runEmpty();
	void clicked_btn_runEmpty();
	void clicked_btn_clearNeedle();
	void clicked_btn_dischargeGlue();
	void clicked_btn_needleCalib_1();
	void clicked_btn_needleCalib_2();

public slots:
	void on_changedRundataLabel(QString str);
	void on_changedRundataText(QString str);
	void on_changedDistanceOffset();
	void on_changedOffsetChart(float x, float y, float A);
	void on_changedDischargeGlue();
	
private:
	// 获取当前时间
	QString getCurrentTime();
};

#endif // OPERATION_H
