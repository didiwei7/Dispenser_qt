#include "qthermometer.h"

QThermometer::QThermometer(QWidget *parent) : QWidget(parent)
{
    // 标尺
    minValue = 0;       // 最小值
    maxValue = 100;     // 最大值
    value = 0;          // 目标值

    precision = 0;      // 精确度,小数点后几位
    longStep = 10;      // 长线条等分步长
    shortStep = 1;      // 短线条等分步长

    lineColor = QColor(255, 255, 255);  // 线条颜色

    space_top = 40;     // 上间距
    space_bottom = 20;  // 下间距
    space_left = 20;    // 左间距
    space_right = 20;   // 右间距

    // 背景色
    bgGradientStart = QColor(100, 100, 100);
    bgGradientEnd   = QColor(60, 60, 60);

    // 柱状条
    barBgColor = QColor(220, 220, 220);
    barColor = QColor(100, 184, 255);

    // 标题栏
    name = "";
    titleColor = QColor(0, 255, 0);

    setFont(QFont("MicroSoft Yahei", 8));
    setFixedSize(100, 350);
}

QThermometer::QThermometer(const QString &sname, const double &svalue, QWidget *parent) : QWidget(parent)
{
    // 标尺
    minValue = 0;       // 最小值
    maxValue = 100;     // 最大值
    value = 0;          // 目标值

    precision = 0;      // 精确度,小数点后几位
    longStep = 10;      // 长线条等分步长
    shortStep = 1;      // 短线条等分步长

    lineColor = QColor(255, 255, 255);  // 线条颜色

    space_top = 40;     // 上间距
    space_bottom = 20;  // 下间距
    space_left = 20;    // 左间距
    space_right = 20;   // 右间距

    // 背景色
    bgGradientStart = QColor(100, 100, 100);
    bgGradientEnd   = QColor(60, 60, 60);

    // 柱状条
    barBgColor = QColor(220, 220, 220);
    barColor = QColor(100, 184, 255);

    // 标题栏
    name = "";
    titleColor = QColor(0, 255, 0);

    setFont(QFont("MicroSoft Yahei", 8));
    setFixedSize(100, 350);

    setName(sname);
    setValue(svalue);
}

void QThermometer::paintEvent(QPaintEvent *)
{
    // 反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 绘制背景
    drawBg(&painter);
    // 绘制标尺
    drawRuler(&painter);
    // 绘制柱状背景
    drawBarBg(&painter);
    // 绘制柱状
    drawBar(&painter);
    // 绘制标题栏
    drawTitle(&painter);
}

// 绘制背景
void QThermometer::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    QLinearGradient bgGradient(QPointF(0, 0), QPointF(0, height()));
    bgGradient.setColorAt(0.0, bgGradientStart);
    bgGradient.setColorAt(1.0, bgGradientEnd);
    painter->setBrush(bgGradient);
    painter->drawRect(rect());
    painter->restore();
}

// 绘制标尺
void QThermometer::drawRuler(QPainter *painter)
{
    painter->save();
    painter->setPen(lineColor);

    double initX = space_left + 20;
    double initY = space_top;

    // 绘制纵向标尺线 20的长度为刻度尺左侧文字的宽度
    QPointF topPot(initX, initY);
    QPointF bottomPot(initX, height() - space_bottom);
    painter->drawLine(topPot, bottomPot);

    //绘制纵向标尺刻度
    double length = height() -space_top - space_bottom;
    //计算每一格移动多少
    double increment = length / (maxValue - minValue);
    //长线条短线条长度
    int longLineLen = 10;
    int shortLineLen = 7;

    //根据范围值绘制刻度值及刻度值
    for (int i = maxValue; i >= minValue; i = i - shortStep)
    {
        if (i % longStep == 0)      // 绘制刻度, 文本
        {
            QPointF leftPot(initX + longLineLen, initY);
            QPointF rightPot(initX, initY);
            painter->drawLine(leftPot, rightPot);

            QString strValue = QString("%1").arg((double)i, 0, 'f', precision);
            double fontWidth = painter->fontMetrics().width(strValue);
            double fontHeight = painter->fontMetrics().height();
            QPointF textPot(initX - fontWidth - 5, initY + fontHeight / 3);
            painter->drawText(textPot, strValue);
        }
        else                        // 绘制刻度
        {
            if (i % (longStep / 2) == 0) shortLineLen = 7;
            else shortLineLen = 4;

            QPointF leftPot(initX + shortLineLen, initY);
            QPointF rightPot(initX, initY);
            painter->drawLine(leftPot, rightPot);
        }

        initY += increment * shortStep;
    }

    painter->restore();
}

