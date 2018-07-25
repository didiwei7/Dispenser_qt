#ifndef QTHERMOMETER_H
#define QTHERMOMETER_H

#include <QWidget>
#include <QtWidgets>

#include "../qmyplugin_global.h"

class QMYPLUGINSHARED_EXPORT QThermometer : public QWidget
{
    Q_OBJECT
public:
    explicit QThermometer(QWidget *parent = nullptr);
    explicit QThermometer(const QString &sname, const double &svalue = 0, QWidget *parent = nullptr);

public:
    double getValue();
    const QString getName();

    void setValue(const double &svalue);
    void setName(const QString &sname);

public:
    void setRange(const double &sminValue, const double &smaxValue);
    void setRange(const int &sminValue, const int &smaxValue);

    void setMinValue(const double &sminValue);
    void setMinValue(const int &sminValue);

    void setMaxValue(const double &smaxValue);
    void setMaxValue(const int &smaxValue);

    void setPrecision(const int &sprecision);

    // 背景
    void setBgGradientStart(const QColor &sbgGradientStart);
    void setBgGradientEnd(const QColor &sbgGradientEnd);

    // 线条
    void setLineColor(const QColor &slineColor);

    // 柱状条
    void setBarBgColor(const QColor &sbarBgColor);
    void setBarColor(const QColor &sbarColor);

    // 标题
    void setTitleColor(const QColor &stitleColor);

protected:
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);
    void drawRuler(QPainter *painter);
    void drawBarBg(QPainter *painter);
    void drawBar(QPainter *painter);
    void drawTitle(QPainter *painter);



private:
    // 标尺
    double maxValue;                // 最小值
    double minValue;                // 最大值
    double value;                   // 目标值
    int precision;                  // 精确度,小数点后几位

    int longStep;                   // 长线条等分步长
    int shortStep;                  // 短线条等分步长
    int space_top;                  // 上间距
    int space_bottom;               // 左间距
    int space_left;                 // 下间距
    int space_right;                // 右间距
    QColor lineColor;               // 线条颜色

    // 背景
    QColor bgGradientStart;         // 背景渐变开始颜色
    QColor bgGradientEnd;           // 背景渐变结束颜色

    // 柱状条
    QColor barBgColor;              // 柱状背景色
    QColor barColor;                // 柱状颜色
    QRectF barRect;                 // 柱状图区域

    // 标题栏
    QColor titleColor;              // 标题文字颜色
    QString name;                   // 标题


};

#endif // QTHERMOMETER_H
