#ifndef COMM_IMPL_H
#define COMM_IMPL_H
#include <QtCore/QStringList>

static bool getStringBySeparator(QString &retStr, const QString &strIn, int index, const QString &separator = " ")
{
	const QStringList &strInList = strIn.split(separator);
	int strInListSize = strInList.size();

	if (!(index >= 0 && index <= strInListSize - 1))
	{
		return false;
	}

	retStr = strInList[index];

	return true;
}

#endif // !COMM_IMPL
