#include <QContextMenuEvent>
#include <QFileDialog>
#include "medialist.h"
#pragma execution_character_set("utf-8")
//构造函数
MediaList::MediaList(QWidget *parent)
    : QListWidget(parent),
      m_stMenu(this),
      m_stActAdd(this),
      m_stActRemove(this),
      m_stActClearList(this)
{
}
//析构函数
MediaList::~MediaList()
{
}
//初始化
bool MediaList::Init()
{
    m_stActAdd.setText("添加");
    m_stMenu.addAction(&m_stActAdd);
    m_stActRemove.setText("移除所选项");
    QMenu* stRemoveMenu = m_stMenu.addMenu("移除");
    stRemoveMenu->addAction(&m_stActRemove);
    m_stActClearList.setText("清空列表");
    m_stMenu.addAction(&m_stActClearList);

    connect(&m_stActAdd, &QAction::triggered, this, &MediaList::AddFile);//添加播放列表
    connect(&m_stActRemove, &QAction::triggered, this, &MediaList::RemoveFile);//移除播放列表
    connect(&m_stActClearList, &QAction::triggered, this, &QListWidget::clear);//清空播放列表

    return true;
}
void MediaList::contextMenuEvent(QContextMenuEvent* event)
{
    m_stMenu.exec(event->globalPos());
}
void MediaList::AddFile()
{
    QStringList listFileName = QFileDialog::getOpenFileNames(this, "打开文件", QDir::homePath(),
        "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp *.mp3)");
    for (QString strFileName : listFileName)
    {
        emit SigAddFile(strFileName);
    }
}
void MediaList::RemoveFile()
{
    takeItem(currentRow());
}

