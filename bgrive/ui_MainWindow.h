/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Mon Mar 17 00:17:21 2014
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCalendarWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *m_action_exit;
    QAction *actionAbout;
    QAction *actionTopic;
    QAction *actionConsole;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QTreeView *m_dir;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGraphicsView *preview;
    QWidget *tab_2;
    QCalendarWidget *calendarWidget;
    QLineEdit *m_title;
    QLabel *lbl_m_title;
    QLineEdit *m_mime_type;
    QLabel *lbl_m_mime_type;
    QProgressBar *usageBar;
    QStatusBar *m_statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 601);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../../../mnt/safer/home/jpuchky/Projects/grive/icon/32x32/32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        m_action_exit = new QAction(MainWindow);
        m_action_exit->setObjectName(QString::fromUtf8("m_action_exit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../../../../../mnt/safer/home/jpuchky/Projects/grive/icon/16x16/16x16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon1);
        actionAbout->setIconVisibleInMenu(true);
        actionTopic = new QAction(MainWindow);
        actionTopic->setObjectName(QString::fromUtf8("actionTopic"));
        actionConsole = new QAction(MainWindow);
        actionConsole->setObjectName(QString::fromUtf8("actionConsole"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        m_dir = new QTreeView(splitter);
        m_dir->setObjectName(QString::fromUtf8("m_dir"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_dir->sizePolicy().hasHeightForWidth());
        m_dir->setSizePolicy(sizePolicy);
        splitter->addWidget(m_dir);

        horizontalLayout->addWidget(splitter);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        preview = new QGraphicsView(tab);
        preview->setObjectName(QString::fromUtf8("preview"));
        preview->setGeometry(QRect(0, 0, 381, 481));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        calendarWidget = new QCalendarWidget(tab_2);
        calendarWidget->setObjectName(QString::fromUtf8("calendarWidget"));
        calendarWidget->setGeometry(QRect(0, 294, 381, 181));
        m_title = new QLineEdit(tab_2);
        m_title->setObjectName(QString::fromUtf8("m_title"));
        m_title->setGeometry(QRect(82, 10, 291, 25));
        m_title->setReadOnly(true);
        lbl_m_title = new QLabel(tab_2);
        lbl_m_title->setObjectName(QString::fromUtf8("lbl_m_title"));
        lbl_m_title->setGeometry(QRect(10, 12, 56, 21));
        m_mime_type = new QLineEdit(tab_2);
        m_mime_type->setObjectName(QString::fromUtf8("m_mime_type"));
        m_mime_type->setGeometry(QRect(82, 40, 291, 25));
        lbl_m_mime_type = new QLabel(tab_2);
        lbl_m_mime_type->setObjectName(QString::fromUtf8("lbl_m_mime_type"));
        lbl_m_mime_type->setGeometry(QRect(10, 45, 71, 16));
        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        usageBar = new QProgressBar(centralwidget);
        usageBar->setObjectName(QString::fromUtf8("usageBar"));
        usageBar->setValue(24);

        verticalLayout->addWidget(usageBar);


        horizontalLayout->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralwidget);
        m_statusbar = new QStatusBar(MainWindow);
        m_statusbar->setObjectName(QString::fromUtf8("m_statusbar"));
        MainWindow->setStatusBar(m_statusbar);

        retranslateUi(MainWindow);
        QObject::connect(m_action_exit, SIGNAL(activated()), MainWindow, SLOT(close()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Grive", 0, QApplication::UnicodeUTF8));
        m_action_exit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("MainWindow", "About grive", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionTopic->setText(QApplication::translate("MainWindow", "Topic", 0, QApplication::UnicodeUTF8));
        actionConsole->setText(QApplication::translate("MainWindow", "Console", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionConsole->setToolTip(QApplication::translate("MainWindow", "Open console window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Preview", 0, QApplication::UnicodeUTF8));
        lbl_m_title->setText(QApplication::translate("MainWindow", "Title", 0, QApplication::UnicodeUTF8));
        lbl_m_mime_type->setText(QApplication::translate("MainWindow", "Mime-type", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Details", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
