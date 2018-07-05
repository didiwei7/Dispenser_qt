#include "pointdebug.h"
#include <iostream>

PointDebug::PointDebug(QWidget *parent) : QWidget(parent)
{
    setupUi();
	setConnect();
	// setThread();
	
	// 由于Qt中Ui对象是线程不安全的, 所以还是在计时器中刷新
	setTimer();
}

PointDebug::~PointDebug()
{
	/*
	// 关闭线程
	close_thread_updateCurrentPos = true;
	close_thread_updateInputStatus = true;

	// 销毁线程池
	thread_pool.waitForDone();
	thread_pool.clear();
	thread_pool.destroyed();
	*/

}

// 初始化Ui
void PointDebug::setupUi()
{
	setGroupPoint();
    setGroupMove();
	setGroupIO();
	setGroupPos();
	setGroupHome();

	QHBoxLayout *layout_1   = new QHBoxLayout();
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	layout_1->setContentsMargins(0, 0, 0, 0);
	layout_1->setSpacing(0);
	layout_2_1->setContentsMargins(8, 8, 8, 8);
	layout_2_1->setSpacing(10);
	
	layout_1->addWidget(w_pointview);
	layout_1->addLayout(layout_2_1);
	layout_1->setStretch(0, 2);
	layout_1->setStretch(1, 1);

	layout_2_1->addWidget(group_move);
	layout_2_1->addWidget(group_home);
	layout_2_1->addWidget(group_pos);
	layout_2_1->addWidget(group_io);
	layout_2_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	setLayout(layout_1);

	// 写入样式表
	QFile file("../bin/qss/pointDebug.qss");
	file.open(QFile::ReadOnly);
	QString style = file.readAll();
	setStyleSheet(style);
	file.close();
}

// 初始化信号槽
void PointDebug::setConnect()
{
	// 【1】 Point 
	connect(hnavigationbar, &QHNavigationBar::currentItemChanged,
		    this,           &PointDebug::setCurrentModel);
	connect(action_go,	  &QAction::triggered, this, &PointDebug::on_action_go);
	connect(action_teach, &QAction::triggered, this, &PointDebug::on_action_teach);
	connect(action_add,	  &QAction::triggered, this, &PointDebug::on_action_add);
	connect(action_insert, &QAction::triggered, this, &PointDebug::on_action_insert);
	connect(action_del,    &QAction::triggered, this, &PointDebug::on_action_del);
	connect(action_save,   &QAction::triggered, this, &PointDebug::on_action_save);
	connect(pointview, &QTableView::customContextMenuRequested,
		    this,      &PointDebug::on_pointview_rightClicked);

	// 【2】 Pos  Radio
    connect(radio_continue, &QRadioButton::clicked, this, &PointDebug::on_radio_continue);
    connect(radio_long,     &QRadioButton::clicked, this, &PointDebug::on_radio_long);
    connect(radio_middle,   &QRadioButton::clicked, this, &PointDebug::on_radio_middle);
    connect(radio_short,    &QRadioButton::clicked, this, &PointDebug::on_radio_short);

	// 【3】 Move Slider
	connect(slider_speed,   &QMySlider::sliderValueChanged, this, &PointDebug::on_slider_speed_Changed);

	// 【4】 Move BTN
    connect(X_positive, &QPushButton::clicked, this, &PointDebug::on_X_positive_clicked);
    connect(X_positive, &QPushButton::pressed, this, &PointDebug::on_X_positive_pressed);
    connect(X_positive, &QPushButton::released, this, &PointDebug::on_X_positive_released);
    connect(X_negative, &QPushButton::clicked, this, &PointDebug::on_X_negative_clicked);
    connect(X_negative, &QPushButton::pressed, this, &PointDebug::on_X_negative_pressed);
    connect(X_negative, &QPushButton::released, this, &PointDebug::on_X_negative_released);

    connect(Y_positive, &QPushButton::clicked, this, &PointDebug::on_Y_positive_clicked);
    connect(Y_positive, &QPushButton::pressed, this, &PointDebug::on_Y_positive_pressed);
    connect(Y_positive, &QPushButton::released, this, &PointDebug::on_Y_positive_released);
    connect(Y_negative, &QPushButton::clicked, this, &PointDebug::on_Y_negative_clicked);
    connect(Y_negative, &QPushButton::pressed, this, &PointDebug::on_Y_negative_pressed);
    connect(Y_negative, &QPushButton::released, this, &PointDebug::on_Y_negative_released);

    connect(Z_positive, &QPushButton::clicked, this, &PointDebug::on_Z_positive_clicked);
    connect(Z_positive, &QPushButton::pressed, this, &PointDebug::on_Z_positive_pressed);
    connect(Z_positive, &QPushButton::released, this, &PointDebug::on_Z_positive_released);
    connect(Z_negative, &QPushButton::clicked, this, &PointDebug::on_Z_negative_clicked);
    connect(Z_negative, &QPushButton::pressed, this, &PointDebug::on_Z_negative_pressed);
    connect(Z_negative, &QPushButton::released, this, &PointDebug::on_Z_negative_released);

	// 【5】 回原 BTN
	connect(btn_x_home, &QPushButton::clicked, this, &PointDebug::on_btn_x_home);
	connect(btn_y_home, &QPushButton::clicked, this, &PointDebug::on_btn_y_home);
	connect(btn_z_home, &QPushButton::clicked, this, &PointDebug::on_btn_z_home);
	connect(btn_stop, &QPushButton::clicked, this, &PointDebug::on_btn_stop);
	connect(btn_station_home, &QPushButton::clicked, this, &PointDebug::on_btn_station_home);
}

// 初始化线程
void PointDebug::setThread()
{
	thread_pool.setMaxThreadCount(2);

	is_updateCurrentPos_ok = false;
	start_thread_updateCurrentPos = true;
	close_thread_updateCurrentPos = false;

	is_updateInputStatus_ok = false;
	start_thread_updateInputStatus = true;
	close_thread_updateInputStatus = false;

	future_thread_updateCurrentPos = QtConcurrent::run(&thread_pool, [&]() { thread_updateCurrentPos(); });
	future_thread_updateInputStatus = QtConcurrent::run(&thread_pool, [&]() { thread_updateInputStatus(); });
}

