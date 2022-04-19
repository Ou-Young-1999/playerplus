#include <QDebug>
#include <QTime>
#include <QSettings>
#include "ctrlbar.h"
#include "ui_ctrlbar.h"
#include "globalhelper.h"
//构造函数
CtrlBar::CtrlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBar)
{
    ui->setupUi(this);
    m_dLastVolumePercent = 1.0;//音量初始值为1
}
//析构函数
CtrlBar::~CtrlBar()
{
    delete ui;
}
//初始化
bool CtrlBar::Init()
{
    setStyleSheet(GlobalHelper::GetQssStr("://res/qss/ctrlbar.css"));//样式表

    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    GlobalHelper::SetIcon(ui->StopBtn, 12, QChar(0xf04d));
    GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
    GlobalHelper::SetIcon(ui->ForwardBtn, 12, QChar(0xf051));
    GlobalHelper::SetIcon(ui->BackwardBtn, 12, QChar(0xf048));

    ui->VolumeBtn->setToolTip("静音");
    ui->ForwardBtn->setToolTip("下一个");
    ui->BackwardBtn->setToolTip("上一个");
    ui->StopBtn->setToolTip("停止");
    ui->PlayOrPauseBtn->setToolTip("播放");
    
    ConnectSignalSlots();//判断信号与槽连接是否正常

    double dPercent = -1.0;
    GlobalHelper::GetPlayVolume(dPercent);
    if (dPercent != -1.0)
    {
        emit SigPlayVolume(dPercent);
        OnVideoVolume(dPercent);
    }
    return true;
}
//信号与槽连接是否成功
bool CtrlBar::ConnectSignalSlots()
{
    //信号与槽
    connect(ui->PlaySlider, &CustomSlider::SigCustomSliderValueChanged, this, &CtrlBar::OnPlaySliderValueChanged);//视频进度条变化
    connect(ui->VolumeSlider, &CustomSlider::SigCustomSliderValueChanged, this, &CtrlBar::OnVolumeSliderValueChanged);//音量进度条变化
    connect(ui->BackwardBtn, &QPushButton::clicked, this, &CtrlBar::SigBackwardPlay);//后退按钮
    connect(ui->ForwardBtn, &QPushButton::clicked, this, &CtrlBar::SigForwardPlay);//快进按钮
    return true;
}
//视频总时长
void CtrlBar::OnVideoTotalSeconds(int nSeconds)
{
    m_nTotalPlaySeconds = nSeconds;
    int thh, tmm, tss;
    thh = nSeconds / 3600;
    tmm = (nSeconds % 3600) / 60;
    tss = (nSeconds % 60);
    QTime TotalTime(thh, tmm, tss);
    ui->VideoTotalTimeTimeEdit->setTime(TotalTime);
}
//当前视频时长
void CtrlBar::OnVideoPlaySeconds(int nSeconds)
{
    int thh, tmm, tss;
    thh = nSeconds / 3600;
    tmm = (nSeconds % 3600) / 60;
    tss = (nSeconds % 60);
    QTime TotalTime(thh, tmm, tss);
    ui->VideoPlayTimeTimeEdit->setTime(TotalTime);
    ui->PlaySlider->setValue(nSeconds * 1.0 / m_nTotalPlaySeconds * MAX_SLIDER_VALUE);
}
//音量
void CtrlBar::OnVideoVolume(double dPercent)
{
    ui->VolumeSlider->setValue(dPercent * MAX_SLIDER_VALUE);
    m_dLastVolumePercent = dPercent;
    if (m_dLastVolumePercent == 0)//如果音量为0，改变图标
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
    }
    else
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
    }
    GlobalHelper::SavePlayVolume(dPercent);
}
//暂停与播放状态改变，改变图标
void CtrlBar::OnPauseState(bool bPaused)
{
    qDebug() << "CtrlBar::OnPauseStat" << bPaused;
    if (bPaused)
    {
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
        ui->PlayOrPauseBtn->setToolTip("播放");
    }
    else
    {
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04c));
        ui->PlayOrPauseBtn->setToolTip("暂停");
    }
}
//播放结束
void CtrlBar::OnStopFinished()
{
    ui->PlaySlider->setValue(0);
    QTime StopTime(0, 0, 0);
    ui->VideoTotalTimeTimeEdit->setTime(StopTime);
    ui->VideoPlayTimeTimeEdit->setTime(StopTime);
    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    ui->PlayOrPauseBtn->setToolTip("播放");
}
//视频进度条变化
void CtrlBar::OnPlaySliderValueChanged()
{
    double dPercent = ui->PlaySlider->value()*1.0 / ui->PlaySlider->maximum();
    emit SigPlaySeek(dPercent);
}
//音量进度条变化
void CtrlBar::OnVolumeSliderValueChanged()
{
    double dPercent = ui->VolumeSlider->value()*1.0 / ui->VolumeSlider->maximum();
    emit SigPlayVolume(dPercent);
    OnVideoVolume(dPercent);
}
//播放与暂停
void CtrlBar::on_PlayOrPauseBtn_clicked()
{
    emit SigPlayOrPause();
}
//音量按钮点击
void CtrlBar::on_VolumeBtn_clicked()
{
    if (ui->VolumeBtn->text() == QChar(0xf028))
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
        ui->VolumeSlider->setValue(0);
        emit SigPlayVolume(0);
    }
    else
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
        ui->VolumeSlider->setValue(m_dLastVolumePercent * MAX_SLIDER_VALUE);
        emit SigPlayVolume(m_dLastVolumePercent);
    }
}
//停止
void CtrlBar::on_StopBtn_clicked()
{
    emit SigStop();
}
