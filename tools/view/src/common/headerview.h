#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QtWidgets/QHeaderView>

class StandardItemModel;

class QCheckBox;

class HeaderView : public QHeaderView
{
	Q_OBJECT

public:
	HeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

	void SetStandardItemModel(StandardItemModel *pStandardItemModel);
	void SetHeaderTexts(const QStringList &texts);
	void SetCheckColumns(const QList<int> &checkList);
	void SetCheckBoxState(int logicalIndex, Qt::CheckState checkState);
	void OnCheckBoxStateChanged(int state);
	void OnCheckBoxClicked();
protected:
	void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
	void mousePressEvent(QMouseEvent *mouseEvent);
signals:
	void AfterDrawCheckBox() const;
	void CheckBoxStateChanged(int, Qt::CheckState);
private:
	const QRect checkBoxRect(const QRect &rect, int logicalIndex) const;
	const QRect textRect(const QRect &rect, int logicalIndex) const;
	void drawCheckBox(QPainter *painter, const QRect &rect, int logicalIndex) const;
	void drawText(QPainter *painter, const QRect &rect, int logicalIndex) const;
	void drawLine(QPainter *painter, const QRect &rect, int logicalIndex) const;

	mutable QMap<int, QCheckBox *> m_checkBoxMap;
	mutable QMap<QString, QRect> m_textRectMap;
	mutable QMap<int, QRect> m_lineRect;
	QStringList m_texts;
	QList<int> m_checkList;
	StandardItemModel *m_pStandardItemModel{nullptr};
};

#endif // HEADERVIEW_H
