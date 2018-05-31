#include "operation.h"

Operation::Operation(QWidget *parent) : QWidget(parent)
{
    setupUi();
	setConnect();
}

void Operation::setupUi()
{
	setGroupWarn();
	setGroupGlue();
	setGroupChart();
	setGroupDebug();
	setGroupConfig();

	QHBoxLayout *layout_1 = new QHBoxLayout();
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	QVBoxLayout *layout_2_2 = new QVBoxLayout();

	layout_2_1->addWidget(group_warn);
	layout_2_1->addWidget(group_glue);
	layout_2_1->addWidget(group_config);
	layout_2_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	layout_2_2->addWidget(group_chart);
	layout_2_2->addWidget(group_debug);
	layout_2_2->setStretch(0, 3);
	layout_2_2->setStretch(1, 2);

	layout_1->addLayout(layout_2_1);
	layout_1->addLayout(layout_2_2);
	layout_1->setStretch(0, 1);
	layout_1->setStretch(1, 4);

	setLayout(layout_1);

	// 写入样式表
	QFile file("../bin/qss/operation.qss");
	file.open(QFile::ReadOnly);
	QString style = file.readAll();
	setStyleSheet(style);
	file.close();
}

void Operation::setConnect()
{
	connect(check_glue1, &QCheckBox::clicked, this, &Operation::on_check_glue);
	connect(check_glue2, &QCheckBox::clicked, this, &Operation::on_check_glue);
	connect(check_glue3, &QCheckBox::clicked, this, &Operation::on_check_glue);
}

void Operation::setGroupWarn()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(20);
	font.setBold(true);

	group_warn = new QGroupBox();

	label_rundata = new QLabel(QStringLiteral("请登陆"));
	label_rundata->setFont(font);
	label_rundata->setAlignment(Qt::AlignCenter);
	label_rundata->setFixedHeight(200);

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->addWidget(label_rundata);

	group_warn->setLayout(layout_1);
}

void Operation::setGroupGlue()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(10);
	
	group_glue = new QGroupBox(QStringLiteral("点胶功能测试"));
	group_glue->setFont(font);

	QHBoxLayout *layout_1 = new QHBoxLayout();
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	QVBoxLayout *layout_2_2 = new QVBoxLayout();

	btn_ccd_calibration = new QPushButton(QStringLiteral("CCD_标定"));
	btn_ccd_laser		= new QPushButton(QStringLiteral("CCD_Laser探高"));
	btn_ccd_runEmpty    = new QPushButton(QStringLiteral("CCD_空跑点胶"));
	btn_pin_calibration = new QPushButton(QStringLiteral("针头校准"));
	
	btn_glueAxisHome  = new QPushButton(QStringLiteral("点胶轴复位"));
	btn_glueClear     = new QPushButton(QStringLiteral("清胶"));
	btn_glueAutoClear = new QPushButton(QStringLiteral("自动排胶"));
	btn_runEmpty	  = new QPushButton(QStringLiteral("空跑点胶"));

	layout_2_1->addWidget(btn_ccd_calibration);
	layout_2_1->addWidget(btn_ccd_laser);
	layout_2_1->addWidget(btn_ccd_runEmpty);
	layout_2_1->addWidget(btn_pin_calibration);

	layout_2_2->addWidget(btn_glueAxisHome);
	layout_2_2->addWidget(btn_glueClear);
	layout_2_2->addWidget(btn_glueAutoClear);
	layout_2_2->addWidget(btn_runEmpty);

	layout_1->addLayout(layout_2_1);
	layout_1->addLayout(layout_2_2);
	layout_1->setMargin(20);
	// layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	group_glue->setLayout(layout_1);
}

