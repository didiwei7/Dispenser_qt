#ifndef QIO_H
#define QIO_H

#include <QWidget>
#include <QtWidgets>

#include "../qmyplugin_global.h"

class QMYPLUGINSHARED_EXPORT QInput : public QWidget
{
    Q_OBJECT
public:
    explicit QInput(QWidget *parent = nullptr);

public:
    void setStatus(int status);
    void setSize(int w, int h);

public:
    int getStatus();

protected:
    void paintEvent(QPaintEvent *);

private:
    int  m_width;
    int  m_height;
    int  m_status;
};

class QMYPLUGINSHARED_EXPORT QInputLabel : public QWidget
{
    Q_OBJECT
public:
    explicit QInputLabel(QWidget *parent = nullptr);
    explicit QInputLabel(const QString &name, const int &state = 0, QWidget *parent = nullptr);

public:
    void setName(const QString &name);
    void setStatus(int state);

public:
    QString getName();
    int getStatus();

private:
    QLabel *label_name;
    QInput *label_state;
};

class QMYPLUGINSHARED_EXPORT QOutputButton : public QWidget
{
    Q_OBJECT
public:
    explicit QOutputButton(QWidget *parent = nullptr);
    explicit QOutputButton(const QString &name, int state = 0, QWidget *parent = nullptr);

public:
    void setName(const QString &name);
    void setStatus(int state);

public:
    QString getName();
    int getStatus();

signals:
    void qclicked();

private:
    void on_btn_output();

private:
    QPushButton *btn_output;
    int m_status;
};

#endif // QIO_H
