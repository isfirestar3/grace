#include "item_delegate.h"


item_delegate::item_delegate(QObject * parent)
	:QItemDelegate(parent)
{

}
//QSpinBox
QWidget *item_delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSpinBox *editor = new QSpinBox(parent);//加上（parent）就会嵌入表格里面，否则回弹出
	editor->setMinimum(0);
	editor->setMaximum(1000);
	return  editor;
}

void item_delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	int  value = index.model()->data(index, Qt::EditRole).toInt();
	QSpinBox *spinBox = static_cast <QSpinBox*>(editor);
	spinBox->setValue(value);
}
void item_delegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QSpinBox *spinBox = static_cast <QSpinBox*>(editor);
	spinBox->interpretText();
	int  value = spinBox->value();
	model->setData(index, value, Qt::EditRole);
}
void item_delegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}
//QLineEdit
//QWidget *item_delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//	QLineEdit *editor = new QLineEdit(parent);
//	QRegExp regExp("^(?:[0-9]{1,3}|1000)$");//0-1000
//	editor->setValidator(new QRegExpValidator(regExp, parent));
//	return editor;
//}
//
//void item_delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{
//	QString text = index.model()->data(index, Qt::EditRole).toString();
//	QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
//	lineEdit->setText(text);
//}
//void item_delegate::setModelData(QWidget *editor, QAbstractItemModel *model,
//	const QModelIndex &index) const
//{
//	QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
//	QString text = lineEdit->text();
//	model->setData(index, text, Qt::EditRole);
//}
//void item_delegate::updateEditorGeometry(QWidget *editor,
//	const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//	editor->setGeometry(option.rect);
//}

