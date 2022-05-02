#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : TranslucentWidget(parent)
{
    currentPage = ToDo;
    addWindow = nullptr;
    moocSetting = nullptr;
    folded = false;
    inHead = false;
    inBottom = false;

    page_ToDo = new Page(this, true);
    page_Done = new Page(this, false);
    addButton = new QPushButton(this);
    delButton = new QPushButton(this);
    settingButton = new QPushButton(this);
    foldButton = new QPushButton(this);
    openButton = new QPushButton(this);
    lockButton = new QPushButton(this);
    titleLabel = new ClicableLabel(this);
    doubleClickedTimer = new QTimer(this);
    colorTimer = new QTimer(this);
    saveTimer = new QTimer(this);
    saveTimer->setSingleShot(true);
    doubleClickedTimer->setSingleShot(true);
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    addButton->setMouseTracking(true);
    delButton->setMouseTracking(true);
    settingButton->setMouseTracking(true);
    foldButton->setMouseTracking(true);
    openButton->setMouseTracking(true);
    lockButton->setMouseTracking(true);

    doubleClickedTimer->setSingleShot(true);
    addButton->setGeometry(getAddButtonRect());
    delButton->setGeometry(getdelButtonRect());
    settingButton->setGeometry(getSettingButtonRect());
    foldButton->setGeometry(getFoldButtonRect());
    openButton->setGeometry(getFoldButtonRect());
    lockButton->setGeometry(getLockButtonRect());
    titleLabel->setGeometry(getTitleRect());

    delButton->hide(), addButton->hide();
    openButton->hide(), foldButton->hide();
    lockButton->hide(), settingButton->hide();

    connect(page_ToDo->lab,&ClicableLabel::clicked,this,&MainWindow::transPage);
    connect(page_Done->lab,&ClicableLabel::clicked,this,&MainWindow::transPage);
    connect(addButton,&QPushButton::clicked,this,&MainWindow::newAddWindow);
    connect(delButton,&QPushButton::clicked,this,&MainWindow::doubleClickedTimerStart);
    connect(settingButton,&QPushButton::clicked,this,&MainWindow::newMoocSettingWindow);
    connect(page_ToDo,&Page::taskClear,colorTimer,&QTimer::stop);
    //connect(page_ToDo,&Page::taskNew,this,&MainWindow::startColorTimer);
    //connect(colorTimer,&QTimer::timeout,this,&MainWindow::updateLabelColor);
    connect(saveTimer,&QTimer::timeout,this,&MainWindow::savePosEvent);
    connect(this,&MainWindow::doubleClicked,this,&MainWindow::delDoneTask);
    connect(this,&MainWindow::paintAdd,this,&MainWindow::paintBackground);
    connect(openButton,&QPushButton::clicked,this,&MainWindow::openEvent);
    connect(foldButton,&QPushButton::clicked,this,&MainWindow::foldEvent);
    connect(lockButton,&QPushButton::clicked,this,&MainWindow::lockEvent);
    //connect(this,&MainWindow::mouseMoveAdd,this,&MainWindow::checkMouseLacation);
    connect(this,&MainWindow::enterHead,this,&MainWindow::showHeadPart);
    connect(this,&MainWindow::leaveHead,this,&MainWindow::hideHeadPart);
    connect(this,&MainWindow::enterResize,this,&MainWindow::showResizePart);
    connect(this,&MainWindow::leaveResize,this,&MainWindow::hideResizePart);

    settingButton->setStyleSheet(buttonStyle("settingButton"));
    addButton->setStyleSheet(buttonStyle("addButton"));
    delButton->setStyleSheet(buttonStyle("delButton"));
    openButton->setStyleSheet(buttonStyle("openButton"));
    foldButton->setStyleSheet(buttonStyle("foldButton"));
    lockButton->setStyleSheet(buttonStyle("lockButton-unlocked"));

    addButton->setToolTip("添加待做");
    delButton->setToolTip("双击按钮，清空已做");
    openButton->setToolTip("展开格子");
    foldButton->setToolTip("收起格子");
    lockButton->setToolTip("锁定后，格子将无法移动");
    settingButton->setToolTip("设置慕课自动获取");

    config = new QSettings("./config.ini",QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF8"));
    if (config->contains("x")) {
        int x = config->value("x").toInt();
        int y = config->value("y").toInt();
        int w = config->value("width").toInt();
        int h = config->value("height").toInt();
        setGeometry(x,y,w,h);
    }
    if (config->contains("autoMooc")) {
        if (config->value("autoMooc").toBool()) moocStart();
    }
    else config->setValue("autoMooc","false");
    if (!config->contains("title")) config->setValue("title","便签");
    titleLabel->setText(config->value("title").toString());
    titleLabel->setStyleSheet("color: rgb(255,255,255);\n"
                              "font: 9pt \"Microsoft YaHei UI\";\n");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->show();
    parse();
    moocParse();
}

MainWindow::~MainWindow() {
    if (addWindow) addWindow->deleteLater();
}

void MainWindow::save()
{
    QFile fp("data.json");
    if (!fp.open(QFile::WriteOnly)) return;

    QJsonArray jsonRoot;
    for (auto it:page_ToDo->items) {
        if (it->m_moocId || !it->m_level) continue;
        QJsonObject add;
        add["text"] = it->lineEdit->text();
        add["time"] = (int)it->m_dateTime.toTime_t();
        add["done"] = it->m_done;
        jsonRoot.append(add);
    }
    for (auto it:page_Done->items) {
        if (it->m_moocId || !it->m_level) continue;
        QJsonObject add;
        add["text"] = it->lineEdit->text();
        add["time"] = (int)it->m_dateTime.toTime_t();
        add["done"] = it->m_done;
        jsonRoot.append(add);
    }
    QJsonDocument jsonDoc(jsonRoot);
    QString output = jsonDoc.toJson(QJsonDocument::Indented);

    fp.write(output.toLocal8Bit());
    fp.close();
}

void MainWindow::parse()
{
    QFile fp("data.json");
    if (fp.exists()) {
        fp.open(QFile::ReadOnly | QFile::Text);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QTextStream(&fp).readAll().toLocal8Bit());
        QJsonArray jsonRoot = jsonDoc.array();
        QList<QJsonObject> addList;
        for (auto it=jsonRoot.begin(); it!=jsonRoot.end(); it++)
            addList.push_back(it->toObject());
        for (auto add:addList)
            addTask(QDateTime::fromTime_t(add["time"].toInt()), add["text"].toString(), add["done"].toBool(), false);
    }
}

