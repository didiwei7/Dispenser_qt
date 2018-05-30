#ifndef POINTDEBUG_H
#define POINTDEBUG_H

#include <QWidget>
#include <QtWidgets>
#include <QtConcurrent>
#include <QtSql>
#include <QMap>

#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

// 控制卡
#include "adt8949.h"
#include "../../adt/adtcontrol.h"

// 载入动态库
#include "../../../sdk/QIO/qio.h"
class QInput;

#define MOTOR_MAX_AXIS 3

// 运动模式 连续,长,中,短距离步进
enum EM_STATION_MVTYPE
{
	EMOVE_CP = 0,	        //连续运动
	EMOVE_LONG_STEP = 1,    //长距离
	EMOVE_MEDIU_STEP = 2,   //中等距离
	EMOVE_SHORT_STEP = 3,   //短距离
};

class PointDebug : public QWidget
{
    Q_OBJECT
public:
    explicit PointDebug(QWidget *parent = nullptr);

private:
    void setupUi();
    void setConnect();
	void setThread();

public:
	QThreadPool thread_pool;
	QFuture<void> future_thread_updateCurrentPos;
	QFuture<void> future_thread_updateInputStatus;
	bool exit_thread_updateCurrentPos;
	bool exit_thread_updateInputStatus;

    void thread_updateCurrentPos();
	void thread_updateInputStatus();

private:
	void setViewPoint();
	void setActions();

	void setGroupMove();
	void setGroupIO();
	void setGroupHome();

	void setGroupPos();
	void setGroupCurrentpos();
	void setGroupStep();

	QGroupBox *group_move;
	QGroupBox *group_io;
	QGroupBox *group_home;

	QGroupBox *group_pos;
    QGroupBox *group_currentpos;
    QGroupBox *group_step;

private:
	void on_pointview_rightClicked(const QPoint &);
	
	void on_action_go();
	void on_action_teach();
	
	void on_action_add();
	void on_action_insert();
	void on_action_del();
	void on_action_save();

	QTableView     *pointview;
	QSqlTableModel *pointmodel;
	QSqlTableModel *model_glue1;
	QSqlTableModel *point_glue2;
	QSqlTableModel *point_glue3;

	QList<QAction *> list_action;
	QAction *action_go;
	QAction *action_teach;
	QAction *action_sepa;
	QAction *action_add;
	QAction *action_insert;
	QAction *action_del;
	QAction *action_save;

private:
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

    void on_slider_speed_Changed(int pos);

    QPushButton *X_positive;
    QPushButton *X_negative;
    QPushButton *Y_positive;
    QPushButton *Y_negative;
    QPushButton *Z_positive;
    QPushButton *Z_negative;
    QSlider     *slider_speed;

private:
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

private:
	float speed;
	float acc;
	float dec;

	void on_radio_continue();
	void on_radio_long();
	void on_radio_middle();
	void on_radio_short();

	QLabel *label_X_currentpos;
	QLabel *label_Y_currentpos;
	QLabel *label_Z_currentpos;

    QRadioButton *radio_continue;
    QRadioButton *radio_long;
    QRadioButton *radio_middle;
    QRadioButton *radio_short;

    QLineEdit *edit_X_step;
    QLineEdit *edit_Y_step;
    QLineEdit *edit_Z_step;

public:
	QInput INPUT_X[4];
	QInput INPUT_Y[4];
	QInput INPUT_Z[4];
	QInput INPUT_A[4];

public:
    // 设置运动模式
    void setMoveType(int moveType);

public:
	QMap<QString, PointGlue> allPoint;
	QMap<QString, PointGlue> getPointInfo();		 // 获取所有点位信息
	PointGlue get_point_name(QString pointName);	 // 通过点位名查找
	PointGlue get_point_index(int index);			 // 通过Index在数据中查找

	
	// QMap<QString, QString> getPointByname(QString pointName);
	/*QMap<QString, QString> PointDebug::getPointByname(QString pointName)
	{
	int index = 0;
	for (int index = 0; index < pointmodel->rowCount(); index++)
	{
	if (pointName == pointmodel->record(index).value("name").toString())
	{
	break;
	}
	}
	qDebug() << index;

	QMap<QString, QString> map;
	map.insert("X", pointmodel->record(index).value("X").toString());
	map.insert("Y", pointmodel->record(index).value("X").toString());
	map.insert("Z", pointmodel->record(index).value("X").toString());
	map.insert("open", pointmodel->record(index).value("open").toString());
	map.insert("openAdvance", pointmodel->record(index).value("openAdvance").toString());
	map.insert("openDelay", pointmodel->record(index).value("openDelay").toString());
	map.insert("close", pointmodel->record(index).value("close").toString());
	map.insert("closeAdvance", pointmodel->record(index).value("closeAdvance").toString());
	map.insert("closeDelay", pointmodel->record(index).value("closeDelay").toString());
	map.insert("type", pointmodel->record(index).value("type").toString());

	qDebug() << map;

	return map;
	}*/
};

#endif // POINTDEBUG_H
