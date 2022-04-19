#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
namespace Ui { class About; };
class About : public QWidget
{
    Q_OBJECT
public:
    About(QWidget *parent = Q_NULLPTR);//构造函数
    ~About();//析构函数
    bool Init();//初始化
private slots:
    void on_ClosePushButton_clicked();//点击“确定”按钮，关闭窗口
private:
    Ui::About *ui;
};