void MainWindow::moocParse()
{
    QFile fp("taskList.json");
    QString type[]={"homeworks", "quizs", "evaluate", "exams"};
    if (fp.exists()) {
        for (auto it=page_ToDo->items.begin(); it!=page_ToDo->items.end();) {
            if ((*it)->m_moocId) {
                (*it)->deleteTaskEvent();
                it = page_ToDo->items.begin();
            }
            else it++;
        }
        fp.open(QFile::ReadOnly | QFile::Text);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QTextStream(&fp).readAll().toLocal8Bit());
        QJsonObject jsonRoot = jsonDoc.object();
        for (auto x:type) {
            QJsonArray now = jsonRoot[x].toArray();
            QList<QJsonObject> addList;
            for (auto it=now.begin(); it!=now.end(); it++)
                addList.push_back(it->toObject());
            for (auto add:addList) {
                int id = add["id"].toInt();
                QString name = add["name"].toString();
                QString belongName = add["belongName"].toString();
                int deadline = add["deadline"].toInt();
                addTask(QDateTime::fromTime_t(deadline), name+" - "+belongName, false, id, x);
            }
        }
    }
    page_ToDo->updateGeometry();
}

void MainWindow::moocStart()
{
    config->setValue("autoMooc","true");
    moocAuto = new QTimer(this);
    moocAuto->start(3600000);
    connect(moocAuto,&QTimer::timeout,this,&MainWindow::moocAutoEvent);
    moocAutoEvent();
}

void MainWindow::moocAutoEvent()
{
    QProcess::execute("getCourseTaskList.exe");
    moocParse();
}

