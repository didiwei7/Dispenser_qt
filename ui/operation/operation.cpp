#include "operation.h"

Operation::Operation(QWidget *parent) : QWidget(parent)
{
    setupUi();
	setConnect();
	setTimer();
}

void Operation::setupUi()
{
	setGroupLogo();
	setGroupWarn();
	setGroupGlue();
	setGroupChart();
	setGroupDebug();
	setGroupConfig();
	setGroupDistanceOffset();

	QHBoxLayout *layout_1 = new QHBoxLayout();
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	QVBoxLayout *layout_2_2 = new QVBoxLayout();

	layout_2_1->addWidget(group_logo);
	layout_2_1->addWidget(group_warn);
	layout_2_1->addWidget(group_glue);
	layout_2_1->addWidget(group_config);
	layout_2_1->addWidget(group_distance_offset);
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
	connect(btn_station_home,  &QPushButton::clicked, this, &Operation::on_btn_station_home);
	connect(btn_ccd_calib,     &QPushButton::clicked, this, &Operation::on_btn_ccd_calib);
	connect(btn_ccd_runEmpty,  &QPushButton::clicked, this, &Operation::on_btn_ccd_runEmpty);
	connect(btn_runEmpty,      &QPushButton::clicked, this, &Operation::on_btn_runEmpty);
	connect(btn_clearNeedle,     &QPushButton::clicked, this, &Operation::on_btn_clearNeedle);
	connect(btn_dischargeGlue, &QPushButton::clicked, this, &Operation::on_btn_dischargeGlue);
	connect(btn_needleCalib_1, &QPushButton::clicked, this, &Operation::on_btn_needleCalib_1);
	connect(btn_needleCalib_2, &QPushButton::clicked, this, &Operation::on_btn_needleCalib_2);

	connect(check_glue1, &QCheckBox::clicked, this, &Operation::on_check_flowConfig);
	connect(check_glue2, &QCheckBox::clicked, this, &Operation::on_check_flowConfig);
	connect(check_glue3, &QCheckBox::clicked, this, &Operation::on_check_flowConfig);

	connect(btn_saveDistanceOffset, &QPushButton::clicked, this, &Operation::on_btn_saveDistanceOffset);
}

void Operation::setTimer()
{
	timer_dischargeGlue = new QTimer(this);
	is_dischargeGlue_ing = false;
}
	

void Operation::setGroupLogo()
{
	group_logo = new QGroupBox();

	QLabel *label_logo = new QLabel();
	label_logo->setFixedHeight(230);

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->setMargin(5);
	layout_1->setSpacing(0);
	layout_1->addWidget(label_logo);

	group_logo->setLayout(layout_1);

	QImage img_logo("../ui/resources/logo.png");
	img_logo.scaled(label_logo->size());
	label_logo->setPixmap(QPixmap::fromImage(img_logo));
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
	label_rundata->setFixedHeight(100);

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->addWidget(label_rundata);

	group_warn->setLayout(layout_1);
}

void Operation::setGroupGlue()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(8);
	
	group_glue = new QGroupBox(QStringLiteral("点胶功能测试"));
	group_glue->setFont(font);

	QHBoxLayout *layout_1 = new QHBoxLayout();
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	QVBoxLayout *layout_2_2 = new QVBoxLayout();

	btn_station_home = new QPushButton(QStringLiteral("点胶轴复位"));
	btn_ccd_calib    = new QPushButton(QStringLiteral("CCD_标定"));
	btn_ccd_runEmpty = new QPushButton(QStringLiteral("CCD_空跑"));
	btn_runEmpty     = new QPushButton(QStringLiteral("空跑不点胶"));

	btn_clearNeedle     = new QPushButton(QStringLiteral("清胶"));
	btn_dischargeGlue = new QPushButton(QStringLiteral("自动排胶"));
	btn_needleCalib_1 = new QPushButton(QStringLiteral("校针-首次校准"));
	btn_needleCalib_2 = new QPushButton(QStringLiteral("校针-自动保存"));

	layout_2_1->addWidget(btn_station_home);
	layout_2_1->addWidget(btn_ccd_calib);
	layout_2_1->addWidget(btn_ccd_runEmpty);
	layout_2_1->addWidget(btn_runEmpty);

	layout_2_2->addWidget(btn_clearNeedle);
	layout_2_2->addWidget(btn_dischargeGlue);
	layout_2_2->addWidget(btn_needleCalib_1);
	layout_2_2->addWidget(btn_needleCalib_2);

	layout_1->addLayout(layout_2_1);
	layout_1->addLayout(layout_2_2);
	layout_1->setContentsMargins(20, 3, 20, 3);
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

	QString s_currentTime = getCurrentTime();
	text_rundata->append(s_currentTime + ":  " + QStringLiteral("您还没有登陆,暂时无法为您提供操作权限哦"));
	text_rundata->append(s_currentTime + ":  " + QStringLiteral("请点击上方目录栏的登陆选项, 或者使用快捷键 Ctrl + L, 进行登陆"));
}

