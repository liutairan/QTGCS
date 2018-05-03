#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class QLabel;

class HelpDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HelpDialog(QWidget *parent = 0);
    virtual ~HelpDialog();

    QLabel *contentLabel;
signals:

public slots:
};

#endif // HELPDIALOG_H
