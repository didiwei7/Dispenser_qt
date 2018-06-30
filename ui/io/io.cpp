#include "io.h"

IO::IO(QWidget *parent) : QWidget(parent)
{
	// 【1】 初始化Ui
	setupUi();

	// 【2】 更新输出状态
	updateOutputStatus();

	// 【3】 设置线程
	setThread();

	// 【4】 连接信号槽
	setConnect();
}

IO::~IO()
{
	close_thread_updateInputStatus = true;

	thread_pool.waitForDone();
	thread_pool.clear();
	thread_pool.destroyed();
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

	// readConfig();
	// QPushButton *btn = findChild<QPushButton*>("dispen");
	// btn->connect(btn, &QPushButton::clicked, this, &IO::updateIoStatus);

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

    /*INPUT[0] = new QInputLabel(QStringLiteral("X轴报警"));
    INPUT[1] = new QInputLabel(QStringLiteral("Y轴报警"));
    INPUT[2] = new QInputLabel(QStringLiteral("Z轴报警"));
    INPUT[3] = new QInputLabel(QStringLiteral("A轴报警"));
    INPUT[4] = new QInputLabel(QStringLiteral("X正限位"));
    INPUT[5] = new QInputLabel(QStringLiteral("X负限位"));
    INPUT[6] = new QInputLabel(QStringLiteral("Y正限位"));
    INPUT[7] = new QInputLabel(QStringLiteral("Y负限位"));
    INPUT[8] = new QInputLabel(QStringLiteral("Z正限位"));
    INPUT[9] = new QInputLabel(QStringLiteral("Z负限位"));
    INPUT[10] = new QInputLabel(QStringLiteral("A正限位"));
    INPUT[11] = new QInputLabel(QStringLiteral("A负限位"));
    INPUT[12] = new QInputLabel(QStringLiteral("X原点"));
    INPUT[13] = new QInputLabel(QStringLiteral("Y原点"));
    INPUT[14] = new QInputLabel(QStringLiteral("Z原点"));
    INPUT[15] = new QInputLabel(QStringLiteral("A原点"));
    INPUT[16] = new QInputLabel(QStringLiteral("急停"));
    INPUT[17] = new QInputLabel(QStringLiteral("启动"));
    INPUT[18] = new QInputLabel(QStringLiteral("点胶"));
    INPUT[19] = new QInputLabel(QStringLiteral("擦胶"));
    INPUT[20] = new QInputLabel(QStringLiteral("复位"));
    INPUT[21] = new QInputLabel(QStringLiteral("照明"));
    INPUT[22] = new QInputLabel(QStringLiteral("擦胶-无尘布感应"));
    INPUT[23] = new QInputLabel(QStringLiteral("擦胶-A轴行程感应"));
    INPUT[24] = new QInputLabel(QStringLiteral("擦胶-擦胶感应器"));
    INPUT[25] = new QInputLabel(QStringLiteral("擦胶-气缸推进感应"));
    INPUT[26] = new QInputLabel(QStringLiteral("擦胶-气缸后退感应"));
    INPUT[27] = new QInputLabel(QStringLiteral("校针-接触式位移传感器"));
    INPUT[28] = new QInputLabel(QStringLiteral("校针-X对射光纤"));
    INPUT[29] = new QInputLabel(QStringLiteral("校针-Y对射光纤"));
    INPUT[30] = new QInputLabel(QStringLiteral("加热-温控器报警"));
    INPUT[31] = new QInputLabel(QStringLiteral("加热-负压表1"));
    INPUT[32] = new QInputLabel(QStringLiteral("加热-负压表2"));
    INPUT[33] = new QInputLabel(QStringLiteral("备用"));*/

    /*INPUT[0] = new QInputLabel(QStringLiteral("急停"));
    INPUT[1] = new QInputLabel(QStringLiteral("启动"));
    INPUT[2] = new QInputLabel(QStringLiteral("点胶"));
    INPUT[3] = new QInputLabel(QStringLiteral("擦胶"));
    INPUT[4] = new QInputLabel(QStringLiteral("复位"));
    INPUT[5] = new QInputLabel(QStringLiteral("照明"));
    INPUT[6] = new QInputLabel(QStringLiteral("擦胶-无尘布感应"));
    INPUT[7] = new QInputLabel(QStringLiteral("擦胶-A轴行程感应"));
    INPUT[8] = new QInputLabel(QStringLiteral("擦胶-擦胶感应器"));
    INPUT[9] = new QInputLabel(QStringLiteral("擦胶-气缸推进感应"));
    INPUT[10] = new QInputLabel(QStringLiteral("擦胶-气缸后退感应"));
    INPUT[11] = new QInputLabel(QStringLiteral("校针-接触式位移传感器"));
    INPUT[12] = new QInputLabel(QStringLiteral("校针-X对射光纤"));
    INPUT[13] = new QInputLabel(QStringLiteral("校针-Y对射光纤"));
    INPUT[14] = new QInputLabel(QStringLiteral("加热-温控器报警"));
    INPUT[15] = new QInputLabel(QStringLiteral("加热-负压表1"));
    INPUT[16] = new QInputLabel(QStringLiteral("加热-负压表2"));
    INPUT[17] = new QInputLabel(QStringLiteral("备用"));

	for (int i = 0; i < IN_COUNT; i++)
	{
		layout_2_1->addWidget( INPUT[i] );
	}*/

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

	/*OUTPUT[0] = new QOutputButton(QStringLiteral("X轴使能"));
	OUTPUT[1] = new QOutputButton(QStringLiteral("X轴报警清除"));
	OUTPUT[2] = new QOutputButton(QStringLiteral("Y轴使能"));
	OUTPUT[3] = new QOutputButton(QStringLiteral("Y轴报警清除"));
	OUTPUT[4] = new QOutputButton(QStringLiteral("Z轴使能"));
	OUTPUT[5] = new QOutputButton(QStringLiteral("Z轴报警清除"));
	OUTPUT[6] = new QOutputButton(QStringLiteral("A轴使能"));
	OUTPUT[7] = new QOutputButton(QStringLiteral("A轴报警清除"));
	OUTPUT[8] = new QOutputButton(QStringLiteral("点胶阀开关"));
	OUTPUT[9] = new QOutputButton(QStringLiteral("加热-温控开关"));
	OUTPUT[10] = new QOutputButton(QStringLiteral("加热-真空吸1"));
	OUTPUT[11] = new QOutputButton(QStringLiteral("加热-真空吸2"));
	OUTPUT[12] = new QOutputButton(QStringLiteral("擦胶气缸"));
	OUTPUT[13] = new QOutputButton(QStringLiteral("备用"));
	OUTPUT[14] = new QOutputButton(QStringLiteral("备用"));*/

	/*OUT0 = new QOutputButton(QStringLiteral("X轴使能"));
	layout_2_2->addWidget(OUT0);

	OUT1 = new QOutputButton(QStringLiteral("X轴报警清除"));
	layout_2_2->addWidget(OUT1);
	OUT1->setStatus(1);

	OUT2 = new QOutputButton(QStringLiteral("Y轴使能"));
	layout_2_2->addWidget(OUT2);

	OUT3 = new QOutputButton(QStringLiteral("Y轴报警清除"));
	layout_2_2->addWidget(OUT3);

	OUT4 = new QOutputButton(QStringLiteral("Z轴使能"));
	layout_2_2->addWidget(OUT4);

	OUT5 = new QOutputButton(QStringLiteral("Z轴报警清除"));
	layout_2_2->addWidget(OUT5);

	OUT6 = new QOutputButton(QStringLiteral("A轴使能"));
	layout_2_2->addWidget(OUT6);

	OUT7 = new QOutputButton(QStringLiteral("A轴报警清除"));
	layout_2_2->addWidget(OUT7);

	OUT8 = new QOutputButton(QStringLiteral("点胶阀开关"));
	layout_2_2->addWidget(OUT8);

	OUT9 = new QOutputButton(QStringLiteral("加热-温控开关"));
	layout_2_2->addWidget(OUT9);

	OUT10 = new QOutputButton(QStringLiteral("加热-真空吸1"));
	layout_2_2->addWidget(OUT10);

	OUT11 = new QOutputButton(QStringLiteral("加热-真空吸2"));
	layout_2_2->addWidget(OUT11);

	OUT12 = new QOutputButton(QStringLiteral("擦胶气缸"));
	layout_2_2->addWidget(OUT12);

	OUT13 = new QOutputButton(QStringLiteral("备用"));
	layout_2_2->addWidget(OUT13);

	OUT14 = new QOutputButton(QStringLiteral("备用"));
	layout_2_2->addWidget(OUT14);*/

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
				Sleep(10);
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

void IO::updateOutputStatus() // 读取一次
{
	if (init_card() == 1)
	{
		for (int i = OUT_VISIBLE_BEGIN; i < OUT_COUNT; i++)
		{
			OUTPUT[i]->setStatus(adt8949_read_bit(0, i));
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
		Sleep(5);
		if (adt8949_get_out(0, bit) == 0)
		{
			OUTPUT[bit]->setStatus(0);
		}
		else
		{
			qDebug() << QStringLiteral("IO: 输出信号有误, 请检查");
		}	
	}
	else
	{
		// 调试用
		adt8949_write_bit(0, bit, 1);
		OUTPUT[bit]->setStatus(1);

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


/*
void IO::readConfig()
{
QFile file("../config/io.json");

if(!file.exists())
{
return;
}

file.open((QIODevice::ReadOnly));
QByteArray date = file.readAll();
QJsonDocument doc = QJsonDocument::fromJson(date);
QJsonObject obj = doc.object();

foreach (QJsonValue vobj, obj)
{
int card  = vobj.toObject().value("card").toInt();
int index = vobj.toObject().value("index").toInt();
QString name = vobj.toObject().value("name").toString();
int state = vobj.toObject().value("state").toInt();
qDebug() << card << index << name << state;

QPushButton *btn = new QPushButton(this);
btn->setObjectName(name);
btn->setText(name);
layout_2_1->addWidget(btn);
}

layout_2_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
// QJsonObject::Iterator i;
// for(i=obj.begin(); i!=obj.end(); i++)
// {
//     QJsonObject i_obj = i.value().toObject();
//     int card  = i_obj.value("card").toInt();
//     int index = i_obj.value("index").toInt();
//     int index = i_obj.value("index").toInt();
//     int index = i_obj.value("index").toInt();
//     int index = i_obj.value("index").toInt();
// }
}
*/
