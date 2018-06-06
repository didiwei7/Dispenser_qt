#include "mainwindow.h"

QtCentral::QtCentral(QWidget* parent) : QWidget(parent)
{
    qApp->setProperty("window", QVariant::fromValue<QObject*>(this));

    // 【0】
    this->setupUi();

    // 【1】 设置窗口
    this->setObjectName("Central");

    // 【2】 开启空布局
    QHBoxLayout* box = new QHBoxLayout(this);
    box->setContentsMargins(0, 0, 0, 0);

    // 【3】 连接信号槽
    connect(FlexManager::instance(), SIGNAL(flexWidgetCreated(FlexWidget*)), SLOT(on_flexWidgetCreated(FlexWidget*)));
    connect(FlexManager::instance(), SIGNAL(flexWidgetDestroying(FlexWidget*)), SLOT(on_flexWidgetDestroying(FlexWidget*)));
    connect(FlexManager::instance(), SIGNAL(dockWidgetCreated(DockWidget*)), SLOT(on_dockWidgetCreated(DockWidget*)));
    connect(FlexManager::instance(), SIGNAL(dockWidgetDestroying(DockWidget*)), SLOT(on_dockWidgetDestroying(DockWidget*)));

    // 【4】 载入QSettings
    QSettings  settings("QtFlex5", "QtGuider");
    QByteArray content = settings.value("Flex").toByteArray();
    QMap<QString, QWidget*> parents;
    parents[objectName()] = this;
    if (content.isEmpty())
    {
        auto content = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "M");
        layout()->addWidget(content);
    }
    else
    {
        FlexManager::instance()->load(content, parents);
    }
}

void QtCentral::setupUi()
{
    // 初始化widget对象
    w_mainwidget = new MainWidget();
    w_vision     = new Vision();
    w_photo      = new Photo();
}

void QtCentral::on_flexWidgetCreated(FlexWidget* flexWidget)
{
    if (flexWidget->objectName() == "M")
    {
        layout()->addWidget(flexWidget);
    }
    else
    {
        flexWidget->show();
    }
}

void QtCentral::on_flexWidgetDestroying(FlexWidget* )
{
}

void QtCentral::on_dockWidgetCreated(DockWidget *dockWidget)
{
    if (dockWidget->objectName() == QStringLiteral("主界面"))
    {
        if (dockWidget->widget() != w_mainwidget)
        {
            dockWidget->setWidget(w_mainwidget);
        }
    }

    if (dockWidget->objectName() == QStringLiteral("视觉"))
    {
        if (dockWidget->widget() != w_vision)
        {
            dockWidget->setWidget(w_vision);
        }
    }

    if (dockWidget->objectName() == QStringLiteral("图像"))
    {
        if (dockWidget->widget() != w_photo)
        {
            dockWidget->setWidget(w_photo);
        }
    }
}

void QtCentral::on_dockWidgetDestroying(DockWidget *dockWidget)
{
    if (dockWidget->objectName() == QStringLiteral("主界面"))
    {
        if (dockWidget->widget() == w_mainwidget)
        {
            dockWidget->detachWidget(w_mainwidget);
        }
    }

    if (dockWidget->objectName() == QStringLiteral("视觉"))
    {
        if (dockWidget->widget() == w_vision)
        {
            dockWidget->detachWidget(w_vision);
        }
    }

    if (dockWidget->objectName() == QStringLiteral("图像"))
    {
        if (dockWidget->widget() == w_photo)
        {
            dockWidget->detachWidget(w_photo);
        }
    }

//    if (dockWidget->objectName() == QStringLiteral("音乐"))
//    {
//        if (dockWidget->widget() == w_music)
//        {
//            dockWidget->detachWidget(w_music);
//        }
//    }
}



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
	// 【1】 初始化板卡
	init_card();



    setupUi();
    setExtras();
    setConnect();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("您确认退出吗?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (ret == QMessageBox::Yes)
    {
        event->accept();

        // 【1】 写配置文件
        writeSettings();

        // 【2】 写注册表
        QByteArray content = FlexManager::instance()->save();
        QSettings settings("QtFlex5", "QtGuider");
        settings.setValue("Flex", content);
        FlexManager::instance()->close();
    
		// 【3】 关闭控制卡
		adt8949_close_card();
	}
    else
    {
        event->ignore();
    }
}


