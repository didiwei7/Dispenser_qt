#ifndef IO_H
#define IO_H

#include <QWidget>
#include <QtWidgets>
#include <QtConcurrent>

// 控制卡
#include "../../adt/adtcontrol.h"

#include "../../sdk/QIO/qio.h"
class QInputLabel;
class QOutputButton;

#define IN_COUNT  36
#define OUT_COUNT 16
#define IN_VISIBLE_BEGIN  16
#define OUT_VISIBLE_BEGIN 0

class IO : public QWidget
{
    Q_OBJECT
public:
    explicit IO(QWidget *parent = nullptr);
	~IO();

private:
	void setupUi();
	void setConnect();
	void setThread();

private:
	void setInput();
	void setOutput();
	void setIOConnect();

private:
	QThreadPool thread_pool;

	bool is_updateInputStatus_ok;
	bool start_thread_updateInputStatus;
	bool close_thread_updateInputStatus;
	QFuture<void> future_thread_updateInputStatus;
	void thread_updateInputStatus();

	void updateOutputStatus();
		
private:
	void  on_btn_output();
	void  changeOutputStatus(int bit);
	
	QHBoxLayout *layout_1;
	QVBoxLayout *layout_2_1;
	QVBoxLayout *layout_2_2;
	QVBoxLayout *layout_2_3;

    QInputLabel   *INPUT[IN_COUNT];
	QOutputButton *OUTPUT[OUT_COUNT];
	// 为防止以后会连接不同的槽函数, 暂时分开写
	//QOutputButton *OUT0;
	//QOutputButton *OUT1;
	//QOutputButton *OUT2;
	//QOutputButton *OUT3;
	//QOutputButton *OUT4;
	//QOutputButton *OUT5;
	//QOutputButton *OUT6;
	//QOutputButton *OUT7;
	//QOutputButton *OUT8;
	//QOutputButton *OUT9;
	//QOutputButton *OUT10;
	//QOutputButton *OUT11;
	//QOutputButton *OUT12;
	//QOutputButton *OUT13;
	//QOutputButton *OUT14;
};

#endif // IO_H
