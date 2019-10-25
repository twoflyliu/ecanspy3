#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QDateTime>

#include "utils/signalaccesstest.h"

#define LOG_TO_LOCAL_ENABLED    (0)

#if LOG_TO_LOCAL_ENABLED
void customMessageHandler(  QtMsgType type,  const char *msg)
{
    QString txt;
    switch (type)
    {
        case QtDebugMsg:
                txt = QString("Debug: %1").arg(msg);
                break;
            case QtWarningMsg:
                txt = QString("Warning: %1").arg(msg);
                break;
            case QtCriticalMsg:
                txt = QString("Critical: %1").arg(msg);
                break;
            case QtFatalMsg:
                txt = QString("Fatal: %1").arg(msg);
                abort();
    }
    QFile outFile("debuglog.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append|QIODevice::Text);
    QTextStream ts(&outFile);
    ts << txt << endl;

    //输出到控制台
    //QDateTime now = QDateTime::currentDateTime();
    //cout << now.toString("hh:mm:ss.zzz").toStdString() << " "<< txt.toStdString() <<endl;
}
#endif


int main(int argc, char *argv[])
{
#if LOG_TO_LOCAL_ENABLED
    //在程序开始前添加
    qInstallMsgHandler(customMessageHandler);
#endif

#if SIGNAL_ACCESS_TEST_ENABLED
    return signalAccessorTest();
#endif

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    QApplication a(argc, argv);
    MainWindow w;

    return a.exec();
}
