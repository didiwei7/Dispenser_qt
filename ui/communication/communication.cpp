#include "communication.h"

Communication::Communication(QWidget *parent) : QWidget(parent)
{
    vnavigationbar = new QVNavigationBar();
    vnavigationbar->setSelectColor("#FFC0CB");
    vnavigationbar->setRowHeight(30);
    vnavigationbar->setWidth(120);
    vnavigationbar->addItem(QStringLiteral("Socket调试"));
    vnavigationbar->addItem(QStringLiteral("串口调试"));
	
	QWidget *widget1 = new QWidget();
	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->addWidget(new QMySocket());
	layout1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout1->setStretch(0, 1);
	layout1->setStretch(1, 2);
	widget1->setLayout(layout1);

	QWidget *widget2 = new QWidget();
	QHBoxLayout *layout2 = new QHBoxLayout();
	layout2->addWidget(new QMySerial());
	layout2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout2->setStretch(0, 1);
	layout2->setStretch(1, 2);
	widget2->setLayout(layout2);

    stackedWidget   = new QStackedWidget();
    stackedWidget->insertWidget(0, widget1);
    stackedWidget->insertWidget(1, widget2);

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addWidget(vnavigationbar);
    layout_1->addWidget(stackedWidget);
    setLayout(layout_1);

    connect(vnavigationbar, &QVNavigationBar::currentItemChanged,
            stackedWidget,  &QStackedWidget::setCurrentIndex);

}