void Operation::setGroupChart()
{
	//chart_1 = new QChart();
	//chart_1->setTitle(QStringLiteral("工站1吸嘴压力"));
	//chart_2 = new QChart();
	//chart_2->setTitle(QStringLiteral("良率"));
	//chart_3 = new QChart();
	//chart_3->setTitle(QStringLiteral("X, Y补偿值"));
	//chart_4 = new QChart();
	//chart_4->setTitle(QStringLiteral("良率"));

	//QGridLayout *layout_1 = new QGridLayout();
	//layout_1->addWidget(new QChartView(setChart_1()), 0, 0);
	//layout_1->addWidget(new QChartView(setChart_3()), 0, 1);
	//layout_1->addWidget(new QChartView(setChart_2()), 1, 0);
	//layout_1->addWidget(new QChartView(setChart_4()), 1, 1);

	setChart_offset();
	setChart_pass();

	QHBoxLayout *layout_1 = new QHBoxLayout();
	layout_1->addWidget(chartView_offset);
	layout_1->addWidget(chartView_quality);

	group_chart = new QGroupBox();
	group_chart->setLayout(layout_1);
}

void Operation::setGroupDebug()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(8);

	group_debug = new QGroupBox();

	text_rundata = new QTextBrowser();
	text_rundata->setFont(font);

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->addWidget(text_rundata);

	group_debug->setLayout(layout_1);

	on_changedRundataText(QStringLiteral("您还没有登陆,暂时无法为您提供操作权限哦"));
	on_changedRundataText(QStringLiteral("请点击上方目录栏的登陆选项, 或者使用快捷键 Ctrl + L, 进行登陆"));
}

void Operation::setGroupConfig()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(10);

	group_config = new QGroupBox(QStringLiteral("点胶流程配置"));
	group_config->setFont(font);

	check_glue1 = new QCheckBox(QStringLiteral("点胶工位1是否执行"));
	check_glue2 = new QCheckBox(QStringLiteral("点胶工位2是否执行"));
	check_glue3 = new QCheckBox(QStringLiteral("点胶工位3是否执行"));

	QHBoxLayout *layout_1 = new QHBoxLayout();
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	layout_2_1->addWidget(check_glue1);
	layout_2_1->addWidget(check_glue2);
	layout_2_1->addWidget(check_glue3);

	layout_1->addLayout(layout_2_1);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	group_config->setLayout(layout_1);

	QFile file("../config/workflow_glue.ini");
	if (!file.exists()) return;
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		check_glue1->setChecked(setting.value("workflow_glue/is_config_gluel").toBool());
		check_glue2->setChecked(setting.value("workflow_glue/is_config_glue2").toBool());
		check_glue3->setChecked(setting.value("workflow_glue/is_config_glue3").toBool());
	}
}


void Operation::on_changedRundataLabel(QString str)
{
	label_rundata->setText(str);
}

void Operation::on_changedRundataText(QString str)
{
	QString s_currentTime = getCurrentTime();
	QString s_str = s_currentTime + ":  " + str;
	text_rundata->append(s_str);
}

void Operation::on_changedOffsetChart(float x, float y, float A)
{
	index_offset += 1;

	lseries_offset_x->append(index_offset, x);
	lseries_offset_y->append(index_offset, y);
	lseries_offset_A->append(index_offset, A);

	chart_offset->removeSeries(lseries_offset_x);
	chart_offset->removeSeries(lseries_offset_y);
	chart_offset->removeSeries(lseries_offset_A);

	chart_offset->addSeries(lseries_offset_x);
	chart_offset->addSeries(lseries_offset_y);
	chart_offset->addSeries(lseries_offset_A);
	chart_offset->createDefaultAxes();
}


void Operation::setChart_offset()
{
	index_offset = 0;

	lseries_offset_x = new QLineSeries();
	lseries_offset_y = new QLineSeries();
	lseries_offset_A = new QLineSeries();
	lseries_offset_x->setName(QStringLiteral("X补偿值(mm)"));
	lseries_offset_y->setName(QStringLiteral("Y补偿值(mm)"));
	lseries_offset_A->setName(QStringLiteral("A补偿值(angle)"));

	lseries_offset_x->append(index_offset, 0);
	lseries_offset_y->append(index_offset, 0);
	lseries_offset_A->append(index_offset, 0);

	chart_offset = new QChart();
	chart_offset->setTitle(QStringLiteral("CCD X,Y,A补偿值"));
	chart_offset->addSeries(lseries_offset_x);
	chart_offset->addSeries(lseries_offset_y);
	chart_offset->addSeries(lseries_offset_A);
	chart_offset->createDefaultAxes();
	chart_offset->legend()->setAlignment(Qt::AlignTop);

	chartView_offset = new QChartView();
	chartView_offset->setChart(chart_offset);

	//on_changedOffsetChart(1, -2, 3);
	//on_changedOffsetChart(-4, 5, 6);
	//on_changedOffsetChart(-6, 9, 7);
}

