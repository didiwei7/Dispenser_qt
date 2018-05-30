#include "motor.h"

Motor::Motor(QWidget *parent) : QWidget(parent)
{
    vnavigationbar = new QVNavigationBar();
    vnavigationbar->setSelectColor("#FFC0CB");
    vnavigationbar->setRowHeight(30);
    vnavigationbar->setWidth(120);
    vnavigationbar->addItem(QStringLiteral("点位调试"));
    vnavigationbar->addItem(QStringLiteral("电机设置"));

    w_pointDebug = new PointDebug(this);
    stackedWidget   = new QStackedWidget();
    stackedWidget->insertWidget(0, w_pointDebug);
    stackedWidget->insertWidget(1, new QLabel(QStringLiteral("电机设置")));

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addWidget(vnavigationbar);
    layout_1->addWidget(stackedWidget);
    setLayout(layout_1);

    connect(vnavigationbar, &QVNavigationBar::currentItemChanged,
            stackedWidget,  &QStackedWidget::setCurrentIndex);
}
