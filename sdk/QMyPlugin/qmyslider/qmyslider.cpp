#include "qmyslider.h"

QMySlider::QMySlider(QWidget *parent) : QWidget(parent)
{
    this->setupUi();
    this->setMouseTracking(true);
    this->setFixedWidth(150);
    connect(slider, &QSlider::valueChanged, this, &QMySlider::on_valueChanged);
}

QMySlider::QMySlider(const QString &name, const int &max_value, const int &value, QWidget *parent) : QWidget(parent)
{
    this->setupUi();
    this->setMouseTracking(true);
    this->setFixedWidth(150);
    connect(slider, &QSlider::valueChanged, this, &QMySlider::on_valueChanged);

    this->setName(name);
    this->setRange(0, max_value);
    this->setValue(value);

    QString str = QString("%1").arg(value);
    label_value->setText(str);
}

void QMySlider::setupUi()
{
    // 【1】 初始化控件
    label_name  = new QLabel();
    label_value = new QLabel();
    slider = new QSlider(Qt::Horizontal);

    // 【2】 布局
    QVBoxLayout *layout_1 = new QVBoxLayout();
    QHBoxLayout *layout_2 = new QHBoxLayout();

    layout_2->setContentsMargins(0, 0, 0, 0);
    layout_2->setSpacing(0);
    layout_2->addWidget(label_name);
    layout_2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout_2->addWidget(label_value);

    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);
    layout_1->addLayout(layout_2);
    layout_1->addWidget(slider);

    this->setLayout(layout_1);

    // 【3】 写入样式表
    QFile file(":/qmyslider/qmyslider.qss");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();
    this->setStyleSheet(style);
    file.close();
}

void QMySlider::setName(const QString &name)
{
    label_name->setText(name);
}

void QMySlider::setValue(const int &value)
{
    slider->setValue(value);
}

void QMySlider::setRange(const int &min_value, const int &max_value)
{
    slider->setRange(min_value, max_value);
}

void QMySlider::setRangeStep(const int &value)
{
    slider->setPageStep(value);
}

void QMySlider::setWidth(const int &width)
{
    this->setFixedWidth(width);
}

int QMySlider::getValue()
{
    return slider->value();
}

QString QMySlider::getName()
{
    return label_name->text();
}

void QMySlider::on_valueChanged(const int &value)
{
    QString str = QString("%1").arg(value);
    label_value->setText(str);

    emit qvalueChanged(value);
}