// 绘制柱状条背景
void QThermometer::drawBarBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    // 20的长度为刻度尺文字的宽度 15为刻度尺到柱状图的宽度
    double initX = space_left + 20 + 15;
    QPointF topLeftPot(initX, space_top);
    QPointF bottomRightPot(width() - space_right , height() - space_bottom);
    barRect = QRectF(topLeftPot, bottomRightPot);

    painter->setBrush(barBgColor);
    painter->drawRect(barRect);
    painter->restore();
}

// 绘制柱状条
void QThermometer::drawBar(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    double barHeight = barRect.height();
    double increment = (double)barHeight / (maxValue - minValue);
    double initY = (value - minValue) * increment;

    QPointF topLeftPot(barRect.topLeft().x(), barRect.bottomLeft().y() - initY);
    QPointF bottomRightPot(barRect.bottomRight());
    QRectF currentRect(topLeftPot, bottomRightPot);

    painter->setBrush(barColor);
    painter->drawRect(currentRect);
    painter->restore();
}

// 绘制标题栏
void QThermometer::drawTitle(QPainter *painter)
{
    painter->save();
    painter->setPen(titleColor);

    QPointF textPoint_name(space_left-10, 20);
    painter->drawText(textPoint_name, name);

    QPointF textPoint_value(width() - space_right - 20, 20);
    QString str_value = QString::number(value, 'f', 1) + QStringLiteral("℃") ;
    painter->drawText(textPoint_value, str_value);

    painter->restore();
}

void QThermometer::setValue(const double &svalue)
{
    if(value != svalue)
    {
        value = svalue;
        update();
    }
}

void QThermometer::setName(const QString &sname)
{
    if (name != sname)
    {
        name = sname;
        update();
    }
}

double QThermometer::getValue()
{
    return value;
}

const QString QThermometer::getName()
{
    return name;
}


void QThermometer::setRange(const double &sminValue, const double &smaxValue)
{
    if (sminValue >= smaxValue)    return;
    else if ( (minValue == sminValue) && (maxValue == smaxValue) ) return;
    else
    {
        minValue = sminValue;
        maxValue = smaxValue;
        update();
    }
}

void QThermometer::setRange(const int &sminValue, const int &smaxValue)
{
    setRange((double)sminValue, (double)smaxValue);
}

void QThermometer::setMinValue(const double &sminValue)
{
    setRange(sminValue, maxValue);
}

void QThermometer::setMinValue(const int &sminValue)
{
    setRange((double)sminValue, maxValue);
}

void QThermometer::setMaxValue(const double &smaxValue)
{
    setRange(minValue, smaxValue);
}

void QThermometer::setMaxValue(const int &smaxValue)
{
    setRange(minValue, (double)smaxValue);
}

void QThermometer::setPrecision(const int &sprecision)
{
    // 最大精确度为2
    if ( precision <= 2 && precision != sprecision) {
        precision = precision;
        update();
    }
}

// 背景
void QThermometer::setBgGradientStart(const QColor &sbgGradientStart)
{
    if (bgGradientStart != sbgGradientStart)
    {
        bgGradientStart = sbgGradientStart;
        update();
    }
}

void QThermometer::setBgGradientEnd(const QColor &sbgGradientEnd)
{
    if (bgGradientEnd != sbgGradientEnd)
    {
        bgGradientEnd = sbgGradientEnd;
        update();
    }
}

// 线条
void QThermometer::setLineColor(const QColor &slineColor)
{
    if (lineColor != slineColor)
    {
        lineColor = slineColor;
        update();
    }
}

// 柱状条
void QThermometer::setBarBgColor(const QColor &sbarBgColor)
{
    if (barBgColor != sbarBgColor)
    {
        barBgColor = sbarBgColor;
        update();
    }
}

void QThermometer::setBarColor(const QColor &sbarColor)
{
    if (barColor != sbarColor)
    {
        barColor = sbarColor;
        update();
    }
}

// 标题
void QThermometer::setTitleColor(const QColor &stitleColor)
{
    if (titleColor != stitleColor)
    {
        titleColor = stitleColor;
        update();
    }
}