// Init
void MainWindow::setupUi()
{
    // 【0】 初始化Menu, Action
    setMenubar();
	setStateBar();
    setShortcuts();

    // 【1】 载入QFlex样式.
    qApp->setProperty("window", QVariant::fromValue<QObject*>(this));

    // 【2】 载入中心窗口
    wQtCentral = new QtCentral(this);
    this->setCentralWidget(wQtCentral);

    // 【3】 读配置文件
    readSettings();
}

void MainWindow::setExtras()
{

}

void MainWindow::setConnect()
{
    // 【1】 Menubar
    connect(action_file_new,  &QAction::triggered, this, &MainWindow::on_action_file_new);
    connect(action_file_open, &QAction::triggered, this, &MainWindow::on_action_file_open);
    connect(action_file_save, &QAction::triggered, this, &MainWindow::on_action_file_save);
    connect(action_file_save_all, &QAction::triggered, this, &MainWindow::on_action_file_save_all);
    connect(action_file_exit, &QAction::triggered, this, &MainWindow::close);

    connect(action_view_mainwidget, &QAction::triggered, this, &MainWindow::on_action_view_mainwidget);
    connect(action_view_vision, &QAction::triggered, this, &MainWindow::on_action_view_vision);
    connect(action_view_photo, &QAction::triggered, this, &MainWindow::on_action_view_photo);

    connect(action_help_help, &QAction::triggered, this, &MainWindow::on_action_help_help);
    connect(action_help_about, &QAction::triggered, this, &MainWindow::on_action_help_about);
    connect(action_help_aboutQt, &QAction::triggered, this, &MainWindow::on_action_help_aboutQt);
}

void MainWindow::writeSettings()
{
    QSettings setting("../config/settings.ini", QSettings::IniFormat);

    // 【1】 窗口大小, 位置
    setting.beginGroup("MainWindow");
    setting.setValue("x", x());
    setting.setValue("y", y());
    setting.setValue("width", width());
    setting.setValue("height", height());
    setting.setValue("size", size());
    setting.setValue("pos", pos());
    setting.endGroup();
}

void MainWindow::readSettings()
{
    // 【1】 判断是否存在改配置文件
    QFile file("../config/settings.ini");
    if (!file.exists())
    {
        this->setGeometry(QApplication::desktop()->availableGeometry().adjusted(100, 100, -100, -100));
    }
    else
    {
        QSettings setting("../config/settings.ini", QSettings::IniFormat);

        // 【2】 窗口大小, 位置
        resize(setting.value("MainWindow/size").toSize());
        move(setting.value("MainWindow/pos").toPoint());
    }
}

// Statusbar
void MainWindow::setStateBar()
{
	QFont font;
	font.setFamily("MicroSoft Yahei");
	font.setPointSize(9);
	label_time.setFont(font);

	// statusBar()->setStyleSheet("background-color: #007ACC;");
	statusBar()->addPermanentWidget(&label_time);

	QTimer *timer = new QTimer(this);
	timer->start(1000); 
	connect(timer, &QTimer::timeout, this, &MainWindow::on_label_time);
}

void MainWindow::on_label_time()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd  hh:mm:ss "));
	label_time.setText(s_currentTime);
}


