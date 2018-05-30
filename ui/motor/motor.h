#ifndef MOTOR_H
#define MOTOR_H

#include <QWidget>
#include <QtWidgets>

// 载入动态库
#include "../../sdk/QNavigationBar/qnavigationbar.h"
class QVNavigationBar;

#include "point/pointdebug.h"

class Motor : public QWidget
{
    Q_OBJECT
public:
    explicit Motor(QWidget *parent = nullptr);

private:
    QStackedWidget  *stackedWidget;
    QVNavigationBar *vnavigationbar;

public:
    PointDebug *w_pointDebug;
};

#endif // MOTOR_H
