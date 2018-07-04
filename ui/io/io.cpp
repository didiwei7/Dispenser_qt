#include "io.h"

IO::IO(QWidget *parent) : QWidget(parent)
{
	// 【1】 初始化Ui
	setupUi();

	// 【2】 更新输出状态
	// updateOutputStatus();

	// 【3】 设置线程
	// setThread();

	// 【4】 连接信号槽
	setConnect();

	// 【5】 设置计时器
	setTimer();
}

IO::~IO()
{
	//close_thread_updateInputStatus = true;

	//thread_pool.waitForDone();
	//thread_pool.clear();
	//thread_pool.destroyed();
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
        connect(OUTPUT[i], &QOutputButton::wclicked, this, &IO::on_btn_output);
	}
}

void IO::setThread()
{
	is_updateInputStatus_ok = false;
	start_thread_updateInputStatus = true;
	close_thread_updateInputStatus = false;

	thread_pool.setMaxThreadCount(1);

	future_thread_updateInputStatus = QtConcurrent::run(&thread_pool, [&]() { thread_updateInputStatus(); });
	// qDebug() << thread_updateOutputStatus.isRunning();
}

void IO::setTimer()
{
	QTimer *timer_input = new QTimer(this);
	connect(timer_input, &QTimer::timeout, this, &IO::timer_updateInputStatus);
	timer_input->start(10);

	QTimer *timer_output = new QTimer(this);
	connect(timer_output, &QTimer::timeout, this, &IO::timer_updateOutputStatus);
	timer_output->start(300);
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
	if (!file.exists())
	{
		return;
	}

	file.open((QIODevice::ReadOnly));
	QByteArray date = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(date);
	QJsonObject obj = doc.object();

	QJsonObject outobj = obj.value("Card0").toObject().value("Input").toObject();
	for ( int i = 0; i < IN_COUNT; i++)
	{
		QString str_i = QString::number(i, 10);
		QString str_i_objname = "INPUT" + str_i;
		// qDebug() << outobj.value(str_i).toObject().value("name").toString();
        INPUT[i] = new QInputLabel(outobj.value(str_i).toObject().value("name").toString(), 0, this);
		INPUT[i]->setVisible(outobj.value(str_i).toObject().value("visible").toBool());
		INPUT[i]->setObjectName(str_i_objname);
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
	if (!file.exists())
	{
		return;
	}

	file.open((QIODevice::ReadOnly));
	QByteArray date = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(date);
	QJsonObject obj = doc.object();

	QJsonObject outobj = obj.value("Card0").toObject().value("Output").toObject();
	for (int i = 0; i < OUT_COUNT; i++)
	{
		QString str_i = QString::number(i, 10);
		QString str_i_objname = "OUTPUT" + str_i;
		// qDebug() << outobj.value(str_i).toObject().value("name").toString();
        OUTPUT[i] = new QOutputButton(outobj.value(str_i).toObject().value("name").toString());
        OUTPUT[i]->setVisible(outobj.value(str_i).toObject().value("visible").toBool());
		OUTPUT[i]->setObjectName(str_i_objname);
		// qDebug() << OUTPUT[i]->objectName();
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

void IO::thread_updateInputStatus()
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
			for (int i = IN_VISIBLE_BEGIN; i < IN_COUNT; i++)
			{
				INPUT[i]->setStatus(!read_in_bit(i));
			}

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

void IO::timer_updateInputStatus()
{
	for (int i = IN_VISIBLE_BEGIN; i < IN_COUNT; i++)
	{
		// 不取反, 急停信号
		if (16 == i)
		{
			if (INPUT[i]->getStatus() != read_in_bit(i)) INPUT[i]->setStatus(read_in_bit(i));
		}
		else
		{
			if (INPUT[i]->getStatus() != !read_in_bit(i)) INPUT[i]->setStatus(!read_in_bit(i));
		}
	}
}

void IO::timer_updateOutputStatus()
{
	for (int i = OUT_VISIBLE_BEGIN; i < OUT_COUNT; i++)
	{
		if (OUTPUT[i]->getStatus() != read_out_bit(i)) OUTPUT[i]->setStatus(read_out_bit(i));
	}
}

void IO::updateOutputStatus() // 读取一次
{
	if (init_card() == 1)
	{
		for (int i = OUT_VISIBLE_BEGIN; i < OUT_COUNT; i++)
		{
			OUTPUT[i]->setStatus(read_out_bit(i));
		}
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
	if (adt8949_get_out(0, bit) == 1)
	{
		adt8949_write_bit(0, bit, 0);
		//Sleep(5);
		//if (adt8949_get_out(0, bit) == 0)
		//{
		//	OUTPUT[bit]->setStatus(0);
		//}
		//else
		//{
		//	qDebug() << QStringLiteral("IO: 输出信号有误, 请检查");
		//}	
	}
	else
	{
		// 调试用
		adt8949_write_bit(0, bit, 1);
		// OUTPUT[bit]->setStatus(1);

		// 正确的代码
		//adt8949_write_bit(0, bit, 1);
		//Sleep(5);
		//if (adt8949_get_out(0, bit) == 1)
		//{
		//	OUTPUT[bit]->setStatus(1);
		//}
		//else
		//{
		//	qDebug() << QStringLiteral("IO: 输出信号有误, 请检查");
		//}
	}
}
