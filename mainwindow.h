#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getFileHash(const QFileInfo &fileInfo,const QString &hashType);

    void readSettings();
    void writeSettings();
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent* event) override;
private slots:
    void on_hashTypeComboBox_currentIndexChanged(int index);

    void on_btnVerify_clicked();

private:
    Ui::MainWindow *ui;

    QFileInfo m_fileInfo;

    QSettings *m_settings = nullptr;
};
#endif // MAINWINDOW_H
