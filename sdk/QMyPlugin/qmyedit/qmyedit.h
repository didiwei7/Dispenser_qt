#ifndef QMYEDIT_H
#define QMYEDIT_H

#include <QWidget>
#include <QtWidgets>

#include "../qmyplugin_global.h"

class QMYPLUGINSHARED_EXPORT QMyEdit : public QWidget
{
    Q_OBJECT
public:
    explicit QMyEdit(QWidget *parent = nullptr);
    explicit QMyEdit(const QString &name, const QString &value = "0.000", const QString &unit = "unit", const bool &btnVisible = true, QWidget *parent = nullptr);

public:
    void setName(const QString &sname);
    void setValue(const QString &svalue);
    void setUnit(const QString &sunit);
    void setBtnName(const QString &sname);
	void setBtnVisible(const bool &svisible);

    QString getValue();
    QString getName();
    QString getUnit();

signals:
    void qclicked();

private:
    void on_btn_change();

    QLabel    *label_name;
    QLineEdit *edit_value;
    QLabel    *label_unit;
    QPushButton *btn_change;

    int m_height;
    int m_width;
};

#endif // QMYEDIT_H
