#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = nullptr);
    ~LogDialog();
    void updateData(QString tempStr);

protected:
    void resizeEvent ( QResizeEvent *event);
private:
    Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
