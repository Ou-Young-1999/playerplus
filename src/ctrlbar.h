#ifndef CTRLBAR_H
#define CTRLBAR_H
#include <QWidget>
#include "CustomSlider.h"
namespace Ui {
class CtrlBar;
}
class CtrlBar : public QWidget
{
    Q_OBJECT
public:
    explicit CtrlBar(QWidget *parent = 0);//构造函数
    ~CtrlBar();//析构函数
    bool Init();//初始化
public:
    void OnVideoTotalSeconds(int nSeconds);//获取音视频总时长
    void OnVideoPlaySeconds(int nSeconds);//获取音视频当前时长
    void OnVideoVolume(double dPercent);//音量大小（百分比）
    void OnPauseState(bool bPaused);//播放状态
    void OnStopFinished();//播放结束
private:
    void OnPlaySliderValueChanged();//播放进度条
    void OnVolumeSliderValueChanged();//音量进度条
private slots:
    void on_PlayOrPauseBtn_clicked();//播放暂停键按钮
    void on_VolumeBtn_clicked();//音量键按钮
    void on_StopBtn_clicked();//停止键按钮
    bool ConnectSignalSlots();//判断连接是否成功
signals:
    void SigPlaySeek(double dPercent);//调整播放进度，百分比
    void SigPlayVolume(double dPercent);//调整音量大小，百分比
    void SigPlayOrPause();//播放状态
    void SigStop();//停止
    void SigForwardPlay();//前进
    void SigBackwardPlay();//后退
    void SigShowMenu();//展示菜单
private:
    Ui::CtrlBar *ui;
    int m_nTotalPlaySeconds;//总时长
    double m_dLastVolumePercent;//音量
};

#endif // CTRLBAR_H
