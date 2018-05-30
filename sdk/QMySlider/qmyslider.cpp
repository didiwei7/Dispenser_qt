#include "qmyslider.h"
#include "qvalidator.h"
#include "qregexp.h"

QMySlider::QMySlider(QWidget *parent) : QWidget(parent)
{
    this->setupUi();

    this->setMouseTracking(true);
    this->setFixedWidth(150);

    // 连接信号槽
    connect(slider, &QSlider::valueChanged, this, &QMySlider::setLabelValue); 
}

/** 初始化 Ui
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-08
 */
void QMySlider::setupUi()
{
    // 【1】 初始化控件
    label_name = new QLabel();
    label_name->setText("..");

    label_value = new QLabel();
    label_value->setText("0");

    slider = new QSlider(Qt::Horizontal);

    // QLineEdit edit = new QLineEdit();
    // 设置为只能输入0-9间的字符
    // QRegExp regx("^[0-9]*[1-9][0-9]*$");
    // QRegExpValidator *pattern = new QRegExpValidator(regx, this);
    // edit->setValidator(pattern);
    // 设置只能输入0-100
    // QIntValidator validator(0, 100);
    // edit->setValidator(&validator);

    // 【2】 布局
    // layout_1 为 1 级布局， layout_2 为 2 级布局
    // layout_2_1 为 2 级布局的第一个布局 ......
    QVBoxLayout *layout_1 = new QVBoxLayout();
    QHBoxLayout *layout_2 = new QHBoxLayout();

    layout_2->addWidget(label_name);
    layout_2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout_2->addWidget(label_value);
    layout_2->setContentsMargins(0, 0, 0, 0);
    layout_2->setSpacing(0);

    layout_1->addLayout(layout_2);
    layout_1->addWidget(slider);
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);

    // 【3】 载入控件
    this->setLayout(layout_1);

    // 写入样式表
    QFile file(":/myslider.qss");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();
    this->setStyleSheet(style);
    file.close();
}


/** 设置控件的宽度
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
void QMySlider::setWidth(const int &width)
{
    this->setFixedWidth(width);
    this->update();
}


/** 设置控件 名称
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
void QMySlider::setText(const QString &name)
{
    label_name->setText(name);
}


/** 设置控件 范围
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
void QMySlider::setRange(const int &min_value, const int &max_value)
{
    slider->setRange(min_value, max_value);
}

/** 设置控件 当前值
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
void QMySlider::setValue(const int &value)
{
    QString str = QString("%1").arg(value);
    label_value->setText(str);

    slider->setValue(value);
}

/** 设置控件 步进值
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
void QMySlider::setRangeStep(const int &value)
{
    slider->setPageStep(value);
}


/** 发射信号
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
void QMySlider::setLabelValue(const int &value)
{
    // 设置Label的值
    QString str = QString("%1").arg(value);
    label_value->setText(str);

    // 发射信号
    emit sliderValueChanged(value);
}


/** 获取slider当前的值
 * @brief
 * @arguments   NULL
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-10
 */
int QMySlider::currentValue()
{
    int v = slider->value();
    return v;
}

