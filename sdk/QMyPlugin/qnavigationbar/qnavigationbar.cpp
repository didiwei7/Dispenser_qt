#include "qnavigationbar.h"

QHNavigationBar::QHNavigationBar(QWidget *parent) : QWidget(parent)
{
    backgroundColor = "#E4E4E4";
    selectedColor   = "#2CA7F8";

    penColor_selected   = "#FFFFFF";
    penColor_background = "#202020";

    columnWidth     = 40;
    currentIndex    = 0;

    this->setMouseTracking(true);
    this->setFixedHeight(30);
}

void QHNavigationBar::addItem(const QString &title)
{
    listItems << title;
    this->update();
}

void QHNavigationBar::setHeight(const int &height)
{
    this->setFixedHeight(height);
    this->update();
}

void QHNavigationBar::setColumnWidth(const int &col_width)
{
    columnWidth = col_width;
    this->update();
}

void QHNavigationBar::setPenColorBackground(const QString &color)
{
    penColor_background = color;
    this->update();
}

void QHNavigationBar::setPenColorSelected(const QString &color)
{
    penColor_selected = color;
    this->update();
}

void QHNavigationBar::setBackgroundColor(const QString &color)
{
    backgroundColor = color;
    this->update();
}

void QHNavigationBar::setSelectColor(const QString &color)
{
    selectedColor = color;
    this->update();
}

void QHNavigationBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(backgroundColor));
    painter.drawRect(rect());

    int count = 0;
    for (const QString &str : listItems)
    {
        QPainterPath itemPath;
        itemPath.addRect(QRect(count * columnWidth, 0, columnWidth, this->height()));

        if (currentIndex == count)
        {
            painter.setPen(penColor_selected);
            painter.fillPath(itemPath, QColor(selectedColor));
        }
        else
        {
            painter.setPen(penColor_background);
            painter.fillPath(itemPath, QColor(backgroundColor));
        }

        painter.drawText(QRect(count * columnWidth, 0, columnWidth, this->height()),
                               Qt::AlignVCenter | Qt::AlignHCenter, str);

        ++count;
    }
}

void QHNavigationBar::mousePressEvent(QMouseEvent *e)
{
    if (e->x() / columnWidth < listItems.count())
    {
        currentIndex = e->x() / columnWidth;
        emit currentItemChanged(currentIndex);
        this->update();
    }
}



QVNavigationBar::QVNavigationBar(QWidget *parent) : QWidget(parent)
{
    backgroundColor = "#E4E4E4";
    selectedColor   = "#2CA7F8";

    penColor_selected   = "#FFFFFF";
    penColor_background = "#202020";

    rowHeight       = 40;
    currentIndex    = 0;

    this->setMouseTracking(true);
    this->setFixedWidth(150);
}

void QVNavigationBar::addItem(const QString &title)
{
    listItems << title;
    this->update();
}

void QVNavigationBar::setWidth(const int &width)
{
    this->setFixedWidth(width);
    this->update();
}

void QVNavigationBar::setRowHeight(const int &height)
{
    rowHeight = height;
    this->update();
}

void QVNavigationBar::setBackgroundColor(const QString &color)
{
    backgroundColor = color;
    this->update();
}

void QVNavigationBar::setSelectColor(const QString &color)
{
    selectedColor = color;
    this->update();
}

void QVNavigationBar::setPenColorBackground(const QString &color)
{
    penColor_background = color;
    this->update();
}

void QVNavigationBar::setPenColorSelected  (const QString &color)
{
    penColor_selected = color;
    this->update();
}

void QVNavigationBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(backgroundColor));
    painter.drawRect(rect());

    int count = 0;
    for (const QString &str : listItems)
    {
        QPainterPath itemPath;
        itemPath.addRect(QRect(0, count * rowHeight, this->width(), rowHeight));

        if (currentIndex == count)
        {
            painter.setPen(penColor_selected);
            painter.fillPath(itemPath, QColor(selectedColor));
        }
        else
        {
            painter.setPen(penColor_background);
            painter.fillPath(itemPath, QColor(backgroundColor));
        }

        painter.drawText(QRect(0, count * rowHeight, width(), rowHeight), Qt::AlignVCenter | Qt::AlignHCenter, str);
        ++count;
    }
}

void QVNavigationBar::mousePressEvent(QMouseEvent *e)
{
    if (e->y() / rowHeight < listItems.count())
    {
        currentIndex = e->y() / rowHeight;

        emit currentItemChanged(currentIndex);

        this->update();
    }
}
