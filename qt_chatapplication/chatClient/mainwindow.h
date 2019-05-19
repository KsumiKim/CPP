#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QMenuBar>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QFrame *mainFrame;
    QVBoxLayout *verticalLayout_2;
    QStackedWidget *stackedWidget;
    QWidget *chatPage;
    QGridLayout *gridLayout;
    QTextEdit *roomTextEdit;
    QListWidget *userListWidget;
    QLineEdit *sayLineEdit;
    QPushButton *sayButton;
    QWidget *loginPage;
    QGridLayout *gridLayout_3;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QFrame *loginFrame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *serverLineEdit;
    QLabel *label_2;
    QLineEdit *userLineEdit;
    QPushButton *loginButton;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;


private slots:
    void on_loginButton_clicked();

    // This gets called when you click the sayButton on
    // the chat page.
    void on_sayButton_clicked();

    // This is a function we'll connect to a socket's readyRead()
    // signal, which tells us there's text to be read from the chat
    // server.
    void readyRead();

    // This function gets called when the socket tells us it's connected.
    void connected();

    void setWidget();

    void save();

private:

    // This is the socket that will let us communitate with the server.
    QTcpSocket *socket;

private slots :


};

#endif


