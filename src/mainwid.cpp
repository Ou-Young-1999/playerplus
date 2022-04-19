#include <QFile>
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QAbstractItemView>
#include <QMimeData>
#include <QSizeGrip>
#include <QWindow>
#include <QDesktopWidget>
#include <QScreen>
#include <QRect>
#include <QFileDialog>

#include "mainwid.h"
#include "ui_mainwid.h"
#include "globalhelper.h"
#include "videoctl.h"

const int FULLSCREEN_MOUSE_DETECT_TIME = 500;//鼠标监听
//构造函数
MainWid::MainWid(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWid),
    m_nShadowWidth(0),
    m_stMenu(this),
    m_stPlaylist(this),
    m_stTitle(this),
    m_bMoveDrag(false),
    m_stActExit(this),
    m_stActAbout(this),
    m_stActOpen(this),
    m_stActFullscreen(this)
{
    ui->setupUi(this);
    //无边框、任务栏点击最小化
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    //设置任务栏图标
    this->setWindowIcon(QIcon("://res/player.png"));
    //加载样式
    QString qss = GlobalHelper::GetQssStr("://res/qss/mainwid.css");
    setStyleSheet(qss);

    //用于全屏时追踪鼠标
    this->setMouseTracking(true);
    //判断视频是否正在播放
    m_bPlaying = false;
    //判断视频是否全屏播放
    m_bFullScreenPlay = false;
    //设置定时器
    m_stCtrlBarAnimationTimer.setInterval(2000);
    m_stFullscreenMouseDetectTimer.setInterval(FULLSCREEN_MOUSE_DETECT_TIME);
}
//析构函数
MainWid::~MainWid()
{
    delete ui;
}
//初始化
bool MainWid::Init()
{
    //设置播放列表界面
    QWidget *em = new QWidget(this);
    ui->PlaylistWid->setTitleBarWidget(em);
    ui->PlaylistWid->setWidget(&m_stPlaylist);
    //设置头部列表界面
    QWidget *emTitle = new QWidget(this);
    ui->TitleWid->setTitleBarWidget(emTitle);
    ui->TitleWid->setWidget(&m_stTitle);
    //连接自定义信号与槽
    if (ConnectSignalSlots() == false)
    {
        return false;
    }
    //初始化失败
    if (ui->CtrlBarWid->Init() == false || 
        m_stPlaylist.Init() == false ||
        ui->ShowWid->Init() == false || 
        m_stTitle.Init() == false)
    {
        return false;
    }
    //全屏时控制面板浮动
    m_stCtrlbarAnimationShow = new QPropertyAnimation(ui->CtrlBarWid, "geometry");//展示
    m_stCtrlbarAnimationHide = new QPropertyAnimation(ui->CtrlBarWid, "geometry");//隐藏
    //‘相关’界面初始化失败
    if (m_stAboutWidget.Init() == false)
    {
        return false;
    }
    //右键菜单
    m_stActFullscreen.setText("全屏");
    m_stActFullscreen.setCheckable(true);
    m_stMenu.addAction(&m_stActFullscreen);

    m_stActOpen.setText("打开");
    m_stMenu.addAction(&m_stActOpen);

    m_stActAbout.setText("关于");
    m_stMenu.addAction(&m_stActAbout);
    
    m_stActExit.setText("退出");
    m_stMenu.addAction(&m_stActExit);

    return true;
}
//判断信号与槽是否连接
bool MainWid::ConnectSignalSlots()
{
    //连接信号与槽
    //标题栏
    connect(&m_stTitle, &Title::SigCloseBtnClicked, this, &MainWid::OnCloseBtnClicked);//关闭按钮
    connect(&m_stTitle, &Title::SigMaxBtnClicked, this, &MainWid::OnMaxBtnClicked);//最大化按钮
    connect(&m_stTitle, &Title::SigMinBtnClicked, this, &MainWid::OnMinBtnClicked);//最小化按钮
    connect(&m_stTitle, &Title::SigDoubleClicked, this, &MainWid::OnMaxBtnClicked);//双击标题栏按钮最大化
    connect(&m_stTitle, &Title::SigFullScreenBtnClicked, this, &MainWid::OnFullScreenPlay);//全屏按钮
    connect(&m_stTitle, &Title::SigOpenFile, &m_stPlaylist, &Playlist::OnAddFileAndPlay);//打开文件
    connect(&m_stTitle, &Title::SigShowMenu, this, &MainWid::OnShowMenu);//展示菜单
    
    //播放列表栏
    connect(&m_stPlaylist, &Playlist::SigPlay, ui->ShowWid, &Show::SigPlay);//播放文件

    //展示栏
    connect(ui->ShowWid, &Show::SigOpenFile, &m_stPlaylist, &Playlist::OnAddFileAndPlay);//打开文件
    connect(ui->ShowWid, &Show::SigFullScreen, this, &MainWid::OnFullScreenPlay);//展示全屏
    connect(ui->ShowWid, &Show::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);//播放与暂停
    connect(ui->ShowWid, &Show::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);//停止
    connect(ui->ShowWid, &Show::SigShowMenu, this, &MainWid::OnShowMenu);//展示菜单
    connect(ui->ShowWid, &Show::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);//快进
    connect(ui->ShowWid, &Show::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);//后退
    connect(ui->ShowWid, &Show::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);//音量加
    connect(ui->ShowWid, &Show::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);//音量减

    //控制栏
    connect(ui->CtrlBarWid, &CtrlBar::SigPlaySeek, VideoCtl::GetInstance(), &VideoCtl::OnPlaySeek);
    connect(ui->CtrlBarWid, &CtrlBar::SigPlayVolume, VideoCtl::GetInstance(), &VideoCtl::OnPlayVolume);//音量
    connect(ui->CtrlBarWid, &CtrlBar::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);//播放暂停
    connect(ui->CtrlBarWid, &CtrlBar::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);//停止
    connect(ui->CtrlBarWid, &CtrlBar::SigBackwardPlay, &m_stPlaylist, &Playlist::OnBackwardPlay);//上一曲/视频
    connect(ui->CtrlBarWid, &CtrlBar::SigForwardPlay, &m_stPlaylist, &Playlist::OnForwardPlay);//下一曲/视频
    connect(ui->CtrlBarWid, &CtrlBar::SigShowMenu, this, &MainWid::OnShowMenu);//菜单

    //整体栏
    connect(this, &MainWid::SigShowMax, &m_stTitle, &Title::OnChangeMaxBtnStyle);
    connect(this, &MainWid::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);//播放暂停
    connect(this, &MainWid::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);//快进
    connect(this, &MainWid::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);//后退
    connect(this, &MainWid::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);//音量加
    connect(this, &MainWid::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);//音量减
    connect(this, &MainWid::SigOpenFile, &m_stPlaylist, &Playlist::OnAddFileAndPlay);//添加文件

    //视频控制
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoTotalSeconds, ui->CtrlBarWid, &CtrlBar::OnVideoTotalSeconds);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoPlaySeconds, ui->CtrlBarWid, &CtrlBar::OnVideoPlaySeconds);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoVolume, ui->CtrlBarWid, &CtrlBar::OnVideoVolume);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigPauseStat, ui->CtrlBarWid, &CtrlBar::OnPauseState, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->CtrlBarWid, &CtrlBar::OnStopFinished, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->ShowWid, &Show::OnStopFinished, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigFrameDimensionsChanged, ui->ShowWid, &Show::OnFrameDimensionsChanged, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, &m_stTitle, &Title::OnStopFinished, Qt::DirectConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStartPlay, &m_stTitle, &Title::OnPlay, Qt::DirectConnection);

    //定时器，追踪光标
    connect(&m_stCtrlBarAnimationTimer, &QTimer::timeout, this, &MainWid::OnCtrlBarAnimationTimeOut);
    connect(&m_stFullscreenMouseDetectTimer, &QTimer::timeout, this, &MainWid::OnFullscreenMouseDetectTimeOut);

    //菜单命令
    connect(&m_stActAbout, &QAction::triggered, this, &MainWid::OnShowAbout);//关于
    connect(&m_stActFullscreen, &QAction::triggered, this, &MainWid::OnFullScreenPlay);//全屏
    connect(&m_stActExit, &QAction::triggered, this, &MainWid::OnCloseBtnClicked);//退出
    connect(&m_stActOpen, &QAction::triggered, this, &MainWid::OpenFile);//打开文件
    
	return true;
}
//监听键盘释放事件
void MainWid::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "MainWid::keyPressEvent:" << event->key();
	switch (event->key())
	{
    case Qt::Key_Return://‘回车’全屏
        OnFullScreenPlay();
		break;
    case Qt::Key_P://P键播放暂停
        emit SigPlayOrPause();
        break;
    case Qt::Key_Left://‘<-’后退
        emit SigSeekBack();//关联信号
        break;
    case Qt::Key_Right://‘->’前进
        qDebug() << "前进5s";
        emit SigSeekForward();//信号
        break;
    case Qt::Key_Up://增加音量
        emit SigAddVolume();
        break;
    case Qt::Key_Down://减少音量
        emit SigSubVolume();
        break;
	default:
		break;
	}
}
//监听鼠标点击
void MainWid::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)//鼠标左键
    {
        if (ui->TitleWid->geometry().contains(event->pos()))
        {
            m_bMoveDrag = true;//移动窗口
            m_DragPosition = event->globalPos() - this->pos();
        }
    }

    QWidget::mousePressEvent(event);
}
//监听鼠标释放
void MainWid::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMoveDrag = false;//移动窗口结束

    QWidget::mouseReleaseEvent(event);
}
//监听鼠标移动
void MainWid::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMoveDrag)//鼠标在左键点击的情况下移动，窗口跟着移动
    {
        move(event->globalPos() - m_DragPosition);
    }

    QWidget::mouseMoveEvent(event);
}
//监听鼠标右键
void MainWid::contextMenuEvent(QContextMenuEvent* event)
{
    m_stMenu.exec(event->globalPos());//无限循环
}
//监视全屏
void MainWid::OnFullScreenPlay()
{
    if (m_bFullScreenPlay == false)//非全屏
    {
        m_bFullScreenPlay = true;
        m_stActFullscreen.setChecked(true);
        //脱离父窗口后才能设置
        ui->ShowWid->setWindowFlags(Qt::Window);
        //多屏情况下，在当前屏幕全屏
        QScreen *pStCurScreen = qApp->screens().at(qApp->desktop()->screenNumber(this));
        ui->ShowWid->windowHandle()->setScreen(pStCurScreen);
        
        ui->ShowWid->showFullScreen();

        //
        QRect stScreenRect = pStCurScreen->geometry();
        int nCtrlBarHeight = ui->CtrlBarWid->height();
        int nX = ui->ShowWid->x();
        m_stCtrlBarAnimationShow = QRect(nX, stScreenRect.height() - nCtrlBarHeight, stScreenRect.width(), nCtrlBarHeight);
        m_stCtrlBarAnimationHide = QRect(nX, stScreenRect.height(), stScreenRect.width(), nCtrlBarHeight);

        m_stCtrlbarAnimationShow->setStartValue(m_stCtrlBarAnimationHide);
        m_stCtrlbarAnimationShow->setEndValue(m_stCtrlBarAnimationShow);
        m_stCtrlbarAnimationShow->setDuration(1000);

        m_stCtrlbarAnimationHide->setStartValue(m_stCtrlBarAnimationShow);
        m_stCtrlbarAnimationHide->setEndValue(m_stCtrlBarAnimationHide);
        m_stCtrlbarAnimationHide->setDuration(1000);
        
        ui->CtrlBarWid->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        ui->CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        ui->CtrlBarWid->raise();
        ui->CtrlBarWid->setWindowOpacity(0.5);
        ui->CtrlBarWid->showNormal();
        ui->CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        
        m_stCtrlbarAnimationShow->start();
        m_bFullscreenCtrlBarShow = true;
        m_stFullscreenMouseDetectTimer.start();

        this->setFocus();
    }
    else//全屏
    {
        m_bFullScreenPlay = false;
        m_stActFullscreen.setChecked(false);

        m_stCtrlbarAnimationShow->stop(); //快速切换时，动画还没结束导致控制面板消失
        m_stCtrlbarAnimationHide->stop();
        ui->CtrlBarWid->setWindowOpacity(1);
        ui->CtrlBarWid->setWindowFlags(Qt::SubWindow);
        
        ui->ShowWid->setWindowFlags(Qt::SubWindow);

        ui->CtrlBarWid->showNormal();
        ui->ShowWid->showNormal();

        m_stFullscreenMouseDetectTimer.stop();
        this->setFocus();
    }
}

