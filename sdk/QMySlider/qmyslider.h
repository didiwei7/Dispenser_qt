#ifndef QMYSLIDER_H
#define QMYSLIDER_H

#include "qmyslider_global.h"

#include <QtWidgets>
#include <QWidget>

class QMYSLIDERSHARED_EXPORT QMySlider : public QWidget
{
    Q_OBJECT
public:
    explicit QMySlider(QWidget *parent = 0);

public:
    void setupUi();

    void setWidth(const int &width);

    void setText(const QString &name);

    void setRange(const int &min_value, const int &max_value);
    void setRangeStep(const int &value);
    void setValue(const int &value);

    int  currentValue();

    void setEditWidth(const int &width);

    void setHandleColor();
    void setHandleSelectedColor();

    void setLabelValue(const int &value);

signals:
    // 自定义的信号,和 slider 的 valueChanged 作用是一样的
    void sliderValueChanged(const int &value);


private:
    QLabel    *label_name;
    QLabel    *label_value;
    QSlider   *slider;
};


#endif // QMYSLIDER_H
