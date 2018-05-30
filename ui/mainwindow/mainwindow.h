#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtWinExtras>
#include <QtWin>

#include <QFile>
#include <QString>
#include <QDebug>

#include "mainwidget.h"
#include "../vision/vision.h"
#include "../photo/photo.h"

// QFlex
#include "../../sdk/QFlex/QtDockGuider.h"
#include "../../sdk/QFlex/QtDockWidget.h"
#include "../../sdk/QFlex/QtDockSide.h"
#include "../../sdk/QFlex/QtDockSite.h"
#include "../../sdk/QFlex/QtFlexHelper.h"
#include "../../sdk/QFlex/QtFlexManager.h"
#include "../../sdk/QFlex/QtFlexStyle.h"
#include "../../sdk/QFlex/QtFlexWidget.h"
//#include "QFlex/QtDockGuider.h"
//#include "QFlex/QtDockWidget.h"
//#include "QFlex/QtDockSide.h"
//#include "QFlex/QtDockSite.h"
//#include "QFlex/QtFlexHelper.h"
//#include "QFlex/QtFlexManager.h"
//#include "QFlex/QtFlexStyle.h"
//#include "QFlex/QtFlexWidget.h"
class QtGuiderImpl;
class FlexWidget;
class DockWidget;


class QtCentral : public QWidget
{
    Q_OBJECT
public:
    QtCentral(QWidget* parent);

public:
    void setupUi();

private slots:
    void on_flexWidgetCreated(FlexWidget*);
    void on_flexWidgetDestroying(FlexWidget*);

    void on_dockWidgetCreated(DockWidget *);
    void on_dockWidgetDestroying(DockWidget *);

public:
    MainWidget *w_mainwidget;
    Vision     *w_vision;
    Photo      *w_photo;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void setupUi();
    void setExtras();
    void setConnect();

    // Menubar
    void setMenubar();
	void setStateBar();
    void setShortcuts();

    // 基于Windows的附加属性
    void setDWM();
    void setTaskbar();
    void setThumbnailToolBar();
    void setJumpList();

private:   // 读写配置
    void writeSettings();
    void readSettings();

private:    // Menubar
    void on_action_file_new();
    void on_action_file_open();
    void on_action_file_save();
    void on_action_file_save_all();

    void on_action_view_mainwidget();
    void on_action_view_vision();
    void on_action_view_photo();
    void on_action_view_4();

    void on_action_help_help();
    void on_action_help_about();
    void on_action_help_aboutQt();

    void on_action_login_login();

    QAction *action_file_new;
    QAction *action_file_open;
    QAction *action_file_save;
    QAction *action_file_save_all;
    QAction *action_file_exit;

    QAction *action_view_mainwidget;
    QAction *action_view_vision;
    QAction *action_view_photo;
    QAction *action_view_4;

    QAction *action_help_help;
    QAction *action_help_about;
    QAction *action_help_aboutQt;

    QAction *action_login_login;

private:
	void on_label_time();
	QLabel label_time;

public:
	QtCentral *wQtCentral;

public:     // 公开给其他类使用
    void on_taskbarProgressDurationChanged(qint64 duration);
    void on_taskbarProgressPositionChanged(qint64 position);
    void on_taskbarButtonChanged(int state);

    QWinTaskbarButton       *taskbarBtn;
    QWinTaskbarProgress     *taskbarProgress;
    QWinThumbnailToolBar    *thumbnailToolBar;
    QWinThumbnailToolButton *thumbnailBtn_playPause;
    QWinThumbnailToolButton *thumbnailBtn_previous;
    QWinThumbnailToolButton *thumbnailBtn_next;
    QWinJumpList            *jumplist;
    QWinJumpListCategory    *jumplistCategory_playlist;
    QWinJumpListCategory    *jumplistCategory_startUp;
    QWinJumpListCategory    *jumplistCategory_folder;
};

#endif // MAINWINDOW_H
