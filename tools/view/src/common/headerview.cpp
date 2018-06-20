#include "headerview.h"
#include "common/standarditemmodel.h"
#include <QtWidgets/QCheckBox>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <vector>
#include <algorithm>

namespace
{
	static const int LEFT_MARGIN = 6;
	static const int TEXT_MARGIN = 3;
	static const int TEXT_BORDER = 1;
	static const char *LOGICAL_INDEX_PROPERTY = "logicalIndex";
}

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
	: QHeaderView(orientation, parent)
{

}

void HeaderView::SetStandardItemModel(StandardItemModel *pStandardItemModel)
{
	m_pStandardItemModel = pStandardItemModel;

	connect(m_pStandardItemModel, &StandardItemModel::UpdateHeaderViewCheckState, this, &HeaderView::SetCheckBoxState);
}

void HeaderView::SetHeaderTexts(const QStringList &texts)
{
	{
		QStringList tmpStrList;
		m_texts.swap(tmpStrList);
	}

	m_texts = texts;
}

void HeaderView::SetCheckColumns(const QList<int> &checkList)
{
	{
		QList<int> tmpCheckList;
		m_checkList.swap(tmpCheckList);
	}

	m_checkList = checkList;
}

void HeaderView::SetCheckBoxState(int logicalIndex, Qt::CheckState checkState)
{
	int size = m_checkBoxMap.size();
	Q_ASSERT(logicalIndex < size);

	if (logicalIndex >= size)
	{
		return;
	}

	QCheckBox *pCheckBox = m_checkBoxMap[logicalIndex];

	if (nullptr == pCheckBox)
	{
		return;
	}

	pCheckBox->setCheckState(checkState);
}

void HeaderView::OnCheckBoxStateChanged(int state)
{
	QCheckBox *pCheckBox = qobject_cast<QCheckBox *>(sender());

	if (nullptr == pCheckBox)
	{
		return;
	}

	bool ok = false;
	int logicalIndex = pCheckBox->property(LOGICAL_INDEX_PROPERTY).toInt(&ok);

	if (!ok)
	{
		return;
	}

	Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
	emit CheckBoxStateChanged(logicalIndex, checkState);
}

void HeaderView::OnCheckBoxClicked()
{
	if (nullptr == m_pStandardItemModel)
	{
		return;
	}

	QCheckBox *pCheckBox = qobject_cast<QCheckBox *>(sender());

	if (nullptr == pCheckBox)
	{
		return;
	}

	bool ok = false;
	int logicalIndex = pCheckBox->property(LOGICAL_INDEX_PROPERTY).toInt(&ok);

	if (!ok)
	{
		return;
	}

	Qt::CheckState checkState = pCheckBox->checkState();

	if (Qt::PartiallyChecked == checkState)
	{
		checkState = Qt::Checked;
	}

	pCheckBox->setCheckState(checkState);
	disconnect(m_pStandardItemModel, &StandardItemModel::UpdateHeaderViewCheckState, this, &HeaderView::SetCheckBoxState);
	m_pStandardItemModel->ChangeTopCheckState(logicalIndex, checkState);
	connect(m_pStandardItemModel, &StandardItemModel::UpdateHeaderViewCheckState, this, &HeaderView::SetCheckBoxState);
}

void HeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	if (nullptr == painter)
	{
		return;
	}

	painter->save();
	QHeaderView::paintSection(painter, rect, logicalIndex);
	painter->restore();

	QStyleOptionHeader optHeader;
	optHeader.initFrom(this);
	optHeader.state &= ~QStyle::State_MouseOver;
	QApplication::style()->drawControl(QStyle::CE_HeaderSection, &optHeader, painter, this);

	drawCheckBox(painter, rect, logicalIndex);
	drawText(painter, rect, logicalIndex);
	drawLine(painter, rect, logicalIndex);
}

void HeaderView::mousePressEvent(QMouseEvent *mouseEvent)
{
	if (nullptr == mouseEvent)
	{
		return;
	}

	QHeaderView::mousePressEvent(mouseEvent);

	if (Qt::LeftButton == mouseEvent->button())
	{
		QPoint pos = mouseEvent->pos();
		int logicalIndex = logicalIndexAt(pos);
		QRect checkBoxRect;

		if (m_checkList.contains(logicalIndex))
		{
			Q_ASSERT(m_checkBoxMap.contains(logicalIndex));

			if (!m_checkBoxMap.contains(logicalIndex))
			{
				return;
			}

			QCheckBox *pCheckBox = m_checkBoxMap[logicalIndex];

			if (nullptr == pCheckBox)
			{
				return;
			}

			checkBoxRect = pCheckBox->geometry();
		}

		int texts_size = m_texts.size();
		Q_ASSERT(logicalIndex < texts_size);

		if (logicalIndex >= texts_size)
		{
			return;
		}

		QString text = m_texts[logicalIndex];
		QRect textRect = fontMetrics().boundingRect(text).adjusted(-TEXT_BORDER, 0, +TEXT_BORDER, 0);
		int minSectionSize = LEFT_MARGIN + checkBoxRect.width() + TEXT_MARGIN + textRect.width() + LEFT_MARGIN;
		setMinimumSectionSize(minSectionSize);
	}
}