void Operation::setGroupConfig()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(8);

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
	layout_1->setContentsMargins(10, 3, 10, 3);

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
	file.close();
}

void Operation::setGroupDistanceOffset()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(8);

	group_distance_offset = new QGroupBox(QStringLiteral("距离与偏移"));
	group_distance_offset->setFont(font);

	QLabel *label_ccd_needle_x = new QLabel(QStringLiteral("CCD到针头距离 X:"));
	QLabel *label_ccd_needle_y = new QLabel(QStringLiteral("CCD到针头距离 Y:"));

	QLabel *label_ccd_laser_x = new QLabel(QStringLiteral("CCD到Laser距离 X:"));
	QLabel *label_ccd_laser_y = new QLabel(QStringLiteral("CCD到Laser距离 Y:"));

	QLabel *label_laser_needle_x = new QLabel(QStringLiteral("Laser到针头距离 X:"));
	QLabel *label_laser_needle_y = new QLabel(QStringLiteral("Laser到针头距离 Y:"));
	QLabel *label_laser_needle_z = new QLabel(QStringLiteral("Laser到针头距离 Z:"));

	QLabel *label_offset_ccd_needle_x = new QLabel(QStringLiteral("CCD与针头偏移 X:"));
	QLabel *label_offset_ccd_needle_y = new QLabel(QStringLiteral("CCD与针头偏移 Y:"));
	QLabel *label_offset_laser_needle_z = new QLabel(QStringLiteral("Laser与针头偏移 Z:"));

	QLabel *label_needle_z = new QLabel(QStringLiteral("针头点胶高度 Z:"));
	
	edit_distance_ccd_needle_x = new QLineEdit();
	edit_distance_ccd_needle_y = new QLineEdit();

	edit_distance_ccd_laser_x = new QLineEdit();
	edit_distance_ccd_laser_y = new QLineEdit();

	edit_distance_laser_needle_x = new QLineEdit();
	edit_distance_laser_needle_y = new QLineEdit();
	edit_distance_laser_needle_z = new QLineEdit();

	edit_offset_ccd_needle_x = new QLineEdit();
	edit_offset_ccd_needle_y = new QLineEdit();
	edit_offset_laser_needle_z = new QLineEdit();

	edit_distance_needle_z = new QLineEdit();

	btn_saveDistanceOffset = new QPushButton(QStringLiteral("保存"));

	QGridLayout *layout_2_1 = new QGridLayout();
	layout_2_1->addWidget(label_ccd_needle_x,   0, 0, Qt::AlignRight);
	layout_2_1->addWidget(label_ccd_needle_y,   1, 0, Qt::AlignRight);
	layout_2_1->addWidget(label_ccd_laser_x,    2, 0, Qt::AlignRight);
	layout_2_1->addWidget(label_ccd_laser_y,    3, 0, Qt::AlignRight);
	layout_2_1->addWidget(label_laser_needle_x, 4, 0, Qt::AlignRight);
	layout_2_1->addWidget(label_laser_needle_y, 5, 0, Qt::AlignRight);
	layout_2_1->addWidget(label_laser_needle_z, 6, 0, Qt::AlignRight);
	layout_2_1->addWidget(edit_distance_ccd_needle_x,   0, 1, Qt::AlignLeft);
	layout_2_1->addWidget(edit_distance_ccd_needle_y,   1, 1, Qt::AlignLeft);
	layout_2_1->addWidget(edit_distance_ccd_laser_x,    2, 1, Qt::AlignLeft);
	layout_2_1->addWidget(edit_distance_ccd_laser_y,    3, 1, Qt::AlignLeft);
	layout_2_1->addWidget(edit_distance_laser_needle_x, 4, 1, Qt::AlignLeft);
	layout_2_1->addWidget(edit_distance_laser_needle_y, 5, 1, Qt::AlignLeft);
	layout_2_1->addWidget(edit_distance_laser_needle_z, 6, 1, Qt::AlignLeft);

	QGridLayout *layout_2_2 = new QGridLayout();
	layout_2_2->addWidget(label_offset_ccd_needle_x,   0, 0, Qt::AlignRight);
	layout_2_2->addWidget(label_offset_ccd_needle_y,   1, 0, Qt::AlignRight);
	layout_2_2->addWidget(label_offset_laser_needle_z, 2, 0, Qt::AlignRight);
	layout_2_2->addWidget(edit_offset_ccd_needle_x,   0, 1, Qt::AlignLeft);
	layout_2_2->addWidget(edit_offset_ccd_needle_y,   1, 1, Qt::AlignLeft);
	layout_2_2->addWidget(edit_offset_laser_needle_z, 2, 1, Qt::AlignLeft);

	QGridLayout *layout_2_3 = new QGridLayout();
	layout_2_3->addWidget(label_needle_z, 0, 0, Qt::AlignRight);
	layout_2_3->addWidget(edit_distance_needle_z, 0, 1, Qt::AlignLeft);

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->setMargin(3);
	layout_1->addLayout(layout_2_1);
	layout_1->addSpacing(5);
	layout_1->addLayout(layout_2_2);
	layout_1->addSpacing(5);
	layout_1->addLayout(layout_2_3);
	layout_1->addWidget(btn_saveDistanceOffset);
	
	group_distance_offset->setLayout(layout_1);

	QFile file("../config/workflow_glue.ini");
	if (!file.exists()) return;
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		edit_distance_ccd_needle_x->setText(setting.value("ccd_needle_distance/offset_x").toString());
		edit_distance_ccd_needle_y->setText(setting.value("ccd_needle_distance/offset_y").toString());

		edit_distance_ccd_laser_x->setText(setting.value("ccd_laser_distance/offset_x").toString());
		edit_distance_ccd_laser_y->setText(setting.value("ccd_laser_distance/offset_y").toString());

		edit_distance_laser_needle_x->setText(setting.value("laser_needle_distance/offset_x").toString());
		edit_distance_laser_needle_y->setText(setting.value("laser_needle_distance/offset_y").toString());
		edit_distance_laser_needle_z->setText(setting.value("laser_needle_distance/offset_z").toString());

		edit_offset_ccd_needle_x->setText(setting.value("calib_needle_optical/calib_offset_x").toString());
		edit_offset_ccd_needle_y->setText(setting.value("calib_needle_optical/calib_offset_y").toString());
		edit_offset_laser_needle_z->setText(setting.value("calib_needle_attach/calib_offset_z").toString());

		edit_distance_needle_z->setText(setting.value("needle_distance/offset_z").toString());
	}
	file.close();
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