// 初始化计时器
void PointDebug::setTimer()
{
	QTimer *timer_pos = new QTimer(this);
	connect(timer_pos, &QTimer::timeout, this, &PointDebug::timer_updateCurrentPos);
	timer_pos->start(10);

	QTimer *timer_io = new QTimer(this);
	connect(timer_io, &QTimer::timeout, this, &PointDebug::timer_updateInputStatus);
	timer_io->start(10);
}

// Ui
void PointDebug::setGroupMove()
{
    group_move = new QGroupBox(QStringLiteral("点位移动"));

    slider_speed = new QMySlider();
	slider_speed->setText(QStringLiteral("当前速度"));
	slider_speed->setRange(0, 10000);
	slider_speed->setValue(100);
	slider_speed->setFixedWidth(250);

	start_v = float(0.1);
	speed = 1.0;
	acc = 1.0;
	dec = 1.0;
	set_speed_mode(start_v, speed, acc, ADMODE::T);

	X_negative = new QPushButton(QIcon("../ui/resources/left.png"), NULL);
    X_positive = new QPushButton(QIcon("../ui/resources/right.png"), NULL);

    Y_positive = new QPushButton(QIcon("../ui/resources/up.png"), NULL);
    Y_negative = new QPushButton(QIcon("../ui/resources/down.png"), NULL);

    Z_positive = new QPushButton(QIcon("../ui/resources/up.png"), QStringLiteral("Z+"));
    Z_negative = new QPushButton(QIcon("../ui/resources/down.png"), QStringLiteral("Z-"));

    QVBoxLayout *layout_1 = new QVBoxLayout();
    QHBoxLayout *layout_2 = new QHBoxLayout();      // 二级布局
    QVBoxLayout *layout_3_1 = new QVBoxLayout();
    QVBoxLayout *layout_3_2 = new QVBoxLayout();

    layout_3_1->addWidget(Y_positive);
	layout_3_1->addSpacing(20);
    layout_3_1->addWidget(Y_negative);
    layout_3_2->addWidget(Z_positive);
	layout_3_2->addSpacing(20);
    layout_3_2->addWidget(Z_negative);

    layout_2->addWidget(X_negative);
    layout_2->addLayout(layout_3_1);
    layout_2->addWidget(X_positive);
    layout_2->addLayout(layout_3_2);
	layout_2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout_1->addWidget(slider_speed);
    layout_1->addLayout(layout_2);

    group_move->setLayout(layout_1);
}

void PointDebug::setGroupHome()
{
	group_home = new QGroupBox(QStringLiteral("回原"));
	//QFont font;
	//font.setFamily("MicroSoft Yahei");
	//font.setPointSize(8);
	//font.setBold(true);
	//group_home->setFont(font);
	
	QHBoxLayout *layout_1 = new QHBoxLayout();

	btn_stop = new QPushButton(QStringLiteral("停止"));
	btn_station_home = new QPushButton(QStringLiteral("工站回原"));
	btn_x_home = new QPushButton(QStringLiteral("X轴回原"));
	btn_y_home = new QPushButton(QStringLiteral("Y轴回原"));
	btn_z_home = new QPushButton(QStringLiteral("Z轴回原"));

	layout_1->addWidget(btn_stop);
	layout_1->addWidget(btn_station_home);
	layout_1->addWidget(btn_x_home);
	layout_1->addWidget(btn_y_home);
	layout_1->addWidget(btn_z_home);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	group_home->setLayout(layout_1);
}

void PointDebug::setGroupIO()
{
	// 【1】 初始化Label
	QLabel *label_p = new QLabel(QStringLiteral("正限位"));
	label_p->setAlignment(Qt::AlignCenter);

	QLabel *label_o = new QLabel(QStringLiteral("原点"));
	label_o->setAlignment(Qt::AlignCenter);

	QLabel *label_n = new QLabel(QStringLiteral("负限位"));
	label_n->setAlignment(Qt::AlignCenter);

	QLabel *label_alarm = new QLabel(QStringLiteral("报警"));
	label_alarm->setAlignment(Qt::AlignCenter);

	QLabel *label_x = new QLabel(QStringLiteral("X"));
	label_x->setAlignment(Qt::AlignCenter);

	QLabel *label_y = new QLabel(QStringLiteral("Y"));
	label_y->setAlignment(Qt::AlignCenter);

	QLabel *label_z = new QLabel(QStringLiteral("Z"));
	label_z->setAlignment(Qt::AlignCenter);

	QLabel *label_a = new QLabel(QStringLiteral("A"));
	label_a->setAlignment(Qt::AlignCenter);

	for (int i = 0; i < 4; i++)
	{
		INPUT_X[i] = new QInput(this);
		INPUT_Y[i] = new QInput(this);
		INPUT_Z[i] = new QInput(this);
		INPUT_A[i] = new QInput(this);
	}

	// 【2】 布局
	QGridLayout *layout_2_1 = new QGridLayout();
	layout_2_1->addWidget(label_x, 0, 1, Qt::AlignCenter);
	layout_2_1->addWidget(label_y, 0, 2, Qt::AlignCenter);
	layout_2_1->addWidget(label_z, 0, 3, Qt::AlignCenter);
	layout_2_1->addWidget(label_a, 0, 4, Qt::AlignCenter);

	layout_2_1->addWidget(label_p, 1, 0, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_X[0], 1, 1, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Y[0], 1, 2, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Z[0], 1, 3, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_A[0], 1, 4, Qt::AlignCenter);

	layout_2_1->addWidget(label_o, 2, 0, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_X[1], 2, 1, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Y[1], 2, 2, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Z[1], 2, 3, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_A[1], 2, 4, Qt::AlignCenter);

	layout_2_1->addWidget(label_n, 3, 0, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_X[2], 3, 1, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Y[2], 3, 2, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Z[2], 3, 3, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_A[2], 3, 4, Qt::AlignCenter);

	layout_2_1->addWidget(label_alarm, 4, 0, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_X[3], 4, 1, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Y[3], 4, 2, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_Z[3], 4, 3, Qt::AlignCenter);
	layout_2_1->addWidget(INPUT_A[3], 4, 4, Qt::AlignCenter);

	QHBoxLayout *layout_1 = new QHBoxLayout();
	layout_1->addLayout(layout_2_1);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	group_io = new QGroupBox(QStringLiteral("IO信号监视"));
	group_io->setLayout(layout_1);
}

