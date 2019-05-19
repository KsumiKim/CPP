#include "mainwindow.h"
#include <QtDebug>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
    setWidget();
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
}


MainWindow::~MainWindow()
{

}

void MainWindow::setWidget()
{
    this->resize(800,600);


    centralwidget = new QWidget(this);
    //titleLabel = new QLabel("chatBox",centralwidget);
    QMenuBar *menubar = new QMenuBar;
    QMenu *menu_file = new QMenu("&File");
    menubar->addMenu(menu_file);
    setMenuBar(menubar);
    QAction *save = new QAction("Save");
    connect(save, SIGNAL(triggered()), this, SLOT(save()));

    menu_file->addAction(save);
    this->menuBar()->setVisible(false);


    verticalLayout = new QVBoxLayout(centralwidget);
    //verticalLayout->addWidget(titleLabel);

    mainFrame = new QFrame(centralwidget);
    verticalLayout_2 = new QVBoxLayout(mainFrame);
    stackedWidget = new QStackedWidget(mainFrame);

    chatPage = new QWidget(this);
    gridLayout = new QGridLayout(chatPage);
    roomTextEdit = new QTextEdit(chatPage);
    roomTextEdit->setReadOnly(true);
    gridLayout->addWidget(roomTextEdit, 0, 0, 1, 1);
    userListWidget = new QListWidget(chatPage);
    gridLayout->addWidget(userListWidget, 0, 1, 1, 2);
    sayLineEdit = new QLineEdit(chatPage);
    gridLayout->addWidget(sayLineEdit, 1, 0, 1, 2);
    sayButton = new QPushButton("say",chatPage);
    sayButton->setMaximumSize(QSize(50, 16777215));
    gridLayout->addWidget(sayButton, 1, 2, 1, 1);

    stackedWidget->addWidget(chatPage);

    loginPage = new QWidget();

    gridLayout_3 = new QGridLayout(loginPage);
    verticalSpacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Fixed);
    gridLayout_3->addItem(verticalSpacer, 0, 1, 1, 1);
    horizontalSpacer = new QSpacerItem(223, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout_3->addItem(horizontalSpacer, 1, 0, 1, 1);

    loginFrame = new QFrame(loginPage);
    loginFrame->setMinimumSize(QSize(300, 0));
    loginFrame->setFrameShape(QFrame::StyledPanel);
    gridLayout_2 = new QGridLayout(loginFrame);
    gridLayout_2->setSpacing(20);
    label = new QLabel("server :",loginFrame);
    gridLayout_2->addWidget(label, 0, 0, 1, 1);
    serverLineEdit = new QLineEdit(loginFrame);
    gridLayout_2->addWidget(serverLineEdit, 0, 1, 1, 1);
    serverLineEdit->setText("10.10.14.99");
    label_2 = new QLabel("user name :",loginFrame);
    gridLayout_2->addWidget(label_2, 1, 0, 1, 1);
    userLineEdit = new QLineEdit(loginFrame);
    gridLayout_2->addWidget(userLineEdit, 1, 1, 1, 1);
    loginButton = new QPushButton("Login",loginFrame);
    gridLayout_2->addWidget(loginButton, 2, 1, 1, 1);
    gridLayout_3->addWidget(loginFrame, 1, 1, 1, 1);
    horizontalSpacer_2 = new QSpacerItem(223, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout_3->addItem(horizontalSpacer_2, 1, 2, 1, 1);
    verticalSpacer_2 = new QSpacerItem(20, 267, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout_3->addItem(verticalSpacer_2, 2, 1, 1, 1);

    stackedWidget->addWidget(loginPage);

    verticalLayout_2->addWidget(stackedWidget);
    verticalLayout->addWidget(mainFrame);
    this->setCentralWidget(centralwidget);

    stackedWidget->setCurrentWidget(loginPage);

    connect(loginButton, SIGNAL(clicked()),this, SLOT(on_loginButton_clicked()));
    connect(sayButton, SIGNAL(clicked()), this, SLOT(on_sayButton_clicked()));

    QShortcut *key;
    key = new QShortcut(QKeySequence(Qt::Key_Enter),chatPage);
    connect(key, SIGNAL(activated()),this,  SLOT(on_sayButton_clicked()));
;


}

void MainWindow::on_loginButton_clicked()
{
    socket->abort();
    socket->connectToHost(serverLineEdit->text(), 8888); //connect server


}

void MainWindow::on_sayButton_clicked()
{
    qWarning("message click");
    QString message = sayLineEdit->text().trimmed();

    // Only send the text to the chat server if it's not empty:
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }

    // Clear out the input box so they can type something else:
    sayLineEdit->clear();

    // Put the focus back into the input box so they can type again:
    sayLineEdit->setFocus();
}


void MainWindow::readyRead()
{
    qWarning("readyRead");
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        QRegExp messageRegex("^([^:]+):(.*)$");
        // Any message that starts with "/users:" is the server sending us a
        // list of users so we can show that list in our GUI:
        QRegExp usersRegex("^/users:(.*)$");

        // Is this a users message:
        if(usersRegex.indexIn(line) != -1)
        {
            // If so, udpate our users list on the right:
            QStringList users = usersRegex.cap(1).split(",");
            userListWidget->clear();
            foreach(QString user, users)
                new QListWidgetItem(user, userListWidget);
        }
        // Is this a normal chat message:
        else if(messageRegex.indexIn(line) != -1)
        {
            // If so, append this message to our chat box:
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            roomTextEdit->append("<b>" + user + "</b>: " + message);
        }
    }
}

// This function gets called when our socket has successfully connected to the chat
// server. (see the connect() call in the MainWindow constructor).
void MainWindow::connected()
{
    qWarning("connected");
    // Flip over to the chat page:
    stackedWidget->setCurrentWidget(chatPage);

    // And send our username to the chat server.
    socket->write(QString("/me:" + userLineEdit->text() + "\n").toUtf8());
     this->menuBar()->setVisible(true);
}

void MainWindow::save()
{
    qWarning("save action");
    QString fileName = QFileDialog::getSaveFileName(this, "Save");
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
       QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
       return;
    }
    QTextStream out(&file);
    QString text = roomTextEdit->toPlainText();
    out << text;
    file.close();
}