void MainWindow::savePosEvent()
{
    config->setValue("x",geometry().x());
    config->setValue("y",geometry().y());
    config->setValue("width",geometry().width());
    config->setValue("height",geometry().height());
}

void MainWindow::addTask(QDateTime DateTime, QString Text, bool Done, int Mooc, QString Type)
{
    if (Done) page_Done->addTask(DateTime,Text,Mooc,Type);
    else page_ToDo->addTask(DateTime,Text,Mooc,Type);
    if (currentPage == ToDo) page_ToDo->setMain(), page_Done->setVice();
    else page_ToDo->setVice(), page_Done->setMain();
    save();
}

void MainWindow::startColorTimer()
{
    if (colorTimer->isActive()) updateLabelColor();
    else colorTimer->start(60000);
}

//未启用
void MainWindow::updateLabelColor()
{
    QString normalTask("color: rgb(255,255,255);\n"
                       "font: 11pt \"Microsoft YaHei UI\";\n"
                       "text-align: left;\n"
                       "text-align: VCenter;");
    QString Date("background-color:transparent;\n"
                      "border-width:0;\n"
                      "border-style:outset;\n"
                      "color: rgb(%1,%2,%3);\n"
                      "font: %4 12pt \"Microsoft YaHei UI\";\n"
                      "text-align: left;\n"
                      "text-align: bottom;");
    QString redTask("color: rgb(255,%1,%1);\n"
                    "font: 11pt \"Microsoft YaHei UI\";\n"
                    "text-align: left;\n"
                    "text-align: VCenter;");
    QDateTime current(QDateTime::currentDateTime());
    QDateTime currentDate = QDateTime::fromString(current.toString("yyyyMMdd"),"yyyyMMdd");
    QDateTime nextDate = QDateTime::fromTime_t(currentDate.toTime_t()+86400);
    for (auto it:page_ToDo->items) {
        if (current.date() == it->m_dateTime.date()) {
            if (it->m_level == 0) it->lineEdit->setStyleSheet(Date.arg(238).arg(63).arg(77).arg("bold"));
            else if (it->m_dateTime > current)
                it->dotLabel->setStyleSheet(
                    redTask.arg((int)1.0*(it->m_dateTime.toTime_t()-current.toTime_t())/(it->m_dateTime.toTime_t()-currentDate.toTime_t())));
            else it->dotLabel->setStyleSheet(redTask.arg(0));
        }
        else if (it->m_level == 0 && nextDate.date() == it->m_dateTime.date())
            it->lineEdit->setStyleSheet(Date.arg(252).arg(203).arg(22).arg(""));/*
        else if (it->m_level == 0 && nextDate.date() < it->m_dateTime.date())
            it->lineEdit->setStyleSheet(Date.arg(32).arg(161).arg(98).arg(""));*/
    }
}
/*
void MainWindow::checkMouseLacation(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //qDebug()<<mousePos;
    if (mousePos.y()>=OVER && mousePos.y() <= 30+OVER && !inHead) showHeadPart();
    else if (mousePos.y() > 30+OVER && inHead) hideHeadPart();
    else if (mousePos.y() >= smallGeometry().height()-4+OVER && mousePos.y() <= smallGeometry().height()-OVER && !inBottom) showBottomPart();
    else if (mousePos.y() < smallGeometry().height()-4+OVER && inBottom) hideBottomPart();
}
*/
void MainWindow::foldEvent()
{
    folded = true;
    page_Done->hide(), page_Done->lab->hide();
    page_ToDo->hide(), page_ToDo->lab->hide();
    resizeLocked = true;
    oldHeight = geometry().height();
    setMinimumHeight(30+2*OVER);
    setGeometry(QRect(geometry().left(),geometry().top(),geometry().width(),28+2*OVER));
    page_ToDo->prevPage->hide(), page_ToDo->nextPage->hide();
    page_Done->prevPage->hide(), page_Done->nextPage->hide();
    foldButton->hide();
    openButton->show();
}

