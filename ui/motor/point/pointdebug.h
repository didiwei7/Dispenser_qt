#ifndef POINTDEBUG_H
#define POINTDEBUG_H

#include <QWidget>
#include <QtWidgets>
#include <QtConcurrent>
#include <QtSql>
#include <QMap>

// 控制卡
#include "adt8949.h"
#include "../../adt/adtcontrol.h"

// 载入动态库
#include "../../../sdk/QMyPlugin/qmyplugin.h"
class QMySlider;
class QInput;
class QHNavigationBar;

#define MOTOR_MAX_AXIS 3

// 运动模式 连续,长,中,短距离步进
enum EM_STATION_MVTYPE
{
	EMOVE_CP = 0,	        //连续运动
	EMOVE_LONG_STEP = 1,    //长距离
	EMOVE_MEDIU_STEP = 2,   //中等距离
	EMOVE_SHORT_STEP = 3    //短距离
};

class PointDebug : public QWidget
{
    Q_OBJECT
public:
    explicit PointDebug(QWidget *parent = nullptr);
	~PointDebug();

private:
    void setupUi();
    void setConnect();
	void setTimer();
	void setCommunication();

private:	// ui初始化
	void setGroupPoint();
	void setViewPoint();
	void setActions();

	void setGroupMove();
	void setGroupIO();
	void setGroupHome();
	void setGroupCurrentpos();
	void setGroupStep();
	void setGroupCalib();
	void setGroupLaser();

	QGroupBox *group_move;
	QGroupBox *group_home;

	QGroupBox *group_pos;
    QGroupBox *group_currentpos;
    QGroupBox *group_step;
	QGroupBox *group_io;
	QGroupBox *group_laser;
	QGroupBox *group_calib;

private:	// 点位
	void setCurrentModel(int index);
	QSqlTableModel *getCurrentModel();

	void on_pointview_rightClicked(const QPoint &);
	void on_action_go();
	void on_action_go_location();
	void on_action_go_laser();
	void on_action_teach();
	void on_action_add();
	void on_action_insert();
	void on_action_del();
	void on_action_save();

	QWidget         *w_pointview;
	QHNavigationBar *hnavigationbar;
	QTableView      *pointview;
	QSqlTableModel *model_general;
	QSqlTableModel *model_glue1;
	QSqlTableModel *model_glue2;
	QSqlTableModel *model_glue3;

	QList<QAction *> list_action;
	QAction *action_go;
	QAction *action_go_location;
	QAction *action_go_laser;
	QAction *action_teach;
	QAction *action_sepa;
	QAction *action_add;
	QAction *action_insert;
	QAction *action_del;
	QAction *action_save;

	// 当前数据库模型的索引, 0:通用点位, 1:工位1, 2:工位2, 3:工位3
	int index_model;

	float distance_ccd_needle_x;
	float distance_ccd_neddle_y;

	float distance_ccd_laser_x;
	float diatance_ccd_laser_y;

private:	// X+- Y+- Z+-
    void on_X_positive_clicked();
    void on_X_positive_pressed();
    void on_X_positive_released();
    void on_X_negative_clicked();
    void on_X_negative_pressed();
    void on_X_negative_released();

    void on_Y_positive_clicked();
    void on_Y_positive_pressed();
    void on_Y_positive_released();
    void on_Y_negative_clicked();
    void on_Y_negative_pressed();
    void on_Y_negative_released();

    void on_Z_positive_clicked();
    void on_Z_positive_pressed();
    void on_Z_positive_released();
    void on_Z_negative_clicked();
    void on_Z_negative_pressed();
    void on_Z_negative_released();

	void on_A_positive_clicked();
	void on_A_positive_pressed();
	void on_A_positive_released();
	void on_A_negative_clicked();
	void on_A_negative_pressed();
	void on_A_negative_released();

    QPushButton *X_positive;
    QPushButton *X_negative;
    QPushButton *Y_positive;
    QPushButton *Y_negative;
    QPushButton *Z_positive;
    QPushButton *Z_negative;
	QPushButton	*A_positive;
	QPushButton *A_negative;
	
private:	// 速度
	void on_slider_speed_Changed(int pos);

	float debug_startv;
	float debug_speed;
	float debug_acc;
	float debug_admode;

	QMySlider *slider_speed;

private:	// 回原
	void on_btn_stop();
	void on_btn_station_home();
	void on_btn_x_home();
	void on_btn_y_home();
	void on_btn_z_home();

	QPushButton *btn_stop;
	QPushButton *btn_station_home;
	QPushButton *btn_x_home;
	QPushButton *btn_y_home;
	QPushButton *btn_z_home;

private:	// 移动步进
	void setMoveType(int moveType);

	void on_radio_continue();
	void on_radio_long();
	void on_radio_middle();
	void on_radio_short();

    QRadioButton *radio_continue;
    QRadioButton *radio_long;
    QRadioButton *radio_middle;
    QRadioButton *radio_short;

    QLineEdit *edit_X_step;
    QLineEdit *edit_Y_step;
    QLineEdit *edit_Z_step;
	QLineEdit *edit_A_step;

private:	// Laser测量
	void timer_updateLaser();
	void on_btn_laser();

	bool isLaserOpen;
	QLabel *label_laser;
	QPushButton *btn_laser;
	QTimer *timer_laser;
	QSerialPort *serial_laser;

private:	// 手动标定
	void on_btn_calib_getccd();
	void on_btn_calib_getneedle();
	void on_btn_calibOk();

	QMyEdit *edit_calib_ccd_x;
	QMyEdit *edit_calib_ccd_y;

	QMyEdit *edit_calib_x;
	QMyEdit *edit_calib_y;
	QMyEdit *edit_calib_z;

	QPushButton *btn_calib_getccd;
	QPushButton *btn_calib_getneedle;
	QPushButton *btn_calibOk;

private:	// 更新数据
	void timer_updateCurrentPos();
	void timer_updateInputStatus();

	QLabel *label_X_currentpos;
	QLabel *label_Y_currentpos;
	QLabel *label_Z_currentpos;

	QInput *INPUT_X[4];
	QInput *INPUT_Y[4];
	QInput *INPUT_Z[4];
	QInput *INPUT_A[4];

signals:	// 自定义信号槽
	void changedSqlModel();	// 连接 Workflow, 应用当前修改

	void changedDistanceOffset();

public slots:
	void on_wchangedSqlModel();
};

#endif // POINTDEBUG_H
