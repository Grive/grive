/********************************************************************************
** Form generated from reading UI file 'Settings.ui'
**
** Created: Sun Mar 16 21:29:00 2014
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QCommandLinkButton>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QTimeEdit>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QToolButton *toolButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_6;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QCheckBox *checkBox_5;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_4;
    QCommandLinkButton *commandLinkButton_2;
    QTimeEdit *timeEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QToolButton *toolButton_2;
    QLabel *label_3;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QString::fromUtf8("Settings"));
        Settings->resize(520, 309);
        buttonBox = new QDialogButtonBox(Settings);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(210, 270, 301, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit = new QLineEdit(Settings);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(80, 10, 401, 25));
        label = new QLabel(Settings);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(5, 12, 71, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        toolButton = new QToolButton(Settings);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setGeometry(QRect(480, 11, 26, 23));
        verticalLayoutWidget = new QWidget(Settings);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(350, 39, 166, 221));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox_2 = new QCheckBox(verticalLayoutWidget);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        verticalLayout->addWidget(checkBox_2);

        checkBox_6 = new QCheckBox(verticalLayoutWidget);
        checkBox_6->setObjectName(QString::fromUtf8("checkBox_6"));

        verticalLayout->addWidget(checkBox_6);

        radioButton = new QRadioButton(verticalLayoutWidget);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));

        verticalLayout->addWidget(radioButton);

        radioButton_2 = new QRadioButton(verticalLayoutWidget);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

        verticalLayout->addWidget(radioButton_2);

        checkBox_5 = new QCheckBox(verticalLayoutWidget);
        checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

        verticalLayout->addWidget(checkBox_5);

        checkBox_3 = new QCheckBox(verticalLayoutWidget);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

        verticalLayout->addWidget(checkBox_3);

        checkBox_4 = new QCheckBox(verticalLayoutWidget);
        checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));

        verticalLayout->addWidget(checkBox_4);

        commandLinkButton_2 = new QCommandLinkButton(Settings);
        commandLinkButton_2->setObjectName(QString::fromUtf8("commandLinkButton_2"));
        commandLinkButton_2->setGeometry(QRect(130, 270, 170, 32));
        timeEdit = new QTimeEdit(Settings);
        timeEdit->setObjectName(QString::fromUtf8("timeEdit"));
        timeEdit->setGeometry(QRect(190, 237, 149, 25));
        timeEdit->setTime(QTime(0, 5, 0));
        label_2 = new QLabel(Settings);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(110, 240, 81, 20));
        lineEdit_2 = new QLineEdit(Settings);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(80, 40, 241, 25));
        toolButton_2 = new QToolButton(Settings);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));
        toolButton_2->setGeometry(QRect(320, 41, 26, 23));
        label_3 = new QLabel(Settings);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(5, 42, 71, 20));
        label_3->setFont(font);

        retranslateUi(Settings);
        QObject::connect(buttonBox, SIGNAL(accepted()), Settings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Settings, SLOT(reject()));

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "Dialog", 0, QApplication::UnicodeUTF8));
        lineEdit->setText(QApplication::translate("Settings", "~/", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Settings", "Grive root", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("Settings", "...", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("Settings", "Force", 0, QApplication::UnicodeUTF8));
        checkBox_6->setText(QApplication::translate("Settings", "Enable Drag and Drop", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("Settings", "Sync local each period", 0, QApplication::UnicodeUTF8));
        radioButton_2->setText(QApplication::translate("Settings", "use iNotify", 0, QApplication::UnicodeUTF8));
        checkBox_5->setText(QApplication::translate("Settings", "Dry mode", 0, QApplication::UnicodeUTF8));
        checkBox_3->setText(QApplication::translate("Settings", "Enable debuging", 0, QApplication::UnicodeUTF8));
        checkBox_4->setText(QApplication::translate("Settings", "Enable logging", 0, QApplication::UnicodeUTF8));
        commandLinkButton_2->setText(QApplication::translate("Settings", "Request disk space", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Settings", "Sync period", 0, QApplication::UnicodeUTF8));
        toolButton_2->setText(QApplication::translate("Settings", "...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Settings", "Token", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
