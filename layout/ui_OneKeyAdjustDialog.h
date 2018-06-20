/********************************************************************************
** Form generated from reading UI file 'OneKeyAdjustDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONEKEYADJUSTDIALOG_H
#define UI_ONEKEYADJUSTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_OneKeyAdjustDialog
{
public:
    QGridLayout *gridLayout;
    QComboBox *wopIdCmb;
    QRadioButton *oneWayRadioBtn;
    QLabel *label;
    QRadioButton *twoWayRadioBtn;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancelBtn;
    QPushButton *okBtn;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *OneKeyAdjustDialog)
    {
        if (OneKeyAdjustDialog->objectName().isEmpty())
            OneKeyAdjustDialog->setObjectName(QStringLiteral("OneKeyAdjustDialog"));
        OneKeyAdjustDialog->resize(397, 111);
        gridLayout = new QGridLayout(OneKeyAdjustDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        wopIdCmb = new QComboBox(OneKeyAdjustDialog);
        wopIdCmb->setObjectName(QStringLiteral("wopIdCmb"));

        gridLayout->addWidget(wopIdCmb, 2, 1, 1, 1);

        oneWayRadioBtn = new QRadioButton(OneKeyAdjustDialog);
        oneWayRadioBtn->setObjectName(QStringLiteral("oneWayRadioBtn"));

        gridLayout->addWidget(oneWayRadioBtn, 1, 0, 1, 3);

        label = new QLabel(OneKeyAdjustDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        twoWayRadioBtn = new QRadioButton(OneKeyAdjustDialog);
        twoWayRadioBtn->setObjectName(QStringLiteral("twoWayRadioBtn"));

        gridLayout->addWidget(twoWayRadioBtn, 0, 0, 1, 3);

        horizontalSpacer_2 = new QSpacerItem(94, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        cancelBtn = new QPushButton(OneKeyAdjustDialog);
        cancelBtn->setObjectName(QStringLiteral("cancelBtn"));

        gridLayout->addWidget(cancelBtn, 3, 4, 1, 1);

        okBtn = new QPushButton(OneKeyAdjustDialog);
        okBtn->setObjectName(QStringLiteral("okBtn"));

        gridLayout->addWidget(okBtn, 3, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(217, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 3, 0, 1, 3);


        retranslateUi(OneKeyAdjustDialog);

        QMetaObject::connectSlotsByName(OneKeyAdjustDialog);
    } // setupUi

    void retranslateUi(QDialog *OneKeyAdjustDialog)
    {
        OneKeyAdjustDialog->setWindowTitle(QApplication::translate("OneKeyAdjustDialog", "Dialog", 0));
        oneWayRadioBtn->setText(QApplication::translate("OneKeyAdjustDialog", "Adjust all edge to one way", 0));
        label->setText(QApplication::translate("OneKeyAdjustDialog", "Wop ID:", 0));
        twoWayRadioBtn->setText(QApplication::translate("OneKeyAdjustDialog", "Adjust all edge to two way", 0));
        cancelBtn->setText(QApplication::translate("OneKeyAdjustDialog", "Cancel", 0));
        okBtn->setText(QApplication::translate("OneKeyAdjustDialog", "Ok", 0));
    } // retranslateUi

};

namespace Ui {
    class OneKeyAdjustDialog: public Ui_OneKeyAdjustDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONEKEYADJUSTDIALOG_H
