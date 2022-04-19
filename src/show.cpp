#include <QDebug>
#include <QMutex>
#include "show.h"
#include "ui_show.h"
#include "globalhelper.h"
#pragma execution_character_set("utf-8")
QMutex g_show_rect_mutex;
//构造函数
Show::Show(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Show),
    //m_stActionGroup(this),
    m_stMenu(this)
{
    ui->setupUi(this);
    //加载样式
    setStyleSheet(GlobalHelper::GetQssStr("://res/qss/show.css"));
    setAcceptDrops(true);
	
    //防止过度刷新显示
    this->setAttribute(Qt::WA_OpaquePaintEvent);
    ui->label->setUpdatesEnabled(false);
    this->setMouseTracking(true);
    
    m_nLastFrameWidth = 0;//记录视频宽高
    m_nLastFrameHeight = 0; 
}
//析构函数
Show::~Show()
{
    delete ui;
}
//初始化
bool Show::Init()
{
    if (ConnectSignalSlots() == false)
    {
        return false;
    }
	return true;
}
//框架大小改变
void Show::OnFrameDimensionsChanged(int nFrameWidth, int nFrameHeight)
{
    qDebug() << "Show::OnFrameDimensionsChanged" << nFrameWidth << nFrameHeight;
    m_nLastFrameWidth = nFrameWidth;
    m_nLastFrameHeight = nFrameHeight;
    ChangeShow();
}

void Show::ChangeShow()
{
    g_show_rect_mutex.lock();
    if (m_nLastFrameWidth == 0 && m_nLastFrameHeight == 0)
    {
        ui->label->setGeometry(0, 0, width(), height());
    }
    else
    {
        float aspect_ratio;
        int width, height, x, y;
        int scr_width = this->width();
        int scr_height = this->height();

        aspect_ratio = (float)m_nLastFrameWidth / (float)m_nLastFrameHeight;

        height = scr_height;
        width = lrint(height * aspect_ratio) & ~1;
        if (width > scr_width)
        {
            width = scr_width;
            height = lrint(width / aspect_ratio) & ~1;
        }
        x = (scr_width - width) / 2;
        y = (scr_height - height) / 2;

        ui->label->setGeometry(x, y, width, height);
    }
    g_show_rect_mutex.unlock();
}

void Show::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Show::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    ChangeShow();
}

void Show::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "Show::keyPressEvent:" << event->key();
    switch (event->key())
    {
    case Qt::Key_Return://全屏
        SigFullScreen();
        break;
    case Qt::Key_Left://后退5s
        emit SigSeekBack();
        break;
    case Qt::Key_Right://前进5s
        qDebug() << "前进5s";
        emit SigSeekForward();
        break;
    case Qt::Key_Up://增加10音量
        emit SigAddVolume();
        break;
    case Qt::Key_Down://减少10音量
        emit SigSubVolume();
        break;
    case Qt::Key_P://P键播放暂停
        emit SigPlayOrPause();
        break;

    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void Show::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton)
    {
        emit SigShowMenu();
    }

    QWidget::mousePressEvent(event);
}
void Show::OnPlay(QString strFile)
{
    VideoCtl::GetInstance()->StartPlay(strFile, ui->label->winId());
}

void Show::OnStopFinished()
{
    update();
}

bool Show::ConnectSignalSlots()
{
	QList<bool> listRet;
	bool bRet;

	bRet = connect(this, &Show::SigPlay, this, &Show::OnPlay);
    listRet.append(bRet);

    listRet.append(bRet);
	for (bool bReturn : listRet)
	{
		if (bReturn == false)
		{
			return false;
		}
	}

	return true;
}

void Show::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        return;
    }
    for(QUrl url: urls)
    {
        QString strFileName = url.toLocalFile();
        qDebug() << strFileName;
        emit SigOpenFile(strFileName);
        break;
    }

}
