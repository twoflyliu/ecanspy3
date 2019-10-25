#include "scrollingfile.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>

ScrollingFile::ScrollingFile(QString filePath, int maxFileSize, ScrollingFile::FileNameRule rule)
{
    QFileInfo info(filePath);
    extName = info.suffix();
    fileName = info.fileName();
    dirName = info.absolutePath();

    if (!extName.isEmpty()) {
        fileName = fileName.left(fileName.size() - extName.size() - 1);
    }

    this->maxFileSize = maxFileSize;
    this->rule = rule;

    generateNextFile();
}

ScrollingFile::~ScrollingFile()
{

}

qint64 ScrollingFile::write(const char *data, qint64 maxSize)
{
    if (file) {
        qint64 ret = file->write(data, maxSize);
        if (file->size() >= maxFileSize) {
            generateNextFile();
        }
        return ret;
    }
    return 0;
}

qint64 ScrollingFile::write(const char *data)
{
    if (file) {
        qint64 ret = file->write(data);
        if (file->size() >= maxFileSize) {
            generateNextFile();
        }
        return ret;
    }
    return 0;
}

qint64 ScrollingFile::write(const QByteArray &byteArray)
{
    if (file) {
        qint64 ret = file->write(byteArray);
        if (file->size() >= maxFileSize) {
            generateNextFile();
        }
        return ret;
    }
    return 0;
}

void ScrollingFile::close()
{
    if (file) {
        file->close();
    }
}

void ScrollingFile::flush()
{
    if (file) {
        file->flush();
    }
}

QString ScrollingFile::generateNextFileName()
{
    switch (rule) {
    case FILE_NAME_RULE_ID_INC:
        return QString("%1/%2_%3.%4").arg(dirName).arg(fileName).arg(id++).arg(extName);
    default:
        Q_ASSERT(false && "invalid file name");
    }
    return QString();
}

void ScrollingFile::generateNextFile()
{
    if (file) {
        file->flush();
        delete file;
    }

    file = new QFile(generateNextFileName());
    if (!file->open(QFile::WriteOnly|QFile::Truncate)) {
        qDebug() << QString("ScrollingFile::generateNextFile()-> Create File [%1] Failed: %2")
                    .arg(file->fileName()).arg(file->errorString());
        Q_ASSERT(false && "Create Scrolling File Failed!");
    }
}