void PointDebug::setGroupPos()
{
	setGroupCurrentpos();
	setGroupStep();

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->addWidget(group_currentpos);
	layout_1->addWidget(group_step);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	group_pos = new QGroupBox(QStringLiteral("位置"));
	group_pos->setLayout(layout_1);
}

void PointDebug::setGroupCurrentpos()
{
    group_currentpos = new QGroupBox(QStringLiteral("当前位置"));

    QLabel *labelx = new QLabel(QStringLiteral("X(mm):"), this);
    QLabel *labely = new QLabel(QStringLiteral("Y(mm):"), this);
    QLabel *labelz = new QLabel(QStringLiteral("Z(mm):"), this);

    label_X_currentpos = new QLabel();
    label_X_currentpos->setAlignment(Qt::AlignLeft);
    label_X_currentpos->setFixedWidth(50);

    label_Y_currentpos = new QLabel();
    label_Y_currentpos->setAlignment(Qt::AlignLeft);
    label_Y_currentpos->setFixedWidth(50);

    label_Z_currentpos = new QLabel();
    label_Z_currentpos->setAlignment(Qt::AlignLeft);
    label_Z_currentpos->setFixedWidth(50);

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->addWidget(labelx);
    layout_1->addWidget(label_X_currentpos);
    layout_1->addSpacing(40);
    layout_1->addWidget(labely);
    layout_1->addWidget(label_Y_currentpos);
    layout_1->addSpacing(40);
    layout_1->addWidget(labelz);
    layout_1->addWidget(label_Z_currentpos);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    group_currentpos->setLayout(layout_1);
}

void PointDebug::setGroupStep()
{
    group_step = new QGroupBox(QStringLiteral("步进距离"));

    QHBoxLayout *layout_1 = new QHBoxLayout();
	layout_1->setSpacing(1);
	QVBoxLayout *layout_2_1 = new QVBoxLayout();
	layout_2_1->setSpacing(5);

    QLabel *labelx = new QLabel(QStringLiteral("X(mm)"));
    labelx->setAlignment(Qt::AlignCenter);
    labelx->setFixedWidth(50);
    QLabel *labely = new QLabel(QStringLiteral("Y(mm)"));
    labely->setAlignment(Qt::AlignCenter);
    labely->setFixedWidth(50);
    QLabel *labelz = new QLabel(QStringLiteral("Z(mm)"));
    labelz->setAlignment(Qt::AlignCenter);
    labelz->setFixedWidth(50);

    edit_X_step = new QLineEdit();
    edit_X_step->setAlignment(Qt::AlignCenter);
    edit_X_step->setFixedWidth(50);
	edit_X_step->setText(QStringLiteral("0.1"));

    edit_Y_step = new QLineEdit();
    edit_Y_step->setAlignment(Qt::AlignCenter);
    edit_Y_step->setFixedWidth(50);
	edit_Y_step->setText(QStringLiteral("0.1"));

    edit_Z_step = new QLineEdit();
    edit_Z_step->setAlignment(Qt::AlignCenter);
    edit_Z_step->setFixedWidth(50);
	edit_Z_step->setText(QStringLiteral("0.1"));

    radio_continue = new QRadioButton(QStringLiteral("连续运动"));
    radio_long   = new QRadioButton(QStringLiteral("长距离"));
    radio_middle = new QRadioButton(QStringLiteral("中距离"));
    radio_short  = new QRadioButton(QStringLiteral("短距离"));
	radio_short->setChecked(true);

	layout_1->addWidget(labelx);
	layout_1->addWidget(edit_X_step);
	layout_1->addSpacing(20);

	layout_1->addWidget(labely);
	layout_1->addWidget(edit_Y_step);
	layout_1->addSpacing(20);
	
	layout_1->addWidget(labelz);
	layout_1->addWidget(edit_Z_step);
	layout_1->addSpacing(20);
	
	layout_2_1->addWidget(radio_continue);
	layout_2_1->addWidget(radio_long);
	layout_2_1->addWidget(radio_middle);
	layout_2_1->addWidget(radio_short);

	layout_1->addLayout(layout_2_1);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	
    group_step->setLayout(layout_1);
}

void PointDebug::setGroupPoint()
{
	index_model = 0;
	setActions();
	setViewPoint();
	
	w_pointview = new QWidget();
	
	hnavigationbar = new QHNavigationBar();
	hnavigationbar->setColumnWidth(100);
	hnavigationbar->addItem(QStringLiteral("调试点位"));
	hnavigationbar->addItem(QStringLiteral("点胶工位1"));
	hnavigationbar->addItem(QStringLiteral("点胶工位2"));
	hnavigationbar->addItem(QStringLiteral("点胶工位3"));

	QVBoxLayout *layout_1 = new QVBoxLayout();
	layout_1->setContentsMargins(0, 0, 0, 0);
	layout_1->setSpacing(0);
	layout_1->addWidget(hnavigationbar);
	layout_1->addWidget(pointview);

	w_pointview->setLayout(layout_1);
}

