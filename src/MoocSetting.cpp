#include "MoocSetting.h"
#include "MainWindow.h"

MoocSetting::MoocSetting(QWidget *parent, MainWindow *Sibling)
    : TranslucentWidget(parent, 300, 300), sibling(Sibling)
{
    this->resizeLocked = true;
    setSmallGeometry(QRect(sibling->geometry().right(),sibling->smallGeometry().top(),300,240));
    currentPageNumber = 0;

    tokenEdit = new QPlainTextEdit(this);
    prevPage = new QPushButton(this);
    nextPage = new QPushButton(this);
    getCourse = new QPushButton(this);
    confirm = new QPushButton(this);

    prevPage->setGeometry(20+OVER, 210+OVER, 20, 20);
    nextPage->setGeometry(43+OVER, 210+OVER, 20, 20);
    tokenEdit->setGeometry(20+OVER,20+OVER,260,60);
    getCourse->setGeometry(width()-185-OVER,210+OVER,80,20);
    confirm->setGeometry(width()-100-OVER,210+OVER,80,20);

    prevPage->setStyleSheet(MainWindow::buttonStyle("prevPage"));
    nextPage->setStyleSheet(MainWindow::buttonStyle("nextPage"));
    confirm->setStyleSheet(MainWindow::buttonStyle("blank"));
    getCourse->setStyleSheet(MainWindow::buttonStyle("blank"));
    tokenEdit->setStyleSheet("font: 9pt \"SimSun\";");
    confirm->setText("确定");
    getCourse->setText("获取课程");
    tokenEdit->setPlaceholderText("在这里输入mob-token，再点击获取课程进行选择，最后点确定");
    confirm->setEnabled(false);
    prevPage->hide(), nextPage->hide();
    prevPage->setToolTip("上一页");
    nextPage->setToolTip("下一页");

    connect(prevPage,&QPushButton::clicked,this,&MoocSetting::turnPrevPage);
    connect(nextPage,&QPushButton::clicked,this,&MoocSetting::turnNextPage);
    connect(getCourse,&QPushButton::clicked,this,&MoocSetting::getAllCourse);
    connect(confirm,&QPushButton::clicked,this,&MoocSetting::confirmEvent);

    QFile fp("moocConfig.json");
    if (fp.exists()) {
        fp.open(QFile::ReadOnly | QFile::Text);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QTextStream(&fp).readAll().toLocal8Bit());
        QJsonObject jsonRoot = jsonDoc.object();
        tokenEdit->appendPlainText(jsonRoot["mob_token"].toString());
    }
}

MoocSetting::~MoocSetting()
{
    sibling->moocSetting = nullptr;
}

void MoocSetting::getAllCourse()
{
    if (tokenEdit->toPlainText() == "") return;
    QString mob_token = tokenEdit->toPlainText();
    mob_token.remove(QRegExp("\\s"));
    getCourse->setEnabled(false);
    QFile saveToken("moocConfig.json");
    saveToken.open(QFile::WriteOnly);
    QJsonObject token;
    token["mob_token"] = mob_token;
    QJsonDocument jsonDoc(token);
    QString output = jsonDoc.toJson(QJsonDocument::Indented);
    saveToken.write(output.toLocal8Bit());
    saveToken.close();

    QProcess::execute("mooc.exe getAllMyCourseList");

    QFile fp("courseList.json");
    if (fp.exists()) {
        fp.open(QFile::ReadWrite);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QTextStream(&fp).readAll().toLocal8Bit());
        QJsonArray jsonRoot = jsonDoc.array();
        for (auto it=jsonRoot.begin(); it!=jsonRoot.end(); it++) {
            QJsonObject tmp = it->toObject();
            elem *add = new elem(tmp["cid"].toInt(),tmp["tid"].toInt(),tmp["name"].toString(),this);
            items.push_back(add);
            add->checkBox->hide();
        }
    }
    updateGeometry();
    confirm->setEnabled(true);
    prevPage->show(), nextPage->show();
}

void MoocSetting::confirmEvent()
{
    QJsonArray jsonRoot;
    for (auto it:items) {
        if (it->checkBox->checkState()) {
            QJsonObject add;
            add["cid"] = it->cid;
            add["tid"] = it->tid;
            add["name"] = it->checkBox->text();
            jsonRoot.append(add);
        }
    }
    QJsonDocument jsonDoc = QJsonDocument(jsonRoot);
    QString output = jsonDoc.toJson(QJsonDocument::Indented);

    QFile fp("courseList.json");
    fp.open(QFile::WriteOnly);
    fp.write(output.toLocal8Bit());
    fp.close();
    sibling->moocStart();
    this->deleteLater();
}

void MoocSetting::updateGeometry()
{
    for (int i=0; i<items.size(); i++) items[i]->checkBox->hide();
    for (int i=0, j=i+currentPageNumber*5; i<5&&j<items.size(); i++, j++) {
        if (i==0) items[j]->checkBox->setGeometry(20+OVER, 85+OVER, 260, 20);
        else items[j]->checkBox->setGeometry(20+OVER, items[j-1]->checkBox->geometry().bottom()+5, 260, 20);
        items[j]->checkBox->show();
    }
}

void MoocSetting::turnPrevPage() { if (currentPageNumber) currentPageNumber--; updateGeometry();}
void MoocSetting::turnNextPage() { if ((currentPageNumber+1)*5 < items.size()) currentPageNumber++; updateGeometry();}
