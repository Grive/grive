/********************************************************************************
** Form generated from reading UI file 'About.ui'
**
** Created: Mon Mar 17 00:17:21 2014
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_About
{
public:
    QDialogButtonBox *buttonBox;
    QListWidget *listWidget;
    QLabel *label;
    QLabel *version;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *About)
    {
        if (About->objectName().isEmpty())
            About->setObjectName(QString::fromUtf8("About"));
        About->resize(448, 308);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../../../mnt/safer/home/jpuchky/Projects/grive/icon/32x32/32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        About->setWindowIcon(icon);
        buttonBox = new QDialogButtonBox(About);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(360, 10, 81, 221));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        listWidget = new QListWidget(About);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(10, 80, 431, 201));
        label = new QLabel(About);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 285, 56, 15));
        version = new QLabel(About);
        version->setObjectName(QString::fromUtf8("version"));
        version->setGeometry(QRect(60, 285, 56, 15));
        label_2 = new QLabel(About);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(130, 60, 91, 16));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_2->setFont(font);
        label_3 = new QLabel(About);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 10, 121, 121));
        label_3->setPixmap(QPixmap(QString::fromUtf8("../../../../../../mnt/safer/home/jpuchky/Projects/grive/icon/128x128/128x128.png")));

        retranslateUi(About);
        QObject::connect(buttonBox, SIGNAL(accepted()), About, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), About, SLOT(reject()));

        QMetaObject::connectSlotsByName(About);
    } // setupUi

    void retranslateUi(QDialog *About)
    {
        About->setWindowTitle(QApplication::translate("About", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("About", "Version:", 0, QApplication::UnicodeUTF8));
        version->setText(QApplication::translate("About", "0.3.1", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("About", "Contributors", 0, QApplication::UnicodeUTF8));
        label_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class About: public Ui_About {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
