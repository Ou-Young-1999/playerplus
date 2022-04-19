#ifndef MainWid_H
#define MainWid_H
#include <QWidget>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMenu>
#include <QAction>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMainWindow>
#include "playlist.h"
#include "title.h"
namespace Ui {
class MainWid;
}
class MainWid : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWid(QMainWindow *parent = 0);//构造函数
    ~MainWid();//析构函数
    bool Init();//初始化
protected:
    void keyReleaseEvent(QKeyEvent *event);//键盘释放事件
    void mousePressEvent(QMouseEvent *event);//鼠标按压事件
    void mouseReleaseEvent(QMouseEvent *event);//鼠标释放事件
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动事件
    void contextMenuEvent(QContextMenuEvent* event);//右键菜单事件
private:
    bool ConnectSignalSlots();//判断是否连接信号槽
    //关闭、最小化、最大化按钮响应
    void OnCloseBtnClicked();
    void OnMinBtnClicked();
    void OnMaxBtnClicked();
    //显示、隐藏播放列表
    //void OnShowOrHidePlaylist();
    /**
    * @brief	全屏播放
    */
    void OnFullScreenPlay();
    void OnCtrlBarAnimationTimeOut();//控制栏动画时间
    void OnFullscreenMouseDetectTimeOut();//全屏时鼠标检测时间
    void OnCtrlBarHideTimeOut();//控制栏隐藏时间
    void OnShowMenu();//展示目录
    void OnShowAbout();//展示关于
    void OpenFile();//打开文件
    //void OnShowSettingWid();
signals:
    void SigShowMax(bool bIfMax);//最大化
    void SigSeekForward();//前进
    void SigSeekBack();//后退
    void SigAddVolume();//音量加
    void SigSubVolume();//音量减
    void SigPlayOrPause();//播放暂停
    void SigOpenFile(QString strFilename);//打开文件
private:
    Ui::MainWid *ui;

    bool m_bPlaying;//正在播放
    const int m_nShadowWidth;//阴影宽度
    bool m_bFullScreenPlay;//全屏播放标志

    QPropertyAnimation *m_stCtrlbarAnimationShow; //全屏时控制面板浮动显示（属性动画）
    QPropertyAnimation *m_stCtrlbarAnimationHide; //全屏时控制面板浮动显示（属性动画）
    QRect m_stCtrlBarAnimationShow;//控制面板显示区域（构建矩形）
    QRect m_stCtrlBarAnimationHide;//控制面板隐藏区域（构建矩形）

    QTimer m_stCtrlBarAnimationTimer;
    QTimer m_stFullscreenMouseDetectTimer;//全屏时鼠标位置监测时钟
    bool m_bFullscreenCtrlBarShow;
    QTimer stCtrlBarHideTimer;

    Playlist m_stPlaylist;//播放列表
    Title m_stTitle;//标题列表

    bool m_bMoveDrag;//移动窗口标志
    QPoint m_DragPosition;//移动位置

    About m_stAboutWidget;//关于窗口
    //SettingWid m_stSettingWid;//设置窗口

    QMenu m_stMenu;//菜单
    QAction m_stActExit;//QMenu菜单的执行
    QAction m_stActAbout;
    QAction m_stActOpen;
    QAction m_stActFullscreen;
};

#endif // MainWid_H
