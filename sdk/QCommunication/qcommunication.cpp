#include "qcommunication.h"

int char2number(char ch)
{
	if ((ch >= '0') && (ch <= '9')) return int(ch - 0x30);	
	else if ((ch >= 'A') && (ch <= 'F')) return int(ch - 'A' + 10);
	else if ((ch >= 'a') && (ch <= 'f')) return int(ch - 'a' + 10);
	else return 0;
}

QMySocket::QMySocket(QWidget *parent) : QWidget(parent)
{
    setSocket();
    setupUi();
    setConnect();
}

void QMySocket::setupUi()
{
    // 【1】 初始化控件
    QLabel *label_ip = new QLabel(QStringLiteral("IP: "));
    text_ip   = new QLineEdit();
    QLabel *label_port = new QLabel(QStringLiteral("Port: "));
    text_port = new QLineEdit();

    radio_client = new QRadioButton(QStringLiteral("客户端"));
    radio_client->setChecked(true);
    radio_server = new QRadioButton(QStringLiteral("服务器"));

    btn_connect    = new QPushButton(QStringLiteral("连接"));
    btn_disconnect = new QPushButton(QStringLiteral("断开"));
    btn_disconnect->setEnabled(false);

    btn_start = new QPushButton(QStringLiteral("启动服务器"));
    btn_start->setVisible(false);
    btn_abort = new QPushButton(QStringLiteral("关闭服务器"));
    btn_abort->setEnabled(false);
    btn_abort->setVisible(false);

    text_receive = new QTextBrowser();
    text_send    = new QTextEdit();

    btn_send = new QPushButton(QStringLiteral("发送"));
    btn_send->setEnabled(false);
    btn_clear = new QCheckBox(QStringLiteral("发送后清空"));
    btn_clear->setChecked(true);

    // 【2】 布局
    QVBoxLayout *layout_1 = new QVBoxLayout();

    QHBoxLayout *layout_2_1 = new QHBoxLayout();
    QHBoxLayout *layout_2_2 = new QHBoxLayout();
    QHBoxLayout *layout_2_3 = new QHBoxLayout();

    QVBoxLayout *layout_3_1 = new QVBoxLayout();

    layout_1->addLayout(layout_2_1);
    layout_1->addLayout(layout_2_2);
    layout_1->addWidget(text_receive);
    layout_1->addLayout(layout_2_3);
    layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout_2_1->addWidget(label_ip);
    layout_2_1->addWidget(text_ip);
    layout_2_1->addWidget(label_port);
    layout_2_1->addWidget(text_port);

    layout_2_2->addLayout(layout_3_1);
    layout_2_2->addWidget(btn_connect);
    layout_2_2->addWidget(btn_disconnect);
    layout_2_2->addWidget(btn_start);
    layout_2_2->addWidget(btn_abort);

    layout_2_3->addWidget(text_send);
    layout_2_3->addWidget(btn_send);
    layout_2_3->addWidget(btn_clear);

    layout_3_1->addWidget(radio_client);
    layout_3_1->addWidget(radio_server);

    setLayout(layout_1);

}

void QMySocket::setSocket()
{
    // 【1】 客户端
    client = new QTcpSocket();

    // 【2】 服务器
    server = new QTcpServer();
    m_socket = new QTcpSocket();
}

void QMySocket::setConnect()
{
    // 【0】 切换
    connect(radio_client, &QRadioButton::clicked, this, &QMySocket::on_radio_client);
    connect(radio_server, &QRadioButton::clicked, this, &QMySocket::on_radio_server);

    // 【1】 客户端
    connect(client, &QTcpSocket::readyRead, this, &QMySocket::clientReceive);
    connect(btn_connect, &QPushButton::clicked, this, &QMySocket::on_btn_connect);
    connect(btn_disconnect, &QPushButton::clicked, this, &QMySocket::on_btn_disconnect);

    // 【2】 服务器
    connect(server, &QTcpServer::newConnection, this, &QMySocket::on_server_accept);
    connect(btn_start, &QPushButton::clicked, this, &QMySocket::on_btn_start);
    connect(btn_abort, &QPushButton::clicked, this, &QMySocket::on_btn_abort);

    // 【3】 发送
    connect(btn_send, &QPushButton::clicked, this, &QMySocket::on_btn_send);
}