void MainWid::OnCtrlBarAnimationTimeOut()
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
}
//监视全屏下的鼠标
void MainWid::OnFullscreenMouseDetectTimeOut()
{

    if (m_bFullScreenPlay)
    {
        if (m_stCtrlBarAnimationShow.contains(cursor().pos()))
        {
            //判断鼠标是否在控制面板上面
            if (ui->CtrlBarWid->geometry().contains(cursor().pos()))
            {
                //继续显示
                m_bFullscreenCtrlBarShow = true;
            }
            else
            {
                //需要显示
                ui->CtrlBarWid->raise();
                
                m_stCtrlbarAnimationShow->start();
                m_stCtrlbarAnimationHide->stop();
                stCtrlBarHideTimer.stop();
            }
        }
        else  
        {
            if (m_bFullscreenCtrlBarShow)
            {
                //需要隐藏
                m_bFullscreenCtrlBarShow = false;
                stCtrlBarHideTimer.singleShot(2000, this, &MainWid::OnCtrlBarHideTimeOut);
            }

        }

    }
}

void MainWid::OnCtrlBarHideTimeOut()
{
    m_stCtrlbarAnimationHide->start();
}
//右键展示菜单
void MainWid::OnShowMenu()
{
    m_stMenu.exec(cursor().pos());
}
//展示相关
void MainWid::OnShowAbout()
{
    m_stAboutWidget.move(cursor().pos().x() - m_stAboutWidget.width()/2, cursor().pos().y() - m_stAboutWidget.height()/2);
    m_stAboutWidget.show();
}
//打开文件
void MainWid::OpenFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this, "打开文件", QDir::homePath(),
        "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp *.mp3)");
    emit SigOpenFile(strFileName);
}
//监视关闭按钮
void MainWid::OnCloseBtnClicked()
{
    this->close();
}
//监视最小化按钮
void MainWid::OnMinBtnClicked()
{
    this->showMinimized();
}
//监视最大化按钮
void MainWid::OnMaxBtnClicked()
{
    if (isMaximized())
    {
        showNormal();
        emit SigShowMax(false);
    }
    else
    {
        showMaximized();
        emit SigShowMax(true);
    }
}
