#pragma once
#include <QListWidget>
#include <QMenu>
#include <QAction>
class MediaList : public QListWidget
{
    Q_OBJECT

public:
    MediaList(QWidget *parent = 0);
    ~MediaList();
    bool Init();//初始化
protected:
    void contextMenuEvent(QContextMenuEvent* event);//鼠标右键
private:
    void AddFile();//添加文件
    void RemoveFile();//移除文件
signals:
    void SigAddFile(QString strFileName);   //添加文件信号
private:
    QMenu m_stMenu;
    QAction m_stActAdd;     //添加文件
    QAction m_stActRemove;  //移除文件
    QAction m_stActClearList;//清空列表
};
