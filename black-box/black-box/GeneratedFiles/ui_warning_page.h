/********************************************************************************
** Form generated from reading UI file 'warning_page.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WARNING_PAGE_H
#define UI_WARNING_PAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_warning_page
{
public:
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QRadioButton *yesRadioButton;
    QRadioButton *noRadioButton;
    QPushButton *yesButton;
    QPushButton *cancelButton;
    QFrame *frame;
    QLabel *label_2;

    void setupUi(QDialog *warning_page)
    {
        if (warning_page->objectName().isEmpty())
            warning_page->setObjectName(QStringLiteral("warning_page"));
        warning_page->resize(433, 274);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        warning_page->setFont(font);
        warning_page->setStyleSheet(QStringLiteral("background-color: rgb(253, 253, 253);"));
        label = new QLabel(warning_page);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(130, 20, 281, 31));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(14);
        label->setFont(font1);
        label_3 = new QLabel(warning_page);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(110, 80, 281, 31));
        label_3->setFont(font1);
        label_4 = new QLabel(warning_page);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(50, 140, 301, 31));
        label_4->setFont(font1);
        yesRadioButton = new QRadioButton(warning_page);
        yesRadioButton->setObjectName(QStringLiteral("yesRadioButton"));
        yesRadioButton->setGeometry(QRect(50, 180, 61, 16));
        yesRadioButton->setFont(font);
        noRadioButton = new QRadioButton(warning_page);
        noRadioButton->setObjectName(QStringLiteral("noRadioButton"));
        noRadioButton->setGeometry(QRect(120, 180, 51, 16));
        noRadioButton->setFont(font);
        noRadioButton->setChecked(true);
        yesButton = new QPushButton(warning_page);
        yesButton->setObjectName(QStringLiteral("yesButton"));
        yesButton->setGeometry(QRect(240, 220, 70, 40));
        yesButton->setFont(font);
        yesButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:40px;\n"
"    min-width:70px;\n"
"	border-radius: 8px;\n"
"	color: rgb(255, 255, 255);\n"
"	\n"
"	background-color: rgb(26, 188, 156);\n"
"    background-repeat:no-repeat;\n"
"}\n"
"\n"
"\n"
".QPushButton:hover{ \n"
"background-color: rgb(74, 215, 187);\n"
"    background-repeat:no-repeat;\n"
"}\n"
"\n"
".QPushButton:pressed{ \n"
"background-color: rgb(19, 159, 131);\n"
"}\n"
""));
        cancelButton = new QPushButton(warning_page);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setGeometry(QRect(330, 220, 70, 40));
        cancelButton->setFont(font);
        cancelButton->setStyleSheet(QLatin1String(".QPushButton {\n"
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
        frame = new QFrame(warning_page);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(10, 20, 91, 91));
        frame->setStyleSheet(QLatin1String("	border-style: none;\n"
"	border: 0px;\n"
"	min-height:88px;\n"
"    min-width:88px;\n"
"	\n"
"background-image: url(:/black-box/icon/warning.png);\n"
"    background-repeat:no-repeat;"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(warning_page);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(110, 50, 311, 31));
        label_2->setFont(font1);

        retranslateUi(warning_page);
        QObject::connect(yesButton, SIGNAL(clicked()), warning_page, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), warning_page, SLOT(reject()));

        QMetaObject::connectSlotsByName(warning_page);
    } // setupUi

    void retranslateUi(QDialog *warning_page)
    {
        warning_page->setWindowTitle(QApplication::translate("warning_page", "Warning\357\274\201", 0));
        label->setText(QApplication::translate("warning_page", "To ensure the safety of the ", 0));
        label_3->setText(QApplication::translate("warning_page", "vehicles are not in operation !", 0));
        label_4->setText(QApplication::translate("warning_page", "Is the operation allowed?", 0));
        yesRadioButton->setText(QApplication::translate("warning_page", "Yes", 0));
        noRadioButton->setText(QApplication::translate("warning_page", "No", 0));
        yesButton->setText(QApplication::translate("warning_page", "OK", 0));
        cancelButton->setText(QApplication::translate("warning_page", "Cancle", 0));
        label_2->setText(QApplication::translate("warning_page", "vehicles,please ensure that these", 0));
    } // retranslateUi

};

namespace Ui {
    class warning_page: public Ui_warning_page {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WARNING_PAGE_H
