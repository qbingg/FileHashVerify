#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QStringList sl;
    // // 1. 先添加 argc（参数个数）
    // sl << "argc:" << QString::number(argc);
    // // 2. 遍历 argv（参数数组），逐个添加到字符串列表
    // sl << "argv:";
    // for (int i = 0; i < argc; ++i) {
    //     // argv[i] 是单个命令行参数，转 QString 后添加
    //     sl << QString("  argv[%1]: %2").arg(i).arg(argv[i]);
    // }
    // // 3. 将字符串列表拼接成一个完整的字符串（用换行分隔），方便显示
    // QString infoText = sl.join("\n");
    // // 4. 在消息框中显示所有参数
    // QMessageBox::information(nullptr, "命令行参数信息", infoText);

    MainWindow w;
    w.show();

    //只处理单个文件
    if(argc == 2){
        QFileInfo f(QString::fromLocal8Bit(argv[1]));
        if(f.exists()){
            w.setFileInfo(f);
        }
    }

    return a.exec();
}
