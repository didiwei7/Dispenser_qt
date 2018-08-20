#include "io.h"


IO::IO(QWidget *parent) : QWidget(parent)
{
	// 【1】 初始化Ui
	setupUi();

	// 【2】 连接信号槽
	setConnect();

	// 【3】 设置计时器
	setTimer();
}

IO::~IO()
{
}

void IO::setupUi()
{
    layout_1   = new QHBoxLayout();
    layout_2_1 = new QVBoxLayout();
    layout_2_2 = new QVBoxLayout();
    layout_2_3 = new QVBoxLayout();

	layout_1->addLayout(layout_2_1);
	layout_1->addSpacing(100);
	layout_1->addLayout(layout_2_2);
	layout_1->addSpacing(100);
	layout_1->addLayout(layout_2_3);
	layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	setLayout(layout_1);

	setInput();
	setOutput();
	setIOConnect();
}

void IO::setConnect()
{
	for (int i = OUT_VISIBLE_BEGIN; i < OUT_COUNT; i++)
	{
        connect(OUTPUT[i], &QOutputButton::qclicked, this, &IO::on_btn_output);
	}
}

void IO::setTimer()
{
	QTimer *timer_input = new QTimer(this);
	connect(timer_input, &QTimer::timeout, this, &IO::timer_updateInputStatus);
	timer_input->start(10);

	QTimer *timer_output = new QTimer(this);
	connect(timer_output, &QTimer::timeout, this, &IO::timer_updateOutputStatus);
	timer_output->start(50);
}

void IO::setInput()
{
    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPointSize(15);
    font.setBold(true);

	// 【1】 标题栏
    QLabel *label_title = new QLabel(QStringLiteral("通用输入:"));
    label_title->setFont(font);
    layout_2_1->addWidget(label_title);
    layout_2_1->addSpacing(10);

	// 【2】 Input
	QFile file("../config/io.json");
	if (!file.exists()) return;
	file.open((QIODevice::ReadOnly));
	QByteArray date = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(date);
	QJsonObject obj = doc.object().value("Card0").toObject().value("Input").toObject();

	for ( int i = 0; i < IN_COUNT; i++)
	{
		QString str_i = QString::number(i, 10);
		QString str_i_objname = "INPUT" + str_i;
        INPUT[i] = new QInputLabel(this);
		INPUT[i]->setObjectName(str_i_objname);
		INPUT[i]->setVisible(obj.value(str_i).toObject().value("visible").toBool());
		INPUT[i]->setName(obj.value(str_i).toObject().value("name").toString());
		INPUT[i]->setStatus(obj.value(str_i).toObject().value("status").toInt());
		layout_2_1->addWidget(INPUT[i]);
	}
	file.close();

    layout_2_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void IO::setOutput()
{
    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPointSize(15);
    font.setBold(true);

	// 【1】 标题栏
    QLabel *label_title = new QLabel(QStringLiteral("通用输出:"));
    label_title->setFont(font);
    layout_2_2->addWidget(label_title);
    layout_2_2->addSpacing(10);

	// 【2】 Output
	QFile file("../config/io.json");
	if (!file.exists()) return;
	file.open((QIODevice::ReadOnly));
	QByteArray date = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(date);
	QJsonObject obj = doc.object().value("Card0").toObject().value("Output").toObject();

	for (int i = 0; i < OUT_COUNT; i++)
	{
		QString str_i = QString::number(i, 10);
		QString str_i_objname = "OUTPUT" + str_i;
		OUTPUT[i] = new QOutputButton(this);
		OUTPUT[i]->setObjectName(str_i_objname);
		OUTPUT[i]->setVisible(obj.value(str_i).toObject().value("visible").toBool());
		OUTPUT[i]->setName(obj.value(str_i).toObject().value("name").toString());
		OUTPUT[i]->setStatus(obj.value(str_i).toObject().value("status").toInt());
		layout_2_2->addWidget(OUTPUT[i]);
	}
	file.close();

	layout_2_2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void IO::setIOConnect()
{
    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPointSize(15);
    font.setBold(true);

    QLabel *label_title = new QLabel(QStringLiteral("输入输出关联:"));
    label_title->setFont(font);

    layout_2_3->addWidget(label_title);
    layout_2_3->addSpacing(10);
    layout_2_3->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void IO::timer_updateInputStatus()
{
	for (int i = IN_VISIBLE_BEGIN; i < IN_COUNT; i++)
	{
		// 不取反的输入: 急停, 
		if (16 == i)
		{
			INPUT[i]->setStatus(read_in_bit(i));	// 已在控件中处理, 改变时更新
		}
		else
		{
			INPUT[i]->setStatus(!read_in_bit(i));
		}
	}
}

void IO::timer_updateOutputStatus()
{
	for (int i = OUT_VISIBLE_BEGIN; i < OUT_COUNT; i++)
	{
		// if (OUTPUT[i]->getStatus() != read_out_bit(i)) OUTPUT[i]->setStatus(read_out_bit(i));
		OUTPUT[i]->setStatus(read_out_bit(i));		// 已在控件中处理, 改变时更新控件状态.
	}
}

void IO::on_btn_output()
{
	QOutputButton* out = qobject_cast<QOutputButton*>(sender());
	if ("OUTPUT0" == out->objectName()) 
	{
		qDebug() << "OUTPUT0";
		changeOutputStatus(0);
	}
	else if ("OUTPUT1" == out->objectName()) changeOutputStatus(1);
	else if ("OUTPUT2" == out->objectName()) changeOutputStatus(2);
	else if ("OUTPUT3" == out->objectName()) changeOutputStatus(3);
	else if ("OUTPUT4" == out->objectName()) changeOutputStatus(4);
	else if ("OUTPUT5" == out->objectName()) changeOutputStatus(5);
	else if ("OUTPUT6" == out->objectName()) changeOutputStatus(6);
	else if ("OUTPUT7" == out->objectName()) changeOutputStatus(7);
	else if ("OUTPUT8" == out->objectName()) changeOutputStatus(8);
	else if ("OUTPUT9" == out->objectName()) changeOutputStatus(9);
	else if ("OUTPUT10" == out->objectName()) changeOutputStatus(10);
	else if ("OUTPUT11" == out->objectName()) changeOutputStatus(11);
	else if ("OUTPUT12" == out->objectName()) changeOutputStatus(12);
	else if ("OUTPUT13" == out->objectName()) changeOutputStatus(13);
	else if ("OUTPUT14" == out->objectName()) changeOutputStatus(14);
	else if ("OUTPUT15" == out->objectName()) changeOutputStatus(15);
	else if ("OUTPUT16" == out->objectName()) changeOutputStatus(16);
	else if ("OUTPUT17" == out->objectName()) changeOutputStatus(17);
	else
	{
		qDebug() << QStringLiteral("IO: 输出信号有误, 请检查");
	}

	// 避免输出需要做额外的处理, 暂时不适用 for 循环
	/*	for ( int i = 0; i < OUT_COUNT; i++)
	{
		QString str = QString("OUTPUT%1").arg(i);
		if (str == out->objectName()) changeOutputStatus(i);
	}*/

}

void IO::changeOutputStatus(int bit)
{
	if (read_out_bit(bit) == 1)
	{
		write_out_bit(bit, 0);
	}
	else
	{
		write_out_bit(bit, 1);
	}
}
