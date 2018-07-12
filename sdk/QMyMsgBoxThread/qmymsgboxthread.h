#ifndef QMYMSGBOXTHREAD_H
#define QMYMSGBOXTHREAD_H

#include <QThread>
#include <QMessageBox>

#include "qmymsgboxthread_global.h"

class QMYMSGBOXTHREADSHARED_EXPORT QMyMsgBoxThread : public QThread
{
    Q_OBJECT
    typedef enum
    {
        mabout = 0,
        maboutqt = 1,
        mcritical = 2,
        minformation = 3,
        mquestion = 4,
        mwarning = 5
    } MSGBOXTYPE;

public:
    QMyMsgBoxThread(QObject * parent = 0) : QThread(parent), m_btnres(QMessageBox::NoButton)
    {
        qRegisterMetaType<QMessageBox::StandardButtons>("QMessageBox::StandardButtons");
        // connect(this, SIGNAL(msgbox_sig(MSGBOXTYPE, QWidget *, const QString, const QString, QMessageBox::StandardButtons, QMessageBox::StandardButtons)), SLOT(on_information(MSGBOXTYPE, QWidget *, const QString , const QString, QMessageBox::StandardButtons , QMessageBox::StandardButtons )), Qt::BlockingQueuedConnection);
        connect(this, &QMyMsgBoxThread::msgbox_sig, this, &QMyMsgBoxThread::on_msgbox);
    }

private slots:
    void on_msgbox(MSGBOXTYPE type, QWidget * parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButtons defaultButton)
    {
       switch(type)
       {
           case mabout:
               QMessageBox::about(parent,title,text);
               break;
           case maboutqt:
               QMessageBox::aboutQt(parent, title);
               break;
           case mcritical:
               m_btnres = QMessageBox::critical(parent, title, text, buttons, defaultButton);
               break;
           case minformation:
               m_btnres = QMessageBox::information(parent, title, text, buttons, defaultButton);
               break;
           case mquestion:
               m_btnres = QMessageBox::question(parent, title, text, buttons, defaultButton);
               break;
           case mwarning:
               m_btnres = QMessageBox::warning(parent, title, text, buttons, defaultButton);
               break;
           default:
               Q_ASSERT_X(false, "QMessageBox in thread", "invalid box type specified.");
               break;
       }
    }

signals:
    void msgbox_sig(MSGBOXTYPE type, QWidget * parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButtons defaultButton);


protected:
   int m_btnres;

public:
   void about(QWidget * parent, const QString &title, const QString &text)
   {
      emit msgbox_sig(mabout, parent, title, text, QMessageBox::NoButton, QMessageBox::NoButton);
   }

   void aboutQt(QWidget *parent, const QString &title = QString())
   {
      emit msgbox_sig(maboutqt, parent, title, tr(""), QMessageBox::NoButton, QMessageBox::NoButton);
   }

   int critical(QWidget * parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton)
   {
      emit msgbox_sig(mcritical, parent, title, text, buttons, defaultButton);
      return m_btnres;
   }

   int information(QWidget * parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton)
   {
      emit msgbox_sig(minformation, parent, title, text, buttons, defaultButton);
      return m_btnres;
   }

   int question(QWidget * parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No), QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton)
   {
      emit msgbox_sig(mquestion, parent, title, text, buttons, defaultButton);
      return m_btnres;
   }

   int warning(QWidget * parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton)
   {
      emit msgbox_sig(mwarning, parent, title, text, buttons, defaultButton);
      return m_btnres;
   }
};


#endif // QMYMSGBOXTHREAD_H
