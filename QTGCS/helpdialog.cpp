#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent)
{
    QString contentString = "     Helper     \n";
    contentString = contentString + "-----Map-----\n";
    contentString = contentString + "-----Manual Flight Control-----\n";
    contentString = contentString + "-----Autonomous Flight Control-----\n";
    contentString = contentString + "-----Missions-----\n";
    contentString = contentString + "-----Voice Service-----\n";
    contentString = contentString + "-----Wireless Connection-----\n";
    contentString = contentString + "-----Server IP Address-----\n";

    contentLabel = new QLabel(contentString);
    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->addWidget(contentLabel);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(contentLayout);

    setLayout(mainLayout);
}

HelpDialog::~HelpDialog(){}