void Operation::on_timer_dischargeGlue()
{
	write_out_bit(8, 1);
	asyncSleep(3000);
	write_out_bit(8, 0);
}

//	Check glue1, glue2, glue3
void Operation::on_check_flowConfig()
{
	QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
	setting.beginGroup("workflow_glue");
	setting.setValue("is_config_gluel", check_glue1->isChecked());
	setting.setValue("is_config_glue2", check_glue2->isChecked());
	setting.setValue("is_config_glue3", check_glue3->isChecked());
	setting.endGroup();

	emit clicked_check_flowConfig();
}

// BTN 保存距离偏移
void Operation::on_btn_saveDistanceOffset()
{
	QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);

	setting.beginGroup("ccd_needle_distance");
	setting.setValue("offset_x", edit_distance_ccd_needle_x->text().toInt());
	setting.setValue("offset_y", edit_distance_ccd_needle_y->text().toInt());
	setting.endGroup();

	setting.beginGroup("ccd_laser_distance");
	setting.setValue("offset_x", edit_distance_ccd_laser_x->text().toInt());
	setting.setValue("offset_y", edit_distance_ccd_laser_y->text().toInt());
	setting.endGroup();

	setting.beginGroup("laser_needle_distance");
	setting.setValue("offset_x", edit_distance_laser_needle_x->text().toInt());
	setting.setValue("offset_y", edit_distance_laser_needle_y->text().toInt());
	setting.setValue("offset_z", edit_distance_laser_needle_z->text().toInt());
	setting.endGroup();

	setting.beginGroup("calib_needle_optical");
	setting.setValue("calib_offset_x", edit_offset_ccd_needle_x->text().toInt());
	setting.setValue("calib_offset_y", edit_offset_ccd_needle_y->text().toInt());
	setting.endGroup();

	setting.beginGroup("calib_needle_attach");
	setting.setValue("calib_offset_z", edit_offset_laser_needle_z->text().toInt());
	setting.endGroup();

	setting.beginGroup("needle_distance");
	setting.setValue("offset_z", edit_distance_needle_z->text().toInt());
	setting.endGroup();

	emit clicked_btn_saveDistanceOffset();
}