void PointDebug::setViewPoint()
{
	// 【1】 连接数据库
	QFileInfo file("../data/point/db_point.db");
	if (!file.isFile())
	{
		// 【1】 创建数据库连接
		QSqlDatabase db_point = QSqlDatabase::addDatabase("QSQLITE");
        db_point.setDatabaseName("../data/point/db_point.db");

		// 【2】 创建数据库表单
		QSqlQuery query_point(db_point);
		db_point.open();
		query_point.exec(QLatin1String(	"create table point_main("
										"ID integer primary key,"
										"name varchar,"
										"description varchar,"
										"X varchar,"
										"Y varchar,"
										"Z varchar,"
										"center_X varchar,"
										"center_Y varchar,"
										"laser bool,"
										"open bool,"
										"openAdvance integer,"
										"openDelay integer,"
										"close bool,"
										"closeAdvance integer,"
										"closeDelay integer,"
										"type varchar)"));

		query_point.exec(QLatin1String( "create table point_glue1("
										"ID integer primary key,"
										"name varchar,"
										"description varchar,"
										"X varchar,"
										"Y varchar,"
										"Z varchar,"
										"center_X varchar,"
										"center_Y varchar,"
										"laser bool,"
										"open bool,"
										"openAdvance integer,"
										"openDelay integer,"
										"close bool,"
										"closeAdvance integer,"
										"closeDelay integer,"
										"type varchar)"));

		query_point.exec(QLatin1String( "create table point_glue2("
										"ID integer primary key,"
										"name varchar,"
										"description varchar,"
										"X varchar,"
										"Y varchar,"
										"Z varchar,"
										"center_X varchar,"
										"center_Y varchar,"
										"laser bool,"
										"open bool,"
										"openAdvance integer,"
										"openDelay integer,"
										"close bool,"
										"closeAdvance integer,"
										"closeDelay integer,"
										"type varchar)"));

		query_point.exec(QLatin1String( "create table point_glue3("
										"ID integer primary key,"
										"name varchar,"
										"description varchar,"
										"X varchar,"
										"Y varchar,"
										"Z varchar,"
										"center_X varchar,"
										"center_Y varchar,"
										"laser bool,"
										"open bool,"
										"openAdvance integer,"
										"openDelay integer,"
										"close bool,"
										"closeAdvance integer,"
										"closeDelay integer,"
										"type varchar)"));
	}
	else
	{
		// 【1】 创建数据库连接
		QSqlDatabase db_point = QSqlDatabase::addDatabase("QSQLITE");
        db_point.setDatabaseName("../data/point/db_point.db");
		db_point.open();
	}

	// 【2】 模型
	model_general = new QSqlTableModel();
    // 使用 submit 时,数据库才会更改,否则做出的更改存储在缓存中
	model_general->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_general->setTable("point_main");
    // 设置按第0列升序排列
	model_general->setSort(0, Qt::AscendingOrder);
    // 更改Model对象的 头信息
	model_general->setHeaderData(model_general->fieldIndex("name"), Qt::Horizontal, QStringLiteral("名称"));
	model_general->setHeaderData(model_general->fieldIndex("description"),  Qt::Horizontal, QStringLiteral("描述"));
	model_general->setHeaderData(model_general->fieldIndex("open"), Qt::Horizontal, QStringLiteral("是否开胶"));
	model_general->setHeaderData(model_general->fieldIndex("openAdvance"), Qt::Horizontal, QStringLiteral("提前开胶"));
	model_general->setHeaderData(model_general->fieldIndex("openDelay"), Qt::Horizontal, QStringLiteral("延迟开胶"));
	model_general->setHeaderData(model_general->fieldIndex("close"), Qt::Horizontal, QStringLiteral("是否关胶"));
	model_general->setHeaderData(model_general->fieldIndex("closeAdvance"), Qt::Horizontal, QStringLiteral("提前关胶"));
	model_general->setHeaderData(model_general->fieldIndex("closeDelay"), Qt::Horizontal, QStringLiteral("延迟关胶"));
	model_general->setHeaderData(model_general->fieldIndex("type"), Qt::Horizontal, QStringLiteral("类型"));
	model_general->select();

	model_glue1 = new QSqlTableModel();
	// 使用 submit 时,数据库才会更改,否则做出的更改存储在缓存中
	model_glue1->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue1->setTable("point_glue1");
	// 设置按第0列升序排列
	model_glue1->setSort(0, Qt::AscendingOrder);
	// 更改Model对象的 头信息
	model_glue1->setHeaderData(model_glue1->fieldIndex("name"), Qt::Horizontal, QStringLiteral("名称"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("description"), Qt::Horizontal, QStringLiteral("描述"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("open"), Qt::Horizontal, QStringLiteral("是否开胶"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("openAdvance"), Qt::Horizontal, QStringLiteral("提前开胶"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("openDelay"), Qt::Horizontal, QStringLiteral("延迟开胶"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("close"), Qt::Horizontal, QStringLiteral("是否关胶"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("closeAdvance"), Qt::Horizontal, QStringLiteral("提前关胶"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("closeDelay"), Qt::Horizontal, QStringLiteral("延迟关胶"));
	model_glue1->setHeaderData(model_glue1->fieldIndex("type"), Qt::Horizontal, QStringLiteral("类型"));
	model_glue1->select();

	model_glue2 = new QSqlTableModel();
	// 使用 submit 时,数据库才会更改,否则做出的更改存储在缓存中
	model_glue2->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue2->setTable("point_glue2");
	// 设置按第0列升序排列
	model_glue2->setSort(0, Qt::AscendingOrder);
	// 更改Model对象的 头信息
	model_glue2->setHeaderData(model_glue2->fieldIndex("name"), Qt::Horizontal, QStringLiteral("名称"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("description"), Qt::Horizontal, QStringLiteral("描述"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("open"), Qt::Horizontal, QStringLiteral("是否开胶"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("openAdvance"), Qt::Horizontal, QStringLiteral("提前开胶"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("openDelay"), Qt::Horizontal, QStringLiteral("延迟开胶"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("close"), Qt::Horizontal, QStringLiteral("是否关胶"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("closeAdvance"), Qt::Horizontal, QStringLiteral("提前关胶"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("closeDelay"), Qt::Horizontal, QStringLiteral("延迟关胶"));
	model_glue2->setHeaderData(model_glue2->fieldIndex("type"), Qt::Horizontal, QStringLiteral("类型"));
	model_glue2->select();

	model_glue3 = new QSqlTableModel();
	// 使用 submit 时,数据库才会更改,否则做出的更改存储在缓存中
	model_glue3->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_glue3->setTable("point_glue3");
	// 设置按第0列升序排列
	model_glue3->setSort(0, Qt::AscendingOrder);
	// 更改Model对象的 头信息
	model_glue3->setHeaderData(model_glue3->fieldIndex("name"), Qt::Horizontal, QStringLiteral("名称"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("description"), Qt::Horizontal, QStringLiteral("描述"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("open"), Qt::Horizontal, QStringLiteral("是否开胶"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("openAdvance"), Qt::Horizontal, QStringLiteral("提前开胶"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("openDelay"), Qt::Horizontal, QStringLiteral("延迟开胶"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("close"), Qt::Horizontal, QStringLiteral("是否关胶"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("closeAdvance"), Qt::Horizontal, QStringLiteral("提前关胶"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("closeDelay"), Qt::Horizontal, QStringLiteral("延迟关胶"));
	model_glue3->setHeaderData(model_glue3->fieldIndex("type"), Qt::Horizontal, QStringLiteral("类型"));
	model_glue3->select();

    // 【3】 视图
	pointview = new QTableView();
	pointview->setModel(model_general);
	// 样式
	pointview->setAlternatingRowColors(true);
	pointview->setStyleSheet("QTableView{background-color: #FFFFFF;"
							 "alternate-background-color: #FFF0F5;}");
	// 设置背景网格虚线
	pointview->setShowGrid (true);
	pointview->setGridStyle(Qt::DotLine);
    // 排序功能开启
	pointview->setSortingEnabled(false);
    // 设置整行选中
	pointview->setSelectionBehavior(QAbstractItemView::SelectRows);
	// 设置列宽
	pointview->setColumnWidth(0, 25);
	pointview->setColumnWidth(1, 200);
	pointview->setColumnWidth(2, 160);
	pointview->setColumnWidth(3, 60);
	pointview->setColumnWidth(4, 60);
	pointview->setColumnWidth(5, 60);
	pointview->setColumnWidth(6, 60);
	pointview->setColumnWidth(7, 60);
	pointview->setColumnWidth(8, 60);
	pointview->setColumnWidth(9, 60);
	pointview->setColumnWidth(10, 60);
	pointview->setColumnWidth(11, 60);
	pointview->setColumnWidth(12, 60);
	pointview->setColumnWidth(13, 60);
	pointview->setColumnWidth(14, 60);
	pointview->setColumnHidden(10, true);
	pointview->setColumnHidden(11, true);
	pointview->setColumnHidden(13, true);
	pointview->setColumnHidden(14, true);
    // 可弹出右键菜单
    pointview->setContextMenuPolicy(Qt::CustomContextMenu);
	// 隐藏表头
	// verticalHeader()->setVisible(false);
}

void PointDebug::setActions()
{
	action_go     = new QAction(QStringLiteral("Go"), this);
	action_teach  = new QAction(QStringLiteral("示教"), this);
	
	action_sepa   = new QAction();
	action_sepa->setSeparator(true);

	action_add    = new QAction(QStringLiteral("添加"), this);
	action_insert = new QAction(QStringLiteral("插入"), this);
	action_del    = new QAction(QStringLiteral("删除"), this);
	action_save   = new QAction(QStringLiteral("保存"), this);

	list_action  << action_go << action_teach << action_sepa
		         << action_add << action_insert << action_del << action_save;
}


// 槽 TableView
void PointDebug::setCurrentModel(int index)
{
	index_model = index;
	if (0 == index_model)	pointview->setModel(model_general);
	else if (1 == index_model) pointview->setModel(model_glue1);
	else if (2 == index_model) pointview->setModel(model_glue2);
	else if (3 == index_model) pointview->setModel(model_glue3);
	else return;
}

QSqlTableModel *PointDebug::getCurrentModel()
{
	if (0 == index_model)	return model_general;
	else if (1 == index_model) return model_glue1;
	else if (2 == index_model) return model_glue2;
	else if (3 == index_model) return model_glue3;
	else
	{
		QMessageBox::warning(NULL, "错误", QStringLiteral("设置数据库模型错误"));
		return NULL;
	}
}

void PointDebug::on_pointview_rightClicked(const QPoint &)
{
	QItemSelectionModel *select_current = pointview->selectionModel();
	if (!select_current->hasSelection())
	{
		QMenu *menu_tableView = new QMenu();
		menu_tableView->addActions(list_action);

		action_go->setEnabled(false);
		action_teach->setEnabled(false);
		action_del->setEnabled(false);
		action_insert->setEnabled(false);

		menu_tableView->exec(QCursor::pos());
	}
	else
	{
		QMenu *menu_tableView = new QMenu();
		menu_tableView->addActions(list_action);

		action_go->setEnabled(true);
		action_teach->setEnabled(true);
		action_del->setEnabled(true);
		action_insert->setEnabled(true);

		menu_tableView->exec(QCursor::pos());
	}
}

void PointDebug::on_action_go()
{
	QSqlTableModel *pointmodel = getCurrentModel();

	int row = pointview->currentIndex().row();
	
	float fx = pointmodel->record(row).value("X").toString().toFloat();
	float fy = pointmodel->record(row).value("Y").toString().toFloat();
	float fz = pointmodel->record(row).value("Z").toString().toFloat();
	// qDebug() << fx << fy << fz;

	QtConcurrent::run( [fx, fy, fz] () {
		// qDebug() << "thread" << fx << fy << fz;

		move_axis_abs(AXISNUM::X, fx);
		move_axis_abs(AXISNUM::Y, fy);
		move_axis_abs(AXISNUM::Z, fz);
		wait_axis_stop(AXISNUM::X);
		wait_axis_stop(AXISNUM::Y);
		wait_axis_stop(AXISNUM::Z);
	});
}

void PointDebug::on_action_teach()
{
	float fx_axis = get_current_pos_axis(AXISNUM::X);
	float fy_axis = get_current_pos_axis(AXISNUM::Y);
	float fz_axis = get_current_pos_axis(AXISNUM::Z);

	QString sx_axis = QString::number(fx_axis, 'f', 3);
	QString sy_axis = QString::number(fy_axis, 'f', 3);
	QString sz_axis = QString::number(fz_axis, 'f', 3);

	int row = pointview->currentIndex().row();

	QSqlTableModel *pointmodel = getCurrentModel();
	pointmodel->setData(pointmodel->index(row, 3), sx_axis);
	pointmodel->setData(pointmodel->index(row, 4), sy_axis);
	pointmodel->setData(pointmodel->index(row, 5), sz_axis);

	on_action_save();
}

void PointDebug::on_action_add()
{
	QSqlTableModel *pointmodel = getCurrentModel();

	int row_count = pointmodel->rowCount();

	QSqlRecord record_point = pointmodel->record();
	record_point.setValue("ID", row_count);
	record_point.setValue("name", "Point");
	record_point.setValue("description", "...");
	record_point.setValue("X", "0.000");
	record_point.setValue("Y", "0.000");
	record_point.setValue("Z", "0.000");
	record_point.setValue("center_X", "0.000");
	record_point.setValue("center_Y", "0.000");
	record_point.setValue("laser", false);
	record_point.setValue("open", false);
	record_point.setValue("openAdvance", 0);
	record_point.setValue("openDelay", 0);
	record_point.setValue("close", false);
	record_point.setValue("closeAdvance", 0); 
	record_point.setValue("closeDelay", 0);
	record_point.setValue("type", "line");

	pointmodel->insertRecord(row_count, record_point);
	// pointmodel->submitAll();
	on_action_save();
}

void PointDebug::on_action_del()
{
	QSqlTableModel *pointmodel = getCurrentModel();

	int ret = QMessageBox::warning(NULL, QStringLiteral("Waring"),
		QStringLiteral("你确定删除当前行吗?"),
		QMessageBox::Yes, QMessageBox::No);
	if (ret == QMessageBox::No)
	{
		pointmodel->revertAll(); //如果不删除，则撤销
	}
	else
	{
		// 【1】 获取当前行, 总行数
		int row = pointview->currentIndex().row();
		int row_count = pointmodel->rowCount();

		// 【2】 删除
		pointmodel->removeRow(row);
		pointmodel->submitAll();

		// 【3】 将下面的行 ID -1, 节约空间
		for (int i = row; i < row_count - 1; ++i)
		{
			pointmodel->setData(pointmodel->index(i, 0), i);
			pointmodel->submitAll();
			
		}
		on_action_save();
	}
}

void PointDebug::on_action_insert()
{
	QSqlTableModel *pointmodel = getCurrentModel();

	// 【1】 获取当前行, 总行数
	int row = pointview->currentIndex().row();
	int row_count = pointmodel->rowCount();

	// 【2】 对当前行,下面的行ID加1, 腾出空间
	for (int i = row_count - 1; i >= row; --i)
	{
		pointmodel->setData(pointmodel->index(i, 0), i + 1);
		pointmodel->submitAll();
	}

	// 【3】 插入一行数据
	QSqlRecord record_point = pointmodel->record();
	record_point.setValue("ID", row);
	record_point.setValue("name", "Point");
	record_point.setValue("description", "...");
	record_point.setValue("X", "0.000");
	record_point.setValue("Y", "0.000");
	record_point.setValue("Z", "0.000");
	record_point.setValue("center_X", "0.000");
	record_point.setValue("center_Y", "0.000");
	record_point.setValue("laser", false);
	record_point.setValue("open", false);
	record_point.setValue("openAdvance", 0);
	record_point.setValue("openDelay", 0);
	record_point.setValue("close", false);
	record_point.setValue("closeAdvance", 0);
	record_point.setValue("closeDelay", 0);
	record_point.setValue("type", "line");

	pointmodel->insertRecord(row, record_point);
	// pointmodel->submitAll();
	on_action_save();
}

void PointDebug::on_action_save()
{
	QSqlTableModel *pointmodel = getCurrentModel();
	pointmodel->submitAll();

	emit changedSqlModel(index_model);

	runPoint = getRunPointInfo();
	//allPoint.clear();
	//allPoint = getPointInfo();
}



// 槽 Step
void PointDebug::on_radio_continue()
{
	setMoveType(EMOVE_CP);
}

void PointDebug::on_radio_long()
{
	setMoveType(EMOVE_LONG_STEP);
}

void PointDebug::on_radio_middle()
{
	setMoveType(EMOVE_MEDIU_STEP);
}

void PointDebug::on_radio_short()
{
	setMoveType(EMOVE_SHORT_STEP);
}

void PointDebug::setMoveType(int moveType)
{
	bool    bEnable = true;
	QString str = "";

	switch (moveType)
	{
	case EMOVE_CP:
		bEnable = false;
		break;
	case EMOVE_LONG_STEP:
		str = "10.00";
		break;
	case EMOVE_MEDIU_STEP:
		str = "1.00";
		break;
	case EMOVE_SHORT_STEP:
		str = "0.10";
		break;
	default:
		break;
	}

	edit_X_step->setEnabled(bEnable);
	edit_Y_step->setEnabled(bEnable);
	edit_Z_step->setEnabled(bEnable);

	edit_X_step->setText(str);
	edit_Y_step->setText(str);
	edit_Z_step->setText(str);

}

// 槽 Move
void PointDebug::on_slider_speed_Changed(int pos)
{
	speed = pos / float(100.0);
	acc = pos / float(100.0);
	dec = pos / float(100.0);

	set_speed_mode(start_v, speed, acc, ADMODE::T);

	qDebug() << QStringLiteral("当前速度:") << speed << acc << dec;
}


void PointDebug::on_X_positive_clicked()
{
	if (!(init_card() == 1)) return;

	if (radio_continue->isChecked()) return;

	float fx = edit_X_step->text().toFloat();
	move_axis_offset(AXISNUM::X, fx);
	
}

void PointDebug::on_X_positive_pressed()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (axis_isMoving(1))
		{
			return;
		}	
		else
		{
			move_axis_continue(AXISNUM::X, 0);
		}
	}
}

void PointDebug::on_X_positive_released()
{
	if (!(init_card() == 1)) return;
	
	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (!axis_isMoving(1))
		{
			return;
		}
		else
		{
			stop_axis(AXISNUM::X);
		}
	}
}

void PointDebug::on_X_negative_clicked()
{
	if (!(init_card() == 1)) return;

	if (radio_continue->isChecked()) return;

	float fx = edit_X_step->text().toFloat();
	move_axis_offset(AXISNUM::X, -fx);
	// wait_axis_stop(AXISNUM::X);
}

void PointDebug::on_X_negative_pressed()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (axis_isMoving(1))
		{
			return;
		}
		else
		{
			move_axis_continue(AXISNUM::X, 1);
		}
	}
}

void PointDebug::on_X_negative_released()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (!axis_isMoving(1))
		{
			return;
		}
		else
		{
			stop_axis(AXISNUM::X);
		}
	}
}


void PointDebug::on_Y_positive_clicked()
{
	if (!(init_card() == 1)) return;

	if (radio_continue->isChecked()) return;

	float fy = edit_Y_step->text().toFloat();
	move_axis_offset(AXISNUM::Y, fy);
	// wait_axis_stop(AXISNUM::Y);
}

void PointDebug::on_Y_positive_pressed()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (axis_isMoving(AXISNUM::Y))
		{
			return;
		}
		else
		{
			move_axis_continue(AXISNUM::Y, 0);
		}
	}
}

void PointDebug::on_Y_positive_released()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (!axis_isMoving(AXISNUM::Y))
		{
			return;
		}
		else
		{
			stop_axis(AXISNUM::Y);
		}
	}
}

void PointDebug::on_Y_negative_clicked()
{
	if (!(init_card() == 1)) return;

	if (radio_continue->isChecked()) return;

	float fy = edit_Y_step->text().toFloat();
	move_axis_offset(AXISNUM::Y, -fy);
	// wait_axis_stop(AXISNUM::Y);
}

void PointDebug::on_Y_negative_pressed()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (axis_isMoving(AXISNUM::Y))
		{
			return;
		}
		else
		{
			move_axis_continue(AXISNUM::Y, 1);
		}
	}
}

void PointDebug::on_Y_negative_released()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (!axis_isMoving(AXISNUM::Y))
		{
			return;
		}
		else
		{
			stop_axis(AXISNUM::Y);
		}
	}
}


void PointDebug::on_Z_positive_clicked()
{
	if (!(init_card() == 1)) return;

	if (radio_continue->isChecked()) return;

	float fz = edit_Z_step->text().toFloat();
	move_axis_offset(AXISNUM::Z, fz);
	// wait_axis_stop(AXISNUM::Z);
}

void PointDebug::on_Z_positive_pressed()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (axis_isMoving(AXISNUM::Z))
		{
			return;
		}
		else
		{
			move_axis_continue(AXISNUM::Z, 0);
		}
	}
}

void PointDebug::on_Z_positive_released()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (!axis_isMoving(AXISNUM::Z))
		{
			return;
		}
		else
		{
			stop_axis(AXISNUM::Z);
		}
	}
}

void PointDebug::on_Z_negative_clicked()
{
	if (!(init_card() == 1)) return;

	if (radio_continue->isChecked()) return;

	float fz = edit_Z_step->text().toFloat();
	move_axis_offset(AXISNUM::Z, -fz);
	// wait_axis_stop(AXISNUM::Z);
}

void PointDebug::on_Z_negative_pressed()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (axis_isMoving(AXISNUM::Z))
		{
			Sleep(1);
			return;
		}
		else
		{
			move_axis_continue(AXISNUM::Z, 1);
		}
	}
}