void MainWindow::openEvent()
{
    folded = false;
    if (!moveLocked) resizeLocked = false;
    setMinimumHeight(300+2*OVER);
    setGeometry(QRect(geometry().left(),geometry().top(),geometry().width(),oldHeight));
    openButton->hide();
    foldButton->show();
    page_Done->lab->show(), page_ToDo->lab->show();
    if (currentPage == ToDo) page_ToDo->setMain(), page_Done->setVice();
    else page_Done->setMain(), page_ToDo->setVice();
}

void MainWindow::lockEvent()
{
    if (moveLocked) {
        moveLocked = false;
        if (!folded) resizeLocked = false;
        lockButton->setStyleSheet(buttonStyle("lockButton-unlocked"));
        lockButton->setToolTip("锁定后，格子将无法移动");
    }
    else {
        moveLocked = true, resizeLocked = true;
        lockButton->setStyleSheet(buttonStyle("lockButton-locked"));
        lockButton->setToolTip("解锁后，可以任意拖动格子位置");
    }
}

void MainWindow::showHeadPart()
{
    qDebug()<<"enterHead"<<QTime::currentTime();
    titleLabel->setText(config->value("title").toString());
    if (currentPage == ToDo) addButton->show();
    else delButton->show();
    if (folded) openButton->show();
    else foldButton->show();
    settingButton->show();
    lockButton->show();
}

void MainWindow::hideHeadPart()
{
    qDebug()<<"leaveHead"<<QTime::currentTime();
    if (currentPage == ToDo) addButton->hide();
    else delButton->hide();
    if (folded) openButton->hide();
    else foldButton->hide();
    settingButton->hide();
    lockButton->hide();
}

void MainWindow::showResizePart()
{
    qDebug()<<"enterBottom"<<QTime::currentTime();
    repaint();
}

void MainWindow::hideResizePart()
{
    qDebug()<<"leaveBottom"<<QTime::currentTime();
    repaint();
}

void MainWindow::paintBackground()
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(255,255,255,70)));
    painter.drawLine(25+OVER,65+OVER,smallGeometry().width()-25+OVER,65+OVER);
}

void MainWindow::transPage()
{
    if (currentPage == Done) {
        page_ToDo->setMain(), page_Done->setVice();
        currentPage = ToDo;
    }
    else {
        page_Done->setMain(), page_ToDo->setVice();
        currentPage = Done;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (currentPage == ToDo) page_ToDo->setMain(), page_Done->setVice();
    else page_ToDo->setVice(), page_Done->setMain();
    addButton->setGeometry(getAddButtonRect());
    delButton->setGeometry(getdelButtonRect());
    settingButton->setGeometry(getSettingButtonRect());
    titleLabel->setGeometry(getTitleRect());

    saveTimer->start(3000);
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event);
    saveTimer->start(3000);
}

void MainWindow::newAddWindow()
{
    if (!addWindow) addWindow = new AddWindow(nullptr, this);
    if (addWindow->isHidden()) addWindow->show();
    else addWindow->cancelEvent();
}

void MainWindow::newMoocSettingWindow()
{
    if (!moocSetting) moocSetting = new MoocSetting(nullptr, this), moocSetting->show();
    else moocSetting->deleteLater(), moocSetting = nullptr;
}

void MainWindow::delDoneTask()
{
    page_Done->hide();
    for (auto it:page_Done->items) delete it;
    page_Done->items.clear();
}

void MainWindow::doubleClickedTimerStart()
{
    if (doubleClickedTimer->isActive()) {
        emit doubleClicked();
        doubleClickedTimer->stop();
    }
    else doubleClickedTimer->start(500);
}

QRect MainWindow::getAddButtonRect() { return QRect(smallGeometry().width()-49+OVER,4+OVER,21,21); }
QRect MainWindow::getdelButtonRect() { return QRect(smallGeometry().width()-49+OVER,4+OVER,21,21); }
QRect MainWindow::getSettingButtonRect() { return QRect(smallGeometry().width()-26+OVER,4+OVER,21,21); }
QRect MainWindow::getFoldButtonRect() { return  QRect(5+OVER,4+OVER,21,21); }
QRect MainWindow::getLockButtonRect() { return QRect(28+OVER,4+OVER,21,21); }
QRect MainWindow::getTitleRect() { return QRect(55+OVER,4+OVER,smallGeometry().width()-110,21); }
