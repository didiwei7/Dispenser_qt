#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QtWidgets>

// 载入动态库
#include "../../sdk/QMyPlugin/qmyplugin.h"
class QVNavigationBar;

// 载入Ui
#include "../operation/operation.h"
#include "../motor/motor.h"
#include "../io/io.h"
#include "../communication/communication.h"
#include "../workflow/workflow.h"
class Workflow;
class Operation;
class Motor;
class IO;
class Communication;


class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

private:
    void setupUi();
    void setConnect();

private:
    QVNavigationBar *vnavigationbar;
    QStackedWidget  *stackedWidget;

public:
    Operation     *w_operation;
    Motor         *w_motor;
    IO            *w_io;
	Communication *w_com;
	Workflow      *w_workflow;
};

#endif // MAINWIDGET_H
