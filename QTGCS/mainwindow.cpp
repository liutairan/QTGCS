#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qpainter.h>
#include <qstandarditemmodel.h>
#include "multiwii.h"
#include "quadstates.h"
#include "dataexchange.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    removeToolBar(ui->mainToolBar);

    currentWorkingPath = QCoreApplication::applicationDirPath();
    //qDebug() << currentWorkingPath;
    cfgFilePath = currentWorkingPath + "/config.json";
    //qDebug() << cfgFilePath;
    QByteArray val;
    val = readJsonFile(cfgFilePath);
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(val);
    QJsonObject obj = doc.object();
    resourcePath = obj["Resource"].toString();

    logFilePath = resourcePath + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz") + ".log";

    deHandle = new DataExchange();
    connect(deHandle, &DataExchange::teleSerialOnChanged, this, &MainWindow::updateSerialInfo);
    connect(deHandle, &DataExchange::quadsStatesChangeRequest, this, &MainWindow::updateQuadsStates);
    InitMap();
    InitOverviewPage();
    InitQuad1Page();
    InitQuad2Page();
    InitQuad3Page();
    currentTab = ui->tabWidget->currentIndex();

    logDialog = new LogDialog(this);
    //logDialog->show();
    //QRect availableRect = QApplication::desktop()->availableGeometry();
    //logDialog->move(availableRect.width()-270, availableRect.y());
    connect(this, &MainWindow::updateLog, logDialog, &LogDialog::updateData);
    connect(deHandle, &DataExchange::logMessageRequest, this, &MainWindow::logMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray MainWindow::readJsonFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        f.close();
        return QString().toUtf8();
    } else {
        QTextStream in(&f);
        QByteArray retValue = in.readAll().toUtf8();
        f.close();
        return retValue;
    }
}

void MainWindow::logMessage(LogMessage tempMessage /*QString tempStr*/)
{
    //qDebug() << tempStr;
    QString styledString="<span style=\" color:#FF0c32;\" > ";
    styledString.append(tempMessage.id);
    styledString.append("</span>");
    styledString.append("<span style=\" color:#000c32;\" > ");
    styledString.append(tempMessage.message);
    styledString.append("</span>");

    //textBrowser->setHtml(styledString);
    QString currentString = ui->logTextBrowser->document()->toHtml();
    currentString = currentString + styledString;
    ui->logTextBrowser->setHtml(currentString);
    QScrollBar *sb = ui->logTextBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    //ui->logTextBrowser->append(tempStr);
}

// update with quad states
void MainWindow::updateQuadsStates(QList<QuadStates *> *tempObjList)
{
    QString logString = "------------------\n";
    logString = logString + QTime::currentTime().toString("hh:mm:ss.zzz") + ", ";
    logString = logString + "\n---STA---\n";
    logString = logString + "Cycle Time:\n" + QString::number(tempObjList->at(0)->msp_status_ex.cycletime, 10) + "\n";
    logString = logString + "Arming Flags:\n" + QString::number(tempObjList->at(0)->msp_status_ex.armingFlags, 2).rightJustified(16, '0') + "\n";
    logString = logString + "Flight Mode Flags:\n" + QString::number(tempObjList->at(0)->msp_status_ex.packFlightModeFlags, 2).rightJustified(32, '0') + "\n";
    logString = logString + "\n---ALT---\n";
    logString = logString + QString::number(tempObjList->at(0)->msp_sonar_altitude.rangefinderGetLatestAltitude, 10);
    logString = logString + "\n---ATT---\n";
    logString = logString + QString::number(tempObjList->at(0)->msp_attitude.roll/10.0, 'f', 1) + ", ";
    logString = logString + QString::number(tempObjList->at(0)->msp_attitude.pitch/10.0, 'f', 1) + ", ";
    logString = logString + QString::number(tempObjList->at(0)->msp_attitude.yaw, 10) + ", ";
    logString = logString + "\n---GPS---\n";
    logString = logString + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_numSat, 10) + ", ";
    logString = logString + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_fixType, 10) + ", ";
    //ui->logTextBrowser->append(logString);
    updateGUILabels(tempObjList);
    updateGPSonMap(tempObjList);
    logData(tempObjList);
    emit updateLog(logString);
}

void MainWindow::logData(QList<QuadStates *> *tempObjList)
{
    for(int i=0; i< tempObjList->length(); i++)
    {
        if (tempObjList->at(i)->address_long.length() > 0)
        {
            QString outputStr = "";
            outputStr = outputStr + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz") + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_status_ex.cycletime, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_status_ex.armingFlags, 2).rightJustified(16, '0') + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_status_ex.packFlightModeFlags, 2).rightJustified(32, '0') + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_numSat, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_fixType, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_llh_lat, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_llh_lon, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_llh_alt, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_hdop, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_attitude.roll, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_attitude.pitch, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_attitude.yaw, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_sonar_altitude.rangefinderGetLatestAltitude, 10) + ", ";
            outputStr = outputStr + QString::number(tempObjList->at(0)->msp_analog.vbat, 10) + ", ";
            outputStr = outputStr + "\n";

            QFile file(logFilePath);
            if (!file.open(QIODevice::Append | QIODevice::Text))
            {;}

            QTextStream out(&file);
            out << outputStr;
            file.close();
        }
    }
}

void MainWindow::updateGUILabels(QList<QuadStates *> *tempObjList)
{
    updateOverviewLabels(tempObjList);
    if (quad1ConnSwitch == true)
    {
        updateQuad1Labels(tempObjList);
    }
    if (quad2ConnSwitch == true)
    {
        updateQuad2Labels(tempObjList);
    }
    if (quad3ConnSwitch == true)
    {
        updateQuad3Labels(tempObjList);
    }
}

void MainWindow::updateGPSonMap(QList<QuadStates *> *tempObjList)
{
    // Only update GPS on map when the displacement on the map is
    //    greater than 3 meters, hope this way can reduce the frequency
    //    of updating the GUI.
    //    To do.
    if (tempObjList->length())
    {}
    //emit paintRequest();
}

