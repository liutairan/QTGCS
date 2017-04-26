#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "quadstates.h"

class QLabel;
class QLineEdit;
class QPushButton;

class InputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InputDialog(QWidget *parent = 0);
    virtual ~InputDialog();
    QLabel *idLabel;
    QLineEdit *idLineEdit;

    QLabel *typeLabel;
    QComboBox *typeCombo;

    QLabel *latLabel;
    QLineEdit *latLineEdit;

    QLabel *lonLabel;
    QLineEdit *lonLineEdit;

    QLabel *altLabel;
    QLineEdit *altLineEdit;

    QLabel *p1Label;
    QLineEdit *p1LineEdit;

    QLabel *p2Label;
    QLineEdit *p2LineEdit;

    QLabel *p3Label;
    QLineEdit *p3LineEdit;

    QDialogButtonBox *buttonBox;
signals:

public slots:
};

#endif // INPUTDIALOG_H
