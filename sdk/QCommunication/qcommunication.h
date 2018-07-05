#ifndef QCOMMUNICATION_H
#define QCOMMUNICATION_H

#include "qcommunication_global.h"

#include <QWidget>
#include <QtWidgets>

#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <QtSerialPort/qtserialportversion.h>
#include <QtNetwork>
#include <QtConcurrent>
#include <Windows.h>

class QCOMMUNICATIONSHARED_EXPORT QMySocket : public QWidget
{
    Q_OBJECT
public:
    explicit QMySocket(QWidget *parent = 0);

private:
    void setSocket();
    void setupUi();
    void setConnect();

private:
    void clientReceive();
    void serverReceive();

private:    // 客户端
    void on_btn_connect();
    void on_btn_disconnect();

    QTcpSocket *client;
    QPushButton *btn_connect;
    QPushButton *btn_disconnect;

private:    // 服务器
    void on_btn_start();
    void on_btn_abort();
    void on_server_accept();

    QTcpServer *server;
    QTcpSocket *m_socket;
    QPushButton *btn_start;
    QPushButton *btn_abort;

private:
    void on_radio_client();
    void on_radio_server();

    QRadioButton *radio_client;
    QRadioButton *radio_server;

private:
    void on_btn_send();

    QLineEdit    *text_ip;
    QLineEdit    *text_port;
    QTextBrowser *text_receive;
    QTextEdit    *text_send;

    QPushButton  *btn_send;
    QCheckBox    *btn_clear;

private:
    QString getCurrentTime();
};

class QCOMMUNICATIONSHARED_EXPORT QMySerial : public QWidget
{
    Q_OBJECT
public:
    explicit QMySerial(QWidget *parent = 0);
	~QMySerial();

private:
    void setSerial();
    void setupUi();
    void setConnect();
    void setThread();

private:
    QSerialPort *serial;

private:
    QThreadPool thread_pool;
    void thread_receive();
	bool clse_thread_receive;

private:
    QString getCurrentTime();

private:
    QSerialPort::DataBits getDatabits();
    QSerialPort::Parity   getParity();
    QSerialPort::StopBits getStopBits();
    QSerialPort::FlowControl getFlowControl();

    QComboBox *combo_port;
    QComboBox *combo_baud;
    QComboBox *combo_data;
    QComboBox *combo_parity;
    QComboBox *combo_stop;
    QComboBox *combo_flow;

signals:
	void serialportReceived(QString rdata);

private slots:
	void on_serialportReceived(QString rdata);
	

private:
    void on_btn_start();
    void on_btn_send();
    void on_btn_stop();

    QTextBrowser *text_receive;
    QTextEdit    *text_send;

    QPushButton  *btn_send;
	QCheckBox    *btn_hex;
	QCheckBox    *btn_clear;
    QPushButton  *btn_start;
    QPushButton  *btn_stop;
};

#endif // QCOMMUNICATION_H
