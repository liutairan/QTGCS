#include "inputdialog.h"

InputDialog::InputDialog(QWidget *parent) : QDialog(parent)
{
    idLabel = new QLabel("ID:");
    idLineEdit = new QLineEdit;
    idLineEdit->setDisabled(true);

    typeLabel = new QLabel("Type:");
    typeCombo = new QComboBox;
    QStringList typeList;
    typeList << "WP" << "RTH" << "POS_UNLIM" << "POS_LIM";
    typeCombo->addItems(typeList);

    latLabel = new QLabel("Lat:");
    latLineEdit = new QLineEdit;

    lonLabel = new QLabel("Lon:");
    lonLineEdit = new QLineEdit;

    altLabel = new QLabel("Alt:");
    altLineEdit = new QLineEdit;

    p1Label = new QLabel("P1:");
    p1LineEdit = new QLineEdit;

    p2Label = new QLabel("P2:");
    p2LineEdit = new QLineEdit;

    p3Label = new QLabel("P3:");
    p3LineEdit = new QLineEdit;

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QHBoxLayout *idLayout = new QHBoxLayout;
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idLineEdit);

    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeCombo);

    QHBoxLayout *latLayout = new QHBoxLayout;
    latLayout->addWidget(latLabel);
    latLayout->addWidget(latLineEdit);

    QHBoxLayout *lonLayout = new QHBoxLayout;
    lonLayout->addWidget(lonLabel);
    lonLayout->addWidget(lonLineEdit);

    QHBoxLayout *altLayout = new QHBoxLayout;
    altLayout->addWidget(altLabel);
    altLayout->addWidget(altLineEdit);

    QHBoxLayout *p1Layout = new QHBoxLayout;
    p1Layout->addWidget(p1Label);
    p1Layout->addWidget(p1LineEdit);

    QHBoxLayout *p2Layout = new QHBoxLayout;
    p2Layout->addWidget(p2Label);
    p2Layout->addWidget(p2LineEdit);

    QHBoxLayout *p3Layout = new QHBoxLayout;
    p3Layout->addWidget(p3Label);
    p3Layout->addWidget(p3LineEdit);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(buttonBox);
    //bottomLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(latLayout);
    mainLayout->addLayout(lonLayout);
    mainLayout->addLayout(altLayout);
    mainLayout->addLayout(p1Layout);
    mainLayout->addLayout(p2Layout);
    mainLayout->addLayout(p3Layout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

InputDialog::~InputDialog(){}