// 选择  客户端, 服务器
void QMySocket::on_radio_client()
{
    btn_start->setVisible(false);
    btn_abort->setVisible(false);

    btn_connect->setVisible(true);
    btn_disconnect->setVisible(true);
}

void QMySocket::on_radio_server()
{
    btn_connect->setVisible(false);
    btn_disconnect->setVisible(false);

    btn_start->setVisible(true);
    btn_abort->setVisible(true);
}

// 客户端  连接, 断开, 接收
void QMySocket::on_btn_connect()
{
    QString str_ip = text_ip->text();
    int iport = text_port->text().toInt();

    client->connectToHost(str_ip, iport);

    btn_send->setEnabled(true);
    btn_connect->setEnabled(false);
    btn_disconnect->setEnabled(true);
}

void QMySocket::on_btn_disconnect()
{
    client->abort();

    btn_send->setEnabled(false);
    btn_connect->setEnabled(true);
    btn_disconnect->setEnabled(true);

}

void QMySocket::clientReceive()
{
    QByteArray byte_receive = client->read(128);
    QString str_recerve = QString(byte_receive);
    QString str = QString("%1: %2").arg(getCurrentTime()).arg(str_recerve);
    text_receive->append(str);
    byte_receive.clear();
}

// 服务器  启动, 关闭, 新连接加入, 接收
void QMySocket::on_btn_start()
{
    // QString str_ip = text_ip->text();
    int iport = text_port->text().toInt();

    server->listen(QHostAddress::Any, iport);

    btn_start->setEnabled(false);
    btn_abort->setEnabled(true);
    btn_send->setEnabled(true);

    text_receive->append(QStringLiteral("服务器已启动"));
}

void QMySocket::on_btn_abort()
{
    server->close();
    m_socket->abort();

    btn_start->setEnabled(true);
    btn_abort->setEnabled(false);
    btn_send->setEnabled(false);
}

void QMySocket::on_server_accept()
{
    m_socket->abort();
    m_socket = server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &QMySocket::serverReceive);
    text_receive->append(QStringLiteral("有新用户加入"));
}

void QMySocket::serverReceive()
{
    QByteArray byte_receive = m_socket->read(128);
    QString str_recerve = QString(byte_receive);
    QString str = QString("%1: %2").arg(getCurrentTime()).arg(str_recerve);
    text_receive->append(str);
    byte_receive.clear();
}

// 发送消息
void QMySocket::on_btn_send()
{
    if(radio_client->isChecked())
    {
        QString msg = text_send->toPlainText();
        // 转换成string
        std::string str = msg.toStdString();
        // 转换成const char*
        const char* buff = str.c_str();
        // 发送
        client->write(buff);

        if(btn_clear->isChecked())
        {
            text_send->clear();
        }
    }
    else
    {
        QString msg = text_send->toPlainText();
        // 转换成string
        std::string str = msg.toStdString();
        // 转换成const char*
        const char* buff = str.c_str();
        // 发送
        m_socket->write(buff);

        if(btn_clear->isChecked())
        {
            text_send->clear();
        }
    }
}

// 获取当前时间
QString QMySocket::getCurrentTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    return s_currentTime;
}





QMySerial::QMySerial(QWidget *parent) : QWidget(parent)
{
    setSerial();
    setupUi();
    setConnect();
    setThread();
}

QMySerial::~QMySerial()
{
	clse_thread_receive = true;

	thread_pool.waitForDone();
	thread_pool.clear();
	thread_pool.destroyed();
}