// BTN 点胶轴复位
void Operation::on_btn_station_home()
{
	if (!(init_card() == 1)) return;

	QtConcurrent::run([&]() {
		home_axis(AXISNUM::Z);
		wait_axis_homeOk(AXISNUM::Z);

		home_axis(AXISNUM::X);
		wait_axis_homeOk(AXISNUM::X);

		home_axis(AXISNUM::Y);
		wait_axis_homeOk(AXISNUM::Y);
	});
}

// BTN CCD 标定
void Operation::on_btn_ccd_calib()
{
	emit clicked_btn_ccd_calib();
}

// BTN CCD 空跑
void Operation::on_btn_ccd_runEmpty()
{
	emit clicked_btn_ccd_runEmpty();
}

// BTN 空跑不点胶
void Operation::on_btn_runEmpty()
{
	emit clicked_btn_runEmpty();
}

// BTN 擦胶
void Operation::on_btn_clearNeedle()
{
	emit clicked_btn_clearNeedle();
}

// BTN 自动排胶
void Operation::on_btn_dischargeGlue()
{
	// emit clicked_btn_dischargeGlue();
	if (is_dischargeGlue_ing == false)
	{
		is_dischargeGlue_ing = true;
		btn_dischargeGlue->setText(QStringLiteral("自动排胶-运行中"));

		write_out_bit(8, 1);
		asyncSleep(3000);
		write_out_bit(8, 0);

		connect(timer_dischargeGlue, &QTimer::timeout, this, &Operation::on_timer_dischargeGlue);
		timer_dischargeGlue->start(30000);
	}
	else
	{
		is_dischargeGlue_ing = false;
		btn_dischargeGlue->setText(QStringLiteral("自动排胶-已结束"));

		disconnect(timer_dischargeGlue, &QTimer::timeout, this, &Operation::on_timer_dischargeGlue);
		timer_dischargeGlue->stop();
	}
}

// BTN 校针1
void Operation::on_btn_needleCalib_1()
{
	emit clicked_btn_needleCalib_1();
}

// BTN 校针2
void Operation::on_btn_needleCalib_2()
{
	emit clicked_btn_needleCalib_2();
}

// 连接Workflow  
void Operation::on_changedRundataLabel(QString str)
{
	label_rundata->setText(str);
}

// 连接Workflow
void Operation::on_changedRundataText(QString str)
{
	QString s_currentTime = getCurrentTime();
	QString s_str = s_currentTime + ":  " + str;
	text_rundata->append(s_str);
}

// 连接Workflow
void Operation::on_changedDistanceOffset()
{
	QFile file("../config/workflow_glue.ini");
	if (!file.exists()) return;
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		edit_distance_ccd_needle_x->setText(setting.value("ccd_needle_distance/offset_x").toString());
		edit_distance_ccd_needle_y->setText(setting.value("ccd_needle_distance/offset_y").toString());

		edit_distance_ccd_laser_x->setText(setting.value("ccd_laser_distance/offset_x").toString());
		edit_distance_ccd_laser_y->setText(setting.value("ccd_laser_distance/offset_y").toString());

		edit_distance_laser_needle_x->setText(setting.value("laser_needle_diatance/offset_x").toString());
		edit_distance_laser_needle_y->setText(setting.value("laser_needle_diatance/offset_y").toString());
		edit_distance_laser_needle_z->setText(setting.value("laser_needle_diatance/offset_z").toString());

		edit_offset_ccd_needle_x->setText(setting.value("calib_needle_optical/calib_offset_x").toString());
		edit_offset_ccd_needle_y->setText(setting.value("calib_needle_optical/calib_offset_y").toString());
		edit_offset_laser_needle_z->setText(setting.value("calib_needle_attach/calib_offset_z").toString());
	
		edit_distance_needle_z->setText(setting.value("needle_distance/offset_z").toString());
	}
	file.close();
}

