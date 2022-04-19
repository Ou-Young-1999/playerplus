#ifndef DISPLAY_H
#define DISPLAY_H
#include <QWidget>
#include <QMimeData>
#include <QDebug>
#include <QTimer>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include "videoctl.h"
namespace Ui {
class Show;
}
class Show : public QWidget
{
    Q_OBJECT
public:
    Show(QWidget *parent);
    ~Show();
	bool Init();
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
public:
    void OnPlay(QString strFile);//播放
    void OnStopFinished();//播放结束
    void OnFrameDimensionsChanged(int nFrameWidth, int nFrameHeight);//框架大小改变
private:
	bool ConnectSignalSlots();
    void ChangeShow();
signals:
    void SigOpenFile(QString strFileName);//打开文件
    void SigPlay(QString strFile); //播放
	void SigFullScreen();//全屏播放
    void SigPlayOrPause();//播放或暂停
    void SigStop();//停止
    void SigShowMenu();//显示菜单
    void SigSeekForward();//快进
    void SigSeekBack();//后退
    void SigAddVolume();//音量加
    void SigSubVolume();//音量减
private:
    Ui::Show *ui;
    int m_nLastFrameWidth; //
    int m_nLastFrameHeight;
    QMenu m_stMenu;
};

#endif // DISPLAY_H