void MainWindow::updateOverviewLabels(QList<QuadStates *> *tempObjList)
{
    if (quad1ConnSwitch == true)
    {
        if (tempObjList->at(0)->msp_sensor_flags.acc == true)
        {
            ui->quad1ACCOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1ACCOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.mag == true)
        {
            ui->quad1MAGOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1MAGOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.baro == true)
        {
            ui->quad1BAROOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1BAROOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.sonar == true)
        {
            ui->quad1SONAROverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1SONAROverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.gps == true)
        {
            ui->quad1GPSOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1GPSOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.pitot == true)
        {
            ui->quad1PITOTOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1PITOTOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.hardware == true)
        {
            ui->quad1HWOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad1HWOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // status/modes
        if (tempObjList->at(0)->msp_flight_modes.arm == true)
        {
            ui->quad1ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad1ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.angle == true)
        {
            ui->quad1LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.nav_althold == true)
        {
            ui->quad1ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.nav_poshold == true)
        {
            ui->quad1POSModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1POSModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if ((tempObjList->at(0)->msp_flight_modes.nav_wp == true) || (tempObjList->at(0)->msp_flight_modes.nav_rth == true))
        {
            ui->quad1NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.nav_rth == true)
        {
            ui->quad1RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.failsafe == true)
        {
            ui->quad1FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad1FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        float tempVoltage1 = tempObjList->at(0)->msp_analog.vbat/10.0;
        ui->quad1VoltageOverview->setText(QString::number(tempVoltage1, 'f', 1) + "V");
        if (tempVoltage1 >= 11.7)
        {
            ui->quad1VoltageOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else if (tempVoltage1 >= 11.1 && tempVoltage1 < 11.7)
        {
            ui->quad1VoltageOverview->setStyleSheet("QLabel {background-color : rgba(255,200,0,1);}");
        }
        else if (tempVoltage1 >= 10.5 && tempVoltage1 < 11.1)
        {
            ui->quad1VoltageOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else if (tempVoltage1 < 10.5)
        {
            ui->quad1VoltageOverview->setStyleSheet("QLabel {background-color : rgba(0,0,255,1);}");
        }

    }
    else  // set to unconnected color
    {
        ui->quad1ACCOverview->setStyleSheet("QLabel {background-color : rgba(220,220,220,1);}");
    }

    if (quad2ConnSwitch == true)
    {
        if (tempObjList->at(1)->msp_sensor_flags.acc == true)
        {
            ui->quad2ACCOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2ACCOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.mag == true)
        {
            ui->quad2MAGOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2MAGOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.baro == true)
        {
            ui->quad2BAROOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2BAROOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.sonar == true)
        {
            ui->quad2SONAROverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2SONAROverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.gps == true)
        {
            ui->quad2GPSOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2GPSOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.pitot == true)
        {
            ui->quad2PITOTOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2PITOTOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.hardware == true)
        {
            ui->quad2HWOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad2HWOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // status/modes
        if (tempObjList->at(1)->msp_flight_modes.arm == true)
        {
            ui->quad2ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad2ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.angle == true)
        {
            ui->quad2LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.nav_althold == true)
        {
            ui->quad2ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.nav_poshold == true)
        {
            ui->quad2POSModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2POSModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if ((tempObjList->at(1)->msp_flight_modes.nav_wp == true) || (tempObjList->at(0)->msp_flight_modes.nav_rth == true))
        {
            ui->quad2NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.nav_rth == true)
        {
            ui->quad2RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.failsafe == true)
        {
            ui->quad2FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad2FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        float tempVoltage2 = tempObjList->at(1)->msp_analog.vbat/10.0;
        ui->quad2VoltageOverview->setText(QString::number(tempVoltage2, 'f', 1) + "V");
        if (tempVoltage2 >= 11.7)
        {
            ui->quad2VoltageOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else if (tempVoltage2 >= 11.1 && tempVoltage2 < 11.7)
        {
            ui->quad2VoltageOverview->setStyleSheet("QLabel {background-color : rgba(255,200,0,1);}");
        }
        else if (tempVoltage2 >= 10.5 && tempVoltage2 < 11.1)
        {
            ui->quad2VoltageOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else if (tempVoltage2 < 10.5)
        {
            ui->quad2VoltageOverview->setStyleSheet("QLabel {background-color : rgba(0,0,255,1);}");
        }
    }
    else  // set to unconnected color
    {
        ui->quad2ACCOverview->setStyleSheet("QLabel {background-color : rgba(220,220,220,1);}");
    }

    if (quad3ConnSwitch == true)
    {
        if (tempObjList->at(2)->msp_sensor_flags.acc == true)
        {
            ui->quad3ACCOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3ACCOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.mag == true)
        {
            ui->quad3MAGOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3MAGOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.baro == true)
        {
            ui->quad3BAROOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3BAROOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.sonar == true)
        {
            ui->quad3SONAROverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3SONAROverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.gps == true)
        {
            ui->quad3GPSOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3GPSOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.pitot == true)
        {
            ui->quad3PITOTOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3PITOTOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.hardware == true)
        {
            ui->quad3HWOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad3HWOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // status/modes
        if (tempObjList->at(2)->msp_flight_modes.arm == true)
        {
            ui->quad3ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad3ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.angle == true)
        {
            ui->quad3LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.nav_althold == true)
        {
            ui->quad3ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.nav_poshold == true)
        {
            ui->quad3POSModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3POSModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if ((tempObjList->at(2)->msp_flight_modes.nav_wp == true) || (tempObjList->at(0)->msp_flight_modes.nav_rth == true))
        {
            ui->quad3NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.nav_rth == true)
        {
            ui->quad3RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.failsafe == true)
        {
            ui->quad3FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad3FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
    }
    else  // set to unconnected color
    {
        ui->quad3ACCOverview->setStyleSheet("QLabel {background-color : rgba(220,220,220,1);}");
    }

}

// When the data from serial port come back, this function will be called.
//    The labels will be updated.
void MainWindow::updateQuad1Labels(QList<QuadStates *> *tempObjList)
{
    if (quad1ConnSwitch == true)
    {
        if (tempObjList->at(0)->msp_sensor_flags.acc == true)
        {
            ui->quad1ACC->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1ACC->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.mag == true)
        {
            ui->quad1MAG->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1MAG->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.baro == true)
        {
            ui->quad1BARO->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1BARO->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.sonar == true)
        {
            ui->quad1SONAR->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1SONAR->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.gps == true)
        {
            ui->quad1GPS->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1GPS->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.pitot == true)
        {
            ui->quad1PITOT->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1PITOT->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_sensor_flags.hardware == true)
        {
            ui->quad1HW->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad1HW->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // status/modes
        if (tempObjList->at(0)->msp_flight_modes.arm == true)
        {
            ui->quad1ARMMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad1ARMMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.angle == true)
        {
            ui->quad1LEVELMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1LEVELMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.nav_althold == true)
        {
            ui->quad1ALTMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1ALTMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.nav_poshold == true)
        {
            ui->quad1POSMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1POSMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if ((tempObjList->at(0)->msp_flight_modes.nav_wp == true) || (tempObjList->at(0)->msp_flight_modes.nav_rth == true))
        {
            ui->quad1NAVMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1NAVMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.nav_rth == true)
        {
            ui->quad1RTHMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad1RTHMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(0)->msp_flight_modes.failsafe == true)
        {
            ui->quad1FAILMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad1FAILMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // inner
        ui->quad1Roll->setText("Roll: " + QString::number(tempObjList->at(0)->msp_attitude.roll/10.0, 'f', 1));
        ui->quad1Pitch->setText("Pitch: " + QString::number(tempObjList->at(0)->msp_attitude.pitch/10.0, 'f', 1));
        ui->quad1Yaw->setText("Yaw: " + QString::number(tempObjList->at(0)->msp_attitude.yaw, 10));
        ui->quad1SensorAlt->setText("Alt: " + QString::number(tempObjList->at(0)->msp_sonar_altitude.rangefinderGetLatestAltitude, 10));
        // outer
        ui->quad1Lat->setText("Lat: " + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_llh_lat/qPow(10.0,7), 'f', 7));
        ui->quad1Lon->setText("Lon: " + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_llh_lon/qPow(10.0,7), 'f', 7));
        ui->quad1Alt->setText("Alt: " + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_llh_alt/qPow(10.0,2), 'f', 2));
        ui->quad1SatNum->setText("No. Sat: " + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_numSat, 10));
        ui->quad1FixType->setText("Fix Type: " + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_fixType, 10));
        ui->quad1HDOP->setText("HDOP: " + QString::number(tempObjList->at(0)->msp_raw_gps.gpsSol_hdop, 10));
    }
    else  // set to unconnected color
    {
        ui->quad1ACC->setStyleSheet("QLabel {background-color : rgba(220,220,220,1);}");
    }
}

void MainWindow::updateQuad2Labels(QList<QuadStates *> *tempObjList)
{
    if (quad2ConnSwitch == true)
    {
        if (tempObjList->at(1)->msp_sensor_flags.acc == true)
        {
            ui->quad2ACC->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2ACC->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.mag == true)
        {
            ui->quad2MAG->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2MAG->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.baro == true)
        {
            ui->quad2BARO->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2BARO->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.sonar == true)
        {
            ui->quad2SONAR->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2SONAR->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.gps == true)
        {
            ui->quad2GPS->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2GPS->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.pitot == true)
        {
            ui->quad2PITOT->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2PITOT->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_sensor_flags.hardware == true)
        {
            ui->quad2HW->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad2HW->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // status/modes
        if (tempObjList->at(1)->msp_flight_modes.arm == true)
        {
            ui->quad2ARMMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad2ARMMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.angle == true)
        {
            ui->quad2LEVELMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2LEVELMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.nav_althold == true)
        {
            ui->quad2ALTMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2ALTMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.nav_poshold == true)
        {
            ui->quad2POSMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2POSMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if ((tempObjList->at(1)->msp_flight_modes.nav_wp == true) || (tempObjList->at(1)->msp_flight_modes.nav_rth == true))
        {
            ui->quad2NAVMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2NAVMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.nav_rth == true)
        {
            ui->quad2RTHMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad2RTHMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(1)->msp_flight_modes.failsafe == true)
        {
            ui->quad2FAILMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad2FAILMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // inner
        ui->quad2Roll->setText("Roll: " + QString::number(tempObjList->at(1)->msp_attitude.roll/10.0, 'f', 1));
        ui->quad2Pitch->setText("Pitch: " + QString::number(tempObjList->at(1)->msp_attitude.pitch/10.0, 'f', 1));
        ui->quad2Yaw->setText("Yaw: " + QString::number(tempObjList->at(1)->msp_attitude.yaw, 10));
        // outer
        ui->quad2Lat->setText("Lat: " + QString::number(tempObjList->at(1)->msp_raw_gps.gpsSol_llh_lat/qPow(10.0,7), 'f', 7));
        ui->quad2Lon->setText("Lon: " + QString::number(tempObjList->at(1)->msp_raw_gps.gpsSol_llh_lon/qPow(10.0,7), 'f', 7));
        ui->quad2Alt->setText("Alt: " + QString::number(tempObjList->at(1)->msp_raw_gps.gpsSol_llh_alt/qPow(10.0,2), 'f', 2));
        ui->quad2SatNum->setText("No. Sat: " + QString::number(tempObjList->at(1)->msp_raw_gps.gpsSol_numSat, 10));
        ui->quad2FixType->setText("Fix Type: " + QString::number(tempObjList->at(1)->msp_raw_gps.gpsSol_fixType, 10));
        ui->quad2HDOP->setText("HDOP: " + QString::number(tempObjList->at(1)->msp_raw_gps.gpsSol_hdop, 10));
    }
    else  // set to unconnected color
    {
        ui->quad2ACC->setStyleSheet("QLabel {background-color : rgba(220,220,220,1);}");
    }
}

void MainWindow::updateQuad3Labels(QList<QuadStates *> *tempObjList)
{
    if (quad3ConnSwitch == true)
    {
        if (tempObjList->at(2)->msp_sensor_flags.acc == true)
        {
            ui->quad3ACC->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3ACC->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.mag == true)
        {
            ui->quad3MAG->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3MAG->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.baro == true)
        {
            ui->quad3BARO->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3BARO->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.sonar == true)
        {
            ui->quad3SONAR->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3SONAR->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.gps == true)
        {
            ui->quad3GPS->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3GPS->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.pitot == true)
        {
            ui->quad3PITOT->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3PITOT->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_sensor_flags.hardware == true)
        {
            ui->quad3HW->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad3HW->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // status/modes
        if (tempObjList->at(2)->msp_flight_modes.arm == true)
        {
            ui->quad3ARMMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad3ARMMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.angle == true)
        {
            ui->quad3LEVELMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3LEVELMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.nav_althold == true)
        {
            ui->quad3ALTMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3ALTMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.nav_poshold == true)
        {
            ui->quad3POSMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3POSMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if ((tempObjList->at(2)->msp_flight_modes.nav_wp == true) || (tempObjList->at(2)->msp_flight_modes.nav_rth == true))
        {
            ui->quad3NAVMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3NAVMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.nav_rth == true)
        {
            ui->quad3RTHMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }
        else
        {
            ui->quad3RTHMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        if (tempObjList->at(2)->msp_flight_modes.failsafe == true)
        {
            ui->quad3FAILMode->setStyleSheet("QLabel {background-color : rgba(255,0,0,1);}");
        }
        else
        {
            ui->quad3FAILMode->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        }

        // inner
        ui->quad3Roll->setText("Roll: " + QString::number(tempObjList->at(2)->msp_attitude.roll/10.0, 'f', 1));
        ui->quad3Pitch->setText("Pitch: " + QString::number(tempObjList->at(2)->msp_attitude.pitch/10.0, 'f', 1));
        ui->quad3Yaw->setText("Yaw: " + QString::number(tempObjList->at(2)->msp_attitude.yaw, 10));
        // outer
        ui->quad3Lat->setText("Lat: " + QString::number(tempObjList->at(2)->msp_raw_gps.gpsSol_llh_lat/qPow(10.0,7), 'f', 7));
        ui->quad3Lon->setText("Lon: " + QString::number(tempObjList->at(2)->msp_raw_gps.gpsSol_llh_lon/qPow(10.0,7), 'f', 7));
        ui->quad3Alt->setText("Alt: " + QString::number(tempObjList->at(2)->msp_raw_gps.gpsSol_llh_alt/qPow(10.0,2), 'f', 2));
        ui->quad3SatNum->setText("No. Sat: " + QString::number(tempObjList->at(2)->msp_raw_gps.gpsSol_numSat, 10));
        ui->quad3FixType->setText("Fix Type: " + QString::number(tempObjList->at(2)->msp_raw_gps.gpsSol_fixType, 10));
        ui->quad3HDOP->setText("HDOP: " + QString::number(tempObjList->at(2)->msp_raw_gps.gpsSol_hdop, 10));
    }
    else  // set to unconnected color
    {
        ui->quad3ACC->setStyleSheet("QLabel {background-color : rgba(220,220,220,1);}");
    }
}


// mouse events
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        leftDown = true;
        mouseX = event->x();
        mouseY = event->y();
        if (quad1WPsEditing == true)
        {
            QPoint clickPoint(mouseX, mouseY);
            quad1WP_inside = insideWP(clickPoint, deHandle->wp_list[0]);
        }
        else if (quad2WPsEditing == true)
        {
            QPoint clickPoint(mouseX, mouseY);
            quad2WP_inside = insideWP(clickPoint, deHandle->wp_list[1]);
        }
        else if (quad3WPsEditing == true)
        {
            QPoint clickPoint(mouseX, mouseY);
            quad3WP_inside = insideWP(clickPoint, deHandle->wp_list[2]);
        }
        else
        {}
        //qDebug() << "Left Down "<<event->x()<<"  "<<event->y();
        //qDebug() << ui->maplabel->rect().contains(event->pos());
    }
    if(event->button() == Qt::RightButton)
    {
        rightDown = true;
        //qDebug() << "Right Down "<<event->x()<<"  "<<event->y() << deHandle->get_serialMode();
    }
}

void MainWindow::mouseReleaseEvent ( QMouseEvent * event )
{
    if(event->button() == Qt::LeftButton)
    {
        leftDown = false;
        //qDebug() << "Left Up";
    }
    if(event->button() == Qt::RightButton)
    {
        rightDown = false;
        //qDebug() << "Right Up";
        if (ui->maplabel->rect().contains(event->pos()))
        {
            PixelPosition tempPoint;
            tempPoint.x = event->pos().x();
            tempPoint.y = event->pos().y();
            GPSCoordinate tempCo = mapHandle->PostoGPS(tempPoint);
            if (currentTab == 1)
            {
                try
                {
                    InputDialog *inputDialog = new InputDialog;
                    inputDialog->setWindowTitle("Add WP");
                    inputDialog->latLineEdit->setText(QString::number(tempCo.lat, 'f', 7));
                    inputDialog->lonLineEdit->setText(QString::number(tempCo.lon, 'f', 7));
                    inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[0].wps.length()+1, 10));
                    int dlgCode = inputDialog->exec();

                    if (dlgCode == QDialog::Accepted)
                    {
                        //qDebug() << "Accepted";
                        // Send log info to main GUI
                        LogMessage tempLogMessage;
                        tempLogMessage.id = QString("Quad1 Mission");
                        tempLogMessage.message = QString("Mission accepted.");
                        logMessage(tempLogMessage);
                        //
                        WP tempWP = {0,"WP",0,0,0,0,0,0,0};
                        tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
                        tempWP.wp_action = inputDialog->typeCombo->currentText();
                        tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
                        tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
                        tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
                        tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
                        tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
                        tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
                        deHandle->wp_list[0].wps.append(tempWP);
                        emit updateQuad1TableViewRequest();
                    }
                    else if (dlgCode == QDialog::Rejected)
                    {
                        //qDebug() << "Rejected";
                        // Send log info to main GUI
                        LogMessage tempLogMessage;
                        tempLogMessage.id = QString("Quad1 Mission");
                        tempLogMessage.message = QString("Mission rejected.");
                        logMessage(tempLogMessage);
                        //
                    }
                }
                catch (...)
                {
                    qDebug() << "Caught exception";
                }
            }
            else if (currentTab == 2)
            {
                try
                {
                    InputDialog *inputDialog = new InputDialog;
                    inputDialog->setWindowTitle("Add WP");
                    inputDialog->latLineEdit->setText(QString::number(tempCo.lat, 'f', 7));
                    inputDialog->lonLineEdit->setText(QString::number(tempCo.lon, 'f', 7));
                    inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[1].wps.length()+1, 10));
                    int dlgCode = inputDialog->exec();

                    if (dlgCode == QDialog::Accepted)
                    {
                        //qDebug() << "Accepted";
                        // Send log info to main GUI
                        LogMessage tempLogMessage;
                        tempLogMessage.id = QString("Quad2 Mission");
                        tempLogMessage.message = QString("Mission accepted.");
                        logMessage(tempLogMessage);
                        //
                        WP tempWP = {0,"WP",0,0,0,0,0,0,0};
                        tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
                        tempWP.wp_action = inputDialog->typeCombo->currentText();
                        tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
                        tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
                        tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
                        tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
                        tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
                        tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
                        deHandle->wp_list[1].wps.append(tempWP);
                        emit updateQuad2TableViewRequest();
                    }
                    else if (dlgCode == QDialog::Rejected)
                    {
                        //qDebug() << "Rejected";
                        // Send log info to main GUI
                        LogMessage tempLogMessage;
                        tempLogMessage.id = QString("Quad2 Mission");
                        tempLogMessage.message = QString("Mission rejected.");
                        logMessage(tempLogMessage);
                        //
                    }
                }
                catch (...)
                {
                    qDebug() << "Caught exception";
                }
            }
            else if (currentTab == 3)
            {
                try
                {
                    InputDialog *inputDialog = new InputDialog;
                    inputDialog->setWindowTitle("Add WP");
                    inputDialog->latLineEdit->setText(QString::number(tempCo.lat, 'f', 7));
                    inputDialog->lonLineEdit->setText(QString::number(tempCo.lon, 'f', 7));
                    inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[2].wps.length()+1, 10));
                    int dlgCode = inputDialog->exec();

                    if (dlgCode == QDialog::Accepted)
                    {
                        //qDebug() << "Accepted";
                        // Send log info to main GUI
                        LogMessage tempLogMessage;
                        tempLogMessage.id = QString("Quad3 Mission");
                        tempLogMessage.message = QString("Mission accepted.");
                        logMessage(tempLogMessage);
                        //
                        WP tempWP = {0,"WP",0,0,0,0,0,0,0};
                        tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
                        tempWP.wp_action = inputDialog->typeCombo->currentText();
                        tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
                        tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
                        tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
                        tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
                        tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
                        tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
                        deHandle->wp_list[2].wps.append(tempWP);
                        emit updateQuad3TableViewRequest();
                    }
                    else if (dlgCode == QDialog::Rejected)
                    {
                        //qDebug() << "Rejected";
                        // Send log info to main GUI
                        LogMessage tempLogMessage;
                        tempLogMessage.id = QString("Quad3 Mission");
                        tempLogMessage.message = QString("Mission rejected.");
                        logMessage(tempLogMessage);
                        //
                    }
                }
                catch (...)
                {
                    qDebug() << "Caught exception";
                }
            }
        }

        if (ui->quad1TableView->rect().contains(event->pos()))
        {
        }
    }
}

void MainWindow::mouseMoveEvent ( QMouseEvent * event )
{
    //Show x and y coordinate values of mouse cursor here
    //this->setText("X:"+QString::number(event->x())+"-- Y:"+QString::number(event->y()));
    if ((ui->maplabel->rect().contains((event->pos()))) && leftDown == true)
    {
        if (quad1WPsEditing)
        {
            if (quad1WP_inside > -1)
            {
                //qDebug() << "Inside" << quad1WP_inside;
                PixelPosition tempPix;
                tempPix.x = event->x();
                tempPix.y = event->y();
                GPSCoordinate tempCo;
                tempCo = mapHandle->PostoGPS(tempPix);
                WP tempWP;
                tempWP = deHandle->wp_list[0].wps.at(quad1WP_inside - 1);
                tempWP.wp_lat = tempCo.lat;
                tempWP.wp_lon = tempCo.lon;
                deHandle->wp_list[0].wps.replace(quad1WP_inside - 1, tempWP);
                emit updateQuad1TableViewRequest();
            }
            else  // outside
            {
                ;
            }
        }
        else if (quad2WPsEditing)
        {
            if (quad2WP_inside > -1)
            {
                //qDebug() << "Inside" << quad2WP_inside;
                PixelPosition tempPix;
                tempPix.x = event->x();
                tempPix.y = event->y();
                GPSCoordinate tempCo;
                tempCo = mapHandle->PostoGPS(tempPix);
                WP tempWP;
                tempWP = deHandle->wp_list[1].wps.at(quad2WP_inside - 1);
                tempWP.wp_lat = tempCo.lat;
                tempWP.wp_lon = tempCo.lon;
                deHandle->wp_list[1].wps.replace(quad2WP_inside - 1, tempWP);
                emit updateQuad2TableViewRequest();
            }
            else  // outside
            {
                ;
            }
        }
        else if (quad3WPsEditing)
        {
            if (quad3WP_inside > -1)
            {
                //qDebug() << "Inside" << quad3WP_inside;
                PixelPosition tempPix;
                tempPix.x = event->x();
                tempPix.y = event->y();
                GPSCoordinate tempCo;
                tempCo = mapHandle->PostoGPS(tempPix);
                WP tempWP;
                tempWP = deHandle->wp_list[2].wps.at(quad3WP_inside - 1);
                tempWP.wp_lat = tempCo.lat;
                tempWP.wp_lon = tempCo.lon;
                deHandle->wp_list[2].wps.replace(quad3WP_inside - 1, tempWP);
                emit updateQuad3TableViewRequest();
            }
            else  // outside
            {
                ;
            }
        }
        else
        {
            int dx = event->x() - mouseX;
            int dy = event->y() - mouseY;
            mapHandle->move(dx,dy);
        }
        emit paintRequest();
    }
}


// paint functions
void MainWindow::updatePaint()
{
    QPixmap tempMap = mapHandle->retImage;

    if (tempMap.isNull())
    {
        //QString currentWorkingPath = QDir::currentPath();
        QString blackPath = resourcePath + "/res/black.jpg";
        imageHandle->load(blackPath);
    }
    else
    {
        imageHandle = &tempMap;
    }
    QPainter painter(imageHandle);

    draw(&painter);
    ui->maplabel->setPixmap(*imageHandle);
    this->repaint();
}

void MainWindow::draw(QPainter *painter)
{
    QPen Red((QColor::fromRgb(255,0,0)),10);
    QPen YellowDot((QColor::fromRgb(255,255,0)),10);
    QPen GreenLine((QColor::fromRgb(0,255,0)),1);
    QPen BlueLine((QColor::fromRgb(0,0,255)),1);
    for (int i =0; i< 3; i++)
    {
        if (deHandle->wp_list[i].wps.length() > 0)
        {
            for (int j = 0; j < deHandle->wp_list[i].wps.length(); j++)
            {
                GPSCoordinate tempCo;
                tempCo.lat = deHandle->wp_list[i].wps.at(j).wp_lat;
                tempCo.lon = deHandle->wp_list[i].wps.at(j).wp_lon;

                PixelPosition tempPix;
                tempPix = mapHandle->GPStoImagePos(tempCo);
                painter->setPen(Red);
                painter->drawEllipse(tempPix.x, tempPix.y, 3, 3);

                if (j == 0)
                {
                    painter->drawText(tempPix.x+10, tempPix.y+3, QString::number(i,10));
                }
            }
            for (int j = 0; j < deHandle->wp_list[i].wps.length(); j++)
            {
                if (j < deHandle->wp_list[i].wps.length()-1)
                {
                    WP tempWP = deHandle->wp_list[i].wps.at(j);
                    GPSCoordinate tempCo;
                    tempCo.lat = tempWP.wp_lat;
                    tempCo.lon = tempWP.wp_lon;
                    PixelPosition tempPix;
                    tempPix = mapHandle->GPStoImagePos(tempCo);

                    WP tempWP_next = deHandle->wp_list[i].wps.at(j+1);
                    GPSCoordinate tempCo_next;
                    tempCo_next.lat = tempWP_next.wp_lat;
                    tempCo_next.lon = tempWP_next.wp_lon;
                    PixelPosition tempPix_next;
                    tempPix_next = mapHandle->GPStoImagePos(tempCo_next);
                    painter->setPen(GreenLine);
                    painter->drawLine(tempPix.x, tempPix.y, tempPix_next.x, tempPix_next.y);
                }
                else
                {
                    WP tempWP = deHandle->wp_list[i].wps.at(j);
                    GPSCoordinate tempCo;
                    tempCo.lat = tempWP.wp_lat;
                    tempCo.lon = tempWP.wp_lon;
                    PixelPosition tempPix;
                    tempPix = mapHandle->GPStoImagePos(tempCo);

                    WP tempWP_next = deHandle->wp_list[i].wps.at(0);
                    GPSCoordinate tempCo_next;
                    tempCo_next.lat = tempWP_next.wp_lat;
                    tempCo_next.lon = tempWP_next.wp_lon;
                    PixelPosition tempPix_next;
                    tempPix_next = mapHandle->GPStoImagePos(tempCo_next);
                    painter->setPen(BlueLine);
                    painter->drawLine(tempPix.x, tempPix.y, tempPix_next.x, tempPix_next.y);
                }
            }
        }
    }
    if (quad1ConnSwitch == true)
    {
        PixelPosition tempPix;
        tempPix = mapHandle->GPStoImagePos(deHandle->current_gps[0]);
        painter->setPen(YellowDot);
        painter->drawEllipse(tempPix.x, tempPix.y, 5, 5);
    }
    if (quad2ConnSwitch == true)
    {
        PixelPosition tempPix;
        tempPix = mapHandle->GPStoImagePos(deHandle->current_gps[1]);
        painter->setPen(YellowDot);
        painter->drawEllipse(tempPix.x, tempPix.y, 5, 5);
    }
    if (quad3ConnSwitch == true)
    {
        PixelPosition tempPix;
        tempPix = mapHandle->GPStoImagePos(deHandle->current_gps[2]);
        painter->setPen(YellowDot);
        painter->drawEllipse(tempPix.x, tempPix.y, 5, 5);
    }
}


// initialize pages and labels
void MainWindow::InitMap()
{
    // Start of loading map
    mapHandle = new Map(30.4081580, -91.1795330, 19, 640, 640);
    connect(this, SIGNAL(paintRequest()), this, SLOT(updatePaint()));
    connect(mapHandle, &Map::logMessageRequest, this, &MainWindow::logMessage);
    ui->zoomLevelLabel->setText("Zoom Level: " + QString::number(mapHandle->_zoomlevel, 10));
    emit paintRequest();
    // End of loading map
}

void MainWindow::InitOverviewPage()
{
    // Start of Overview page
    // Set up connection ports
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        // serialPortComboBox is teleSerialPort
        ui->serialPortComboBox->addItem(port.portName());
        // auxSerialPortComboBox is rcSerialPort
        ui->auxSerialPortComboBox->addItem(port.portName());
    }
    QStringList comMethodList;
    comMethodList << "USB" << "API" << "AT";
    ui->comMethodComboBox->addItems(comMethodList);

    ui->manual1RadioButton->setEnabled(false);
    ui->manual2RadioButton->setEnabled(false);
    ui->manual3RadioButton->setEnabled(false);
    // End of Overview page

    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
             //qDebug() << address.toString();
        ui->ipAddressLabel->setText("IP: " + address.toString());
    }
}

void MainWindow::InitQuad1Page()
{
    // Start of Quad 1 page
    QStringList quad1AddrList;
    quad1AddrList << "0013A20040C14306" << "0013A20040C1430B" << "0013A20040C1430F";
    ui->quad1AddressComboBox->addItems(quad1AddrList);

    // Start of Quad 1 table
    model1 = new QStandardItemModel(30,7,this); //2 Rows and 3 Columns
    //model->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    model1->setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    model1->setHorizontalHeaderItem(1, new QStandardItem(QString("Lat")));
    model1->setHorizontalHeaderItem(2, new QStandardItem(QString("Lon")));
    model1->setHorizontalHeaderItem(3, new QStandardItem(QString("Alt")));
    model1->setHorizontalHeaderItem(4, new QStandardItem(QString("P1")));
    model1->setHorizontalHeaderItem(5, new QStandardItem(QString("P2")));
    model1->setHorizontalHeaderItem(6, new QStandardItem(QString("P3")));

    ui->quad1TableView->setModel(model1);
    ui->quad1TableView->setColumnWidth(0,60);
    ui->quad1TableView->setColumnWidth(1,80);
    ui->quad1TableView->setColumnWidth(2,80);
    ui->quad1TableView->setColumnWidth(3,70);
    ui->quad1TableView->setColumnWidth(4,50);
    ui->quad1TableView->setColumnWidth(5,50);
    ui->quad1TableView->setColumnWidth(6,50);

    ui->quad1TableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->quad1TableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(this, SIGNAL(updateQuad1TableViewRequest()), this, SLOT(updateQuad1TableView()) );
    // End of Quad 1 table

    quad1WPsEditing = false;
    quad1WP_inside = -1;
    quad1ConnSwitch = false;
    // End of Quad 1 page
}

void MainWindow::InitQuad2Page()
{
    // Start of Quad 2 page
    QStringList quad2AddrList;
    quad2AddrList << "0013A20040C14306" << "0013A20040C1430B" << "0013A20040C1430F";
    ui->quad2AddressComboBox->addItems(quad2AddrList);

    // Start of Quad 2 table
    model2 = new QStandardItemModel(30,7,this); //2 Rows and 3 Columns
    //model->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    model2->setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    model2->setHorizontalHeaderItem(1, new QStandardItem(QString("Lat")));
    model2->setHorizontalHeaderItem(2, new QStandardItem(QString("Lon")));
    model2->setHorizontalHeaderItem(3, new QStandardItem(QString("Alt")));
    model2->setHorizontalHeaderItem(4, new QStandardItem(QString("P1")));
    model2->setHorizontalHeaderItem(5, new QStandardItem(QString("P2")));
    model2->setHorizontalHeaderItem(6, new QStandardItem(QString("P3")));

    ui->quad2TableView->setModel(model2);
    ui->quad2TableView->setColumnWidth(0,60);
    ui->quad2TableView->setColumnWidth(1,80);
    ui->quad2TableView->setColumnWidth(2,80);
    ui->quad2TableView->setColumnWidth(3,70);
    ui->quad2TableView->setColumnWidth(4,50);
    ui->quad2TableView->setColumnWidth(5,50);
    ui->quad2TableView->setColumnWidth(6,50);

    ui->quad2TableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->quad2TableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(this, SIGNAL(updateQuad2TableViewRequest()), this, SLOT(updateQuad2TableView()) );
    // End of Quad 2 table

    quad2WPsEditing = false;
    quad2WP_inside = -1;
    quad2ConnSwitch = false;
    // End of Quad 2 page
}

void MainWindow::InitQuad3Page()
{
    // Start of Quad 3 page
    QStringList quad3AddrList;
    quad3AddrList << "0013A20040C14306" << "0013A20040C1430B" << "0013A20040C1430F";
    ui->quad3AddressComboBox->addItems(quad3AddrList);

    // Start of Quad 3 table
    model3 = new QStandardItemModel(30,7,this); //2 Rows and 3 Columns
    //model->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    model3->setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    model3->setHorizontalHeaderItem(1, new QStandardItem(QString("Lat")));
    model3->setHorizontalHeaderItem(2, new QStandardItem(QString("Lon")));
    model3->setHorizontalHeaderItem(3, new QStandardItem(QString("Alt")));
    model3->setHorizontalHeaderItem(4, new QStandardItem(QString("P1")));
    model3->setHorizontalHeaderItem(5, new QStandardItem(QString("P2")));
    model3->setHorizontalHeaderItem(6, new QStandardItem(QString("P3")));

    ui->quad3TableView->setModel(model3);
    ui->quad3TableView->setColumnWidth(0,60);
    ui->quad3TableView->setColumnWidth(1,80);
    ui->quad3TableView->setColumnWidth(2,80);
    ui->quad3TableView->setColumnWidth(3,70);
    ui->quad3TableView->setColumnWidth(4,50);
    ui->quad3TableView->setColumnWidth(5,50);
    ui->quad3TableView->setColumnWidth(6,50);

    ui->quad3TableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->quad3TableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(this, SIGNAL(updateQuad3TableViewRequest()), this, SLOT(updateQuad3TableView()) );
    // End of Quad 3 table

    quad3WPsEditing = false;
    quad3WP_inside = -1;
    quad3ConnSwitch = false;
    // End of Quad 3 page
}

void MainWindow::on_refreshConnButton_clicked()
{
    // Set up connection ports
    // Clear old connection ports
    ui->serialPortComboBox->clear();
    ui->auxSerialPortComboBox->clear();
    // Set new connection ports
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        ui->serialPortComboBox->addItem(port.portName());
        ui->auxSerialPortComboBox->addItem(port.portName());
    }
}

void MainWindow::on_ipRefreshButton_clicked()
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
             //qDebug() << address.toString();
        ui->ipAddressLabel->setText("IP: " + address.toString());
    }
}


void MainWindow::resetLabels()
{
    ui->quad1ConnectButton->setText("Connect");
    quad1ConnSwitch = false;
    deHandle->teleAddressList[0] = "";
    ui->quad1ConnectionStatusOverview->setText("NO CON");
    ui->quad1ConnectionStatus->setText("NO CON");
    ui->quad1VoltageOverview->setText("0.0 V");
    ui->quad1Voltage->setText("0.0 V");

    ui->quad2ConnectButton->setText("Connect");
    quad2ConnSwitch = false;
    deHandle->teleAddressList[1] = "";
    ui->quad2ConnectionStatusOverview->setText("NO CON");
    ui->quad2ConnectionStatus->setText("NO CON");
    ui->quad2VoltageOverview->setText("0.0 V");
    ui->quad2Voltage->setText("0.0 V");

    ui->quad3ConnectButton->setText("Connect");
    quad3ConnSwitch = false;
    deHandle->teleAddressList[2] = "";
    ui->quad3ConnectionStatusOverview->setText("NO CON");
    ui->quad3ConnectionStatus->setText("NO CON");
    ui->quad3VoltageOverview->setText("0.0 V");
    ui->quad3Voltage->setText("0.0 V");

    // Overview page
    ui->quad1ACCOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1MAGOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1BAROOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1SONAROverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1GPSOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1PITOTOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1HWOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad1ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1POSModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1VoltageOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad2ACCOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2MAGOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2BAROOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2SONAROverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2GPSOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2PITOTOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2HWOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad2ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2POSModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2VoltageOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad3ACCOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3MAGOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3BAROOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3SONAROverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3GPSOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3PITOTOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3HWOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad3ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3ARMModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3LEVELModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3ALTModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3POSModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3NAVModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3RTHModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3FAILModeOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3VoltageOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    // Quad 1 page
    ui->quad1ACC->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1MAG->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1BARO->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1SONAR->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1GPS->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1PITOT->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1HW->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad1ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1ARMMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1LEVELMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1ALTMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1POSMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1NAVMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1RTHMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1FAILMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad1Voltage->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    // Quad 2 page
    ui->quad2ACC->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2MAG->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2BARO->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2SONAR->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2GPS->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2PITOT->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2HW->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad2ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2ARMMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2LEVELMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2ALTMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2POSMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2NAVMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2RTHMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2FAILMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad2Voltage->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    // Quad 3 page
    ui->quad3ACC->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3MAG->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3BARO->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3SONAR->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3GPS->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3PITOT->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3HW->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");

    ui->quad3ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3ARMMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3LEVELMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3ALTMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3POSMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3NAVMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3RTHMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3FAILMode->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    ui->quad3Voltage->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
}


// overview page control buttons
void MainWindow::on_serialConnectButton_clicked()
{
    QString serialConnectButtonText;
    serialConnectButtonText = ui->serialConnectButton->text();
    if (serialConnectButtonText == "Connect")
    {
        if ( deHandle->teleAddressList[0] == "" && deHandle->teleAddressList[1] == "" && deHandle->teleAddressList[2] == "")
        {
            QMessageBox msgBox;
            msgBox.setText("No agent is connected.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
        else
        {
            deHandle->teleSerialPortName = ui->serialPortComboBox->currentText();
            deHandle->teleConnectionMethod = ui->comMethodComboBox->currentText();
            deHandle->set_teleSerialOn(true);
        }


        //tempLogMessage.id = "MainWindow";
        //tempLogMessage.message = "Connected";
        //logMessage(tempLogMessage);
        //qDebug() << ui->comMethodComboBox->currentText();
    }
    else if (serialConnectButtonText == "Disconnect")
    {
        deHandle->set_teleSerialOn(false);
    }
}

void MainWindow::updateSerialInfo(bool value)
{
    //qDebug()<< "Data received" << value;
    if (value == true)
    {
        ui->serialConnectButton->setText("Disconnect");
        //qDebug() << "Connected";
        // Send log info to main GUI
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("Tele Serial");
        tempLogMessage.message = QString("Connected.");
        logMessage(tempLogMessage);
        //
        this->repaint();
    }
    else if(value == false)
    {
        ui->serialConnectButton->setText("Connect");
        resetLabels();
        //qDebug() << "Disconnected";
        // Send log info to main GUI
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("Tele Serial");
        tempLogMessage.message = QString("Disconnected.");
        logMessage(tempLogMessage);
        //
        this->repaint();
    }
}

void MainWindow::on_auxSerialConnectButton_clicked()
{
    QString auxSerialConnectButtonText;
    auxSerialConnectButtonText = ui->auxSerialConnectButton->text();
    if (auxSerialConnectButtonText == "Connect")
    {
        deHandle->rcSerialPortName = ui->auxSerialPortComboBox->currentText();
        deHandle->rcConnectionMethod = "AT";
        deHandle->set_rcSerialOn(true);
        ui->auxSerialConnectButton->setText("Disconnect");
        //qDebug() << "Aux connected" << deHandle->auxSerialPortName;
        // Send to main GUI log window
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("Main Window");
        tempLogMessage.message = QString("RC connected: "+ deHandle->rcSerialPortName);
        logMessage(tempLogMessage);
        //
        ui->manual1RadioButton->setEnabled(true);
        ui->manual2RadioButton->setEnabled(true);
        ui->manual3RadioButton->setEnabled(true);
        this->repaint();
    }
    else if (auxSerialConnectButtonText == "Disconnect")
    {
        deHandle->set_rcSerialOn(false);
        ui->auxSerialConnectButton->setText("Connect");
        //qDebug() << "Aux disconnected";
        // Send to main GUI log window
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("Main Window");
        tempLogMessage.message = QString("RC disconnected.");
        logMessage(tempLogMessage);
        //
        ui->manual1RadioButton->setEnabled(false);
        ui->manual2RadioButton->setEnabled(false);
        ui->manual3RadioButton->setEnabled(false);
        this->repaint();
    }
}


void MainWindow::on_manualOffRadioButton_clicked()
{
    deHandle->set_manualMode(0);
}

void MainWindow::on_manual1RadioButton_clicked()
{
    if (deHandle->get_rcSerialOn() == true)
    {
        deHandle->set_manualMode(1);
    }
    else
    {
        ui->manual1RadioButton->setChecked(false);
        ui->manualOffRadioButton->setChecked(true);
    }
    //deHandle->set_manualMode(1);
}

void MainWindow::on_manual2RadioButton_clicked()
{
    if (deHandle->get_rcSerialOn() == true)
    {
        deHandle->set_manualMode(2);
    }
    else
    {
        ui->manual2RadioButton->setChecked(false);
        ui->manualOffRadioButton->setChecked(true);
    }
    //deHandle->set_manualMode(2);
}

void MainWindow::on_manual3RadioButton_clicked()
{
    if (deHandle->get_rcSerialOn() == true)
    {
        deHandle->set_manualMode(3);
    }
    else
    {
        ui->manual3RadioButton->setChecked(false);
        ui->manualOffRadioButton->setChecked(true);
    }
    //deHandle->set_manualMode(3);
}


void MainWindow::on_voiceButton_clicked()
{

}

void MainWindow::on_radioButton_clicked()  // more work on different cases needed
{
    if (ui->radioButton->text() == "ON")
    {
        if (deHandle->get_rcSerialOn() == true)  // radioButton used to be working with main serial, but assigned to aux serial now.
        {
            int tempStatus = deHandle->get_rcMode();
            int newStatus = (tempStatus | (1 << 0));
            ui->radioButton->setText("OFF");
            deHandle->set_rcMode(newStatus);
        }
        else if (deHandle->get_rcSerialOn() == false)
        {
            QMessageBox msgBox;
            msgBox.setText("Serial Communication Port is not open.\nCannot use radio services.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
    else if (ui->radioButton->text() == "OFF")
    {
        int tempStatus = deHandle->get_rcMode();
        //int newStatus = (tempStatus & (0xFFFF ^ (1 << 0)));
        int newStatus = (tempStatus & (0x0000));
        ui->radioButton->setText("ON");
        ui->armAllButton->setText("ARM");
        ui->arm1Button->setText("ARM");
        ui->nav1Button->setText("NAV");
        ui->rth1Button->setText("RTH");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_armAllButton_clicked()  // to do
{
    if (ui->armAllButton->text() == "ARM")
    {
        int tempStatus = deHandle->get_rcMode();
        //int tempStatus = deHandle->get_serialMode();
        int newStatus = (tempStatus | (1 << 1) | (1 << 2) | (1 << 3));
        ui->armAllButton->setText("DISARM");
        ui->arm1Button->setText("DISARM");
        ui->arm2Button->setText("DISARM");
        ui->arm3Button->setText("DISARM");
        deHandle->set_rcMode(newStatus);
        //deHandle->set_serialMode(newStatus);
    }
    else if (ui->armAllButton->text() == "DISARM")
    {
        int tempStatus = deHandle->get_rcMode();
        //int tempStatus = deHandle->get_serialMode();
        int newStatus = (tempStatus & (0xFFFF ^ ((1 << 1) | (1 << 2) | (1 << 3))) );
        ui->armAllButton->setText("ARM");
        ui->arm1Button->setText("ARM");
        ui->arm2Button->setText("ARM");
        ui->arm3Button->setText("ARM");
        deHandle->set_rcMode(newStatus);
        //deHandle->set_serialMode(newStatus);
    }
}

void MainWindow::on_navAllButton_clicked()  // to do
{
    if (ui->navAllButton->text() == "NAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus1 = (tempStatus | (1 << 4) | (1 << 5) | (1 << 6));
        int newStatus = (newStatus1 & (0xFFFF ^ ((1 << 7) | (1 << 8) | (1 << 9))) );
        ui->navAllButton->setText("DISNAV");
        ui->nav1Button->setText("DISNAV");
        ui->nav2Button->setText("DISNAV");
        ui->nav3Button->setText("DISNAV");
        ui->rthAllButton->setText("RTH");
        ui->rth1Button->setText("RTH");
        ui->rth2Button->setText("RTH");
        ui->rth3Button->setText("RTH");
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->navAllButton->text() == "DISNAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ ((1 << 4) | (1 << 5) | (1 << 6))) );
        ui->navAllButton->setText("NAV");
        ui->nav1Button->setText("NAV");
        ui->nav2Button->setText("NAV");
        ui->nav3Button->setText("NAV");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_rthAllButton_clicked()
{
    if (ui->rthAllButton->text() == "RTH")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus1 = (tempStatus | (1 << 7) | (1 << 8) | (1 << 9));
        int newStatus = (newStatus1 & (0xFFFF ^ ((1 << 4) | (1 << 5) | (1 << 6))) );
        ui->rthAllButton->setText("DISRTH");
        ui->rth1Button->setText("DISRTH");
        ui->rth2Button->setText("DISRTH");
        ui->rth3Button->setText("DISRTH");
        ui->navAllButton->setText("NAV");
        ui->nav1Button->setText("NAV");
        ui->nav2Button->setText("NAV");
        ui->nav3Button->setText("NAV");
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->rthAllButton->text() == "DISRTH")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ ((1 << 7) | (1 << 8) | (1 << 9))) );
        ui->rthAllButton->setText("RTH");
        ui->rth1Button->setText("RTH");
        ui->rth2Button->setText("RTH");
        ui->rth3Button->setText("RTH");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_arm1Button_clicked()
{
    if (ui->arm1Button->text() == "ARM")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 1));
        ui->arm1Button->setText("DISARM");
        if ((newStatus & 0x000E) == 14)
        {
            ui->armAllButton->setText("DISARM");
        }
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->arm1Button->text() == "DISARM")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 1)));
        newStatus = (newStatus & (0xFFFF ^ (1 << 4)));
        newStatus = (newStatus & (0xFFFF ^ (1 << 7)));
        ui->armAllButton->setText("ARM");
        ui->arm1Button->setText("ARM");
        ui->navAllButton->setText("NAV");
        ui->nav1Button->setText("NAV");
        ui->rthAllButton->setText("RTH");
        ui->rth1Button->setText("RTH");
        deHandle->set_rcMode(newStatus);
    }

}

void MainWindow::on_arm2Button_clicked()
{
    if (ui->arm2Button->text() == "ARM")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 2));
        ui->arm2Button->setText("DISARM");
        if ((newStatus & 0x000E) == 14)
        {
            ui->armAllButton->setText("DISARM");
        }
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->arm2Button->text() == "DISARM")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 2)));
        ui->armAllButton->setText("ARM");
        ui->arm2Button->setText("ARM");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_arm3Button_clicked()
{
    if (ui->arm3Button->text() == "ARM")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 3));
        ui->arm3Button->setText("DISARM");
        if ((newStatus & 0x000E) == 14)
        {
            ui->armAllButton->setText("DISARM");
        }
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->arm3Button->text() == "DISARM")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 3)));
        ui->armAllButton->setText("ARM");
        ui->arm3Button->setText("ARM");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_nav1Button_clicked()
{
    if (ui->nav1Button->text() == "NAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 4));
        newStatus = (newStatus & (0xFFFF ^ (1 << 7)));
        ui->nav1Button->setText("DISNAV");
        ui->rth1Button->setText("RTH");
        if ((newStatus & 0x0070) == 112)
        {
            ui->navAllButton->setText("DISNAV");
        }
        //qDebug() << "Here" << newStatus;
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->nav1Button->text() == "DISNAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 4)));
        ui->navAllButton->setText("NAV");
        ui->nav1Button->setText("NAV");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_nav2Button_clicked()
{
    if (ui->nav2Button->text() == "NAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 5));
        ui->nav2Button->setText("DISNAV");
        if ((newStatus & 0x0070) == 112)
        {
            ui->navAllButton->setText("DISNAV");
        }
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->nav2Button->text() == "DISNAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 5)));
        ui->navAllButton->setText("NAV");
        ui->nav2Button->setText("NAV");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_nav3Button_clicked()
{
    if (ui->nav3Button->text() == "NAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 6));
        ui->nav3Button->setText("DISNAV");
        if ((newStatus & 0x0070) == 112)
        {
            ui->navAllButton->setText("DISNAV");
        }
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->nav3Button->text() == "DISNAV")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 6)));
        ui->navAllButton->setText("NAV");
        ui->nav3Button->setText("NAV");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_rth1Button_clicked()
{
    if (ui->rth1Button->text() == "RTH")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus | (1 << 7));
        newStatus = (newStatus & (0xFFFF ^ (1 << 4)));
        ui->rth1Button->setText("DISRTH");
        ui->nav1Button->setText("NAV");
        if ((newStatus & 0x0380) == 896)
        {
            ui->rthAllButton->setText("DISRTH");
        }
        deHandle->set_rcMode(newStatus);
    }
    else if (ui->rth1Button->text() == "DISRTH")
    {
        int tempStatus = deHandle->get_rcMode();
        int newStatus = (tempStatus & (0xFFFF ^ (1 << 7)));
        ui->rthAllButton->setText("RTH");
        ui->rth1Button->setText("RTH");
        deHandle->set_rcMode(newStatus);
    }
}

