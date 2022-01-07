#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

namespace SettingsString {
    static const QLatin1String windowGeomerty("WindowGeomerty");
    static const QLatin1String windowState("WindowState");
    static const QLatin1String qtDirectory("QtDirectory");
}

namespace ColorString {
    static const QLatin1String green("green");
    static const QLatin1String red("red");
}

static const QLatin1String s_home("/home");



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow},
    m_worker{new Worker(this)}
{
    ui->setupUi(this);

    readSetting();

    setWindowTitle(qApp->applicationName());
    ui->pushButton_run->setEnabled(false);

    connect(ui->toolButton_app,     &QToolButton::released, this, &MainWindow::onToolButtonAppReleased);
    connect(ui->toolButton_qtDir,   &QToolButton::released, this, &MainWindow::onToolButtonQtDirReleased);
    connect(ui->comboBox_kit,       QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::checkIsRunEnabled);
    connect(ui->pushButton_run,     &QPushButton::released, this, &MainWindow::run);
    connect(ui->pushButton_about,   &QPushButton::released, this, &MainWindow::showAboutMessageBox);

    connect(m_worker, &Worker::textOutput, this, &MainWindow::onWorkerTextOutput);
    connect(m_worker, &Worker::finished, this, &MainWindow::onWorkerFinished);
    connect(m_worker, &Worker::errorOccurred, this, &MainWindow::onWorkerErrorOccurred);
}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::readSetting()
{
    QSettings settings;

    restoreGeometry(settings.value(SettingsString::windowGeomerty).toByteArray());
    restoreState(settings.value(SettingsString::windowState).toByteArray());
    setQtDirectory(settings.value(SettingsString::qtDirectory).toString());
}



void MainWindow::onToolButtonAppReleased()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Select application executable file"),
                                                          s_home,
                                                          tr("Executable file (*.exe)"));

    if (!fileName.isEmpty() && fileName != ui->lineEdit_app->text()) {
        ui->lineEdit_app->setText(fileName);
        checkIsRunEnabled();
    }
}



void MainWindow::onToolButtonQtDirReleased()
{
    const QString directory = QFileDialog::getExistingDirectory(this,
                                                                tr("Select Qt directory path"),
                                                                s_home,
                                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!directory.isEmpty() && directory != ui->lineEdit_qtDir->text())
        setQtDirectory(directory);
}



void MainWindow::setQtDirectory(const QString &directory)
{
    const QStringList names = m_worker->setQtDirectory(directory);

    ui->lineEdit_qtDir->setText(directory);
    ui->comboBox_kit->clear();
    ui->comboBox_kit->addItems(names);

    checkIsRunEnabled();
}



void MainWindow::run()
{
    setWidgetsEnabled(false);

    m_worker->data.app = ui->lineEdit_app->text();
    m_worker->data.kit = ui->comboBox_kit->currentIndex();
    m_worker->data.configuration = ui->comboBox_configuration->currentIndex();
    m_worker->data.runMode = ui->comboBox_runMode->currentIndex();
    m_worker->data.outputMode = ui->comboBox_outputMode->currentIndex();
    m_worker->data.pdb = ui->checkBox_pdb->isChecked();
    m_worker->data.qml = ui->checkBox_qml->isChecked();
    m_worker->data.noPlugins = ui->checkBox_noPlugins->isChecked();
    m_worker->data.noLibraries = ui->checkBox_noLibraries->isChecked();
    m_worker->data.noTranslations = ui->checkBox_noTranslations->isChecked();
    m_worker->data.noD3d = ui->checkBox_noD3d->isChecked();
    m_worker->data.noAngle = ui->checkBox_noAngle->isChecked();
    m_worker->data.noOpengl = ui->checkBox_noOpengl->isChecked();

    m_worker->run();
}



void MainWindow::showAboutMessageBox()
{
    QMessageBox::about(this,
                       tr("About"),
                       QString("<h3>%1</h3><br>"
                               "%2<br><br>"
                               "%3: %4<br><br>"
                               "%5<br><br>"
                               "<a href=\"%6\">%7</a>")
                       .arg(qApp->applicationName(),
                            tr(APP_DESCRIPTION),
                            tr("Version"),
                            qApp->applicationVersion(),
                            APP_COPYRIGHT,
                            qApp->organizationDomain(),
                            tr("Github repository")));
}



void MainWindow::onWorkerTextOutput(const QString &text)
{
    ui->textEdit->append(text);
}



void MainWindow::onWorkerFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
        appendText(QStringLiteral("The process exited normally."), ColorString::green);
    else
        appendText(QString("The process crashed. Exit code: %1.").arg(exitCode), ColorString::red);

    setWidgetsEnabled(true);
}



void MainWindow::onWorkerErrorOccurred(const QString &error)
{
    appendText(error, ColorString::red);
    setWidgetsEnabled(true);
}



void MainWindow::checkIsRunEnabled()
{
    const bool enabled = !ui->lineEdit_app->text().isEmpty()
            && !ui->lineEdit_qtDir->text().isEmpty()
            && ui->comboBox_kit->currentIndex() >= 0;

    ui->pushButton_run->setEnabled(enabled);
}



void MainWindow::setWidgetsEnabled(bool enabled)
{
    ui->toolButton_app->setEnabled(enabled);
    ui->toolButton_qtDir->setEnabled(enabled);

    ui->comboBox_kit->setEnabled(enabled);
    ui->comboBox_configuration->setEnabled(enabled);
    ui->comboBox_runMode->setEnabled(enabled);
    ui->comboBox_outputMode->setEnabled(enabled);

    ui->checkBox_pdb->setEnabled(enabled);
    ui->checkBox_qml->setEnabled(enabled);
    ui->checkBox_noPlugins->setEnabled(enabled);
    ui->checkBox_noLibraries->setEnabled(enabled);
    ui->checkBox_noTranslations->setEnabled(enabled);
    ui->checkBox_noD3d->setEnabled(enabled);
    ui->checkBox_noAngle->setEnabled(enabled);
    ui->checkBox_noOpengl->setEnabled(enabled);

    ui->pushButton_run->setEnabled(enabled);
}



void MainWindow::appendText(const QString &text, const QString &color)
{
    ui->textEdit->append(QString("<font color=\"%1\">%2</font><br>").arg(color, text));
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    settings.setValue(SettingsString::windowGeomerty, saveGeometry());
    settings.setValue(SettingsString::windowState,    saveState());
    settings.setValue(SettingsString::qtDirectory,    ui->lineEdit_qtDir->text());

    event->accept();
}
