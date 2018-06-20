#ifndef GENERATEEMAPDIALOG_H
#define GENERATEEMAPDIALOG_H

#include "ui_generateemapdialog.h"
#include <QtWidgets/QDialog>
#include <QtCore/QScopedPointer>

class GenerateEmapDialogPrivate;

class GenerateEmapDialog : public QDialog
{
	Q_DECLARE_PRIVATE(GenerateEmapDialog);
	Q_DISABLE_COPY(GenerateEmapDialog);
public:
	GenerateEmapDialog(QWidget *parent = nullptr, Qt::WindowFlags f = nullptr);
	~GenerateEmapDialog();

	void initUi();
	void initConnect();
	int getRowCount() const;
	int getColumnCount() const;
public slots:
	void on_okBtn_clicked();
private:
	Ui::GenerateEmapDialog ui;
	QScopedPointer<GenerateEmapDialogPrivate> d_ptr;
};

class GenerateEmapDialogPrivate
{
	Q_DECLARE_PUBLIC(GenerateEmapDialog);
public:
	GenerateEmapDialogPrivate(GenerateEmapDialog *q_ptr_);
	~GenerateEmapDialogPrivate();

	void init();
private:
	GenerateEmapDialog *q_ptr{ nullptr };
	int minSpinBoxValue{ 0 };
	int maxSpinBoxValue{ 0 };
};

#endif // GENERATEEMAPDIALOG_H
