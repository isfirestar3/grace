/********************************************************************************
** Form generated from reading UI file 'mainMind.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINMIND_H
#define UI_MAINMIND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_log_selectClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_4;
    QStackedWidget *stackedWidget;
    QWidget *loginPage;
    QGridLayout *gridLayout_3;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_13;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_8;
    QRadioButton *radioButton;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *getIpButton;
    QFrame *line_2;
    QRadioButton *radioButton2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QComboBox *ipComboBox;
    QLabel *label_9;
    QLabel *label_4;
    QLineEdit *portEdit;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_9;
    QPushButton *getIpButton2;
    QCheckBox *AllcheckBox;
    QListWidget *iplistWidget;
    QSpacerItem *horizontalSpacer_14;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_12;
    QPushButton *connectButton;
    QSpacerItem *verticalSpacer;
    QWidget *getInfoPage;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QSpacerItem *horizontalSpacer_15;
    QLabel *label_12;
    QFrame *line_3;
    QSpacerItem *horizontalSpacer_5;
    QTableView *tableView;
    QRadioButton *userDefButton;
    QPushButton *selectButton;
    QFrame *line;
    QSpacerItem *horizontalSpacer_4;
    QListWidget *connectedListWidget;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QLineEdit *ftsPortEdit;
    QLabel *label_11;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout;
    QRadioButton *todayButton;
    QRadioButton *yestButton;
    QRadioButton *allTimeButton;
    QRadioButton *lastTowHourButton;
    QRadioButton *orderButton;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *disconnectButton;
    QLabel *label_7;
    QPushButton *sendLogButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QDateTimeEdit *startTimeEdit;
    QLabel *label_2;
    QDateTimeEdit *endTimeEdit;
    QCheckBox *newTimeBox;
    QSpacerItem *horizontalSpacer_6;
    QCheckBox *checkAllBox;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *checkBox;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_8;
    QLineEdit *selectEdit;
    QHBoxLayout *horizontalLayout_9;
    QRadioButton *tempButton;
    QSpacerItem *horizontalSpacer_11;
    QPushButton *templateButton;
    QCheckBox *systemLogBox;
    QCheckBox *positionBox;
    QCheckBox *repositionBox;

    void setupUi(QMainWindow *log_selectClass)
    {
        if (log_selectClass->objectName().isEmpty())
            log_selectClass->setObjectName(QStringLiteral("log_selectClass"));
        log_selectClass->resize(810, 770);
        log_selectClass->setStyleSheet(QLatin1String("QMainWindow{background-color: rgb(253, 253, 253);}\n"
""));
        centralWidget = new QWidget(log_selectClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 808, 768));
        gridLayout_4 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        stackedWidget = new QStackedWidget(scrollAreaWidgetContents);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setStyleSheet(QStringLiteral(""));
        loginPage = new QWidget();
        loginPage->setObjectName(QStringLiteral("loginPage"));
        gridLayout_3 = new QGridLayout(loginPage);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        verticalSpacer_2 = new QSpacerItem(20, 32, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_13);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        radioButton = new QRadioButton(loginPage);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        radioButton->setFont(font);
        radioButton->setChecked(true);

        horizontalLayout_8->addWidget(radioButton);

        horizontalSpacer_8 = new QSpacerItem(348, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);

        getIpButton = new QPushButton(loginPage);
        getIpButton->setObjectName(QStringLiteral("getIpButton"));
        getIpButton->setFont(font);
        getIpButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
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

        horizontalLayout_8->addWidget(getIpButton);


        verticalLayout->addLayout(horizontalLayout_8);

        line_2 = new QFrame(loginPage);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        radioButton2 = new QRadioButton(loginPage);
        radioButton2->setObjectName(QStringLiteral("radioButton2"));
        radioButton2->setFont(font);

        verticalLayout->addWidget(radioButton2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_3 = new QLabel(loginPage);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);

        horizontalLayout_5->addWidget(label_3);

        ipComboBox = new QComboBox(loginPage);
        ipComboBox->setObjectName(QStringLiteral("ipComboBox"));
        ipComboBox->setMinimumSize(QSize(150, 0));
        ipComboBox->setMaximumSize(QSize(200, 16777215));
        ipComboBox->setFont(font);
        ipComboBox->setStyleSheet(QString::fromUtf8("font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        ipComboBox->setEditable(true);

        horizontalLayout_5->addWidget(ipComboBox);

        label_9 = new QLabel(loginPage);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_5->addWidget(label_9);

        label_4 = new QLabel(loginPage);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font);

        horizontalLayout_5->addWidget(label_4);

        portEdit = new QLineEdit(loginPage);
        portEdit->setObjectName(QStringLiteral("portEdit"));
        portEdit->setMaximumSize(QSize(100, 16777215));
        portEdit->setFont(font);

        horizontalLayout_5->addWidget(portEdit);

        label_10 = new QLabel(loginPage);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_5->addWidget(label_10);

        horizontalSpacer_9 = new QSpacerItem(78, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_9);

        getIpButton2 = new QPushButton(loginPage);
        getIpButton2->setObjectName(QStringLiteral("getIpButton2"));
        getIpButton2->setFont(font);
        getIpButton2->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
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

        horizontalLayout_5->addWidget(getIpButton2);


        verticalLayout->addLayout(horizontalLayout_5);

        AllcheckBox = new QCheckBox(loginPage);
        AllcheckBox->setObjectName(QStringLiteral("AllcheckBox"));
        AllcheckBox->setFont(font);

        verticalLayout->addWidget(AllcheckBox);

        iplistWidget = new QListWidget(loginPage);
        iplistWidget->setObjectName(QStringLiteral("iplistWidget"));
        iplistWidget->setFont(font);
        iplistWidget->setStyleSheet(QString::fromUtf8("font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        iplistWidget->setMovement(QListView::Static);
        iplistWidget->setFlow(QListView::LeftToRight);
        iplistWidget->setProperty("isWrapping", QVariant(true));
        iplistWidget->setResizeMode(QListView::Fixed);

        verticalLayout->addWidget(iplistWidget);


        horizontalLayout_6->addLayout(verticalLayout);

        horizontalSpacer_14 = new QSpacerItem(58, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_14);


        gridLayout_3->addLayout(horizontalLayout_6, 1, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalSpacer_12 = new QSpacerItem(548, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_12);

        connectButton = new QPushButton(loginPage);
        connectButton->setObjectName(QStringLiteral("connectButton"));
        connectButton->setFont(font);
        connectButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
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

        horizontalLayout_7->addWidget(connectButton);


        gridLayout_3->addLayout(horizontalLayout_7, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 48, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 4, 0, 1, 1);

        stackedWidget->addWidget(loginPage);
        getInfoPage = new QWidget();
        getInfoPage->setObjectName(QStringLiteral("getInfoPage"));
        gridLayout_2 = new QGridLayout(getInfoPage);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        tabWidget = new QTabWidget(getInfoPage);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setMaximumSize(QSize(16777215, 200));
        tabWidget->setFont(font);

        gridLayout_2->addWidget(tabWidget, 6, 0, 1, 7);

        horizontalSpacer_15 = new QSpacerItem(672, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_15, 4, 1, 1, 6);

        label_12 = new QLabel(getInfoPage);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setFont(font);

        gridLayout_2->addWidget(label_12, 0, 0, 1, 1);

        line_3 = new QFrame(getInfoPage);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(line_3, 3, 0, 1, 7);

        horizontalSpacer_5 = new QSpacerItem(537, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_5, 7, 2, 1, 5);

        tableView = new QTableView(getInfoPage);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setMinimumSize(QSize(0, 200));
        tableView->setFont(font);
        tableView->setStyleSheet(QString::fromUtf8("font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        tableView->horizontalHeader()->setCascadingSectionResizes(false);
        tableView->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->verticalHeader()->setCascadingSectionResizes(false);
        tableView->verticalHeader()->setStretchLastSection(false);

        gridLayout_2->addWidget(tableView, 9, 0, 1, 7);

        userDefButton = new QRadioButton(getInfoPage);
        userDefButton->setObjectName(QStringLiteral("userDefButton"));
        userDefButton->setFont(font);
        userDefButton->setChecked(true);

        gridLayout_2->addWidget(userDefButton, 7, 0, 1, 1);

        selectButton = new QPushButton(getInfoPage);
        selectButton->setObjectName(QStringLiteral("selectButton"));
        selectButton->setFont(font);
        selectButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
"    min-width:120px;\n"
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

        gridLayout_2->addWidget(selectButton, 4, 0, 1, 1);

        line = new QFrame(getInfoPage);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(line, 11, 0, 1, 7);

        horizontalSpacer_4 = new QSpacerItem(537, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 12, 2, 1, 5);

        connectedListWidget = new QListWidget(getInfoPage);
        connectedListWidget->setObjectName(QStringLiteral("connectedListWidget"));
        connectedListWidget->setMaximumSize(QSize(16777215, 160));
        connectedListWidget->setFont(font);
        connectedListWidget->setStyleSheet(QString::fromUtf8("font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        gridLayout_2->addWidget(connectedListWidget, 2, 0, 1, 7);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_6 = new QLabel(getInfoPage);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setFont(font);

        horizontalLayout_4->addWidget(label_6);

        ftsPortEdit = new QLineEdit(getInfoPage);
        ftsPortEdit->setObjectName(QStringLiteral("ftsPortEdit"));
        ftsPortEdit->setMaximumSize(QSize(100, 16777215));
        ftsPortEdit->setFont(font);

        horizontalLayout_4->addWidget(ftsPortEdit);

        label_11 = new QLabel(getInfoPage);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_4->addWidget(label_11);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_7);


        gridLayout_2->addLayout(horizontalLayout_4, 15, 0, 1, 3);

        horizontalSpacer_10 = new QSpacerItem(466, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_10, 10, 4, 1, 3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        todayButton = new QRadioButton(getInfoPage);
        todayButton->setObjectName(QStringLiteral("todayButton"));
        todayButton->setFont(font);

        horizontalLayout->addWidget(todayButton);

        yestButton = new QRadioButton(getInfoPage);
        yestButton->setObjectName(QStringLiteral("yestButton"));
        yestButton->setFont(font);

        horizontalLayout->addWidget(yestButton);

        allTimeButton = new QRadioButton(getInfoPage);
        allTimeButton->setObjectName(QStringLiteral("allTimeButton"));
        allTimeButton->setFont(font);

        horizontalLayout->addWidget(allTimeButton);

        lastTowHourButton = new QRadioButton(getInfoPage);
        lastTowHourButton->setObjectName(QStringLiteral("lastTowHourButton"));
        lastTowHourButton->setFont(font);

        horizontalLayout->addWidget(lastTowHourButton);

        orderButton = new QRadioButton(getInfoPage);
        orderButton->setObjectName(QStringLiteral("orderButton"));
        orderButton->setFont(font);
        orderButton->setChecked(true);

        horizontalLayout->addWidget(orderButton);

        horizontalSpacer_3 = new QSpacerItem(367, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        gridLayout_2->addLayout(horizontalLayout, 13, 0, 1, 7);

        horizontalSpacer = new QSpacerItem(497, 26, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 16, 0, 1, 5);

        disconnectButton = new QPushButton(getInfoPage);
        disconnectButton->setObjectName(QStringLiteral("disconnectButton"));
        disconnectButton->setMaximumSize(QSize(200, 16777215));
        disconnectButton->setFont(font);
        disconnectButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
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

        gridLayout_2->addWidget(disconnectButton, 16, 6, 1, 1);

        label_7 = new QLabel(getInfoPage);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setFont(font);

        gridLayout_2->addWidget(label_7, 12, 0, 1, 1);

        sendLogButton = new QPushButton(getInfoPage);
        sendLogButton->setObjectName(QStringLiteral("sendLogButton"));
        sendLogButton->setMaximumSize(QSize(400, 16777215));
        sendLogButton->setFont(font);
        sendLogButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
"    min-width:200px;\n"
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

        gridLayout_2->addWidget(sendLogButton, 16, 5, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(getInfoPage);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);

        horizontalLayout_2->addWidget(label);

        startTimeEdit = new QDateTimeEdit(getInfoPage);
        startTimeEdit->setObjectName(QStringLiteral("startTimeEdit"));
        startTimeEdit->setFont(font);
        startTimeEdit->setStyleSheet(QString::fromUtf8("font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        startTimeEdit->setCalendarPopup(true);

        horizontalLayout_2->addWidget(startTimeEdit);

        label_2 = new QLabel(getInfoPage);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);

        horizontalLayout_2->addWidget(label_2);

        endTimeEdit = new QDateTimeEdit(getInfoPage);
        endTimeEdit->setObjectName(QStringLiteral("endTimeEdit"));
        endTimeEdit->setFont(font);
        endTimeEdit->setStyleSheet(QString::fromUtf8("font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        endTimeEdit->setCalendarPopup(true);
        endTimeEdit->setTimeSpec(Qt::LocalTime);

        horizontalLayout_2->addWidget(endTimeEdit);

        newTimeBox = new QCheckBox(getInfoPage);
        newTimeBox->setObjectName(QStringLiteral("newTimeBox"));
        newTimeBox->setFont(font);

        horizontalLayout_2->addWidget(newTimeBox);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);


        gridLayout_2->addLayout(horizontalLayout_2, 14, 0, 1, 6);

        checkAllBox = new QCheckBox(getInfoPage);
        checkAllBox->setObjectName(QStringLiteral("checkAllBox"));
        checkAllBox->setFont(font);

        gridLayout_2->addWidget(checkAllBox, 1, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        checkBox = new QCheckBox(getInfoPage);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setFont(font);

        horizontalLayout_3->addWidget(checkBox);

        horizontalSpacer_2 = new QSpacerItem(211, 23, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        label_8 = new QLabel(getInfoPage);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setFont(font);

        horizontalLayout_3->addWidget(label_8);

        selectEdit = new QLineEdit(getInfoPage);
        selectEdit->setObjectName(QStringLiteral("selectEdit"));
        selectEdit->setMaximumSize(QSize(200, 16777215));
        selectEdit->setFont(font);

        horizontalLayout_3->addWidget(selectEdit);


        gridLayout_2->addLayout(horizontalLayout_3, 8, 0, 1, 7);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        tempButton = new QRadioButton(getInfoPage);
        tempButton->setObjectName(QStringLiteral("tempButton"));
        tempButton->setFont(font);

        horizontalLayout_9->addWidget(tempButton);

        horizontalSpacer_11 = new QSpacerItem(518, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_11);

        templateButton = new QPushButton(getInfoPage);
        templateButton->setObjectName(QStringLiteral("templateButton"));
        templateButton->setFont(font);
        templateButton->setStyleSheet(QLatin1String(".QPushButton {\n"
"	border-style: none;\n"
"	border: 0px;\n"
"	min-height:32px;\n"
"    min-width:100px;\n"
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

        horizontalLayout_9->addWidget(templateButton);


        gridLayout_2->addLayout(horizontalLayout_9, 5, 0, 1, 7);

        systemLogBox = new QCheckBox(getInfoPage);
        systemLogBox->setObjectName(QStringLiteral("systemLogBox"));
        systemLogBox->setFont(font);

        gridLayout_2->addWidget(systemLogBox, 10, 3, 1, 1);

        positionBox = new QCheckBox(getInfoPage);
        positionBox->setObjectName(QStringLiteral("positionBox"));
        positionBox->setFont(font);

        gridLayout_2->addWidget(positionBox, 10, 0, 1, 1);

        repositionBox = new QCheckBox(getInfoPage);
        repositionBox->setObjectName(QStringLiteral("repositionBox"));
        repositionBox->setFont(font);

        gridLayout_2->addWidget(repositionBox, 10, 1, 1, 2);

        stackedWidget->addWidget(getInfoPage);

        gridLayout_4->addWidget(stackedWidget, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);

        log_selectClass->setCentralWidget(centralWidget);

        retranslateUi(log_selectClass);

        stackedWidget->setCurrentIndex(0);
        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(log_selectClass);
    } // setupUi

    void retranslateUi(QMainWindow *log_selectClass)
    {
        log_selectClass->setWindowTitle(QApplication::translate("log_selectClass", "black_box", 0));
        radioButton->setText(QApplication::translate("log_selectClass", "Get IP list from the local", 0));
        getIpButton->setText(QApplication::translate("log_selectClass", "obtain", 0));
        radioButton2->setText(QApplication::translate("log_selectClass", "Get IP list from the DHCP server", 0));
        label_3->setText(QApplication::translate("log_selectClass", "Ip\357\274\232", 0));
        ipComboBox->clear();
        ipComboBox->insertItems(0, QStringList()
         << QApplication::translate("log_selectClass", "192.168.1.28", 0)
        );
        label_9->setText(QApplication::translate("log_selectClass", "<html><head/><body><p><span style=\" color:#ff0000;\">*</span></p></body></html>", 0));
        label_4->setText(QApplication::translate("log_selectClass", "Port\357\274\232", 0));
        portEdit->setText(QApplication::translate("log_selectClass", "4422", 0));
        label_10->setText(QApplication::translate("log_selectClass", "<html><head/><body><p><span style=\" color:#ff0000;\">*</span></p></body></html>", 0));
        getIpButton2->setText(QApplication::translate("log_selectClass", "obtain", 0));
        AllcheckBox->setText(QApplication::translate("log_selectClass", "check all", 0));
        connectButton->setText(QApplication::translate("log_selectClass", "connect", 0));
        label_12->setText(QApplication::translate("log_selectClass", "Connected vehicle:", 0));
        userDefButton->setText(QApplication::translate("log_selectClass", "user-defined", 0));
        selectButton->setText(QApplication::translate("log_selectClass", "get log types", 0));
        label_6->setText(QApplication::translate("log_selectClass", "fts port\357\274\232", 0));
        ftsPortEdit->setText(QApplication::translate("log_selectClass", "4412", 0));
        label_11->setText(QApplication::translate("log_selectClass", "<html><head/><body><p><span style=\" color:#ff0000;\">*</span></p></body></html>", 0));
        todayButton->setText(QApplication::translate("log_selectClass", "Today", 0));
        yestButton->setText(QApplication::translate("log_selectClass", "Yesterday", 0));
        allTimeButton->setText(QApplication::translate("log_selectClass", "All", 0));
        lastTowHourButton->setText(QApplication::translate("log_selectClass", "Last 2 hour", 0));
        orderButton->setText(QApplication::translate("log_selectClass", "Specified time period:", 0));
        disconnectButton->setText(QApplication::translate("log_selectClass", "Disconnect", 0));
        label_7->setText(QApplication::translate("log_selectClass", "choose time\357\274\232", 0));
        sendLogButton->setText(QApplication::translate("log_selectClass", "filtrate and get the logs", 0));
        label->setText(QApplication::translate("log_selectClass", "Start time\357\274\232", 0));
        label_2->setText(QApplication::translate("log_selectClass", "End time\357\274\232", 0));
        newTimeBox->setText(QApplication::translate("log_selectClass", "Latest time", 0));
        checkAllBox->setText(QApplication::translate("log_selectClass", "Check all", 0));
        checkBox->setText(QApplication::translate("log_selectClass", "Check all", 0));
        label_8->setText(QApplication::translate("log_selectClass", "Fuzzy search:", 0));
        selectEdit->setText(QString());
        tempButton->setText(QApplication::translate("log_selectClass", "log template", 0));
        templateButton->setText(QApplication::translate("log_selectClass", " template manage ", 0));
        systemLogBox->setText(QApplication::translate("log_selectClass", "system_log", 0));
        positionBox->setText(QApplication::translate("log_selectClass", "localization", 0));
        repositionBox->setText(QApplication::translate("log_selectClass", "deviation", 0));
    } // retranslateUi

};

namespace Ui {
    class log_selectClass: public Ui_log_selectClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINMIND_H
