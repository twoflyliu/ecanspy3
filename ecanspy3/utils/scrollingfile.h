#ifndef SCROLLINGFILE_H
#define SCROLLINGFILE_H

#include <QFile>

class ScrollingFile
{
public:
    enum {
        DEFAULT_SINGILE_FILE_COUNT = 40 * 1024 * 1024      //!< 单位字节
    };

    enum FileNameRule {
        FILE_NAME_RULE_ID_INC = 0,  //!< 以ID递增方式
    };

    ScrollingFile(QString filePath, int maxFileSize = DEFAULT_SINGILE_FILE_COUNT,
                  FileNameRule rule = FILE_NAME_RULE_ID_INC);
    ~ScrollingFile();

    qint64 write(const char *data, qint64 maxSize);
    qint64 write(const char *data);
    qint64 write(const QByteArray &byteArray);

    void close();
    void flush();

    void setMaxFileSize(int maxFileSize)
    {
        this->maxFileSize = maxFileSize;
    }

    int getMaxFileSize() const
    {
        return this->maxFileSize;
    }

    void setFileNameRule(FileNameRule rule)
    {
        this->rule = rule;
    }

    FileNameRule getFileNameRule() const
    {
        return this->rule;
    }

protected:
    void generateNextFile();
    QString generateNextFileName();

private:
    QFile *file = nullptr;

    QString fileName;
    QString extName;
    QString dirName;
    FileNameRule rule;

    int maxFileSize;


    int id = 0;
};

#endif // SCROLLINGFILE_H
