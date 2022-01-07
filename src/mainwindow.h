#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "worker.h"

#include <QCloseEvent>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    Worker *m_worker;

private:
    void readSetting();

    void onToolButtonAppReleased();
    void onToolButtonQtDirReleased();
    void setQtDirectory(const QString &directory);
    void run();
    void showAboutMessageBox();

    void onWorkerTextOutput(const QString &text);
    void onWorkerFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onWorkerErrorOccurred(const QString &error);

    void checkIsRunEnabled();
    void setWidgetsEnabled(bool enabled);
    void appendText(const QString &text, const QString &color);
};

#endif // MAINWINDOW_H
