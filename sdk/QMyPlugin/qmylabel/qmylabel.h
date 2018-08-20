#ifndef QMYLABEL_H
#define QMYLABEL_H

#include <QWidget>
#include <QtWidgets>

#include "../qmyplugin_global.h"

class QMYPLUGINSHARED_EXPORT QMyLabel : public QWidget
{
    Q_OBJECT
public:
    explicit QMyLabel(QWidget *parent = nullptr);
    explicit QMyLabel(const QString &name, const QString &value = "0.000", const QString &unit = "unit", QWidget *parent = nullptr);

public:
    void setName(const QString &sname);
    void setValue(const QString &svalue);
    void setUnit(const QString &sunit);

    QString getValue();
    QString getName();
    QString getUnit();

private:
    QLabel *label_name;
    QLabel *label_value;
    QLabel *label_unit;
};

#endif // QMYLABEL_H
