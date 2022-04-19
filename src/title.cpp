/*
 * @file 	title.cpp
 * @date 	2018/01/22 23:08
 *
 * @author 	itisyang
 * @Contact	itisyang@gmail.com
 *
 * @brief 	播放器标题栏
 * @note
 */

#include <QPainter>
#include <QFileInfo>
#include <QFontMetrics>
#include <QMessageBox>
#include <QFileDialog>
#include "title.h"
#include "ui_title.h"
#include "globalhelper.h"

#pragma execution_character_set("utf-8")

Title::Title(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Title),
    m_stMenu(this)
{
    ui->setupUi(this);
    //信号与槽连接
    connect(ui->CloseBtn, &QPushButton::clicked, this, &Title::SigCloseBtnClicked);//点击关闭按钮
    connect(ui->MinBtn, &QPushButton::clicked, this, &Title::SigMinBtnClicked);//点击最小化按钮
    connect(ui->MaxBtn, &QPushButton::clicked, this, &Title::SigMaxBtnClicked);//点击最大化按钮
    connect(ui->FullScreenBtn, &QPushButton::clicked, this, &Title::SigFullScreenBtnClicked);//点击全屏按钮
    connect(ui->MenuBtn, &QPushButton::clicked, this, &Title::SigShowMenu);//点击目录按钮

    //鼠标悬停提示
    ui->MenuBtn->setToolTip("显示主菜单");
    ui->MinBtn->setToolTip("最小化");
    ui->MaxBtn->setToolTip("最大化");
    ui->CloseBtn->setToolTip("关闭");
    ui->FullScreenBtn->setToolTip("全屏");
}

Title::~Title()
{
    delete ui;
}

bool Title::Init()
{
    if (InitUi() == false)
    {
        return false;
    }

    return true;
}
//初始化
bool Title::InitUi()
{
    ui->MovieNameLab->clear();

    //保证窗口不被绘制上的部分透明
    setAttribute(Qt::WA_TranslucentBackground);
    //载入样式表
    setStyleSheet(GlobalHelper::GetQssStr("://res/qss/title.css"));
    //设置un
    GlobalHelper::SetIcon(ui->MaxBtn, 9, QChar(0xf2d0));
    GlobalHelper::SetIcon(ui->MinBtn, 9, QChar(0xf2d1));
    GlobalHelper::SetIcon(ui->CloseBtn, 9, QChar(0xf00d));
    GlobalHelper::SetIcon(ui->FullScreenBtn, 9, QChar(0xf065));
    if (about.Init() == false)
    {
        return false;
    }
    return true;
}
//打开文件
void Title::OpenFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this, "打开文件", QDir::homePath(), 
        "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp *.mp3)");
    emit SigOpenFile(strFileName);
}

//鼠标双击事件
void Title::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit SigDoubleClicked();
    }
}

//改变视频名称的展示
void Title::ChangeMovieNameShow()
{
    QFontMetrics fontMetrics(ui->MovieNameLab->font());
    int fontSize = fontMetrics.width(m_strMovieName);
    int showwidth = ui->MovieNameLab->width();
    if (fontSize > showwidth)
    {
        QString str = fontMetrics.elidedText(m_strMovieName, Qt::ElideRight, ui->MovieNameLab->width());//返回一个带有省略号的字符串
        ui->MovieNameLab->setText(str);
    }
    else
    {
        ui->MovieNameLab->setText(m_strMovieName);
    }
}
//改变最大化按钮的样式
void Title::OnChangeMaxBtnStyle(bool bIfMax)
{
    if (bIfMax)
    {
        GlobalHelper::SetIcon(ui->MaxBtn, 9, QChar(0xf2d2));
        ui->MaxBtn->setToolTip("还原");
        //ChangeMovieNameShow();
    }
    else
    {
        GlobalHelper::SetIcon(ui->MaxBtn, 9, QChar(0xf2d0));
        ui->MaxBtn->setToolTip("最大化");
        //ChangeMovieNameShow();
    }
}
//视频播放中
void Title::OnPlay(QString strMovieName)
{
    qDebug() << "Title::OnPlay";
    QFileInfo fileInfo(strMovieName);
    m_strMovieName = fileInfo.fileName();
    ui->MovieNameLab->setText(m_strMovieName);
    ChangeMovieNameShow();
}
//视频播放结束
void Title::OnStopFinished()
{
    qDebug() << "Title::OnStopFinished";
    ui->MovieNameLab->clear();
}

