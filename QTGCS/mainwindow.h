#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QObject>
#include <QPainter>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QFileDialog>
#include <QLineEdit>
#include <QDateTime>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QListWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QTextToSpeech>

#include "map.h"
#include "multiwii.h"
#include "inputdialog.h"
#include "helpdialog.h"
#include "dataexchange.h"
#include "logdialog.h"
#include "fileio.h"

class QFileInfo;
class QTabWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //QByteArray readJsonFile(const QString &filename);
    void InitMap();
    void InitOverviewPage();
    void InitQuad1Page();
    void InitQuad2Page();
    void InitQuad3Page();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    void draw(QPainter *painter);

    void _auto_zoom_and_center();

    void updateGPSonMap(QList<QuadStates *> *);
    void updateGUILabels(QList<QuadStates *> *);
    void updateOverviewLabels(QList<QuadStates *> *);
    void updateQuad1Labels(QList<QuadStates *> *);
    void updateQuad2Labels(QList<QuadStates *> *);
    void updateQuad3Labels(QList<QuadStates *> *);

    QString currentWorkingPath;
    QString cfgFilePath;
    QString resourcePath;
    QString xbeeAddrPath;
    bool inMapFlag;
    bool leftDown;
    bool rightDown;
    int currentTab;
    int mouseX;
    int mouseY;
    Map* mapHandle;
    QPixmap* imageHandle;
    DataExchange *deHandle;
    QTextToSpeech *voiceHandle;
    //LogMessage tempLogMessage;

    bool quad1ConnSwitch;
    bool quad2ConnSwitch;
    bool quad3ConnSwitch;
    bool quad1RCConnSwitch;
    bool quad2RCConnSwitch;
    bool quad3RCConnSwitch;
signals:
    void paintRequest();
    void updateQuad1TableViewRequest();
    void updateQuad2TableViewRequest();
    void updateQuad3TableViewRequest();
    void updateLog(QString);
public slots:
    void updateSerialInfo(bool value);
    void updateAuxSerialInfo(bool value);
    void logMessage(LogMessage);
    void updatePaint();
    void updateQuad1TableView();
    void updateQuad2TableView();
    void updateQuad3TableView();
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states

private slots:
    void on_serialConnectButton_clicked();

    void on_voiceButton_clicked();

    void on_radioButton_clicked();

    void on_armAllButton_clicked();

    void on_navAllButton_clicked();

    void on_incZoomButton_clicked();

    void on_decZoomButton_clicked();

    void on_returnHomeButton_clicked();

    void on_autoZoomButton_clicked();

    void on_arm1Button_clicked();

    void on_arm2Button_clicked();

    void on_arm3Button_clicked();

    void on_nav1Button_clicked();

    void on_nav2Button_clicked();

    void on_nav3Button_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_serialPortComboBox_currentIndexChanged(int index);

    // quad 1
    void on_quad1TableView_clicked(const QModelIndex &index);
    void on_quad1TableView_doubleClicked(const QModelIndex &index);
    void on_quad1TableView_customContextMenuRequested(const QPoint &pos);

    void Quad1AddWP();
    void Quad1EditWP();
    void Quad1DeleteWP();
    void Quad1ClearWP();
    void Quad1SaveWP();
    void Quad1LoadWP();

    void on_quad1ConnectButton_clicked();
    void on_quad1UploadButton_clicked();
    void on_quad1DownloadButton_clicked();
    void on_quad1LoadButton_clicked();
    void on_quad1SaveButton_clicked();
    void on_quad1EditButton_clicked();

    // quad 2
    void on_quad2TableView_clicked(const QModelIndex &index);
    void on_quad2TableView_doubleClicked(const QModelIndex &index);
    void on_quad2TableView_customContextMenuRequested(const QPoint &pos);

    void Quad2AddWP();
    void Quad2EditWP();
    void Quad2DeleteWP();
    void Quad2ClearWP();
    void Quad2SaveWP();
    void Quad2LoadWP();

    void on_quad2ConnectButton_clicked();
    void on_quad2UploadButton_clicked();
    void on_quad2DownloadButton_clicked();
    void on_quad2LoadButton_clicked();
    void on_quad2SaveButton_clicked();
    void on_quad2EditButton_clicked();

    // quad 3
    void on_quad3TableView_clicked(const QModelIndex &index);
    void on_quad3TableView_doubleClicked(const QModelIndex &index);
    void on_quad3TableView_customContextMenuRequested(const QPoint &pos);

    void Quad3AddWP();
    void Quad3EditWP();
    void Quad3DeleteWP();
    void Quad3ClearWP();
    void Quad3SaveWP();
    void Quad3LoadWP();

    void on_quad3ConnectButton_clicked();
    void on_quad3UploadButton_clicked();
    void on_quad3DownloadButton_clicked();
    void on_quad3LoadButton_clicked();
    void on_quad3SaveButton_clicked();
    void on_quad3EditButton_clicked();
    void on_rthAllButton_clicked();

    void on_rth1Button_clicked();

    void on_rth2Button_clicked();

    void on_rth3Button_clicked();

    void on_manualOffRadioButton_clicked();

    void on_manual1RadioButton_clicked();

    void on_manual2RadioButton_clicked();

    void on_manual3RadioButton_clicked();

    void on_auxSerialConnectButton_clicked();

    void on_helpButton_clicked();

    void on_refreshConnButton_clicked();

    void on_ipRefreshButton_clicked();

    void on_expandButton_clicked();

    void on_quad1RCConnectButton_clicked();

    void on_quad2RCConnectButton_clicked();

    void on_quad3RCConnectButton_clicked();

private:
    Ui::MainWindow *ui;
    LogDialog *logDialog;
    QTabWidget *tabWidget;
    QStandardItemModel *model1;
    QStandardItemModel *model2;
    QStandardItemModel *model3;
    bool quad1WPsEditing;
    int quad1WP_inside;
    bool quad2WPsEditing;
    int quad2WP_inside;
    bool quad3WPsEditing;
    int quad3WP_inside;
    Mission_list WPsToMissions(WP_list);
    int insideWP(QPoint, WP_list);
    void resetTeleLabels();
    void resetRCLabels();
    void logData(QList<QuadStates *> *tempObjList);
    QString logFilePath;
    QList<QString> logStringList;
protected:
    //void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
