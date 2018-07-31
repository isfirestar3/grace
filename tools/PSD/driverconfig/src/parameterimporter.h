#ifndef PARAMETERIMPORTER_H
#define PARAMETERIMPORTER_H

#include <QFile>

class ParameterImporter
{
public:
	ParameterImporter(const QString &filename);
	~ParameterImporter();

	bool openImportFile();
	bool importParameter();
private:
	bool read(char *data, qint64 dataLen);

	QFile m_file;
};

#endif // PARAMETERIMPORTER_H
