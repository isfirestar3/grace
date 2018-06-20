/********************************************************************************
** Form generated from reading UI file 'generateemapdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERATEEMAPDIALOG_H
#define UI_GENERATEEMAPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_GenerateEmapDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *rowCountSpinBox;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QSpinBox *columnCountSpinBox;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QPushButton *okBtn;

    void setupUi(QDialog *GenerateEmapDialog)
    {
        if (GenerateEmapDialog->objectName().isEmpty())
            GenerateEmapDialog->setObjectName(QStringLiteral("GenerateEmapDialog"));
        GenerateEmapDialog->resize(263, 145);
        gridLayout = new QGridLayout(GenerateEmapDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(GenerateEmapDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 1, 1, 1);

        rowCountSpinBox = new QSpinBox(GenerateEmapDialog);
        rowCountSpinBox->setObjectName(QStringLiteral("rowCountSpinBox"));

        gridLayout->addWidget(rowCountSpinBox, 0, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 0, 1, 1);

        label_2 = new QLabel(GenerateEmapDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 1, 1, 1);

        columnCountSpinBox = new QSpinBox(GenerateEmapDialog);
        columnCountSpinBox->setObjectName(QStringLiteral("columnCountSpinBox"));

        gridLayout->addWidget(columnCountSpinBox, 1, 2, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(28, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 3, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(75, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 3, 1, 1, 1);

        okBtn = new QPushButton(GenerateEmapDialog);
        okBtn->setObjectName(QStringLiteral("okBtn"));

        gridLayout->addWidget(okBtn, 3, 2, 1, 1);


        retranslateUi(GenerateEmapDialog);

        QMetaObject::connectSlotsByName(GenerateEmapDialog);
    } // setupUi

    void retranslateUi(QDialog *GenerateEmapDialog)
    {
        GenerateEmapDialog->setWindowTitle(QApplication::translate("GenerateEmapDialog", "GenerateEmapDialog", 0));
        label->setText(QApplication::translate("GenerateEmapDialog", "Row count:", 0));
        label_2->setText(QApplication::translate("GenerateEmapDialog", "Column count:", 0));
        okBtn->setText(QApplication::translate("GenerateEmapDialog", "ok", 0));
    } // retranslateUi

};

namespace Ui {
    class GenerateEmapDialog: public Ui_GenerateEmapDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERATEEMAPDIALOG_H
