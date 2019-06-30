#include "widget.h"
#include <QCheckBox>
#include <QString>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    QString chk_label[6] = {"A", "B", "C", "D", "E", "F"};
    QCheckBox *chk_btn[6];
    btnGrp = new QButtonGroup(this);

    int x_pos = 30;
    int y_pos = 30;
    int width = 100;
    int height = 30;

    for(int i = 0; i < 6; i++)
    {
        chk_btn[i] = new QCheckBox(chk_label[i], this);
        btnGrp->addButton(chk_btn[i]);

        if (i < 3)
            chk_btn[i]->setGeometry(x_pos, y_pos, width, height);
        else {
            chk_btn[i]->setGeometry(x_pos + 120, y_pos - 120, width, height);
        }
        y_pos += 40;
    }

    btnGrp->setExclusive(true);

    chk_show_label = new QLabel(this);
    chk_show_label->setGeometry(30, 170, 200, 30);
    //chk_show_label->setText("is clicked");

    connect(btnGrp, SIGNAL(buttonClicked(int)),
            this, SLOT(checked_btn(int)));

    // UI를 구성하는 기본 폼을 생성자에 만든다.
    // (너무 길어지면 디자인에 대한 따로 함수를 만들어 생성자에서 호출한다.)
    // UI는 계속 존재할 것이기 때문에 동적할당해서 만든다. (new)

//    QCheckBox *chk_a = new QCheckBox("A", this);
//    chk_a->setChecked(true);
//    chk_a->setGeometry(30, 30, 100, 30);

//    QCheckBox *chk_b = new QCheckBox("B", this);
//    chk_b->setGeometry(30, 70, 100, 30);

//    QCheckBox *chk_c = new QCheckBox("C", this);
//    chk_c->setGeometry(30, 110, 100, 30);

//    QCheckBox *chk_d = new QCheckBox("D", this);
//    chk_d->setGeometry(150, 30, 100, 30);

//    QCheckBox *chk_e = new QCheckBox("E", this);
//    chk_e->setGeometry(150, 70, 100, 30);

//    QCheckBox *chk_f = new QCheckBox("F", this);
//    chk_f->setGeometry(150, 110, 100, 30);

//    btnGrp->addButton(chk_a);
//    btnGrp->addButton(chk_b);
//    btnGrp->addButton(chk_c);
//    btnGrp->addButton(chk_d);
//    btnGrp->addButton(chk_e);
//    btnGrp->addButton(chk_f);

}

void Widget::checked_btn(int id)
{
    // qDebug() << id;
    qDebug() << btnGrp->button(id)->text();
    QString print_label = "Button " + btnGrp->button(id)->text() + " is clicked";
    chk_show_label->setText(print_label);
}

Widget::~Widget()
{

}
