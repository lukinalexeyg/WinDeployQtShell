#ifndef WORKER_H
#define WORKER_H

#include <QProcess>

class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);

    QStringList setQtDirectory(const QString& directory);
    void run();

public:
    struct Data {
        QString app;
        int kit = 0;
        int configuration = 0;
        int runMode = 0;
        int outputMode = 0;
        bool pdb = false;
        bool qml = false;
        bool noPlugins = false;
        bool noLibraries = false;
        bool noTranslations = false;
        bool noD3d = false;
        bool noAngle = false;
        bool noOpengl = false;
    } data;

signals:
    void textOutput(const QString &text);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void errorOccurred(const QString &error);

private:
    QProcess *m_process;
    QStringList m_kitsPathes;

private:
    QString windeployqtFilePath(const QString &path) const;
    QStringList arguments() const;
    void onProcessReadyReadStandardOutput();
    void onProcessErrorOccurred();
};

#endif // WORKER_H
