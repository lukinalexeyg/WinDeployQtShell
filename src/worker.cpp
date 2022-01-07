#include "worker.h"

#include <QDirIterator>

static const QLatin1String s_binDirectory("bin");
static const QStringList s_directoryFilters = {"4.*", "5.*", "6.*"};
static const QStringList s_configuration = {"--release", "--debug"};
static const QStringList s_runMode = {"", "--force", "--dry-run"};
static const QStringList s_outputMode = {"", "--json", "--list source", "--list target", "--list relative", "--list mapping"};



Worker::Worker(QObject *parent)
    : QObject{parent},
      m_process{new QProcess(this)}
{
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Worker::onProcessReadyReadStandardOutput);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Worker::finished);
    connect(m_process, &QProcess::errorOccurred, this, &Worker::onProcessErrorOccurred);
}



QStringList Worker::setQtDirectory(const QString &directory)
{
    QStringList names;
    m_kitsPathes.clear();

    if (directory.isEmpty())
        return names;

    QDirIterator dirIterator(directory, s_directoryFilters, QDir::Dirs | QDir::NoDotAndDotDot);

    while (dirIterator.hasNext()) {
        dirIterator.next();
        QDirIterator dirIterator2(dirIterator.filePath(), QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);

        while (dirIterator2.hasNext()) {
            dirIterator2.next();

            if (QFile::exists(windeployqtFilePath(dirIterator2.filePath()))) {
                const QString name = QString("%1 - %2").arg(dirIterator.fileName(), dirIterator2.fileName());
                names.append(name);
                m_kitsPathes.append(dirIterator2.filePath());
            }
        }
    }

    return names;
}



void Worker::run()
{
    m_process->setProgram(windeployqtFilePath(m_kitsPathes.at(data.kit)));
    m_process->setArguments(arguments());
    m_process->setWorkingDirectory(QString("%1/%2").arg(m_kitsPathes.at(data.kit), s_binDirectory));
    m_process->start();
}



QString Worker::windeployqtFilePath(const QString &path) const
{
    return QString("%1/%2/windeployqt.exe").arg(path, s_binDirectory);
}



QStringList Worker::arguments() const
{
    QStringList args;

    args.append(s_configuration.at(data.configuration));

    if (data.runMode > 0)
        args.append(s_runMode.at(data.runMode));

    if (data.outputMode > 0)
        args.append(s_outputMode.at(data.outputMode).split(QLatin1Char(' ')));

    if (data.pdb)
        args.append(QStringLiteral("-pdb"));

    if (data.qml) {
        args.append(QStringLiteral("--qmldir"));
        args.append(QString("%1/qml").arg(m_kitsPathes.at(data.kit)));
    }

    if (data.noPlugins)
        args.append(QStringLiteral("--no-plugins"));

    if (data.noLibraries)
        args.append(QStringLiteral("--no-libraries"));

    if (data.noTranslations)
        args.append(QStringLiteral("--no-translations"));

    if (data.noD3d)
        args.append(QStringLiteral("--no-system-d3d-compiler"));

    if (data.noAngle)
        args.append(QStringLiteral("--no-angle"));

    if (data.noOpengl)
        args.append(QStringLiteral("--no-opengl-sw"));

    args.append(data.app);

    return args;
}



void Worker::onProcessReadyReadStandardOutput()
{
    const QString text = m_process->read(65536);
    emit textOutput(text);
}



void Worker::onProcessErrorOccurred()
{
    emit errorOccurred(m_process->errorString());
}