void MainWindow::on_rth2Button_clicked()
{

}

void MainWindow::on_rth3Button_clicked()
{

}


// map control functions
void MainWindow::on_incZoomButton_clicked()
{
    mapHandle->zoom(1);
    ui->zoomLevelLabel->setText("Zoom Level: " + QString::number(mapHandle->_zoomlevel, 10));
    emit paintRequest();
}

void MainWindow::on_decZoomButton_clicked()
{
    mapHandle->zoom(-1);
    ui->zoomLevelLabel->setText("Zoom Level: " + QString::number(mapHandle->_zoomlevel, 10));
    emit paintRequest();
}

void MainWindow::on_returnHomeButton_clicked()
{
    mapHandle->return_origin();
    ui->zoomLevelLabel->setText("Zoom Level: " + QString::number(mapHandle->_zoomlevel, 10));
    emit paintRequest();
}

void MainWindow::on_autoZoomButton_clicked()
{
    _auto_zoom_and_center();
    ui->zoomLevelLabel->setText("Zoom Level: " + QString::number(mapHandle->_zoomlevel, 10));
    emit paintRequest();
}

void MainWindow::_auto_zoom_and_center()
{
    //QList<GPSCoordinate> gpsCoList;
    QList<double> latList;
    QList<double> lonList;
    // for i = 1, 2, 3 means for all 3 agents.
    for (int i=0; i < 3; i++)
    {
        double cur_gps_lat = deHandle->current_gps[i].lat;
        double cur_gps_lon = deHandle->current_gps[i].lon;
        // qSqrt() > 10.0 has no real meaning, only used to rule out
        //    the initial gps coordinates with 0.0, 0.0 as lat and lon.
        //    Since if no valid reading sending back, cur_gps will be
        //    0.0 and 0.0. So this if sentence is used to rule out this
        //    situation. It needs to be changed if the real gps is around
        //    0.0 and 0.0.
        if (qSqrt(cur_gps_lat*cur_gps_lat+cur_gps_lon*cur_gps_lon) > 10.0)
        {
            latList.append(cur_gps_lat);
            lonList.append(cur_gps_lon);
        }

        if (deHandle->wp_list[i].wps.length() > 0)
        {
            foreach (WP tempWP, deHandle->wp_list[i].wps) {
                latList.append(tempWP.wp_lat);
                lonList.append(tempWP.wp_lon);
            }
        }
    }

    // only repaint if there are more than 2 points, as 2 different
    //    points are the minimum requirement for auto zoom.
    if( latList.length() >= 2)
    {
        qSort(latList.begin(), latList.end());
        qSort(lonList.begin(), lonList.end());
        double max_lat = latList.last();
        double min_lat = latList.first();
        double max_lon = lonList.last();
        double min_lon = lonList.first();

        AutoZoomGeoMapInfo tempInfo = mapHandle->_find_zoomlevel(min_lat, max_lat, min_lon, max_lon);
        mapHandle->_reload(tempInfo);
        emit paintRequest();
    }
    else if (latList.length() == 1)
    {
        ;
        // to do. if only one point, then center with that point.
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    currentTab = index;
    if (deHandle->get_teleSerialOn() == true)
    {
        deHandle->set_teleMode(index);
    }
}

void MainWindow::on_serialPortComboBox_currentIndexChanged(int index)
{
    // Send log info to main GUI
    LogMessage tempLogMessage;
    tempLogMessage.id = QString("Main Serial");
    tempLogMessage.message = QString::number(index, 10);
    logMessage(tempLogMessage);
    //
}

Mission_list MainWindow::WPsToMissions(WP_list tempWPList)
{
    Mission_list tempMissionList;
    foreach (WP tempWP, tempWPList.wps) {
        Mission tempMission;
        tempMission.wp_no = tempWP.wp_no;
        if (tempWP.wp_action == "WP")
        {
            tempMission.wp_action = 1;
        }
        else if (tempWP.wp_action == "RTH")
        {
            tempMission.wp_action = 4;
        }
        else if (tempWP.wp_action == "POS_UNLIM")
        {
            tempMission.wp_action = 2;
        }
        else if (tempWP.wp_action == "POS_LIM")
        {
            tempMission.wp_action = 3;
        }
        tempMission.wp_lat = int(tempWP.wp_lat * qPow(10, 7));
        tempMission.wp_lon = int(tempWP.wp_lon * qPow(10, 7));
        tempMission.wp_alt = int(tempWP.wp_alt);
        tempMission.wp_p1 = int(tempWP.wp_p1);
        tempMission.wp_p2 = int(tempWP.wp_p2);
        tempMission.wp_p3 = int(tempWP.wp_p3);
        tempMission.wp_flag = uint(0);
        if (tempMission.wp_no == tempWPList.wps.length())
        {
            tempMission.wp_flag = uint(165);
        }
        tempMissionList.missions.append(tempMission);
    }
    return tempMissionList;
}

int MainWindow::insideWP(QPoint po,WP_list tempWPList)
{
    int statusInd = -1;
    foreach (WP tempWP, tempWPList.wps) {
        GPSCoordinate tempCo;
        tempCo.lat = tempWP.wp_lat;
        tempCo.lon = tempWP.wp_lon;

        PixelPosition tempPix;
        tempPix = mapHandle->GPStoImagePos(tempCo);

        if (qSqrt((po.x() - tempPix.x)*(po.x() - tempPix.x) + (po.y() - tempPix.y)*(po.y() - tempPix.y)) < 10)
        {
            statusInd = tempWP.wp_no;
            return statusInd;
        }
    }
    return statusInd;
}

// quad 1 functions

void MainWindow::on_quad1TableView_clicked(const QModelIndex &index)
{
    // Currently not used, might be used in the future.
    //qDebug() << "single clicked" << index.row();
}

void MainWindow::on_quad1TableView_doubleClicked(const QModelIndex &index)
{
    // When double click one cell, choose this line and pop up
    //    a dialog to edit the mission in this line.
    //    If this line is empty, and this line is the first
    //    empty line, then pop up a dialog to add new mission.
    //qDebug() << "double clicked" << index.row();
    try
    {
        if (index.row() == deHandle->wp_list[0].wps.length())  // Add
        {
            InputDialog *inputDialog = new InputDialog;
            inputDialog->setWindowTitle("Add WP");
            inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[0].wps.length()+1, 10));
            int dlgCode = inputDialog->exec();

            if (dlgCode == QDialog::Accepted)
            {
                //qDebug() << "Accepted";
                // Send log info to main GUI
                LogMessage tempLogMessage;
                tempLogMessage.id = QString("Quad1 Mission");
                tempLogMessage.message = QString("Mission accepted.");
                logMessage(tempLogMessage);
                //
                WP tempWP = {0,"WP",0,0,0,0,0,0,0};
                tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
                tempWP.wp_action = inputDialog->typeCombo->currentText();
                tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
                tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
                tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
                tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
                tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
                tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
                deHandle->wp_list[0].wps.append(tempWP);
                emit updateQuad1TableViewRequest();
            }
            else if (dlgCode == QDialog::Rejected)
            {
                //qDebug() << "Rejected";
                // Send log info to main GUI
                LogMessage tempLogMessage;
                tempLogMessage.id = QString("Quad1 Mission");
                tempLogMessage.message = QString("Mission rejected.");
                logMessage(tempLogMessage);
                //
            }
        }
        else if (index.row() < deHandle->wp_list[0].wps.length())  // Edit
        {
            InputDialog *inputDialog = new InputDialog;
            inputDialog->setWindowTitle("Edit WP");
            WP tempWP;
            int index = ui->quad1TableView->currentIndex().row();
            tempWP = deHandle->wp_list[0].wps.at(index);
            inputDialog->idLineEdit->setText(QString::number(index+1, 10));
            if (tempWP.wp_action == "WP")
            {
                inputDialog->typeCombo->setCurrentIndex(0);
            }
            else if (tempWP.wp_action == "RTH")
            {
                inputDialog->typeCombo->setCurrentIndex(1);
            }
            else if (tempWP.wp_action == "POS_UNLIM")
            {
                inputDialog->typeCombo->setCurrentIndex(2);
            }
            else if (tempWP.wp_action == "POS_LIM")
            {
                inputDialog->typeCombo->setCurrentIndex(3);
            }

            inputDialog->latLineEdit->setText(QString::number(tempWP.wp_lat, 'f', 7));
            inputDialog->lonLineEdit->setText(QString::number(tempWP.wp_lon, 'f', 7));
            inputDialog->altLineEdit->setText(QString::number(tempWP.wp_alt, 'f', 3));
            inputDialog->p1LineEdit->setText(QString::number(tempWP.wp_p1, 10));
            inputDialog->p2LineEdit->setText(QString::number(tempWP.wp_p2, 10));
            inputDialog->p3LineEdit->setText(QString::number(tempWP.wp_p3, 10));
            int dlgCode = inputDialog->exec();

            if (dlgCode == QDialog::Accepted)
            {
                //qDebug() << "Accepted";
                // Send log info to main GUI
                LogMessage tempLogMessage;
                tempLogMessage.id = QString("Quad1 Mission");
                tempLogMessage.message = QString("Mission accepted.");
                logMessage(tempLogMessage);
                //
                WP tempWP = {0,"WP",0,0,0,0,0,0,0};
                tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
                tempWP.wp_action = inputDialog->typeCombo->currentText();
                tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
                tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
                tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
                tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
                tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
                tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
                deHandle->wp_list[0].wps.replace(index, tempWP);
                emit updateQuad1TableViewRequest();
            }
            else if (dlgCode == QDialog::Rejected)
            {
                //qDebug() << "Rejected";
                // Send log info to main GUI
                LogMessage tempLogMessage;
                tempLogMessage.id = QString("Quad1 Mission");
                tempLogMessage.message = QString("Mission rejected.");
                logMessage(tempLogMessage);
                //
            }
        }
        else if (index.row() > deHandle->wp_list[0].wps.length())  // Do nothing
        {}
        else  // Do nothing
        {}

    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::on_quad1TableView_customContextMenuRequested(const QPoint &pos)
{
    QAction actionAdd("Add", this);
    QAction actionEdit("Edit", this);
    QAction actionDelete("Delete", this);
    QAction actionClear("Clear", this);
    QAction actionSave("Save", this);
    QAction actionLoad("Load", this);
    connect(&actionAdd, SIGNAL(triggered()), this, SLOT(Quad1AddWP()));
    connect(&actionEdit, SIGNAL(triggered()), this, SLOT(Quad1EditWP()));
    connect(&actionDelete, SIGNAL(triggered()), this, SLOT(Quad1DeleteWP()));
    connect(&actionClear, SIGNAL(triggered()), this, SLOT(Quad1ClearWP()));
    connect(&actionSave, SIGNAL(triggered()), this, SLOT(Quad1SaveWP()));
    connect(&actionLoad, SIGNAL(triggered()), this, SLOT(Quad1LoadWP()));
    QMenu myMenu;
    myMenu.addAction(&actionAdd);
    myMenu.addAction(&actionEdit);
    myMenu.addAction(&actionDelete);
    myMenu.addAction(&actionClear);
    myMenu.addAction(&actionSave);
    myMenu.addAction(&actionLoad);

    if (ui->quad1TableView->indexAt(pos).row() >= deHandle->wp_list[0].wps.length()) // empty line
    {
        actionEdit.setEnabled(false);
        actionDelete.setEnabled(false);
        if (deHandle->wp_list[0].wps.length() == 0)
        {
            actionClear.setEnabled(false);
            actionSave.setEnabled(false);
        }
    }
    else if (ui->quad1TableView->indexAt(pos).row() < deHandle->wp_list[0].wps.length()) // not empty line
    {
        actionAdd.setEnabled(false);
    }
    myMenu.exec(ui->quad1TableView->viewport()->mapToGlobal(pos));
}

void MainWindow::Quad1AddWP()
{
    try
    {
        InputDialog *inputDialog = new InputDialog;
        inputDialog->setWindowTitle("Add WP");
        inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[0].wps.length()+1, 10));
        int dlgCode = inputDialog->exec();

        if (dlgCode == QDialog::Accepted)
        {
            //qDebug() << "Accepted";
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Quad1 Mission");
            tempLogMessage.message = QString("Mission accepted.");
            logMessage(tempLogMessage);
            //
            WP tempWP = {0,"WP",0,0,0,0,0,0,0};
            tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
            tempWP.wp_action = inputDialog->typeCombo->currentText();
            tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
            tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
            tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
            tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
            tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
            tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
            deHandle->wp_list[0].wps.append(tempWP);
            emit updateQuad1TableViewRequest();
        }
        else if (dlgCode == QDialog::Rejected)
        {
            //qDebug() << "Rejected";
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Quad1 Mission");
            tempLogMessage.message = QString("Mission rejected.");
            logMessage(tempLogMessage);
            //
        }
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::Quad1EditWP()
{
    try
    {
        InputDialog *inputDialog = new InputDialog;
        inputDialog->setWindowTitle("Edit WP");
        WP tempWP;
        int index = ui->quad1TableView->currentIndex().row();
        tempWP = deHandle->wp_list[0].wps.at(index);
        inputDialog->idLineEdit->setText(QString::number(index+1, 10));
        if (tempWP.wp_action == "WP")
        {
            inputDialog->typeCombo->setCurrentIndex(0);
        }
        else if (tempWP.wp_action == "RTH")
        {
            inputDialog->typeCombo->setCurrentIndex(1);
        }
        else if (tempWP.wp_action == "POS_UNLIM")
        {
            inputDialog->typeCombo->setCurrentIndex(2);
        }
        else if (tempWP.wp_action == "POS_LIM")
        {
            inputDialog->typeCombo->setCurrentIndex(3);
        }

        inputDialog->latLineEdit->setText(QString::number(tempWP.wp_lat, 'f', 7));
        inputDialog->lonLineEdit->setText(QString::number(tempWP.wp_lon, 'f', 7));
        inputDialog->altLineEdit->setText(QString::number(tempWP.wp_alt, 'f', 3));
        inputDialog->p1LineEdit->setText(QString::number(tempWP.wp_p1, 10));
        inputDialog->p2LineEdit->setText(QString::number(tempWP.wp_p2, 10));
        inputDialog->p3LineEdit->setText(QString::number(tempWP.wp_p3, 10));
        int dlgCode = inputDialog->exec();

        if (dlgCode == QDialog::Accepted)
        {
            //qDebug() << "Accepted";
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Quad2 Mission");
            tempLogMessage.message = QString("Mission accepted.");
            logMessage(tempLogMessage);
            //
            WP tempWP = {0,"WP",0,0,0,0,0,0,0};
            tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
            tempWP.wp_action = inputDialog->typeCombo->currentText();
            tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
            tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
            tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
            tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
            tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
            tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
            deHandle->wp_list[0].wps.replace(index, tempWP);
            emit updateQuad1TableViewRequest();
        }
        else if (dlgCode == QDialog::Rejected)
        {
            //qDebug() << "Rejected";
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Quad2 Mission");
            tempLogMessage.message = QString("Mission rejected.");
            logMessage(tempLogMessage);
            //
        }
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::Quad1DeleteWP()
{
    int index = ui->quad1TableView->currentIndex().row();
    deHandle->wp_list[0].wps.removeAt(index);
    if (index < deHandle->wp_list[0].wps.length())
    {
        for (int i = index; i< deHandle->wp_list[0].wps.length(); i++)
        {
            WP tempWP = deHandle->wp_list[0].wps.at(i);
            tempWP.wp_no = tempWP.wp_no - 1;
            deHandle->wp_list[0].wps.replace(i, tempWP);
        }
    }

    emit updateQuad1TableViewRequest();
}

void MainWindow::Quad1ClearWP() {}

void MainWindow::Quad1SaveWP()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Waypoints to File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        //qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //qDebug() << "Cannot open file";
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Quad1 Mission Save");
            tempLogMessage.message = QString("Cannot open file.");
            logMessage(tempLogMessage);
            //
        }

        QTextStream out(&file);
        foreach (WP tempWP, deHandle->wp_list[0].wps) {
            QString outputStr = "";
            outputStr = outputStr + QString::number(tempWP.wp_no, 10) + ",";
            outputStr = outputStr + tempWP.wp_action + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lat, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lon, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_alt, 'f', 2) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p1, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p2, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p3, 10) + "\n";
            out << outputStr;
        }
        file.close();
    }
}

