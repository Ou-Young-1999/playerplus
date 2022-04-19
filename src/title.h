#ifndef MAINWIDTITLE_H
#define MAINWIDTITLE_H

#include <QWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include "about.h"

namespace Ui {
class Title;
}

class Title : public QWidget
{
    Q_OBJECT

public:
    explicit Title(QWidget *parent = 0);
    ~Title();
    bool Init();//初始化
private:
    void mouseDoubleClickEvent(QMouseEvent *event);//双击鼠标事件
    void ChangeMovieNameShow();//视频名称展示
    bool InitUi();//初始化
    void OpenFile();//打开文件
public:
	/**
	 * @brief	改变最大化按钮显示图标
	 * 
	 * @param	bIfMax 主窗口是否是最大化状态
	 * @note 	
	 */
    void OnChangeMaxBtnStyle(bool bIfMax);//最大化按钮样式变化
    void OnPlay(QString strMovieName);//播放时展示视频名称
    void OnStopFinished();//播放结束
signals:
    void SigCloseBtnClicked();//点击关闭按钮
    void SigMinBtnClicked();//点击最小化按钮
    void SigMaxBtnClicked();//点击最大化按钮
    void SigDoubleClicked();//双击标题栏
    void SigFullScreenBtnClicked();//点击全屏按钮
    void SigOpenFile(QString strFileName);//打开文件
    void SigShowMenu();//展示菜单
private:
    Ui::Title *ui;

    QString m_strMovieName;//视频名称
    QMenu m_stMenu;//菜单
    About about;//关于
};

#endif // MAINWIDTITLE_H