void Operation::setChart_pass()
{
	slice_pass = new QPieSlice(QStringLiteral("良品 95%"), 95);
	slice_pass->setLabelVisible(true);
	slice_pass->setExploded(true);
	slice_pass->setLabelColor(Qt::green);

	slice_fail = new QPieSlice(QStringLiteral("不良 5%"), 5);
	slice_fail->setLabelVisible(true);
	slice_fail->setExploded(true);
	slice_fail->setLabelColor(Qt::red);

	pseries_quality = new QPieSeries();
	pseries_quality->append(slice_pass);
	pseries_quality->append(slice_fail);

	chart_quality = new QChart();
	chart_quality->setTitle(QStringLiteral("产品良率"));
	chart_quality->addSeries(pseries_quality);
	chart_quality->legend()->setAlignment(Qt::AlignRight);

	chartView_quality = new QChartView();
	chartView_quality->setChart(chart_quality);
}



QChart *Operation::setChart_2()
{
    lseries_2 = new QLineSeries();
    lseries_2->setName(QStringLiteral("良率"));

    double y = 0.9;
    for(double_t x=0; x<5; x+=0.1)
    {
        y += 0.001;
        lseries_2->append(x, y);
    }

    for(double_t x=5; x<10; x+=0.1)
    {
        y -= 0.001;
        lseries_2->append(x, y);
    }

    chart_2->addSeries(lseries_2);
    chart_2->createDefaultAxes();
    chart_2->legend()->setAlignment(Qt::AlignTop);

    return chart_2;
}

QChart *Operation::setChart_3()
{
    lseries_3_1 = new QLineSeries();
    lseries_3_1->setName(QStringLiteral("X补偿值(mm)"));

    lseries_3_1->append(1, 0.11);
    lseries_3_1->append(2, 0.1);
    lseries_3_1->append(3, 0.12);
    lseries_3_1->append(4, 0.09);
    lseries_3_1->append(5, 0.08);
    lseries_3_1->setColor(Qt::red);

    lseries_3_2 = new QLineSeries();
    lseries_3_2->setName(QStringLiteral("Y补偿值(mm)"));

    lseries_3_2->append(1, 0.12);
    lseries_3_2->append(2, 0.1);
    lseries_3_2->append(3, 0.09);
    lseries_3_2->append(4, 0.08);
    lseries_3_2->append(5, 0.1);
    lseries_3_2->setColor(Qt::green);


    chart_3->addSeries(lseries_3_1);
    chart_3->addSeries(lseries_3_2);
    chart_3->createDefaultAxes();
    chart_3->legend()->setAlignment(Qt::AlignTop);

    return chart_3;
}

QChart *Operation::setChart_4()
{
    pseries_4 = new QPieSeries();

    QPieSlice *slice_1 = new QPieSlice(QStringLiteral("良品 90%"), 90);
    QPieSlice *slice_2 = new QPieSlice(QStringLiteral("不良 10%"), 10);
    slice_1->setLabelVisible(true);
    slice_1->setExploded(true);
    slice_1->setLabelColor(Qt::green);
    slice_2->setLabelVisible(true);
    slice_2->setExploded(true);
    slice_2->setLabelColor(Qt::red);
    pseries_4->append(slice_1);
    pseries_4->append(slice_2);

    chart_4->addSeries(pseries_4);
    chart_4->legend()->setAlignment(Qt::AlignRight);

    return chart_4;
}

// 获取当前时间
QString Operation::getCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
	return s_currentTime;
}


void Operation::on_check_glue()
{
	QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
	setting.beginGroup("workflow_glue");
	setting.setValue("is_config_gluel", check_glue1->isChecked());
	setting.setValue("is_config_glue2", check_glue2->isChecked());
	setting.setValue("is_config_glue3", check_glue3->isChecked());
	setting.endGroup();

	emit changedConfigGlue(check_glue1->isChecked(), check_glue2->isChecked(), check_glue3->isChecked());
}