void MainWindow::Quad1LoadWP()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose Waypoint File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        //qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            //qDebug() << "Cannot open file";
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Quad1 Mission Load");
            tempLogMessage.message = QString("Cannot open file.");
            logMessage(tempLogMessage);
            //
        }
        QString inputStr;
        QTextStream in(&file);
        inputStr = in.readAll();
        file.close();
        QStringList tempSL = inputStr.split('\n');
        QList<WP> wps;
        foreach (QString tempStr, tempSL) {
            if (tempStr.length() > 0)
            {
                QStringList fields = tempStr.split(',');
                WP tempWP;
                tempWP.wp_no = fields.at(0).toUInt();
                tempWP.wp_action = fields.at(1);
                tempWP.wp_lat = fields.at(2).toDouble();
                tempWP.wp_lon = fields.at(3).toDouble();
                tempWP.wp_alt = fields.at(4).toFloat();
                tempWP.wp_p1 = fields.at(5).toInt();
                tempWP.wp_p2 = fields.at(6).toInt();
                tempWP.wp_p3 = fields.at(7).toInt();
                wps.append(tempWP);
            }
        }
        deHandle->wp_list[0].wps = wps;
        emit updateQuad1TableViewRequest();
    }
}

void MainWindow::updateQuad1TableView()
{
    model1->clear();
    model1->setRowCount(30);
    model1->setColumnCount(7);
    model1->setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    model1->setHorizontalHeaderItem(1, new QStandardItem(QString("Lat")));
    model1->setHorizontalHeaderItem(2, new QStandardItem(QString("Lon")));
    model1->setHorizontalHeaderItem(3, new QStandardItem(QString("Alt")));
    model1->setHorizontalHeaderItem(4, new QStandardItem(QString("P1")));
    model1->setHorizontalHeaderItem(5, new QStandardItem(QString("P2")));
    model1->setHorizontalHeaderItem(6, new QStandardItem(QString("P3")));

    ui->quad1TableView->setModel(model1);
    ui->quad1TableView->setColumnWidth(0,60);
    ui->quad1TableView->setColumnWidth(1,80);
    ui->quad1TableView->setColumnWidth(2,80);
    ui->quad1TableView->setColumnWidth(3,70);
    ui->quad1TableView->setColumnWidth(4,50);
    ui->quad1TableView->setColumnWidth(5,50);
    ui->quad1TableView->setColumnWidth(6,50);

    for (int i = 0; i< deHandle->wp_list[0].wps.length(); i++)
    {
        WP tempWP = deHandle->wp_list[0].wps.at(i);
        QStandardItem *item;
        item = new QStandardItem(QString(tempWP.wp_action));
        model1->setItem(i,0,item);
        item = new QStandardItem(QString::number(tempWP.wp_lat, 'f', 7));
        model1->setItem(i,1,item);
        item = new QStandardItem(QString::number(tempWP.wp_lon, 'f', 7));
        model1->setItem(i,2,item);
        item = new QStandardItem(QString::number(tempWP.wp_alt, 'f', 3));
        model1->setItem(i,3,item);
        item = new QStandardItem(QString::number(tempWP.wp_p1, 10));
        model1->setItem(i,4,item);
        item = new QStandardItem(QString::number(tempWP.wp_p2, 10));
        model1->setItem(i,5,item);
        item = new QStandardItem(QString::number(tempWP.wp_p3, 10));
        model1->setItem(i,6,item);
    }
    ui->quad1TableView->setModel(model1);
    emit paintRequest();
}

