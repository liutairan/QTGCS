#include "logdialog.h"
#include "ui_logdialog.h"

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::resizeEvent(QResizeEvent * /*event*/)
{
    QRect newGeo = LogDialog::geometry();
    ui->logTextBrowser->resize(newGeo.width()-20, newGeo.height()-20);
}

void LogDialog::updateData(QString tempStr)
{
    //qDebug() << tempStr;
    ui->logTextBrowser->append(tempStr);
}