const QRect HeaderView::checkBoxRect(const QRect &rect, int logicalIndex) const
{
	if (!m_checkList.contains(logicalIndex))
	{
		return QRect();
	}

	QStyleOptionButton optBtn;
	optBtn.initFrom(this);
	int indicatorWidth = QApplication::style()->pixelMetric(QStyle::PM_IndicatorWidth, &optBtn);
	int indicatorHeight = QApplication::style()->pixelMetric(QStyle::PM_IndicatorHeight, &optBtn);
	int rectWidth = rect.width();
	int rectHeight = rect.height();
	std::vector<int> sizeVec{ indicatorWidth, indicatorHeight, rectWidth, rectHeight };
	int w = *std::min_element(sizeVec.begin(), sizeVec.end());
	return QRect(rect.topLeft().x() + LEFT_MARGIN, rect.topLeft().y() + (rect.height() - w) / 2, w, w);
}

const QRect HeaderView::textRect(const QRect &rect, int logicalIndex) const
{
	bool hasCheckBox = false;

	if (m_checkList.contains(logicalIndex))
	{
		hasCheckBox = true;
	}

	QRect r;

	if (hasCheckBox)
	{
		r = checkBoxRect(rect, logicalIndex);
	}
	else
	{
		r = rect;
	}

	int x = 0;

	if (hasCheckBox)
	{
		x = r.topLeft().x() + r.width() + TEXT_MARGIN;
	}
	else
	{
		x = r.topLeft().x() + TEXT_MARGIN;
	}

	int y = rect.topLeft().y();
	int w = rect.topRight().x() - LEFT_MARGIN - x;
	int h = rect.height();

	return QRect(x, y, w, h);
}

void HeaderView::drawCheckBox(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	if (nullptr == painter)
	{
		return;
	}

	QRect cbRect = checkBoxRect(rect, logicalIndex);

	if (!cbRect.isValid())
	{
		return;
	}

	QCheckBox *pCheckBox = nullptr;

	if (!m_checkBoxMap.contains(logicalIndex))
	{
		pCheckBox = new(std::nothrow) QCheckBox((QWidget *)this);
		m_checkBoxMap.insert(logicalIndex, pCheckBox);

		connect(pCheckBox, &QCheckBox::stateChanged, this, &HeaderView::OnCheckBoxStateChanged);
		connect(pCheckBox, &QCheckBox::clicked, this, &HeaderView::OnCheckBoxClicked);
		emit AfterDrawCheckBox();
	}
	else
	{
		pCheckBox = m_checkBoxMap[logicalIndex];
	}

	if (nullptr == pCheckBox)
	{
		return;
	}

	pCheckBox->setProperty(LOGICAL_INDEX_PROPERTY, logicalIndex);
	pCheckBox->setGeometry(cbRect);
	pCheckBox->setVisible(true);
}

void HeaderView::drawText(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	if (nullptr == painter)
	{
		return;
	}

	int size = m_texts.size();

	if (logicalIndex >= size)
	{
		return;
	}

	QString text = m_texts[logicalIndex];
	QRect tRect = textRect(rect, logicalIndex);
	m_textRectMap[text] = tRect;

	QMapIterator<QString, QRect> mapItor(m_textRectMap);
	QString _text;
	QRect _rect;

	while (mapItor.hasNext())
	{
		mapItor.next();
		_text = mapItor.key();
		_rect = mapItor.value();
		painter->drawText(_rect, Qt::AlignLeft | Qt::AlignVCenter, _text);
	}
}

void HeaderView::drawLine(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	if (nullptr == painter)
	{
		return;
	}

	m_lineRect[logicalIndex] = rect;

	QMapIterator<int, QRect> mapItor(m_lineRect);
	QRect r;

	while (mapItor.hasNext())
	{
		mapItor.next();
		r = mapItor.value();
		painter->drawLine(r.topRight().x(), r.topRight().y(), r.bottomRight().x(), r.bottomRight().y());
	}
}