void PointDebug::on_Z_negative_released()
{
	if (!(init_card() == 1)) return;

	if (!radio_continue->isChecked())
	{
		return;
	}
	else
	{
		if (!axis_isMoving(AXISNUM::Z))
		{
			Sleep(1);
			return;
		}
		else
		{
			stop_axis(AXISNUM::Z);
		}
	}
}


// 回原
void PointDebug::on_btn_stop()
{
	if (!(init_card() == 1)) return;

	stop_axis(AXISNUM::X);
	Sleep(1);
	stop_axis(AXISNUM::Y);
	Sleep(1);
	stop_axis(AXISNUM::Z);
}

void PointDebug::on_btn_station_home()
{
	if (!(init_card() == 1)) return;

	QtConcurrent::run([&]() {
		home_axis(AXISNUM::X);
		wait_axis_homeOk(AXISNUM::X);
	});

	QtConcurrent::run([&]() {
		home_axis(AXISNUM::Y);
		wait_axis_homeOk(AXISNUM::Y);
	});

}

void PointDebug::on_btn_x_home()
{
	// set_home_mode();
	// set_home_speed();
	if (!(init_card() == 1)) return;

	// adt8949_SetHomeMode_Ex(0, 1, 3, 0, 1, -1, 15, 20, 0);
	// adt8949_SetHomeSpeed_Ex(0, 1, 5, 20, 5, 20, 0.5);

	QtConcurrent::run([&]() { 
		home_axis(AXISNUM::X);
		wait_axis_homeOk(AXISNUM::X);
	});
}

