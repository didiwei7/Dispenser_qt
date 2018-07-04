#ifndef QIO_H
#define QIO_H

#include "qio_global.h"

#include <QtWidgets>
#include <QDebug>

class QIOSHARED_EXPORT QInput : public QWidget
{
    Q_OBJECT
public:
    explicit QInput(QWidget *parent = Q_NULLPTR);

public:
    void setStatus(int status);
    void setSize(int w, int h);
	int  getStatus();

private:
    int  m_width;
    int  m_height;
    int  m_status;

protected:
    void paintEvent(QPaintEvent *);
};


class QIOSHARED_EXPORT QInputLabel : public QWidget
{
    Q_OBJECT
public:
    explicit QInputLabel(QWidget *parent = Q_NULLPTR);
    explicit QInputLabel(const QString &name, int state = 0, QWidget *parent = Q_NULLPTR);

public:
    void setName(const QString &name);
    void setStatus(int state);
	int  getStatus();

private:
    QLabel *label_name;
    QInput *label_state;
};


class QIOSHARED_EXPORT QOutputButton : public QWidget
{
    Q_OBJECT
public:
    explicit QOutputButton(QWidget *parent = Q_NULLPTR);
    explicit QOutputButton(const QString &name, int state = 0, QWidget *parent = Q_NULLPTR);

signals:
	void wclicked();

public:
	void setStatus(int state);
	int  getStatus();

private:
	void on_btn_output();

private:
	QPushButton *btn_output;
	int m_istatus;
};

#endif // QIO_H