// 初始化
void QMySerial::setupUi()
{
    // 【1】 comboBox
    QStringList slist_baud;
    QStringList slist_data;
    QStringList slist_parity;
    QStringList slist_stop;
    QStringList slist_flow;
    slist_baud << "9600" << "19200" << "38400" << "134" << "115200";
    slist_data << "5" << "6" << "7" << "8";
    slist_parity << "None" << "Even" << "Odd" << "Space" << "Mask";
    slist_stop << "1" << "1.5" << "2";
    slist_flow << "None" << "Hardware" << "Software";

    combo_port = new QComboBox();
    combo_baud = new QComboBox();
    combo_data = new QComboBox();
    combo_parity = new QComboBox();
    combo_stop = new QComboBox();
    combo_flow = new QComboBox();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort sserial;
        sserial.setPort(info);
        if(sserial.open(QIODevice::ReadWrite))
        {
            //将串口号添加到cmb
            combo_port->addItem(info.portName());
            //关闭串口等待人为(打开串口按钮)打开
            sserial.close();
        }
    }
    combo_baud->addItems(slist_baud);
    combo_data->addItems(slist_data);
    combo_parity->addItems(slist_parity);
    combo_stop->addItems(slist_stop);
    combo_flow->addItems(slist_flow);

    combo_port->setCurrentIndex(0);
    combo_baud->setCurrentIndex(0);
    combo_data->setCurrentIndex(3);
    combo_parity->setCurrentIndex(0);
    combo_stop->setCurrentIndex(0);
    combo_flow->setCurrentIndex(0);

    // 【2】 其他控件
	btn_start = new QPushButton(QStringLiteral("启动"));
	btn_stop = new QPushButton(QStringLiteral("关闭"));
	btn_stop->setEnabled(false);
    text_receive = new QTextBrowser();
    text_send = new QTextEdit();
    btn_send  = new QPushButton(QStringLiteral("发送"));
    btn_send->setEnabled(false);
	btn_hex = new QCheckBox(QStringLiteral("16进制"));
	btn_hex->setChecked(true);
	btn_clear = new QCheckBox(QStringLiteral("发送后清空"));
	btn_clear->setChecked(true);


    // 【3】 布局
    QVBoxLayout *layout_1 = new QVBoxLayout();
    QHBoxLayout *layout_2_1 = new QHBoxLayout();
    QHBoxLayout *layout_2_2 = new QHBoxLayout();
    QGridLayout *layout_3_1 = new QGridLayout();

	layout_3_1->addWidget(new QLabel(QStringLiteral("串口")), 0, 0);
	layout_3_1->addWidget(new QLabel(QStringLiteral("波特率")), 1, 0);
	layout_3_1->addWidget(new QLabel(QStringLiteral("数据位")), 2, 0);
	layout_3_1->addWidget(new QLabel(QStringLiteral("校验位")), 3, 0);
	layout_3_1->addWidget(new QLabel(QStringLiteral("停止位")), 4, 0);
	layout_3_1->addWidget(new QLabel(QStringLiteral("流控")), 5, 0);

	layout_3_1->addWidget(combo_port, 0, 1);
	layout_3_1->addWidget(combo_baud, 1, 1);
	layout_3_1->addWidget(combo_data, 2, 1);
	layout_3_1->addWidget(combo_parity, 3, 1);
	layout_3_1->addWidget(combo_stop, 4, 1);
	layout_3_1->addWidget(combo_flow, 5, 1);

    layout_2_1->addLayout(layout_3_1);
	layout_2_1->addWidget(btn_start);
	layout_2_1->addWidget(btn_stop);
    layout_2_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout_2_2->addWidget(text_send);
    layout_2_2->addWidget(btn_send);
	layout_2_2->addWidget(btn_hex);
	layout_2_2->addWidget(btn_clear);
	
    layout_1->addLayout(layout_2_1);
    layout_1->addWidget(text_receive);
    layout_1->addLayout(layout_2_2);
    layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    setLayout(layout_1);
}

void QMySerial::setConnect()
{
    connect(btn_start, &QPushButton::clicked, this, &QMySerial::on_btn_start);
    connect(btn_send,  &QPushButton::clicked, this, &QMySerial::on_btn_send);
    connect(btn_stop,  &QPushButton::clicked, this, &QMySerial::on_btn_stop);

	connect(this, &QMySerial::serialportReceived, this, &QMySerial::on_serialportReceived);
}

void QMySerial::setSerial()
{
    serial = new QSerialPort(this);
}

void QMySerial::setThread()
{
    thread_pool.setMaxThreadCount(1);
	clse_thread_receive = false;
}

