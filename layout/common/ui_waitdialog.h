/********************************************************************************
** Form generated from reading UI file 'waitdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAITDIALOG_H
#define UI_WAITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WaitDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;

    void setupUi(QWidget *WaitDialog)
    {
        if (WaitDialog->objectName().isEmpty())
            WaitDialog->setObjectName(QStringLiteral("WaitDialog"));
        WaitDialog->resize(228, 45);
        gridLayout = new QGridLayout(WaitDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(WaitDialog);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);


        retranslateUi(WaitDialog);

        QMetaObject::connectSlotsByName(WaitDialog);
    } // setupUi

    void retranslateUi(QWidget *WaitDialog)
    {
        WaitDialog->setWindowTitle(QApplication::translate("WaitDialog", "WaitDialog", 0));
        label->setText(QApplication::translate("WaitDialog", "Please wait...", 0));
    } // retranslateUi

};

namespace Ui {
    class WaitDialog: public Ui_WaitDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAITDIALOG_H
