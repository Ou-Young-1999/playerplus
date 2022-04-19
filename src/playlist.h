#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <QWidget>
#include <QListWidgetItem>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
namespace Ui {
class Playlist;
}
class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = 0);
    ~Playlist();
    bool Init();
    //bool GetPlaylistStatus();
public:
    void OnAddFile(QString strFileName);//添加文件
    void OnAddFileAndPlay(QString strFileName);//添加文件并播放
    void OnBackwardPlay();//上一首/视频
    void OnForwardPlay();//下一首/视频
    /* 在这里定义dock的初始大小 */
    QSize sizeHint() const
    {
        return QSize(150, 900);//构造长宽大小的对象
    }
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
signals:
    void SigUpdateUi();//界面排布更新
    void SigPlay(QString strFile);//播放文件
private:
    bool InitUi();
    bool ConnectSignalSlots();   
private slots:
    void on_List_itemDoubleClicked(QListWidgetItem *item);//双击播放列表
private:
    Ui::Playlist *ui;
    int m_nCurrentPlayListIndex;
};

#endif // PLAYLIST_H
