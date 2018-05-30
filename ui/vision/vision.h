#ifndef VISION_H
#define VISION_H

#include <QWidget>
#include <QtWidgets>

// 载入动态库
#include "../../sdk/QNavigationBar/qnavigationbar.h"
class QVNavigationBar;

class VisionConfig : public QWidget
{
    Q_OBJECT
public:
    explicit VisionConfig(QWidget *parent = nullptr);
};


class VisionShow : public QWidget
{
    Q_OBJECT
public:
    explicit VisionShow(QWidget *parent = nullptr);

private:
    QLabel *label_show;
    QLabel *label_show_2;

};


class Vision : public QWidget
{
    Q_OBJECT
public:
    explicit Vision(QWidget *parent = nullptr);

private:
    QVNavigationBar *vnavigationbar;
    QStackedWidget  *stackedWidget;
};






#endif // VISION_H
