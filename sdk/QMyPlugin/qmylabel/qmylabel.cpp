#include "qmylabel.h"

QMyLabel::QMyLabel(QWidget *parent) : QWidget(parent)
{
    QFont font("MicroSoft Yahei", 9);

    label_name = new QLabel();
    label_name->setAlignment(Qt::AlignRight);
    label_name->setFont(font);
    label_name->setFixedWidth(100);

    label_value = new QLabel();
    label_value->setAlignment(Qt::AlignRight);
    label_value->setFont(font);
    label_value->setFixedWidth(55);

    label_unit = new QLabel();
    label_unit->setAlignment(Qt::AlignLeft);
    label_unit->setFont(font);
    label_unit->setFixedWidth(40);

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setMargin(0);
    layout_1->setSpacing(0);
    layout_1->addWidget(label_name);
    layout_1->addWidget(label_value);
    layout_1->addSpacing(5);
    layout_1->addWidget(label_unit);

    setLayout(layout_1);
    setFixedSize(200, 20);
}

QMyLabel::QMyLabel(const QString &name, const QString &value, const QString &unit, QWidget *parent) : QWidget(parent)
{
    QFont font("MicroSoft Yahei", 9);

    label_name = new QLabel();
    label_name->setAlignment(Qt::AlignRight);
    label_name->setFont(font);
    label_name->setFixedWidth(100);

    label_value = new QLabel();
    label_value->setAlignment(Qt::AlignRight);
    label_value->setFont(font);
    label_value->setFixedWidth(55);

    label_unit = new QLabel();
    label_unit->setAlignment(Qt::AlignLeft);
    label_unit->setFont(font);
    label_unit->setFixedWidth(40);

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setMargin(0);
    layout_1->setSpacing(0);
    layout_1->addWidget(label_name);
    layout_1->addWidget(label_value);
    layout_1->addSpacing(5);
    layout_1->addWidget(label_unit);

    setLayout(layout_1);
    setFixedSize(200, 20);

    setName(name);
    setValue(value);
    setUnit(unit);
}

void QMyLabel::setName(const QString &sname)
{
    if (label_name->text() != sname) label_name->setText(sname);
}

void QMyLabel::setValue(const QString &svalue)
{
    if (label_value->text() != svalue) label_value->setText(svalue);
}

void QMyLabel::setUnit(const QString &sunit)
{
    if (label_unit->text() != sunit) label_unit->setText(sunit);
}

QString QMyLabel::getName()
{
    return label_name->text();
}

QString QMyLabel::getValue()
{
    return label_value->text();
}

QString QMyLabel::getUnit()
{
    return label_unit->text();
}