// 线程
void QMySerial::thread_receive()
{
    while (clse_thread_receive == false)
    {
        QByteArray readData = serial->read(10);
        if( readData.size() > 5 )  //!readData.isNull() && readData != ""
        {
			// QString str1 = QString(readData).remove(0, 1);
            serialportReceived( QString(readData).remove(QChar(2)).remove(QChar(3)) );
            readData.clear();
        }
        Sleep(5);
    }
}

// 槽函数
void QMySerial::on_btn_start()
{
    serial->setPortName(combo_port->currentText());

    if(!serial->open(QIODevice::ReadWrite))
    {
        return;
    }

    serial->setBaudRate(combo_baud->currentText().toInt());
    serial->setDataBits(getDatabits());
    serial->setParity(getParity());
    serial->setStopBits(getStopBits());
    serial->setFlowControl(getFlowControl());

    btn_send->setEnabled(true);
    btn_stop->setEnabled(true);
    btn_start->setEnabled(false);

    // 启动接收线程
    QtConcurrent::run(&thread_pool, [&]() { thread_receive(); });
}

void QMySerial::on_btn_stop()
{
    btn_send->setEnabled(false);
    btn_stop->setEnabled(false);
    btn_start->setEnabled(true);

    // 关闭接收线程
    clse_thread_receive = true;
    Sleep(10);
    thread_pool.waitForDone();

    // 关闭串口
    serial->close();
}

void QMySerial::on_btn_send()
{
    QString str = text_send->toPlainText();
    if(str == "")
    {
        return;
    }
    else
    {
		if (!btn_hex->isChecked())
		{
			QByteArray data = str.toLatin1();
			serial->write(data);
		}
		else
		{
			QStringList ldata = str.trimmed().split(" ");
			int length = ldata.length();
			char hex_data[1024];
			char lstr, hstr;
			int lnum, hnum;
			for (int i = 0; i < length; i++)
			{
				hstr = ldata.at(i)[0].toLatin1();
				lstr = ldata.at(i)[1].toLatin1();

				hnum = char2number(hstr);
				lnum = char2number(lstr);

				hex_data[i] = (char)(hnum * 16 + lnum);
			}
			serial->write(hex_data, length);
		}

		if (btn_clear->isChecked())
		{
			text_send->clear();
		}
    }

}

void QMySerial::on_serialportReceived(QString rdata)
{
	QString stime = getCurrentTime();
	QString str = QString("%1: %2").arg(stime).arg(rdata);
	text_receive->append(str);
}

// 获取ComboBox状态
QSerialPort::DataBits QMySerial::getDatabits()
{
    QString str = combo_parity->currentText();
    if (str == "5") return QSerialPort::Data5;
    else if (str == "6") return QSerialPort::Data6;
    else if (str == "7") return QSerialPort::Data7;
    else if (str == "8") return QSerialPort::Data8;
}

QSerialPort::Parity QMySerial::getParity()
{
    QString str = combo_parity->currentText();
    if (str == "None") return QSerialPort::NoParity;
    else if (str == "Even") return QSerialPort::EvenParity;
    else if (str == "Odd") return QSerialPort::OddParity;
    else if (str == "Space") return QSerialPort::SpaceParity;
    else if (str == "Mask") return QSerialPort::MarkParity;
}

QSerialPort::StopBits QMySerial::getStopBits()
{
    QString str = combo_stop->currentText();
    if (str == "1") return QSerialPort::OneStop;
    else if (str == "1.5") return QSerialPort::OneAndHalfStop;
    else if (str == "2") return QSerialPort::TwoStop;
    // else return QSerialPort::UnknownStopBits;
}

QSerialPort::FlowControl QMySerial::getFlowControl()
{
    QString str = combo_flow->currentText();
    if (str == "None") return QSerialPort::NoFlowControl;
    else if (str == "Hardware") return QSerialPort::HardwareControl;
    else if (str == "Software") return QSerialPort::SoftwareControl;
    // else return QSerialPort::UnknownFlowControl;
}

// 获取当前时间
QString QMySerial::getCurrentTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    return s_currentTime;
}