// Menubar
void MainWindow::setMenubar()
{
    action_file_new      = new QAction(QStringLiteral("新建"), this);
    action_file_open     = new QAction(QStringLiteral("打开"), this);
    action_file_save     = new QAction(QStringLiteral("保存"), this);
    action_file_save_all = new QAction(QStringLiteral("保存全部"), this);
    action_file_exit     = new QAction(QStringLiteral("退出"), this);

    action_view_mainwidget = new QAction(QStringLiteral("主界面"), this);
    action_view_vision          = new QAction(QStringLiteral("视觉"), this);
    action_view_photo          = new QAction(QStringLiteral("图像"), this);
    action_view_4          = new QAction(QStringLiteral("视图4"), this);

    action_help_help    = new QAction(QStringLiteral("帮助"));
    action_help_about   = new QAction(QStringLiteral("关于"));
    action_help_aboutQt = new QAction(QStringLiteral("关于Qt"));

    action_login_login = new QAction(QStringLiteral("登陆"));

    QList<QAction*> list_action_file;
    QList<QAction*> list_action_view;
    QList<QAction*> list_action_help;
    QList<QAction*> list_action_login;

    list_action_file << action_file_new << action_file_open << action_file_save
                     << action_file_save_all << action_file_exit;
    list_action_view << action_view_mainwidget
                     << action_view_vision << action_view_photo << action_view_4;
    list_action_help << action_help_help << action_help_about << action_help_aboutQt;
    list_action_login << action_login_login;

    QMenu *menu_file = new QMenu(QStringLiteral("文件(&F)"));
    QMenu *menu_view = new QMenu(QStringLiteral("视图(&V)"));
    QMenu *menu_help = new QMenu(QStringLiteral("帮助(&H)"));
    QMenu *menu_login = new QMenu(QStringLiteral("登陆(&L)"));
    menu_file->addActions(list_action_file);
    menu_view->addActions(list_action_view);
    menu_help->addActions(list_action_help);
    menu_login->addActions(list_action_login);

    this->menuBar()->addMenu(menu_file);
    this->menuBar()->addMenu(menu_view);
    this->menuBar()->addMenu(menu_help);
    this->menuBar()->addMenu(menu_login);
}

void MainWindow::setShortcuts()
{
    action_file_new      ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    action_file_open     ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    action_file_save     ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    action_file_save_all ->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    action_file_exit     ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    action_view_mainwidget ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    action_view_vision ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    action_view_photo ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    action_view_4 ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));

    action_help_help   ->setShortcut(QKeySequence(Qt::Key_F1));
    action_help_about  ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F1));
    action_help_aboutQt->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F1));

    action_login_login->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
}

// WinExtras
void MainWindow::setDWM()
{

}

void MainWindow::setTaskbar()
{

}

void MainWindow::setThumbnailToolBar()
{

}

void MainWindow::setJumpList()
{

}


void MainWindow::on_action_file_new()
{

}

void MainWindow::on_action_file_open()
{

}

void MainWindow::on_action_file_save()
{

}

void MainWindow::on_action_file_save_all()
{

}

void MainWindow::on_action_view_mainwidget()
{
    QString dockWidgetName = QStringLiteral("主界面");
    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, flexWidget, Flex::widgetFlags(), dockWidgetName);

            dockWidget->setViewMode(Flex::ToolView);
            dockWidget->setWindowTitle(dockWidgetName);

            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();

            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void MainWindow::on_action_view_vision()
{
    QString dockWidgetName = QStringLiteral("视觉");
    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, flexWidget, Flex::widgetFlags(), dockWidgetName);

            dockWidget->setViewMode(Flex::ToolView);
            dockWidget->setWindowTitle(dockWidgetName);

            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();

            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void MainWindow::on_action_view_photo()
{
    QString dockWidgetName = QStringLiteral("图像");
    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, flexWidget, Flex::widgetFlags(), dockWidgetName);

            dockWidget->setViewMode(Flex::ToolView);
            dockWidget->setWindowTitle(dockWidgetName);

            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();

            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void MainWindow::on_action_view_4()
{

}

void MainWindow::on_action_help_help()
{
    QMessageBox::about(NULL, QStringLiteral("帮助"), QStringLiteral("快捷键：\n"
                                                                  "帮助\t F1\n"
                                                                  "关于\t Ctrl + F1\n"
                                                                  "关于Qt\t Ctrl + Shift + F1\n"
                                                                  "新建\t Ctrl + N\n"
                                                                  "打开\t Ctrl + O\n"
                                                                  "保存\t Ctrl + S\n"
                                                                  "保存所有\t Ctrl + Shift + S\n"
                                                                  "退出\t Ctrl + Q\n"
                                                                  "视图1\t Ctrl + 1\n"
                                                                  "视图2\t Ctrl + 2\n"
                                                                  "视图3\t Ctrl + 3\n"
                                                                  "视图4\t Ctrl + 4\n"));
}

void MainWindow::on_action_help_about()
{
    QMessageBox::about(NULL, QStringLiteral("关于"), QStringLiteral("点胶机-泰达机器人\n"
                                                                   "作者:\t 敌敌畏\n"
                                                                   "邮箱:\t 15927513928@163.com"));
}

void MainWindow::on_action_help_aboutQt()
{
    QMessageBox::aboutQt(NULL, QStringLiteral("关于Qt"));
}
