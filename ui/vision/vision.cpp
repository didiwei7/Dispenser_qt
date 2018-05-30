#include "vision.h"

VisionConfig::VisionConfig(QWidget *parent) : QWidget(parent)
{

}



VisionShow::VisionShow(QWidget *parent) : QWidget(parent)
{
    label_show = new QLabel(this);
    label_show->setMinimumSize(200, 400);
    // label_show->setFixedHeight(200);
    label_show->setStyleSheet(QStringLiteral("background-color: #000000;"));

    label_show_2 = new QLabel(this);
    label_show_2->setMinimumSize(200, 400);
    // label_show->setFixedHeight(200);
    label_show_2->setStyleSheet(QStringLiteral("background-color: #000000;"));


    QVBoxLayout *layout_1 = new QVBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addWidget(label_show);
    layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout_1->addWidget(label_show_2);

    this->setLayout(layout_1);
}



Vision::Vision(QWidget *parent) : QWidget(parent)
{
    // 【1】 导航栏
    vnavigationbar = new QVNavigationBar(this);
    vnavigationbar->setSelectColor("#FFC0CB");
    vnavigationbar->setRowHeight(30);
    vnavigationbar->setWidth(80);
    vnavigationbar->addItem(QStringLiteral("视觉显示"));
    vnavigationbar->addItem(QStringLiteral("视觉配置"));

    // 【2】 stackedWidget
    stackedWidget = new QStackedWidget(this);
    stackedWidget->insertWidget(0, new VisionShow(this));
    stackedWidget->insertWidget(1, new VisionConfig(this));

    // 【3】 布局
    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addWidget(stackedWidget);
    layout_1->addWidget(vnavigationbar);
    this->setLayout(layout_1);

    // 【4】 连接信号槽
    connect(vnavigationbar, &QVNavigationBar::currentItemChanged,
            stackedWidget,  &QStackedWidget::setCurrentIndex);
}
