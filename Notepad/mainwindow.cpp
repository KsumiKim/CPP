#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFont>
#include <QFontDialog>
#include <QShortcut>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mainEdit = new QTextEdit(this);
    this->setCentralWidget(mainEdit);
    createMenu();
    this->resize(600, 900);
    // action에 관한게 아니고 mainEdit에 관한 것이므로 생성자에 connect를 작성한다.
    connect(mainEdit, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));
    this->setShortcut();


}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenu()
{
    QMenuBar *menubar = new QMenuBar(this); // 메뉴들의 모임

    QMenu *menu_file = new QMenu("File");   // file 메뉴 하나

    QAction *file_new = new QAction("New"); // file에 action 추가
    connect(file_new, SIGNAL(triggered(bool)),this,SLOT(slot_new()));

    QAction *file_open = new QAction("Open");
    connect(file_open, SIGNAL(triggered(bool)),this,SLOT(slot_open()));

    QAction *file_save = new QAction("Save");
    connect(file_save, SIGNAL(triggered(bool)),this,SLOT(slot_save()));

    QAction *file_saveAs = new QAction("SaveAs");
    connect(file_saveAs, SIGNAL(triggered(bool)),this,SLOT(slot_saveAs()));

    QAction *file_form = new QAction("Form");

    QAction *file_close = new QAction("Close");
    connect(file_close, SIGNAL(triggered(bool)),this,SLOT(slot_close()));


    menu_file->addAction(file_new);         // file menu에 file_new action 추가
    menu_file->addAction(file_open);
    menu_file->addAction(file_save);
    menu_file->addAction(file_saveAs);
    menu_file->addAction(file_form);
    menu_file->addAction(file_close);

    QMenu *menu_Edit = new QMenu("Edit");
    QAction *edit_undo = new QAction("Undo");
    connect(edit_undo, SIGNAL(triggered(bool)),this,SLOT(slot_undo()));
    QAction *edit_redo = new QAction("Redo");
    connect(edit_redo, SIGNAL(triggered(bool)),this,SLOT(slot_redo()));
    QAction *edit_copy = new QAction("Copy");
    connect(edit_copy, SIGNAL(triggered(bool)),this,SLOT(slot_copy()));
    QAction *edit_paste = new QAction("Paste");
    connect(edit_paste, SIGNAL(triggered(bool)),this,SLOT(slot_paste()));

    menu_Edit->addAction(edit_undo);
    menu_Edit->addAction(edit_redo);
    menu_Edit->addAction(edit_copy);
    menu_Edit->addAction(edit_paste);

    QMenu *menu_Form = new QMenu("Form");


    QAction *form_font = new QAction("Font");
    connect(form_font, SIGNAL(triggered(bool)),this,SLOT(slot_font()));
    menu_Form->addAction(form_font);

    QMenu *menu_About = new QMenu("About");
    QAction *about_information = new QAction("Information");
    connect(about_information, SIGNAL(triggered(bool)),this,SLOT(slot_about()));
    menu_About->addAction(about_information);

    menubar->addMenu(menu_file);
    menubar->addMenu(menu_Edit);
    menubar->addMenu(menu_Form);
    menubar->addMenu(menu_About);

    this->setMenuBar(menubar);
    this->menuBar()->setVisible(true);
}

void MainWindow::slot_textChanged()
{
    this->textChanged = true;
}

void MainWindow::slot_new()
{
    if(textChanged){
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Save)
            slot_save();
        else if(ret == QMessageBox::Cancel)
            return;
        else {
            qDebug() << "Discard";
        }

        qDebug() << "text changed" << ret;
    }
    current_file = "";
    this->setWindowTitle("");
    mainEdit->clear();
    textChanged = false;
}

void MainWindow::slot_open()
{
    if(textChanged){
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Save)
            slot_save();
        else if(ret == QMessageBox::Cancel)
            return;
        else
            qDebug() << "Discard";

        qDebug() << "text changed" << ret;
    }

    qDebug() << "slot Open";
    QString filename = QFileDialog::getOpenFileName(this, "open");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
        return;

    current_file = filename;
    this->setWindowTitle(filename);

    QTextStream in(&file); // 파일을 textstream에 넣는다.
    QString text = in.readAll();
    mainEdit->setText(text);
    file.close();
    textChanged = false;
}

void MainWindow::slot_saveAs()
{
    qDebug() << "slot SaveAs";
    QString filename = QFileDialog::getSaveFileName(this, "Save");

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QFile::Text))
        return;

    // filename으로 메모장 이름이 변경된다.
    current_file = filename;
    this->setWindowTitle(filename);

    QTextStream out(&file);
    QString text = mainEdit->toPlainText();
    out << text;
    file.close();

    textChanged = false;
}

void MainWindow::slot_save()
{
    if(current_file.isEmpty())    // 파일이 처음 저장되는 경우
    {
        slot_saveAs();
    }
    else  // 그 외의 경우
    {
        QString filename = current_file;

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QFile::Text))
            return;

        current_file = filename;
        this->setWindowTitle(filename);

        QTextStream out(&file);
        QString text = mainEdit->toPlainText();
        out << text;
        file.close();

        textChanged = false;
    }
}

void MainWindow::slot_close()
{
    if(textChanged){
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Save)
            slot_save();
        else if(ret == QMessageBox::Cancel)
            return;
        else {
            qDebug() << "Discard";
        }

        qDebug() << "text changed" << ret;
    }

    this->close();
}

void MainWindow::slot_undo() {mainEdit->undo();}
void MainWindow::slot_redo(){mainEdit->redo();}
void MainWindow::slot_copy(){mainEdit->copy();}
void MainWindow::slot_paste(){mainEdit->paste();}

void MainWindow::slot_font()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Helvetica [Cronyx]", 10), this);
    // QFont font = QFontDialog::getFont(&ok, QFont ("Times New Roman", 10, QFont::Bold), this);

    if (ok) {
        mainEdit->setFont(font);
    } else {
        mainEdit->setFont(font); // "Helvetica [Cronyx]", 10 으로 setting
    }
}

void MainWindow::slot_about()
{
    QMessageBox::about(this, tr("About Notepad"), tr("......................"));
    QProcess gcc;
    QString gcc_path = "C:\\Qt\\Tools\\mingw530.32\\bin\\g**";
    QString target_path = "C:\testfolder\aa.cpp";
    gcc.start(gcc_path);
}

void MainWindow::setShortcut()
{
    QShortcut *key;
    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_save()));

    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_saveAs()));

    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_open()));

    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_copy()));

    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_paste()));

    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_X), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_undo()));

    key = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(key, SIGNAL(activated()), this, SLOT(slot_redo()));
}