void MainWindow::on_quad1ConnectButton_clicked()
{
    if (ui->quad1ConnectButton->text() == "Connect")
    {
        ui->quad1ConnectButton->setText("Disconnect");
        quad1ConnSwitch = true;
        deHandle->teleAddressList[0] = ui->quad1AddressComboBox->currentText();
        ui->quad1ConnectionStatusOverview->setText("CONN");
        ui->quad1ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        ui->quad1ConnectionStatus->setText("CONN");
        ui->quad1ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
    }
    else if (ui->quad1ConnectButton->text() == "Disconnect")
    {
        ui->quad1ConnectButton->setText("Connect");
        quad1ConnSwitch = false;
        deHandle->teleAddressList[0] = "";
        ui->quad1ConnectionStatusOverview->setText("NO CON");
        ui->quad1ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
        ui->quad1ConnectionStatus->setText("NO CON");
        ui->quad1ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    }
}

void MainWindow::on_quad1UploadButton_clicked()
{
    if (deHandle->wp_list[0].wps.length() > 0)
    {
        deHandle->mi_list_air[0] = WPsToMissions(deHandle->wp_list[0]);
        if (deHandle->get_teleSerialOn() == true)
        {
            deHandle->set_teleMode(11);
        }
    }
}

void MainWindow::on_quad1DownloadButton_clicked()
{
    deHandle->set_teleMode(21);
}