void PointDebug::on_btn_y_home()
{
	if (!(init_card() == 1)) return;

	QtConcurrent::run([&]() {
		home_axis(AXISNUM::Y);
		wait_axis_homeOk(AXISNUM::Y);
	});
}

void PointDebug::on_btn_z_home()
{
	if (!(init_card() == 1)) return;

	QtConcurrent::run([&]() {
		home_axis(AXISNUM::Z);
		wait_axis_homeOk(AXISNUM::Z);
	});
}


// 更新当前位置
void PointDebug::thread_updateCurrentPos()
{
    // if (!(init_card() == 1)) return;

	int step_pos = 0;

	while (close_thread_updateCurrentPos == false)
	{
		switch (step_pos)
		{
		case 0:		// 等待触发
		{
			if (start_thread_updateCurrentPos == false)
			{
				Sleep(1);
				step_pos = 0;
			}
			else
			{
				step_pos = 10;
			}
		}
		break;

		case 10:	// 刷新Input
		{
			float fx_axis = get_current_pos_axis(AXISNUM::X);
			float fy_axis = get_current_pos_axis(AXISNUM::Y);
			float fz_axis = get_current_pos_axis(AXISNUM::Z);

			QString sx_axis = QString::number(fx_axis, 'f', 3);
			QString sy_axis = QString::number(fy_axis, 'f', 3);
			QString sz_axis = QString::number(fz_axis, 'f', 3);

			label_X_currentpos->setText(sx_axis);
			label_Y_currentpos->setText(sy_axis);
			label_Z_currentpos->setText(sz_axis);

			Sleep(5);
			step_pos = 10;
		}
		break;

		case 7777:	// 退出流程
		{
			start_thread_updateCurrentPos = false;
			step_pos = 0;
		}
		break;

		case 9999:	// 退出线程
		{
			// 安全退出该线程
			close_thread_updateCurrentPos = true;
		}
		break;

		default:
			break;
		}
	}

}

