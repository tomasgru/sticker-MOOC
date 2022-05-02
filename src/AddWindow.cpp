#include "AddWindow.h"
#include "MainWindow.h"
#include <QDebug>

QString textStyle("color: rgb(0,0,0);"
                           "font: 10pt \"Microsoft YaHei UI\";");

QString labelStyle("color: rgb(255,255,255);"
                   "font: 12pt \"Microsoft YaHei UI\";");
QString titleStyle("color: rgb(255,255,255);"
                   "font: bold 24pt \"Microsoft YaHei UI\";");



AddWindow::AddWindow(QWidget *parent, MainWindow *Sibling)
    : TranslucentWidget(parent, 300, 170), sibling(Sibling)
{
    setRect();
    createElem();
    setLayout();
    setStyle();
    m_Month->setValidator(new QRegExpValidator(QRegExp("^[1-9]|[0][1-9]|[1][0-2]$")));
    m_Day->setValidator(new QRegExpValidator(QRegExp("^[1-9]|[0][1-9]|[1-2][0-9]|[3][0-1]$")));
    m_Hour->setValidator(new QRegExpValidator(QRegExp("^[0-1][0-9]|[2][0-3]|[0-9]$")));
    m_Minute->setValidator(new QRegExpValidator(QRegExp("^[0-5][0-9]|[0-9]$")));
    connect(m_Confirm,&QPushButton::clicked,this,&AddWindow::sendTask);
    connect(m_Cancel,&QPushButton::clicked,this,&AddWindow::cancelEvent);
    this->resizeLocked = true;
}

AddWindow::~AddWindow() {
    sibling->addWindow = nullptr;
}

void AddWindow::sendTask()
{
    if (m_Text->text() == "") return;
    QString month, day, hour, minute;
    if (m_Month->text() == "") month = QDate::currentDate().toString("MM");
    else month = m_Month->text();
    if (m_Day->text() == "") day = QDate::currentDate().toString("dd");
    else day = m_Day->text();
    if (m_Hour->text() == "") hour = "23";
    else hour = m_Hour->text();
    if (m_Minute->text() == "") minute = "59";
    else minute = m_Minute->text();
    QDate current = QDate::currentDate();
    QString tmpTimeString = current.toString("yyyy") + QString("%1").arg(month,2,QLatin1Char('0'))
            + QString("%1").arg(day,2,QLatin1Char('0')) + QString("%1").arg(hour,2,QLatin1Char('0'))
            + QString("%1").arg(minute,2,QLatin1Char('0'));
    QDateTime tmpTime(QDateTime::fromString(tmpTimeString,"yyyyMMddhhmm"));
    if (!tmpTime.isValid()) {
        QMessageBox::critical(nullptr,"Error","Invalid date! Please check the date.");
        return;
    }
    //qDebug()<<current<<endl<<tmpTime<<endl;
    if (tmpTime.date().month() < current.month()
        || (tmpTime.date().month() == current.month() && tmpTime.date().day() < current.day()))
        tmpTime.setDate(QDate(tmpTime.date().year()+1, tmpTime.date().month(),tmpTime.date().day()));
    //qDebug()<<tmpTime<<endl;
    sibling->addTask(tmpTime, m_Text->text(),false,0);
    m_Text->clear();
}

void AddWindow::cancelEvent()
{
    hide();
    delTimer = new QTimer(this);
    delTimer->start(60000);
    delTimer->setSingleShot(true);
    connect(delTimer,&QTimer::timeout,this,&AddWindow::deleteLater);
}

//写成函数是因为 MainWindow 的 moveEvent() 要调用
//现在不调用了，留着吧
void AddWindow::setRect()
{
    setSmallGeometry(QRect(sibling->geometry().right(),sibling->smallGeometry().top(),300,170));
}

void AddWindow::setStyle()
{
    m_Title->setText("A New Task!");
    m_Date->setText("Date :");
    m_Time->setText("Time :");
    m_Dot->setText(".");
    m_Colon->setText("：");
    m_Confirm->setText("确认");
    m_Cancel->setText("取消");
    m_Text->setPlaceholderText("Enter the task title here.");
    m_Month->setPlaceholderText(QDate::currentDate().toString("MM"));
    m_Day->setPlaceholderText(QDate::currentDate().toString("dd"));
    m_Hour->setPlaceholderText("23");
    m_Minute->setPlaceholderText("59");

    m_Title->setStyleSheet(titleStyle);
    m_Time->setStyleSheet(labelStyle);
    m_Date->setStyleSheet(labelStyle);
    m_Dot->setStyleSheet(labelStyle);
    m_Colon->setStyleSheet(labelStyle);
    m_Text->setStyleSheet(textStyle);
    m_Month->setStyleSheet(textStyle);
    m_Day->setStyleSheet(textStyle);
    m_Hour->setStyleSheet(textStyle);
    m_Minute->setStyleSheet(textStyle);
    m_Confirm->setStyleSheet(MainWindow::buttonStyle("blank"));
    m_Cancel->setStyleSheet(MainWindow::buttonStyle("blank"));
    m_Month->setAlignment(Qt::AlignRight);
    m_Hour->setAlignment(Qt::AlignRight);
}

void AddWindow::setLayout()
{
    m_Title->setGeometry(45+OVER,10+OVER,200,40);
    m_Text->setGeometry(25+OVER,62+OVER,250,21);
    m_Month->setGeometry(100+OVER,102+OVER,50,21);
    m_Day->setGeometry(160+OVER,102+OVER,50,21);
    m_Hour->setGeometry(100+OVER,132+OVER,50,21);
    m_Minute->setGeometry(160+OVER,132+OVER,50,21);
    m_Date->setGeometry(35+OVER,100+OVER,50,20);
    m_Time->setGeometry(35+OVER,130+OVER,50,20);
    m_Dot->setGeometry(152+OVER,105+OVER,10,25);
    m_Colon->setGeometry(150+OVER,130+OVER,10,25);
    m_Confirm->setGeometry(225+OVER,100+OVER,40,25);
    m_Cancel->setGeometry(225+OVER,130+OVER,40,25);
}

void AddWindow::createElem()
{
    m_Title = new QLabel(this);
    m_Date = new QLabel(this);
    m_Time = new QLabel(this);
    m_Dot = new QLabel(this);
    m_Colon = new QLabel(this);
    m_Text = new QLineEdit(this);
    m_Month = new QLineEdit(this);
    m_Day = new QLineEdit(this);
    m_Hour = new QLineEdit(this);
    m_Minute = new QLineEdit(this);
    m_Confirm = new QPushButton(this);
    m_Cancel = new QPushButton(this);
}