void MainWindow::on_quad1LoadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose Waypoint File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }
        QString inputStr;
        QTextStream in(&file);
        inputStr = in.readAll();
        file.close();
        QStringList tempSL = inputStr.split('\n');
        QList<WP> wps;
        foreach (QString tempStr, tempSL) {
            if (tempStr.length() > 0)
            {
                QStringList fields = tempStr.split(',');
                WP tempWP;
                tempWP.wp_no = fields.at(0).toUInt();
                tempWP.wp_action = fields.at(1);
                tempWP.wp_lat = fields.at(2).toDouble();
                tempWP.wp_lon = fields.at(3).toDouble();
                tempWP.wp_alt = fields.at(4).toFloat();
                tempWP.wp_p1 = fields.at(5).toInt();
                tempWP.wp_p2 = fields.at(6).toInt();
                tempWP.wp_p3 = fields.at(7).toInt();
                wps.append(tempWP);
            }
        }
        deHandle->wp_list[0].wps = wps;
        emit updateQuad1TableViewRequest();
    }
}

void MainWindow::on_quad1SaveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Waypoints to File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }

        QTextStream out(&file);
        foreach (WP tempWP, deHandle->wp_list[0].wps) {
            QString outputStr = "";
            outputStr = outputStr + QString::number(tempWP.wp_no, 10) + ",";
            outputStr = outputStr + tempWP.wp_action + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lat, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lon, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_alt, 'f', 2) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p1, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p2, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p3, 10) + "\n";
            out << outputStr;
        }
        file.close();
    }
}

void MainWindow::on_quad1EditButton_clicked()
{
    if (ui->quad1EditButton->text() == "Edit")
    {
        ui->quad1EditButton->setText("Editing");
        quad1WPsEditing = true;
        ui->quad2EditButton->setText("Edit");
        quad2WPsEditing = false;
        ui->quad3EditButton->setText("Edit");
        quad3WPsEditing = false;
    }
    else if (ui->quad1EditButton->text() == "Editing")
    {
        ui->quad1EditButton->setText("Edit");
        quad1WPsEditing = false;
    }
}

// quad 2 functions

void MainWindow::on_quad2TableView_clicked(const QModelIndex &index)
{
    qDebug() << "single clicked" << index.row();
}

void MainWindow::on_quad2TableView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "double clicked" << index.row();
}

