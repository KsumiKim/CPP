#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QButtonGroup>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    QLabel * chk_show_label;
    QButtonGroup *btnGrp;

    // signal이 가진 특별한 함수이다. 사용하기 위해서는 head파일에 public으로 따로 만들어야 한다.
public slots :
    void checked_btn(int id);

};

#endif // WIDGET_H
