/********************************************************************************
** Form generated from reading UI file 'Console.ui'
**
** Created: Mon Mar 17 00:17:21 2014
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>

QT_BEGIN_NAMESPACE

class Ui_Console
{
public:
    QDialogButtonBox *buttonBox;
    QListView *listView;

    void setupUi(QDialog *Console)
    {
        if (Console->objectName().isEmpty())
            Console->setObjectName(QString::fromUtf8("Console"));
        Console->resize(640, 480);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../../../mnt/safer/home/jpuchky/Projects/grive/icon/32x32/32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        Console->setWindowIcon(icon);
        buttonBox = new QDialogButtonBox(Console);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 440, 621, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        listView = new QListView(Console);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setGeometry(QRect(10, 10, 621, 421));

        retranslateUi(Console);
        QObject::connect(buttonBox, SIGNAL(accepted()), Console, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Console, SLOT(reject()));

        QMetaObject::connectSlotsByName(Console);
    } // setupUi

    void retranslateUi(QDialog *Console)
    {
        Console->setWindowTitle(QApplication::translate("Console", "Console", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Console->setToolTip(QApplication::translate("Console", "Console window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class Console: public Ui_Console {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSOLE_H