// 连接Workflow
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

// 连接Workflow
void Operation::on_changedDischargeGlue()
{
	on_btn_dischargeGlue();
}


// 获取当前时间
QString Operation::getCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
	return s_currentTime;
}

// 获取所有点位
QMap<QString, PointRun> Operation::getAllRunPointInfo()
{
	// 【2】 点位配置
	QSqlTableModel *model_general = new QSqlTableModel();
	// 使用 submit 时,数据库才会更改,否则做出的更改存储在缓存中
	model_general->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_general->setTable("point_main");
	// 设置按第0列升序排列
	model_general->setSort(0, Qt::AscendingOrder);
	model_general->select();

	QSqlTableModel *model_glue1 = new QSqlTableModel();
	model_glue1->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue1->setTable("point_glue1");
	model_glue1->setSort(0, Qt::AscendingOrder);
	model_glue1->select();

	QSqlTableModel *model_glue2 = new QSqlTableModel();
	model_glue2->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue2->setTable("point_glue2");
	model_glue2->setSort(0, Qt::AscendingOrder);
	model_glue2->select();

	QSqlTableModel *model_glue3 = new QSqlTableModel();
	model_glue3->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue3->setTable("point_glue3");
	model_glue3->setSort(0, Qt::AscendingOrder);
	model_glue3->select();

	QMap<QString, PointRun> _allPoint;

	for (int index = 0; index < model_general->rowCount(); index++)
	{
		QString name = model_general->record(index).value("name").toString();
		QString description = model_general->record(index).value("description").toString();
		float X = model_general->record(index).value("X").toString().toFloat();
		float Y = model_general->record(index).value("Y").toString().toFloat();
		float Z = model_general->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;
		_allPoint.insert(name, point);
	}

	for (int index = 0; index < model_glue1->rowCount(); index++)
	{
		QString name = model_glue1->record(index).value("name").toString();
		QString description = model_glue1->record(index).value("description").toString();
		float X = model_glue1->record(index).value("X").toString().toFloat();
		float Y = model_glue1->record(index).value("Y").toString().toFloat();
		float Z = model_glue1->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;

		_allPoint.insert(name, point);
	}

	for (int index = 0; index < model_glue2->rowCount(); index++)
	{
		QString name = model_glue2->record(index).value("name").toString();
		QString description = model_glue2->record(index).value("description").toString();
		float X = model_glue2->record(index).value("X").toString().toFloat();
		float Y = model_glue2->record(index).value("Y").toString().toFloat();
		float Z = model_glue2->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;

		_allPoint.insert(name, point);
	}

	for (int index = 0; index < model_glue3->rowCount(); index++)
	{
		QString name = model_glue3->record(index).value("name").toString();
		QString description = model_glue3->record(index).value("description").toString();
		float X = model_glue3->record(index).value("X").toString().toFloat();
		float Y = model_glue3->record(index).value("Y").toString().toFloat();
		float Z = model_glue3->record(index).value("Z").toString().toFloat();

		PointRun point;
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;

		_allPoint.insert(name, point);
	}

	return _allPoint;
}

// 移动到点, 没有速度要求, 暂时先这么写
bool Operation::move_point_name(QString pointname)
{
	QMap<QString, PointRun> allpoint_pointRun = getAllRunPointInfo();

	if (!allpoint_pointRun.contains(pointname))
	{
		QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("找不到点: %1 数据").arg(pointname));
		return false;
	}
	else
	{
		PointRun point = allpoint_pointRun[pointname];

		move_axis_abs(AXISNUM::X, point.X);
		move_axis_abs(AXISNUM::Y, point.Y);
		move_axis_abs(AXISNUM::Z, point.Z);

		wait_axis_stop(AXISNUM::X);
		wait_axis_stop(AXISNUM::Y);
		wait_axis_stop(AXISNUM::Z);
	}

	return true;
}