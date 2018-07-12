#include "mainwidget.h"


MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setConnect();

}

void MainWidget::setupUi()
{
    // 【1】 导航栏
    vnavigationbar = new QVNavigationBar(this);
    vnavigationbar->setBackgroundColor   ("#303947");
    vnavigationbar->setPenColorBackground("#E4EBF3");
    vnavigationbar->setPenColorSelected  ("#FFFFFF");
    vnavigationbar->setWidth(120);
    vnavigationbar->setRowHeight(40);
    vnavigationbar->addItem(QStringLiteral("操作界面"));
    vnavigationbar->addItem(QStringLiteral("电机"));
    vnavigationbar->addItem(QStringLiteral("I/O"));
    vnavigationbar->addItem(QStringLiteral("通讯调试"));

    w_operation = new Operation(this);
    w_motor = new Motor(this);
    w_io = new IO(this);
	w_com = new Communication(this);
	w_workflow = new Workflow(this);
    stackedWidget = new QStackedWidget(this);
    stackedWidget->insertWidget(0, w_operation);
    stackedWidget->insertWidget(1, w_motor);
    stackedWidget->insertWidget(2, w_io);
	stackedWidget->insertWidget(3, w_com); // new QLabel(QStringLiteral("<font size='+1'><b><p align='center'>敌敌畏Demo系列</p></b></font>")));

    // 【2】 布局
    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addWidget(vnavigationbar);
    layout_1->addWidget(stackedWidget);

    setLayout(layout_1);
}

void MainWidget::setConnect()
{
    // 【1】 导航栏
    connect(vnavigationbar, &QVNavigationBar::currentItemChanged,
            stackedWidget,  &QStackedWidget::setCurrentIndex);

	// 【2】 自定义信号槽
	connect(w_workflow, &Workflow::changedRundataLabel,   w_operation, &Operation::on_changedRundataLabel);
	connect(w_workflow, &Workflow::changedRundataText,    w_operation, &Operation::on_changedRundataText);
	connect(w_workflow, &Workflow::changedDistanceOffset, w_operation, &Operation::on_changedDistanceOffset);
	connect(w_workflow, &Workflow::changedOffsetChart,    w_operation, &Operation::on_changedOffsetChart);	
	
	connect(w_operation, &Operation::clicked_check_flowConfig,       w_workflow, &Workflow::on_clicked_check_flowConfig);
	connect(w_operation, &Operation::clicked_btn_saveDistanceOffset, w_workflow, &Workflow::on_clicked_btn_saveDistanceOffset);
	connect(w_operation, &Operation::clicked_btn_ccd_calib,          w_workflow, &Workflow::on_clicked_btn_ccd_calib);
	connect(w_operation, &Operation::clicked_btn_ccd_runEmpty,       w_workflow, &Workflow::on_clicked_btn_ccd_runEmpty);
	connect(w_operation, &Operation::clicked_btn_runEmpty,		     w_workflow, &Workflow::on_clicked_btn_runEmpty);
	connect(w_operation, &Operation::clicked_btn_clearGlue,			 w_workflow, &Workflow::on_clicked_btn_clearGlue);
	connect(w_operation, &Operation::clicked_btn_dischargeGlue,		 w_workflow, &Workflow::on_clicked_btn_dischargeGlue);
	connect(w_operation, &Operation::clicked_btn_needleCalib_1,		 w_workflow, &Workflow::on_clicked_btn_needleCalib_1);
	connect(w_operation, &Operation::clicked_btn_needleCalib_2,		 w_workflow, &Workflow::on_clicked_btn_needleCalib_2);
	
	connect(w_motor->w_pointDebug, &PointDebug::changedSqlModel, w_workflow, &Workflow::on_changedSqlModel);		
}
