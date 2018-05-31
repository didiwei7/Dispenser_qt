#include "communication.h"

Communication::Communication(QWidget *parent) : QWidget(parent)
{
    vnavigationbar = new QVNavigationBar();
    vnavigationbar->setSelectColor("#FFC0CB");
    vnavigationbar->setRowHeight(30);
    vnavigationbar->setWidth(120);
    vnavigationbar->addItem(QStringLiteral("Socket调试"));
    vnavigationbar->addItem(QStringLiteral("串口调试"));
	
	QWidget *w_socket = new QWidget();
	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->addWidget(new QMySocket());
	layout1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout1->setStretch(0, 1);
	layout1->setStretch(1, 2);
	w_socket->setLayout(layout1);

	QWidget *w_serial = new QWidget();
	QHBoxLayout *layout2 = new QHBoxLayout();
	layout2->addWidget(new QMySerial());
	layout2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout2->setStretch(0, 1);
	layout2->setStretch(1, 2);
	w_serial->setLayout(layout2);

    stackedWidget   = new QStackedWidget();
    stackedWidget->insertWidget(0, w_socket);
    stackedWidget->insertWidget(1, w_serial);

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addWidget(vnavigationbar);
    layout_1->addWidget(stackedWidget);
    setLayout(layout_1);

    connect(vnavigationbar, &QVNavigationBar::currentItemChanged,
            stackedWidget,  &QStackedWidget::setCurrentIndex);

}