// 更新IO信息
void PointDebug::thread_updateInputStatus()
{
	// if (!(init_card() == 1)) return;

	int step_input = 0;

	while (close_thread_updateInputStatus == false)
	{
		switch (step_input)
		{
		case 0:		// 等待触发
		{
			if (start_thread_updateInputStatus == false)
			{
				Sleep(2);
				step_input = 0;
			}
			else
			{
				step_input = 10;
			}
		}
		break;

		case 10:	// 刷新Input
		{
			INPUT_X[0]->setStatus(!read_in_bit(4));
			INPUT_X[1]->setStatus(!read_in_bit(12));
			INPUT_X[2]->setStatus(!read_in_bit(5));
			INPUT_X[3]->setStatus(!read_in_bit(0));

			INPUT_Y[0]->setStatus(!read_in_bit(6));
			INPUT_Y[1]->setStatus(!read_in_bit(13));
			INPUT_Y[2]->setStatus(!read_in_bit(7));
			INPUT_Y[3]->setStatus(!read_in_bit(1));

			INPUT_Z[0]->setStatus(!read_in_bit(8));
			INPUT_Z[1]->setStatus(!read_in_bit(14));
			INPUT_Z[2]->setStatus(!read_in_bit(9));
			INPUT_Z[3]->setStatus(!read_in_bit(2));

			Sleep(10);

			step_input = 0;
		}
		break;

		case 8888:	// 退出流程
		{
			start_thread_updateInputStatus = false;
			step_input = 0;
		}
		break;

		case 9999:	// 退出线程
		{
			// 安全退出该线程
			close_thread_updateInputStatus = true;
		}
		break;

		default:
			break;
		}
	}

}

