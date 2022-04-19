#include <QDebug>
#include <QDir>
#include "playlist.h"
#include "ui_playlist.h"
#include "globalhelper.h"
Playlist::Playlist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Playlist)
{
    ui->setupUi(this);	
}
Playlist::~Playlist()
{
    QStringList strListPlayList;
    for (int i = 0; i < ui->List->count(); i++)
    {
        strListPlayList.append(ui->List->item(i)->toolTip());
    }
    GlobalHelper::SavePlaylist(strListPlayList);//保存播放列表
    delete ui;
}
//初始化
bool Playlist::Init()
{
    if (ui->List->Init() == false)
    {
        return false;
    }
    if (InitUi() == false)
    {
        return false;
    }
    if (ConnectSignalSlots() == false)
    {
        return false;
    }
    setAcceptDrops(true);
	return true;
}
bool Playlist::InitUi()
{
    setStyleSheet(GlobalHelper::GetQssStr("://res/qss/playlist.css"));
    ui->List->clear();
    QStringList strListPlaylist;
    GlobalHelper::GetPlaylist(strListPlaylist);
    for (QString strVideoFile : strListPlaylist)
    {
        QFileInfo fileInfo(strVideoFile);
        if (fileInfo.exists())
        {
            QListWidgetItem *pItem = new QListWidgetItem(ui->List);
            pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));// 用户数据
            pItem->setText(QString("%1").arg(fileInfo.fileName()));// 显示文本
            pItem->setToolTip(fileInfo.filePath());
            ui->List->addItem(pItem);
        }
    }
    if (strListPlaylist.length() > 0)
    {
        ui->List->setCurrentRow(0);
    }
    return true;
}
//信号与槽连接
bool Playlist::ConnectSignalSlots()
{
	QList<bool> listRet;
	bool bRet;

    bRet = connect(ui->List, &MediaList::SigAddFile, this, &Playlist::OnAddFile);
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
//双击播放
void Playlist::on_List_itemDoubleClicked(QListWidgetItem *item)
{
	emit SigPlay(item->data(Qt::UserRole).toString());
    m_nCurrentPlayListIndex = ui->List->row(item);
    ui->List->setCurrentRow(m_nCurrentPlayListIndex);
}
/*
bool Playlist::GetPlaylistStatus()
{
    if (this->isHidden())
    {
        return false;
    }

    return true;
}*/
//添加文件
void Playlist::OnAddFile(QString strFileName)
{
    bool bSupportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp3", Qt::CaseInsensitive);
    if (!bSupportMovie)
    {
        return;
    }
    QFileInfo fileInfo(strFileName);
	QList<QListWidgetItem *> listItem = ui->List->findItems(fileInfo.fileName(), Qt::MatchExactly);
    QListWidgetItem *pItem = nullptr;
	if (listItem.isEmpty())
	{
        pItem = new QListWidgetItem(ui->List);
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));// 用户数据
        pItem->setText(fileInfo.fileName());// 显示文本
        pItem->setToolTip(fileInfo.filePath());
        ui->List->addItem(pItem);
	}
    else
    {
        pItem = listItem.at(0);
    }
}
//添加文件并播放
void Playlist::OnAddFileAndPlay(QString strFileName)
{
    bool bSupportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp3", Qt::CaseInsensitive);
    if (!bSupportMovie)
    {
        return;
    }
    QFileInfo fileInfo(strFileName);
    QList<QListWidgetItem *> listItem = ui->List->findItems(fileInfo.fileName(), Qt::MatchExactly);
    QListWidgetItem *pItem = nullptr;
    if (listItem.isEmpty())
    {
        pItem = new QListWidgetItem(ui->List);
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));// 用户数据
        pItem->setText(fileInfo.fileName());// 显示文本
        pItem->setToolTip(fileInfo.filePath());
        ui->List->addItem(pItem);
    }
    else
    {
        pItem = listItem.at(0);
    }
    on_List_itemDoubleClicked(pItem);
}
//上一首
void Playlist::OnBackwardPlay()
{
    if (m_nCurrentPlayListIndex == 0)
    {
        m_nCurrentPlayListIndex = ui->List->count() - 1;
        on_List_itemDoubleClicked(ui->List->item(m_nCurrentPlayListIndex));
        ui->List->setCurrentRow(m_nCurrentPlayListIndex);
    }
    else
    {
        m_nCurrentPlayListIndex--;
        on_List_itemDoubleClicked(ui->List->item(m_nCurrentPlayListIndex));
        ui->List->setCurrentRow(m_nCurrentPlayListIndex);
    }
}
//下一首
void Playlist::OnForwardPlay()
{
    if (m_nCurrentPlayListIndex == ui->List->count() - 1)
    {
        m_nCurrentPlayListIndex = 0;
        on_List_itemDoubleClicked(ui->List->item(m_nCurrentPlayListIndex));
        ui->List->setCurrentRow(m_nCurrentPlayListIndex);
    }
    else
    {
        m_nCurrentPlayListIndex++;
        on_List_itemDoubleClicked(ui->List->item(m_nCurrentPlayListIndex));
        ui->List->setCurrentRow(m_nCurrentPlayListIndex);
    }
}
//拖拽播放
void Playlist::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }
    for (QUrl url : urls)
    {
        QString strFileName = url.toLocalFile();

        OnAddFile(strFileName);
    }
}
void Playlist::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
