#ifndef QIO_H
#define QIO_H

#include "qio_global.h"

#include <QtWidgets>
#include <QDebug>

class QIOSHARED_EXPORT QInput : public QWidget
{
    Q_OBJECT
public:
    explicit QInput(QWidget *parent=0);

public:
    void setStatus(int status);
    void setSize(int w, int h);

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

private:
    QLabel  *label_name;
    QInput *label_state;
};


class QIOSHARED_EXPORT QOutputButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QOutputButton(QPushButton *parent=0);
    explicit QOutputButton(const QString &name, int state = 0, QPushButton *parent = Q_NULLPTR);

public:
    void setStatus(int state);
};

#endif // QIO_H