void MainWindow::on_quad2TableView_customContextMenuRequested(const QPoint &pos)
{
    QAction actionAdd("Add", this);
    QAction actionEdit("Edit", this);
    QAction actionDelete("Delete", this);
    QAction actionClear("Clear", this);
    QAction actionSave("Save", this);
    QAction actionLoad("Load", this);
    connect(&actionAdd, SIGNAL(triggered()), this, SLOT(Quad2AddWP()));
    connect(&actionEdit, SIGNAL(triggered()), this, SLOT(Quad2EditWP()));
    connect(&actionDelete, SIGNAL(triggered()), this, SLOT(Quad2DeleteWP()));
    connect(&actionClear, SIGNAL(triggered()), this, SLOT(Quad2ClearWP()));
    connect(&actionSave, SIGNAL(triggered()), this, SLOT(Quad2SaveWP()));
    connect(&actionLoad, SIGNAL(triggered()), this, SLOT(Quad2LoadWP()));
    QMenu myMenu;
    myMenu.addAction(&actionAdd);
    myMenu.addAction(&actionEdit);
    myMenu.addAction(&actionDelete);
    myMenu.addAction(&actionClear);
    myMenu.addAction(&actionSave);
    myMenu.addAction(&actionLoad);

    if (ui->quad2TableView->indexAt(pos).row() >= deHandle->wp_list[1].wps.length()) // empty line
    {
        actionEdit.setEnabled(false);
        actionDelete.setEnabled(false);
        if (deHandle->wp_list[1].wps.length() == 0)
        {
            actionClear.setEnabled(false);
            actionSave.setEnabled(false);
        }
    }
    else if (ui->quad2TableView->indexAt(pos).row() < deHandle->wp_list[1].wps.length()) // not empty line
    {
        actionAdd.setEnabled(false);
    }
    myMenu.exec(ui->quad2TableView->viewport()->mapToGlobal(pos));
}

void MainWindow::Quad2AddWP()
{
    try
    {
        InputDialog *inputDialog = new InputDialog;
        inputDialog->setWindowTitle("Add WP");
        //inputDialog->latLineEdit->setText(QString::number(tempCo.lat, 'f', 7));
        //inputDialog->lonLineEdit->setText(QString::number(tempCo.lon, 'f', 7));
        inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[1].wps.length()+1, 10));
        int dlgCode = inputDialog->exec();

        if (dlgCode == QDialog::Accepted)
        {
            qDebug() << "Accepted";
            WP tempWP = {0,"WP",0,0,0,0,0,0,0};
            tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
            tempWP.wp_action = inputDialog->typeCombo->currentText();
            tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
            tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
            tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
            tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
            tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
            tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
            deHandle->wp_list[1].wps.append(tempWP);
            emit updateQuad2TableViewRequest();
        }
        else if (dlgCode == QDialog::Rejected)
        {
            qDebug() << "Rejected";
        }
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::Quad2EditWP()
{
    try
    {
        InputDialog *inputDialog = new InputDialog;
        inputDialog->setWindowTitle("Edit WP");
        WP tempWP;
        int index = ui->quad2TableView->currentIndex().row();
        tempWP = deHandle->wp_list[1].wps.at(index);
        inputDialog->idLineEdit->setText(QString::number(index+1, 10));
        if (tempWP.wp_action == "WP")
        {
            inputDialog->typeCombo->setCurrentIndex(0);
        }
        else if (tempWP.wp_action == "RTH")
        {
            inputDialog->typeCombo->setCurrentIndex(1);
        }
        else if (tempWP.wp_action == "POS_UNLIM")
        {
            inputDialog->typeCombo->setCurrentIndex(2);
        }
        else if (tempWP.wp_action == "POS_LIM")
        {
            inputDialog->typeCombo->setCurrentIndex(3);
        }

        inputDialog->latLineEdit->setText(QString::number(tempWP.wp_lat, 'f', 7));
        inputDialog->lonLineEdit->setText(QString::number(tempWP.wp_lon, 'f', 7));
        inputDialog->altLineEdit->setText(QString::number(tempWP.wp_alt, 'f', 3));
        int dlgCode = inputDialog->exec();

        if (dlgCode == QDialog::Accepted)
        {
            qDebug() << "Accepted";
            WP tempWP = {0,"WP",0,0,0,0,0,0,0};
            tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
            tempWP.wp_action = inputDialog->typeCombo->currentText();
            tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
            tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
            tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
            tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
            tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
            tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
            deHandle->wp_list[1].wps.replace(index, tempWP);
            emit updateQuad2TableViewRequest();
        }
        else if (dlgCode == QDialog::Rejected)
        {
            qDebug() << "Rejected";
        }
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::Quad2DeleteWP()
{
    int index = ui->quad1TableView->currentIndex().row();
    deHandle->wp_list[1].wps.removeAt(index);
    if (index < deHandle->wp_list[1].wps.length())
    {
        for (int i = index; i< deHandle->wp_list[1].wps.length(); i++)
        {
            WP tempWP = deHandle->wp_list[1].wps.at(i);
            tempWP.wp_no = tempWP.wp_no - 1;
            deHandle->wp_list[1].wps.replace(i, tempWP);
        }
    }

    emit updateQuad2TableViewRequest();
}

void MainWindow::Quad2ClearWP() {}

void MainWindow::Quad2SaveWP()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Waypoints to File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }

        QTextStream out(&file);
        foreach (WP tempWP, deHandle->wp_list[1].wps) {
            QString outputStr = "";
            outputStr = outputStr + QString::number(tempWP.wp_no, 10) + ",";
            outputStr = outputStr + tempWP.wp_action + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lat, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lon, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_alt, 'f', 2) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p1, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p2, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p3, 10) + "\n";
            out << outputStr;
        }
        file.close();
    }
}

void MainWindow::Quad2LoadWP()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose Waypoint File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }
        QString inputStr;
        QTextStream in(&file);
        inputStr = in.readAll();
        file.close();
        QStringList tempSL = inputStr.split('\n');
        QList<WP> wps;
        foreach (QString tempStr, tempSL) {
            if (tempStr.length() > 0)
            {
                QStringList fields = tempStr.split(',');
                WP tempWP;
                tempWP.wp_no = fields.at(0).toUInt();
                tempWP.wp_action = fields.at(1);
                tempWP.wp_lat = fields.at(2).toDouble();
                tempWP.wp_lon = fields.at(3).toDouble();
                tempWP.wp_alt = fields.at(4).toFloat();
                tempWP.wp_p1 = fields.at(5).toInt();
                tempWP.wp_p2 = fields.at(6).toInt();
                tempWP.wp_p3 = fields.at(7).toInt();
                wps.append(tempWP);
            }
        }
        deHandle->wp_list[1].wps = wps;
        emit updateQuad2TableViewRequest();
    }
}

void MainWindow::updateQuad2TableView()
{
    model2->clear();
    model2->setRowCount(30);
    model2->setColumnCount(7);
    model2->setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    model2->setHorizontalHeaderItem(1, new QStandardItem(QString("Lat")));
    model2->setHorizontalHeaderItem(2, new QStandardItem(QString("Lon")));
    model2->setHorizontalHeaderItem(3, new QStandardItem(QString("Alt")));
    model2->setHorizontalHeaderItem(4, new QStandardItem(QString("P1")));
    model2->setHorizontalHeaderItem(5, new QStandardItem(QString("P2")));
    model2->setHorizontalHeaderItem(6, new QStandardItem(QString("P3")));

    ui->quad2TableView->setModel(model2);
    ui->quad2TableView->setColumnWidth(0,60);
    ui->quad2TableView->setColumnWidth(1,80);
    ui->quad2TableView->setColumnWidth(2,80);
    ui->quad2TableView->setColumnWidth(3,70);
    ui->quad2TableView->setColumnWidth(4,50);
    ui->quad2TableView->setColumnWidth(5,50);
    ui->quad2TableView->setColumnWidth(6,50);

    for (int i = 0; i< deHandle->wp_list[1].wps.length(); i++)
    {
        WP tempWP = deHandle->wp_list[1].wps.at(i);
        QStandardItem *item;
        item = new QStandardItem(QString(tempWP.wp_action));
        model2->setItem(i,0,item);
        item = new QStandardItem(QString::number(tempWP.wp_lat, 'f', 7));
        model2->setItem(i,1,item);
        item = new QStandardItem(QString::number(tempWP.wp_lon, 'f', 7));
        model2->setItem(i,2,item);
        item = new QStandardItem(QString::number(tempWP.wp_alt, 'f', 3));
        model2->setItem(i,3,item);
        item = new QStandardItem(QString::number(tempWP.wp_p1, 10));
        model2->setItem(i,4,item);
        item = new QStandardItem(QString::number(tempWP.wp_p2, 10));
        model2->setItem(i,5,item);
        item = new QStandardItem(QString::number(tempWP.wp_p3, 10));
        model2->setItem(i,6,item);
    }
    ui->quad2TableView->setModel(model2);
    emit paintRequest();
}

void MainWindow::on_quad2ConnectButton_clicked()
{
    if (ui->quad2ConnectButton->text() == "Connect")
    {
        ui->quad2ConnectButton->setText("Disconnect");
        quad2ConnSwitch = true;
        deHandle->teleAddressList[1] = ui->quad2AddressComboBox->currentText();
        ui->quad2ConnectionStatusOverview->setText("CONN");
        ui->quad2ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        ui->quad2ConnectionStatus->setText("CONN");
        ui->quad2ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
    }
    else if (ui->quad2ConnectButton->text() == "Disconnect")
    {
        ui->quad2ConnectButton->setText("Connect");
        quad2ConnSwitch = false;
        deHandle->teleAddressList[1] = "";
        ui->quad2ConnectionStatusOverview->setText("NO CON");
        ui->quad2ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
        ui->quad2ConnectionStatus->setText("NO CON");
        ui->quad2ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    }
}

void MainWindow::on_quad2UploadButton_clicked()
{
    if (deHandle->wp_list[1].wps.length() > 0)
    {
        deHandle->mi_list_air[1] = WPsToMissions(deHandle->wp_list[1]);
        if (deHandle->get_teleSerialOn() == true)
        {
            deHandle->set_teleMode(12);
        }
    }
}

void MainWindow::on_quad2DownloadButton_clicked()
{
    deHandle->set_teleMode(22);
}

void MainWindow::on_quad2LoadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose Waypoint File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }
        QString inputStr;
        QTextStream in(&file);
        inputStr = in.readAll();
        file.close();
        QStringList tempSL = inputStr.split('\n');
        QList<WP> wps;
        foreach (QString tempStr, tempSL) {
            if (tempStr.length() > 0)
            {
                QStringList fields = tempStr.split(',');
                WP tempWP;
                tempWP.wp_no = fields.at(0).toUInt();
                tempWP.wp_action = fields.at(1);
                tempWP.wp_lat = fields.at(2).toDouble();
                tempWP.wp_lon = fields.at(3).toDouble();
                tempWP.wp_alt = fields.at(4).toFloat();
                tempWP.wp_p1 = fields.at(5).toInt();
                tempWP.wp_p2 = fields.at(6).toInt();
                tempWP.wp_p3 = fields.at(7).toInt();
                wps.append(tempWP);
            }
        }
        deHandle->wp_list[1].wps = wps;
        emit updateQuad2TableViewRequest();
    }
}

void MainWindow::on_quad2SaveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Waypoints to File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }

        QTextStream out(&file);
        foreach (WP tempWP, deHandle->wp_list[1].wps) {
            QString outputStr = "";
            outputStr = outputStr + QString::number(tempWP.wp_no, 10) + ",";
            outputStr = outputStr + tempWP.wp_action + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lat, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lon, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_alt, 'f', 2) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p1, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p2, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p3, 10) + "\n";
            out << outputStr;
        }
        file.close();
    }
}

void MainWindow::on_quad2EditButton_clicked()
{
    if (ui->quad2EditButton->text() == "Edit")
    {
        ui->quad2EditButton->setText("Editing");
        quad2WPsEditing = true;
        ui->quad1EditButton->setText("Edit");
        quad1WPsEditing = false;
        ui->quad3EditButton->setText("Edit");
        quad3WPsEditing = false;
    }
    else if (ui->quad2EditButton->text() == "Editing")
    {
        ui->quad2EditButton->setText("Edit");
        quad2WPsEditing = false;
    }
}

// quad 3 functions

void MainWindow::on_quad3TableView_clicked(const QModelIndex &index)
{
    qDebug() << "single clicked" << index.row();
}

void MainWindow::on_quad3TableView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "double clicked" << index.row();
}

void MainWindow::on_quad3TableView_customContextMenuRequested(const QPoint &pos)
{
    QAction actionAdd("Add", this);
    QAction actionEdit("Edit", this);
    QAction actionDelete("Delete", this);
    QAction actionClear("Clear", this);
    QAction actionSave("Save", this);
    QAction actionLoad("Load", this);
    connect(&actionAdd, SIGNAL(triggered()), this, SLOT(Quad3AddWP()));
    connect(&actionEdit, SIGNAL(triggered()), this, SLOT(Quad3EditWP()));
    connect(&actionDelete, SIGNAL(triggered()), this, SLOT(Quad3DeleteWP()));
    connect(&actionClear, SIGNAL(triggered()), this, SLOT(Quad3ClearWP()));
    connect(&actionSave, SIGNAL(triggered()), this, SLOT(Quad3SaveWP()));
    connect(&actionLoad, SIGNAL(triggered()), this, SLOT(Quad3LoadWP()));
    QMenu myMenu;
    myMenu.addAction(&actionAdd);
    myMenu.addAction(&actionEdit);
    myMenu.addAction(&actionDelete);
    myMenu.addAction(&actionClear);
    myMenu.addAction(&actionSave);
    myMenu.addAction(&actionLoad);

    if (ui->quad3TableView->indexAt(pos).row() >= deHandle->wp_list[2].wps.length()) // empty line
    {
        actionEdit.setEnabled(false);
        actionDelete.setEnabled(false);
        if (deHandle->wp_list[2].wps.length() == 0)
        {
            actionClear.setEnabled(false);
            actionSave.setEnabled(false);
        }
    }
    else if (ui->quad3TableView->indexAt(pos).row() < deHandle->wp_list[2].wps.length()) // not empty line
    {
        actionAdd.setEnabled(false);
    }
    myMenu.exec(ui->quad3TableView->viewport()->mapToGlobal(pos));
}

