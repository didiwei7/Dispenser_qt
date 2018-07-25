#ifndef QMYSLIDER_H
#define QMYSLIDER_H

#include <QWidget>
#include <QtWidgets>

#include "../qmyplugin_global.h"

class QMYPLUGINSHARED_EXPORT QMySlider : public QWidget
{
    Q_OBJECT
public:
    explicit QMySlider(QWidget *parent = nullptr);
    explicit QMySlider(const QString &name, const int &max_value = 100, const int &value = 0, QWidget *parent = nullptr);

private:
    void setupUi();

public:
    void setName(const QString &name);
    void setValue(const int &value);
    void setRange(const int &min_value, const int &max_value);
    void setRangeStep(const int &value);
    void setWidth(const int &width);

public:
    int     getValue();
    QString getName();

signals:
    void qvalueChanged(const int &value);


private:
    void on_valueChanged(const int &value);

private:
    QLabel    *label_name;
    QLabel    *label_value;
    QSlider   *slider;
};

#endif // QMYSLIDER_H
