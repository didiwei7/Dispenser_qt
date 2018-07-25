#ifndef IO_H
#define IO_H

#include <QWidget>
#include <QtWidgets>
#include <QtConcurrent>

// 控制卡
#include "../adt/adtcontrol.h"

#include "../../sdk/QMyPlugin/qmyplugin.h"
class QInputLabel;
class QOutputButton;

#define IN_COUNT  36
#define OUT_COUNT 18
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
	void setTimer();

private:
	void setInput();
	void setOutput();
	void setIOConnect();

private:
	void timer_updateInputStatus();
	void timer_updateOutputStatus();
	
private:
	void  on_btn_output();
	void  changeOutputStatus(int bit);
	
	QHBoxLayout *layout_1;
	QVBoxLayout *layout_2_1;
	QVBoxLayout *layout_2_2;
	QVBoxLayout *layout_2_3;

    QInputLabel   *INPUT[IN_COUNT];
	QOutputButton *OUTPUT[OUT_COUNT];
};

#endif // IO_H