void MainWindow::Quad3AddWP()
{
    try
    {
        InputDialog *inputDialog = new InputDialog;
        inputDialog->setWindowTitle("Add WP");
        //inputDialog->latLineEdit->setText(QString::number(tempCo.lat, 'f', 7));
        //inputDialog->lonLineEdit->setText(QString::number(tempCo.lon, 'f', 7));
        inputDialog->idLineEdit->setText(QString::number(deHandle->wp_list[2].wps.length()+1, 10));
        int dlgCode = inputDialog->exec();

        if (dlgCode == QDialog::Accepted)
        {
            qDebug() << "Accepted";
            WP tempWP = {0,"WP",0,0,0,0,0,0,0};
            tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
            tempWP.wp_action = inputDialog->typeCombo->currentText();
            tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
            tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
            tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
            tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
            tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
            tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
            deHandle->wp_list[2].wps.append(tempWP);
            emit updateQuad3TableViewRequest();
        }
        else if (dlgCode == QDialog::Rejected)
        {
            qDebug() << "Rejected";
        }
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::Quad3EditWP()
{
    try
    {
        InputDialog *inputDialog = new InputDialog;
        inputDialog->setWindowTitle("Edit WP");
        WP tempWP;
        int index = ui->quad1TableView->currentIndex().row();
        tempWP = deHandle->wp_list[2].wps.at(index);
        inputDialog->idLineEdit->setText(QString::number(index+1, 10));
        if (tempWP.wp_action == "WP")
        {
            inputDialog->typeCombo->setCurrentIndex(0);
        }
        else if (tempWP.wp_action == "RTH")
        {
            inputDialog->typeCombo->setCurrentIndex(1);
        }
        else if (tempWP.wp_action == "POS_UNLIM")
        {
            inputDialog->typeCombo->setCurrentIndex(2);
        }
        else if (tempWP.wp_action == "POS_LIM")
        {
            inputDialog->typeCombo->setCurrentIndex(3);
        }

        inputDialog->latLineEdit->setText(QString::number(tempWP.wp_lat, 'f', 7));
        inputDialog->lonLineEdit->setText(QString::number(tempWP.wp_lon, 'f', 7));
        inputDialog->altLineEdit->setText(QString::number(tempWP.wp_alt, 'f', 3));
        int dlgCode = inputDialog->exec();

        if (dlgCode == QDialog::Accepted)
        {
            qDebug() << "Accepted";
            WP tempWP = {0,"WP",0,0,0,0,0,0,0};
            tempWP.wp_no = inputDialog->idLineEdit->text().toInt();
            tempWP.wp_action = inputDialog->typeCombo->currentText();
            tempWP.wp_lat = inputDialog->latLineEdit->text().toDouble();
            tempWP.wp_lon = inputDialog->lonLineEdit->text().toDouble();
            tempWP.wp_alt = inputDialog->altLineEdit->text().toDouble();
            tempWP.wp_p1 = inputDialog->p1LineEdit->text().toInt();
            tempWP.wp_p2 = inputDialog->p2LineEdit->text().toInt();
            tempWP.wp_p3 = inputDialog->p3LineEdit->text().toInt();
            deHandle->wp_list[2].wps.replace(index, tempWP);
            emit updateQuad3TableViewRequest();
        }
        else if (dlgCode == QDialog::Rejected)
        {
            qDebug() << "Rejected";
        }
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::Quad3DeleteWP()
{
    int index = ui->quad3TableView->currentIndex().row();
    deHandle->wp_list[2].wps.removeAt(index);
    if (index < deHandle->wp_list[2].wps.length())
    {
        for (int i = index; i< deHandle->wp_list[2].wps.length(); i++)
        {
            WP tempWP = deHandle->wp_list[2].wps.at(i);
            tempWP.wp_no = tempWP.wp_no - 1;
            deHandle->wp_list[2].wps.replace(i, tempWP);
        }
    }

    emit updateQuad3TableViewRequest();
}

void MainWindow::Quad3ClearWP() {}

void MainWindow::Quad3SaveWP()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Waypoints to File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }

        QTextStream out(&file);
        foreach (WP tempWP, deHandle->wp_list[2].wps) {
            QString outputStr = "";
            outputStr = outputStr + QString::number(tempWP.wp_no, 10) + ",";
            outputStr = outputStr + tempWP.wp_action + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lat, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lon, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_alt, 'f', 2) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p1, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p2, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p3, 10) + "\n";
            out << outputStr;
        }
        file.close();
    }
}

void MainWindow::Quad3LoadWP()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose Waypoint File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }
        QString inputStr;
        QTextStream in(&file);
        inputStr = in.readAll();
        file.close();
        QStringList tempSL = inputStr.split('\n');
        QList<WP> wps;
        foreach (QString tempStr, tempSL) {
            if (tempStr.length() > 0)
            {
                QStringList fields = tempStr.split(',');
                WP tempWP;
                tempWP.wp_no = fields.at(0).toUInt();
                tempWP.wp_action = fields.at(1);
                tempWP.wp_lat = fields.at(2).toDouble();
                tempWP.wp_lon = fields.at(3).toDouble();
                tempWP.wp_alt = fields.at(4).toFloat();
                tempWP.wp_p1 = fields.at(5).toInt();
                tempWP.wp_p2 = fields.at(6).toInt();
                tempWP.wp_p3 = fields.at(7).toInt();
                wps.append(tempWP);
            }
        }
        deHandle->wp_list[2].wps = wps;
        emit updateQuad3TableViewRequest();
    }
}

void MainWindow::updateQuad3TableView()
{
    model3->clear();
    model3->setRowCount(30);
    model3->setColumnCount(7);
    model3->setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    model3->setHorizontalHeaderItem(1, new QStandardItem(QString("Lat")));
    model3->setHorizontalHeaderItem(2, new QStandardItem(QString("Lon")));
    model3->setHorizontalHeaderItem(3, new QStandardItem(QString("Alt")));
    model3->setHorizontalHeaderItem(4, new QStandardItem(QString("P1")));
    model3->setHorizontalHeaderItem(5, new QStandardItem(QString("P2")));
    model3->setHorizontalHeaderItem(6, new QStandardItem(QString("P3")));

    ui->quad3TableView->setModel(model3);
    ui->quad3TableView->setColumnWidth(0,60);
    ui->quad3TableView->setColumnWidth(1,80);
    ui->quad3TableView->setColumnWidth(2,80);
    ui->quad3TableView->setColumnWidth(3,70);
    ui->quad3TableView->setColumnWidth(4,50);
    ui->quad3TableView->setColumnWidth(5,50);
    ui->quad3TableView->setColumnWidth(6,50);

    for (int i = 0; i< deHandle->wp_list[2].wps.length(); i++)
    {
        WP tempWP = deHandle->wp_list[2].wps.at(i);
        QStandardItem *item;
        item = new QStandardItem(QString(tempWP.wp_action));
        model3->setItem(i,0,item);
        item = new QStandardItem(QString::number(tempWP.wp_lat, 'f', 7));
        model3->setItem(i,1,item);
        item = new QStandardItem(QString::number(tempWP.wp_lon, 'f', 7));
        model3->setItem(i,2,item);
        item = new QStandardItem(QString::number(tempWP.wp_alt, 'f', 3));
        model3->setItem(i,3,item);
        item = new QStandardItem(QString::number(tempWP.wp_p1, 10));
        model3->setItem(i,4,item);
        item = new QStandardItem(QString::number(tempWP.wp_p2, 10));
        model3->setItem(i,5,item);
        item = new QStandardItem(QString::number(tempWP.wp_p3, 10));
        model3->setItem(i,6,item);
    }
    ui->quad3TableView->setModel(model3);
    emit paintRequest();
}

void MainWindow::on_quad3ConnectButton_clicked()
{
    if (ui->quad3ConnectButton->text() == "Connect")
    {
        ui->quad3ConnectButton->setText("Disconnect");
        quad3ConnSwitch = true;
        deHandle->teleAddressList[2] = ui->quad3AddressComboBox->currentText();
        ui->quad3ConnectionStatusOverview->setText("CONN");
        ui->quad3ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
        ui->quad3ConnectionStatus->setText("CONN");
        ui->quad3ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(0,255,0,1);}");
    }
    else if (ui->quad3ConnectButton->text() == "Disconnect")
    {
        ui->quad3ConnectButton->setText("Connect");
        quad3ConnSwitch = false;
        deHandle->teleAddressList[2] = "";
        ui->quad3ConnectionStatusOverview->setText("NO CON");
        ui->quad3ConnectionStatusOverview->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
        ui->quad3ConnectionStatus->setText("NO CON");
        ui->quad3ConnectionStatus->setStyleSheet("QLabel {background-color : rgba(217,217,217,1);}");
    }
}

void MainWindow::on_quad3UploadButton_clicked()
{
    if (deHandle->wp_list[2].wps.length() > 0)
    {
        deHandle->mi_list_air[2] = WPsToMissions(deHandle->wp_list[2]);
        if (deHandle->get_teleSerialOn() == true)
        {
            deHandle->set_teleMode(13);
        }
    }
}

void MainWindow::on_quad3DownloadButton_clicked()
{
    deHandle->set_teleMode(23);
}

void MainWindow::on_quad3LoadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose Waypoint File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }
        QString inputStr;
        QTextStream in(&file);
        inputStr = in.readAll();
        file.close();
        QStringList tempSL = inputStr.split('\n');
        QList<WP> wps;
        foreach (QString tempStr, tempSL) {
            if (tempStr.length() > 0)
            {
                QStringList fields = tempStr.split(',');
                WP tempWP;
                tempWP.wp_no = fields.at(0).toUInt();
                tempWP.wp_action = fields.at(1);
                tempWP.wp_lat = fields.at(2).toDouble();
                tempWP.wp_lon = fields.at(3).toDouble();
                tempWP.wp_alt = fields.at(4).toFloat();
                tempWP.wp_p1 = fields.at(5).toInt();
                tempWP.wp_p2 = fields.at(6).toInt();
                tempWP.wp_p3 = fields.at(7).toInt();
                wps.append(tempWP);
            }
        }
        deHandle->wp_list[2].wps = wps;
        emit updateQuad3TableViewRequest();
    }
}

void MainWindow::on_quad3SaveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Waypoints to File", "", "Waypoints files (*.wps);;Text files (*.txt)");
    if (fileName.length() > 0)
    {
        qDebug() << fileName;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open file";
        }

        QTextStream out(&file);
        foreach (WP tempWP, deHandle->wp_list[2].wps) {
            QString outputStr = "";
            outputStr = outputStr + QString::number(tempWP.wp_no, 10) + ",";
            outputStr = outputStr + tempWP.wp_action + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lat, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_lon, 'f', 7) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_alt, 'f', 2) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p1, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p2, 10) + ",";
            outputStr = outputStr + QString::number(tempWP.wp_p3, 10) + "\n";
            out << outputStr;
        }
        file.close();
    }
}

void MainWindow::on_quad3EditButton_clicked()
{
    if (ui->quad3EditButton->text() == "Edit")
    {
        ui->quad3EditButton->setText("Editing");
        quad3WPsEditing = true;
        ui->quad1EditButton->setText("Edit");
        quad1WPsEditing = false;
        ui->quad2EditButton->setText("Edit");
        quad2WPsEditing = false;
    }
    else if (ui->quad3EditButton->text() == "Editing")
    {
        ui->quad3EditButton->setText("Edit");
        quad3WPsEditing = false;
    }
}


void MainWindow::on_helpButton_clicked()
{
    try
    {
        HelpDialog *helpDialog = new HelpDialog;
        helpDialog->setWindowTitle("Help");
        int dlgCode = helpDialog->exec();
        if (dlgCode)
        {}
    }
    catch (...)
    {
        qDebug() << "Caught exception";
    }
}

void MainWindow::on_expandButton_clicked()
{
    //QSize tempGeo = size();
    QString currentText = ui->expandButton->text();
    if (currentText == ">")
    {
        ui->expandButton->setText("<");
        //qDebug() << logDialog->geometry();
        QSize winGeo = size();
        QPoint winPos = pos();
        QPoint expPos = ui->expandButton->pos();
        QSize expSize = ui->expandButton->size();
        ui->expandButton->setGeometry(winGeo.width()-expSize.width(), 0, expSize.width(), winGeo.height());
        move(winPos.x()-300,winPos.y());
        logDialog->show();
        QRect availableRect = QApplication::desktop()->availableGeometry();
        logDialog->move(availableRect.width()-300, availableRect.y());
        //qDebug() << logDialog->geometry();
    }
    else if (currentText == "<")
    {
        ui->expandButton->setText(">");
        QSize winGeo = size();
        QPoint winPos = pos();
        QPoint expPos = ui->expandButton->pos();
        QSize expSize = ui->expandButton->size();
        ui->expandButton->setGeometry(winGeo.width()-expSize.width(), 0, expSize.width(), winGeo.height());
        move(winPos.x()+300,winPos.y());
        logDialog->hide();
    }
}
