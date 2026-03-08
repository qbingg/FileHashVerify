#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);// 开启对整个窗口的拖放操作的支持

    ui->fileHashLineEdit->setReadOnly(true);
    ui->hashTypeComboBox->addItem("MD5");
    ui->hashTypeComboBox->addItem("SHA256");


    //初始化QSettings
    QString appDirPath = QCoreApplication::applicationDirPath();// 获取exe所在目录
    QString configFilePath = appDirPath + "/config.ini";
    // 检查配置文件是否存在
    // QFile::exists(config_path);
    // 初始化QSettings（即使文件不存在，QSettings也会在写入时自动创建）
    m_settings = new QSettings(configFilePath, QSettings::IniFormat);
    // 设置组织名和应用名（影响INI文件的[General]部分）
    m_settings->setFallbacksEnabled(false); // 禁用回退到系统注册表

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getFileHash(const QFileInfo &fileInfo, const QString &hashType)
{
    /*单例异步*/
    QString program = "";
    QStringList arguments;
    QString workingDir = "";

    program = "powershell";
    // 关键改造：PowerShell 命令只输出纯哈希值
    arguments << "-Command"  // 指定执行命令模式
              << QString("(Get-FileHash -Path '%1' -Algorithm %2).Hash").arg(
                     fileInfo.absoluteFilePath().replace("'", "''"),  // 转义单引号避免命令错误
                     hashType
                     );


    QProcess *process = new QProcess(this);
    process->setProgram(program);
    process->setArguments(arguments);
    if (!workingDir.isEmpty())
        process->setWorkingDirectory(workingDir);
    // 连接信号槽
    // connect(process, &QProcess::readyReadStandardOutput, [=]() {
    //     qDebug() << "stdOutput:" << process->readAllStandardOutput();
    // });
    // connect(process, &QProcess::readyReadStandardError, [=]() {
    //     qDebug() << "stdError:" << process->readAllStandardError();
    // });
    connect(process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    });
    connect(process, &QProcess::finished, [=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            QString output = process->readAllStandardOutput();
            qDebug() << "Finished:" << output;
            ui->fileHashLineEdit->setText(output.trimmed());//去除尾行空格
            ui->statusbar->showMessage("完成"+hashType+"："+fileInfo.fileName(), 3000);
        } else {
            QString error = process->readAllStandardError();
            qDebug() << "Exited,Error occurred:" << error;
            ui->statusbar->showMessage("错误"+hashType+"："+fileInfo.fileName(), 3000);
            QMessageBox::information(this, "错误", error);
        }
        process->deleteLater();
    });
    process->start();
    ui->statusbar->showMessage("正在获取hash码");

}

void MainWindow::readSettings()
{
    const auto geometry = m_settings->value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    const auto hashTypeIndex = m_settings->value("hashTypeIndex", 0);
    if (!hashTypeIndex.isNull())
        ui->hashTypeComboBox->setCurrentIndex(hashTypeIndex.toInt());
}

void MainWindow::writeSettings()
{
    // 将“窗口位置和大小”写入ini
    m_settings->setValue("geometry", saveGeometry());

    //保存hashType的索引index
    m_settings->setValue("hashTypeIndex", ui->hashTypeComboBox->currentIndex());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction(); // 接受默认的拖放行为
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    // 确保文件数量仅为一个
    if (urls.size() != 1) {
        if (urls.size() > 1) {
            // 报错
            QMessageBox::warning(this, "warning", "请拖入单个文件");
        }
        return;
    }

    const QUrl& url = urls.first();
    QString filePath = url.toLocalFile();

    // 检查文件是否存在
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "warning", "请拖入有效的文件");
        return;
    }

    //获取文件信息，并显示到标题栏上
    m_fileInfo = fileInfo;
    setWindowTitle(m_fileInfo.absoluteFilePath());

    getFileHash(m_fileInfo,ui->hashTypeComboBox->currentText());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭窗口时，将“窗口位置和大小”写入ini
    writeSettings();
}

void MainWindow::on_hashTypeComboBox_currentIndexChanged(int index)
{
    // 检查文件是否存在
    QFileInfo fileInfo(m_fileInfo);
    if (fileInfo.exists()) {
        getFileHash(m_fileInfo,ui->hashTypeComboBox->currentText());
    }
}


void MainWindow::on_btnVerify_clicked()
{
    QString fileHash = ui->fileHashLineEdit->text().trimmed();
    QString inputHash = ui->inputHashLineEdit->text().trimmed();

    if((fileHash.isEmpty())||(inputHash.isEmpty()))
        return;

    if(fileHash == inputHash){
        QMessageBox::information(this, "", "相等");
    }
    else{
        QMessageBox::critical(this, "", "不相等");
    }
}

