#include "qmyedit.h"

QMyEdit::QMyEdit(QWidget *parent) : QWidget(parent)
{
    m_height = 270;
    m_width = 20;

    QFont font("MicroSoft Yahei", 9);

    label_name = new QLabel();
    label_name->setAlignment(Qt::AlignRight);
    label_name->setFont(font);
    label_name->setFixedWidth(100);

    edit_value = new QLineEdit();
    edit_value->setAlignment(Qt::AlignRight);
    edit_value->setFont(font);
    edit_value->setFixedSize(55, 18);

    label_unit = new QLabel();
    label_unit->setAlignment(Qt::AlignLeft);
    label_unit->setFont(font);
    label_unit->setFixedWidth(40);

    btn_change = new QPushButton(QStringLiteral("修改"));
    btn_change->setFont(font);
    btn_change->setFixedWidth(60);

    QHBoxLayout *layout_1 = new QHBoxLayout();
    layout_1->setMargin(0);
    layout_1->setSpacing(0);
    layout_1->addWidget(label_name);
    layout_1->addSpacing(5);
    layout_1->addWidget(edit_value);
    layout_1->addSpacing(5);
    layout_1->addWidget(label_unit);
    layout_1->addSpacing(5);
    layout_1->addWidget(btn_change);

    QString style_btn = QLatin1String(
                "QPushButton { border-radius: 3px;\n"
                              "border: none;\n"
                              "width:  40px;\n"
                              "height: 18px;\n"
                              "background: #78AADC;\n"
                              "color: white;}\n"
                 "QPushButton:hover   { background: #9AC0CD; }\n"
                 "QPushButton:pressed { background: #007ACC; }");
    btn_change->setStyleSheet(style_btn);

    setLayout(layout_1);
    setFixedSize(m_height, m_width);

    connect(btn_change, &QPushButton::clicked, this, &QMyEdit::on_btn_change);
}

QMyEdit::QMyEdit(const QString &name, const QString &value, const QString &unit, const bool &btnVisible, QWidget *parent) : QWidget(parent)
{
	m_height = 270;
	m_width = 20;

	QFont font("MicroSoft Yahei", 9);

	label_name = new QLabel();
	label_name->setAlignment(Qt::AlignRight);
	label_name->setFont(font);
	label_name->setFixedWidth(100);

	edit_value = new QLineEdit();
	edit_value->setAlignment(Qt::AlignRight);
	edit_value->setFont(font);
	edit_value->setFixedSize(55, 18);

	label_unit = new QLabel();
	label_unit->setAlignment(Qt::AlignLeft);
	label_unit->setFont(font);
	label_unit->setFixedWidth(40);

	btn_change = new QPushButton(QStringLiteral("修改"));
	btn_change->setFont(font);
	btn_change->setFixedWidth(60);

	QHBoxLayout *layout_1 = new QHBoxLayout();
	layout_1->setMargin(0);
	layout_1->setSpacing(0);
	layout_1->addWidget(label_name);
	layout_1->addSpacing(5);
	layout_1->addWidget(edit_value);
	layout_1->addSpacing(5);
	layout_1->addWidget(label_unit);
	layout_1->addSpacing(5);
	layout_1->addWidget(btn_change);

	QString style_btn = QLatin1String(
		"QPushButton { border-radius: 3px;\n"
		"border: none;\n"
		"width:  40px;\n"
		"height: 18px;\n"
		"background: #78AADC;\n"
		"color: white;}\n"
		"QPushButton:hover   { background: #9AC0CD; }\n"
		"QPushButton:pressed { background: #007ACC; }");
	btn_change->setStyleSheet(style_btn);

	setLayout(layout_1);
	setFixedSize(m_height, m_width);

	connect(btn_change, &QPushButton::clicked, this, &QMyEdit::on_btn_change);

    setName(name);
    setValue(value);
    setUnit(unit);
	setBtnVisible(btnVisible);
}

void QMyEdit::setName(const QString &sname)
{
    if (label_name->text() !=  sname) label_name->setText(sname);
}

void QMyEdit::setValue(const QString &svalue)
{
    if (edit_value->text() != svalue) edit_value->setText(svalue);
}

void QMyEdit::setUnit(const QString &sunit)
{
    if (label_unit->text() != sunit) label_unit->setText(sunit);
}

void QMyEdit::setBtnName(const QString &sname)
{
    if (btn_change->text() != sname) btn_change->setText(sname);
}

void QMyEdit::setBtnVisible(const bool &svisible)
{
	btn_change->setVisible(svisible);
}

QString QMyEdit::getName()
{
    return label_name->text();
}

QString QMyEdit::getValue()
{
    return edit_value->text();
}

QString QMyEdit::getUnit()
{
    return label_unit->text();
}


void QMyEdit::on_btn_change()
{
    emit qclicked();
}

