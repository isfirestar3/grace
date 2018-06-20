/********************************************************************************
** Form generated from reading UI file 'login_wid.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_WID_H
#define UI_LOGIN_WID_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_login_wid
{
public:
    QGridLayout *gridLayout;
    QLabel *label;

    void setupUi(QDialog *login_wid)
    {
        if (login_wid->objectName().isEmpty())
            login_wid->setObjectName(QStringLiteral("login_wid"));
        login_wid->resize(351, 77);
        gridLayout = new QGridLayout(login_wid);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(login_wid);
        label->setObjectName(QStringLiteral("label"));
        label->setStyleSheet(QString::fromUtf8("	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
"    min-width:130px;\n"
"	border-radius: 8px;\n"
"	\n"
"color: rgb(0, 0, 0);\n"
"	\n"
"background-color: rgb(207, 207, 207);\n"
"font: 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        gridLayout->addWidget(label, 0, 0, 1, 1);


        retranslateUi(login_wid);

        QMetaObject::connectSlotsByName(login_wid);
    } // setupUi

    void retranslateUi(QDialog *login_wid)
    {
        login_wid->setWindowTitle(QApplication::translate("login_wid", "login_wid", 0));
        label->setText(QApplication::translate("login_wid", "It's connecting\357\274\214please wait \342\200\246", 0));
    } // retranslateUi

};

namespace Ui {
    class login_wid: public Ui_login_wid {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_WID_H
