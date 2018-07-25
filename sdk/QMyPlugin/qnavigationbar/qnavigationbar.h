#ifndef QNAVIGATIONBAR_H
#define QNAVIGATIONBAR_H

#include <QWidget>
#include <QtWidgets>

#include "../qmyplugin_global.h"

class QMYPLUGINSHARED_EXPORT QHNavigationBar : public QWidget
{
    Q_OBJECT
public:
    explicit QHNavigationBar(QWidget *parent = nullptr);

    void addItem  (const QString &title);

    void setHeight(const int &height);
    void setColumnWidth(const int &col_width);

    void setBackgroundColor(const QString &color);
    void setSelectColor    (const QString &color);

    void setPenColorSelected  (const QString &color);
    void setPenColorBackground(const QString &color);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    QList<QString> listItems;

    QString backgroundColor;
    QString selectedColor;

    QString penColor_selected;
    QString penColor_background;

    int columnWidth;
    int currentIndex;

signals:
    void currentItemChanged(const int &index);
};


class QMYPLUGINSHARED_EXPORT QVNavigationBar : public QWidget
{
    Q_OBJECT
public:
    explicit QVNavigationBar(QWidget *parent=0);

    void addItem (const QString &title);

    void setWidth(const int &width);
    void setRowHeight(const int &height);

    void setBackgroundColor(const QString &color);
    void setSelectColor(const QString &color);

    void setPenColorSelected  (const QString &color);
    void setPenColorBackground(const QString &color);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    QList<QString> listItems;

    QString backgroundColor;
    QString selectedColor;

    QString penColor_selected;
    QString penColor_background;

    int rowHeight;
    int currentIndex;

signals:
    void currentItemChanged(const int &index);
};


#endif // QNAVIGATIONBAR_H
