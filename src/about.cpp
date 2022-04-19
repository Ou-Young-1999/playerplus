#include "about.h"
#include "ui_about.h"
#include "globalhelper.h"
//构造函数
About::About(QWidget *parent)
    : QWidget(parent)
{
    ui = new Ui::About();
    ui->setupUi(this);
}
//析构函数
About::~About()
{
    delete ui;
}
//初始化
bool About::Init()
{
    this->setWindowModality(Qt::ApplicationModal);//弹出关于窗口，主窗口不可点击
    this->setWindowIcon(QIcon("://res/player.png"));//设置Icon图标
    ui->LogoLabel->setPixmap(QPixmap("://res/player.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //设置版本号
    QString strVersion = QString("版本：%1\n时间：%2").arg(GlobalHelper::GetAppVersion()).arg(QString(__DATE__) + " " + QString(__TIME__));
    ui->VersionLabel->setText(strVersion);
    return true;
}
//槽函数，点击确定按钮，关闭窗口
void About::on_ClosePushButton_clicked()
{
    this->close();
}
