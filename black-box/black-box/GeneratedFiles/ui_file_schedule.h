/********************************************************************************
** Form generated from reading UI file 'file_schedule.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILE_SCHEDULE_H
#define UI_FILE_SCHEDULE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_file_schedule
{
public:
    QProgressBar *progressBar;
    QPushButton *pushButton;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *countLabel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *file_schedule)
    {
        if (file_schedule->objectName().isEmpty())
            file_schedule->setObjectName(QStringLiteral("file_schedule"));
        file_schedule->resize(420, 213);
        file_schedule->setStyleSheet(QStringLiteral("background-color: rgb(253, 253, 253);"));
        progressBar = new QProgressBar(file_schedule);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(30, 80, 361, 41));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        progressBar->setFont(font);
        progressBar->setValue(24);
        pushButton = new QPushButton(file_schedule);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(341, 164, 70, 40));
        pushButton->setFont(font);
        pushButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:40px;\n"
"    min-width:70px;\n"
"	border-radius: 8px;\n"
"	color: rgb(255, 255, 255);\n"
"	\n"
"	background-color: rgb(26, 137, 188);\n"
"    background-repeat:no-repeat;\n"
"}\n"
"\n"
"\n"
".QPushButton:hover{ \n"
"background-color: rgb(88, 174, 214);\n"
"    background-repeat:no-repeat;\n"
"}\n"
"\n"
".QPushButton:pressed{ \n"
"background-color: rgb(20, 113, 157);\n"
"}\n"
""));
        layoutWidget = new QWidget(file_schedule);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 30, 299, 23));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);

        horizontalLayout->addWidget(label);

        countLabel = new QLabel(layoutWidget);
        countLabel->setObjectName(QStringLiteral("countLabel"));
        countLabel->setFont(font);

        horizontalLayout->addWidget(countLabel);

        horizontalSpacer_2 = new QSpacerItem(237, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        retranslateUi(file_schedule);
        QObject::connect(pushButton, SIGNAL(clicked()), file_schedule, SLOT(accept()));

        QMetaObject::connectSlotsByName(file_schedule);
    } // setupUi

    void retranslateUi(QDialog *file_schedule)
    {
        file_schedule->setWindowTitle(QApplication::translate("file_schedule", "Schedule", 0));
        pushButton->setText(QApplication::translate("file_schedule", "cancel", 0));
        label->setText(QApplication::translate("file_schedule", "Total Number\357\274\232", 0));
        countLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class file_schedule: public Ui_file_schedule {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILE_SCHEDULE_H