void PointDebug::timer_updateCurrentPos()
{
	float fx_axis = get_current_pos_axis(AXISNUM::X);
	float fy_axis = get_current_pos_axis(AXISNUM::Y);
	float fz_axis = get_current_pos_axis(AXISNUM::Z);

	QString sx_axis = QString::number(fx_axis, 'f', 3);
	QString sy_axis = QString::number(fy_axis, 'f', 3);
	QString sz_axis = QString::number(fz_axis, 'f', 3);

	if ((label_X_currentpos->text() != sx_axis)) label_X_currentpos->setText(sx_axis);
	if ((label_Y_currentpos->text() != sy_axis)) label_Y_currentpos->setText(sy_axis);
	if ((label_Z_currentpos->text() != sz_axis)) label_Z_currentpos->setText(sz_axis);
}

void PointDebug::timer_updateInputStatus()
{
	if (INPUT_X[0]->getStatus() != read_in_bit(4))  INPUT_X[0]->setStatus(read_in_bit(4));
	if (INPUT_X[1]->getStatus() != !read_in_bit(12)) INPUT_X[1]->setStatus(!read_in_bit(12));
	if (INPUT_X[2]->getStatus() != read_in_bit(5))  INPUT_X[2]->setStatus(read_in_bit(5));
	if (INPUT_X[3]->getStatus() != !read_in_bit(0))  INPUT_X[3]->setStatus(!read_in_bit(0));

	if (INPUT_Y[0]->getStatus() != read_in_bit(6))  INPUT_Y[0]->setStatus(read_in_bit(6));
	if (INPUT_Y[1]->getStatus() != !read_in_bit(13)) INPUT_Y[1]->setStatus(!read_in_bit(13));
	if (INPUT_Y[2]->getStatus() != read_in_bit(7))  INPUT_Y[2]->setStatus(read_in_bit(7));
	if (INPUT_Y[3]->getStatus() != !read_in_bit(1))  INPUT_Y[3]->setStatus(!read_in_bit(1));

	if (INPUT_Z[0]->getStatus() != read_in_bit(8))  INPUT_Z[0]->setStatus(read_in_bit(8));
	if (INPUT_Z[1]->getStatus() != !read_in_bit(14)) INPUT_Z[1]->setStatus(!read_in_bit(14));
	if (INPUT_Z[2]->getStatus() != read_in_bit(9))  INPUT_Z[2]->setStatus(read_in_bit(9));
	if (INPUT_Z[3]->getStatus() != !read_in_bit(2))  INPUT_Z[3]->setStatus(!read_in_bit(2));
}


// 获取当前选中的Model的数据
QMap<QString, PointRun> PointDebug::getCurrentModelPointInfo()
{
	QSqlTableModel *pointmodel = getCurrentModel();

	QMap<QString, PointRun> _allPoint;
	int index = 0;
	for (index; index < pointmodel->rowCount(); index++)
	{
		QString name = pointmodel->record(index).value("name").toString();
		QString description = pointmodel->record(index).value("description").toString();
		float X = pointmodel->record(index).value("X").toString().toFloat();
		float Y = pointmodel->record(index).value("Y").toString().toFloat();
		float Z = pointmodel->record(index).value("Z").toString().toFloat();

		PointRun point; // = { name, description, X, Y, Z, open, openAdvance, openDelay, close, closeAdvance, closeDelay, type };
		point.name = name;
		point.description = description;
		point.X = X;
		point.Y = Y;
		point.Z = Z;
		_allPoint.insert(name, point);
	}
	return _allPoint;
}

PointRun PointDebug::get_point_name(QString name)
{
	QMap<QString, PointRun>::iterator Point_it;
	Point_it = currentModelPoint.find(name);
	PointRun point = Point_it.value();
	return point;
}

PointRun PointDebug::get_point_index(int index)
{
	QSqlTableModel *pointmodel = getCurrentModel();

	QAbstractItemModel *abstractmodel = pointview->model();
	pointmodel->setParent(abstractmodel);

	QString name = pointmodel->record(index).value("name").toString();

	QMap<QString, PointRun>::iterator Point_it;
	Point_it = currentModelPoint.find(name);
	PointRun point = Point_it.value();
	return point;
}


// 获取所有的 RunPoint
QMap<QString, PointRun> PointDebug::getRunPointInfo()
{
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


int PointDebug::move_thread_xyz(float x_pos, float y_pos, float z_pos)
{
	move_axis_abs(AXISNUM::X, x_pos);
	move_axis_abs(AXISNUM::Y, y_pos);
	move_axis_abs(AXISNUM::Z, z_pos);
	wait_axis_stop(AXISNUM::X);
	wait_axis_stop(AXISNUM::Y);
	wait_axis_stop(AXISNUM::Z);

	return 0;
}