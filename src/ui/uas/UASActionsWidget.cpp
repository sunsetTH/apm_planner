#include "QsLog.h"
#include "ArduPilotMegaMAV.h"
#include "UASActionsWidget.h"
#include "APMShortcutModesDialog.h"
#include <QMessageBox>
#include <UAS.h>
#include <QSettings>

void UASActionsWidget::setupApmPlaneComboBox(QComboBox *comboBox)
{
    comboBox->addItem("Manual", ApmPlane::MANUAL);
    comboBox->addItem("Circle", ApmPlane::CIRCLE);
    comboBox->addItem("Stabilize", ApmPlane::STABILIZE);
    comboBox->addItem("Training", ApmPlane::TRAINING);
    comboBox->addItem("FBW A", ApmPlane::FLY_BY_WIRE_A);
    comboBox->addItem("FBW B", ApmPlane::FLY_BY_WIRE_B);
    comboBox->addItem("Auto", ApmPlane::AUTO);
    comboBox->addItem("RTL", ApmPlane::RTL);
    comboBox->addItem("Loiter", ApmPlane::LOITER);
    comboBox->addItem("Guided", ApmPlane::GUIDED);
}

void UASActionsWidget::setupApmPlaneModes()
{
    QLOG_INFO() << "UASActionWidget: Set for Plane";
    setupApmPlaneComboBox(ui.modeComboBox);

    ui.armDisarmButton->setVisible(false);
    ui.armedStatuslabel->setVisible(false);

    // Setup configurable shortcut action
    ui.opt1ModeButton->setText("FBW A");
    ui.opt2ModeButton->setText("FBW B");
    ui.opt3ModeButton->setText("Circle");
    ui.opt4ModeButton->setText("Manual");

}

void UASActionsWidget::setupApmCopterComboBox(QComboBox *comboBox)
{
    comboBox->addItem("Stabilize", ApmCopter::STABILIZE);
    comboBox->addItem("Acro", ApmCopter::ACRO);
    comboBox->addItem("Alt Hold", ApmCopter::ALT_HOLD);
    comboBox->addItem("Auto", ApmCopter::AUTO);
    comboBox->addItem("Guided", ApmCopter::GUIDED);
    comboBox->addItem("Loiter", ApmCopter::LOITER);
    comboBox->addItem("RTL", ApmCopter::RTL);
    comboBox->addItem("Circle", ApmCopter::CIRCLE);
    comboBox->addItem("Position", ApmCopter::POSITION);
    comboBox->addItem("Land", ApmCopter::LAND);
    comboBox->addItem("Loiter", ApmCopter::OF_LOITER);
    comboBox->addItem("Drift", ApmCopter::DRIFT);
    comboBox->addItem("Sport", ApmCopter::SPORT);
}

void UASActionsWidget::setupApmCopterModes()
{
    QLOG_INFO() << "UASActionWidget: set for Copter";
    setupApmCopterComboBox(ui.modeComboBox);

    ui.armDisarmButton->setVisible(true);
    ui.armedStatuslabel->setVisible(true);

    // Setup configurable shortcut action
    ui.opt1ModeButton->setText("none");
    ui.opt2ModeButton->setText("Acro");
    ui.opt3ModeButton->setText("Alt Hold");
    ui.opt4ModeButton->setText("Land");
    configureModeButtonEnableDisable();
}

void UASActionsWidget::setupApmRoverComboBox(QComboBox *comboBox)
{
    comboBox->addItem("Manual", ApmRover::MANUAL);
    comboBox->addItem("Learning", ApmRover::LEARNING);
    comboBox->addItem("Steering", ApmRover::STEERING);
    comboBox->addItem("Hold", ApmRover::HOLD);
    comboBox->addItem("Auto", ApmRover::AUTO);
    comboBox->addItem("RTL", ApmRover::RTL);
    comboBox->addItem("Guided", ApmRover::GUIDED);
    comboBox->addItem("Initializing", ApmRover::INITIALIZING);
}

void UASActionsWidget::setupApmRoverModes()
{
    QLOG_INFO() << "UASActionWidget: Setfor Rover";
    setupApmRoverComboBox(ui.modeComboBox);

    ui.armDisarmButton->setVisible(false);
    ui.armedStatuslabel->setVisible(false);

    // Setup configurable shortcut action
    ui.opt1ModeButton->setText("Learn");
    ui.opt2ModeButton->setText("Steer");
    ui.opt3ModeButton->setText("Hold");
    ui.opt4ModeButton->setText("none");
    configureModeButtonEnableDisable();

}

UASActionsWidget::UASActionsWidget(QWidget *parent) : QWidget(parent)
{
    QLOG_INFO() << "UASActionsWidget creating " << this;
    m_uas = NULL;
    ui.setupUi(this);
    connect(ui.changeAltitudeButton,SIGNAL(clicked()),this,SLOT(changeAltitudeClicked()));
    connect(ui.changeSpeedButton,SIGNAL(clicked()),this,SLOT(changeSpeedClicked()));
    connect(ui.goToWaypointButton,SIGNAL(clicked()),this,SLOT(goToWaypointClicked()));
    connect(ui.armDisarmButton,SIGNAL(clicked()),this,SLOT(armButtonClicked()));
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));

    if (UASManager::instance()->getActiveUAS())
    {
        activeUASSet(UASManager::instance()->getActiveUAS());
    }

    // Setup Action Combo Box
    ui.actionComboBox->addItem("Loiter Unlimited", MAV_CMD_NAV_LOITER_UNLIM);
    ui.actionComboBox->addItem("Return To Launch", MAV_CMD_NAV_RETURN_TO_LAUNCH);
    ui.actionComboBox->addItem("Preflight Calibration", MAV_CMD_PREFLIGHT_CALIBRATION);
    ui.actionComboBox->addItem("Mission Start", MAV_CMD_MISSION_START);
    ui.actionComboBox->addItem("Preflight Reboot", MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN);

    connect(ui.exeActionButton, SIGNAL(clicked()),
            this, SLOT(setAction()));

    // Mode Shortcut Buttons
    connect(ui.autoModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));
    connect(ui.stabilizeModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));
    connect(ui.rtlModeButton, SIGNAL(clicked()),
            this, SLOT(setRTLMode()));
    connect(ui.loiterModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));

    connect(ui.opt1ModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));
    connect(ui.opt2ModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));
    connect(ui.opt3ModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));
    connect(ui.opt4ModeButton, SIGNAL(clicked()),
            this, SLOT(setShortcutMode()));
}

void UASActionsWidget::activeUASSet(UASInterface *uas)
{
    QLOG_INFO() << "UASActionWidget::activeUASSet";
    if (uas == NULL) {
        QLOG_ERROR() << "uas object NULL";
        return;
    }

    if (m_uas) {
        // disconnect previous connections
        disconnect(m_uas->getWaypointManager(),SIGNAL(waypointEditableListChanged()),
                this,SLOT(updateWaypointList()));
        disconnect(m_uas->getWaypointManager(),SIGNAL(currentWaypointChanged(quint16)),
                this,SLOT(currentWaypointChanged(quint16)));
        disconnect(m_uas,SIGNAL(armingChanged(bool)),this,SLOT(armingChanged(bool)));

        disconnect(m_uas, SIGNAL(connected()), this, SLOT(uasConnected()));
        disconnect(m_uas, SIGNAL(disconnected()), this, SLOT(uasDisconnected()));

        disconnect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
                this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));
    }

    // enable the controls
    ui.actionsGroupBox->setDisabled(false);
    ui.missionGroupBox->setDisabled(false);
    ui.altitudeTypeComboBox->addItem(tr("Relative (AGL)"), MAV_FRAME_GLOBAL_RELATIVE_ALT);
    ui.altitudeTypeComboBox->addItem(tr("Absolute (ASL)"), MAV_FRAME_GLOBAL);
    ui.shortcutGroupBox->setDisabled(false);
    ui.altitudeDoubleSpinBox->setValue(50.0f); // set a default altitude

    m_uas = static_cast<UAS*>(uas);

    connect(m_uas->getWaypointManager(),SIGNAL(waypointEditableListChanged()),
            this,SLOT(updateWaypointList()));
    connect(m_uas->getWaypointManager(),SIGNAL(currentWaypointChanged(quint16)),
            this,SLOT(currentWaypointChanged(quint16)));
    connect(m_uas,SIGNAL(armingChanged(bool)),this,SLOT(armingChanged(bool)));

    connect(m_uas, SIGNAL(connected()), this, SLOT(uasConnected()));
    connect(m_uas, SIGNAL(disconnected()), this, SLOT(uasDisconnected()));

    connect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
            this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));

    armingChanged(m_uas->isArmed());
    updateWaypointList();

    switch (uas->getAutopilotType()){
        case MAV_AUTOPILOT_ARDUPILOTMEGA: {
            if (uas->isFixedWing()){
                setupApmPlaneModes();

            } else if (uas->isMultirotor()){
                setupApmCopterModes();

            } else if (uas->isGroundRover()){
                setupApmRoverModes();

            } else {
                QLOG_WARN() << "UASActionWidget: Unsupported System Type" << uas->getSystemType();
            }
            // Setup Final Connections
            connect(ui.setModeButton, SIGNAL(clicked()),this, SLOT(setMode()));

        } break;
        case MAV_AUTOPILOT_PX4:
        {
            // [TODO] PX4 flight controller go here
        }
        case MAV_AUTOPILOT_GENERIC:
        default:
        {
            // [TODO] Generic, and other flight controllers
        }

        loadApmSettings();
        configureModeButtonEnableDisable();
    }
}

void UASActionsWidget::uasConnected()
{
    QLOG_INFO() << "UASActionsWidget::connected()" << m_uas;
    ui.actionsGroupBox->setDisabled(false);
    ui.missionGroupBox->setDisabled(false);
    ui.shortcutGroupBox->setDisabled(false);
}

void UASActionsWidget::uasDisconnected()
{
    QLOG_INFO() << "UASActionsWidget::disconnected()" << m_uas;
    ui.actionsGroupBox->setDisabled(true);
    ui.missionGroupBox->setDisabled(true);
    ui.shortcutGroupBox->setDisabled(true);
}

void UASActionsWidget::armButtonClicked()
{
    QLOG_INFO() << "UASActionsWidget::armButtonClicked";

    if(!activeUas())
        return;

    if (m_uas->isArmed())
    {
        QLOG_INFO() << "UAS:: Attempt to Disarm System";
        m_uas->disarmSystem();

    }
    else
    {
        QLOG_INFO() << "UAS:: Attempt to Arm System";
        m_uas->armSystem();
    }
}

void UASActionsWidget::armingChanged(bool state)
{
    QLOG_INFO() << "Arming State Changed: " << (state?"ARMED":"DISARM");

    //TODO:
    //Figure out why arm/disarm is in UAS.h and not part of the interface, and fix.
    if (state)
    {
        ui.armDisarmButton->setText("DISARM");
        ui.armedStatuslabel->setText("<html><head/><body><p><span style=color:#ff0000>ARMED</span></p></body></html>");
    }
    else
    {
        ui.armDisarmButton->setText("ARM");
        ui.armedStatuslabel->setText("<html><head/><body><p><span style=color:#00AA00>DISARMED</span></p></body></html>");
    }

}

void UASActionsWidget::currentWaypointChanged(quint16 wpid)
{
    if (m_last_wpid != wpid) {
        m_last_wpid = wpid;
        QLOG_INFO() << "Waypoint Changed to: " << wpid;
        ui.currentWaypointLabel->setText("Current: " + QString::number(wpid));
        ui.waypointListComboBox->setCurrentIndex(ui.waypointListComboBox->findData(QString::number(wpid)));
    }
}

void UASActionsWidget::updateWaypointList()
{
    if(!activeUas())
        return;
    QLOG_INFO() << "updateWaypointList: ";

    ui.waypointListComboBox->clear();
    for (int i=0;i<m_uas->getWaypointManager()->getWaypointEditableList().size();i++)
    {
        QLOG_INFO() << "  WP:" << i;
        ui.waypointListComboBox->addItem(QString::number(i));
    }
}

UASActionsWidget::~UASActionsWidget()
{
}

void UASActionsWidget::goToWaypointClicked()
{
    if(!activeUas())
        return;
    QLOG_INFO() << "Go to Waypoint" << ui.waypointListComboBox->currentIndex();
    m_uas->getWaypointManager()->setCurrentWaypoint(ui.waypointListComboBox->currentIndex());
}

void UASActionsWidget::changeAltitudeClicked()
{
    QLOG_WARN() << "changeAltitudeClicked";

    QLOG_DEBUG() << "Start guided action requested. Lat:" << m_uas->getLatitude()<< "Lon:" << m_uas->getLongitude()
                 << "Alt:" << ui.altitudeDoubleSpinBox->value() << "MAV_FRAME:"
                 << (ui.altitudeTypeComboBox->itemData(ui.altitudeTypeComboBox->currentIndex()).toInt() == MAV_FRAME_GLOBAL_RELATIVE_ALT? "AGL": "ASL");
    Waypoint wp;
    wp.setFrame(static_cast<MAV_FRAME>(ui.altitudeTypeComboBox->itemData(ui.altitudeTypeComboBox->currentIndex()).toInt()));
    wp.setLatitude(m_uas->getLatitude());
    wp.setLongitude(m_uas->getLongitude());
    wp.setAltitude(ui.altitudeDoubleSpinBox->value());
    m_uas->getWaypointManager()->goToWaypoint(&wp);
}

void UASActionsWidget::changeSpeedClicked()
{
    if(!activeUas())
        return;

    QLOG_INFO() << "Change Vehicle Speed ";

    if (m_uas->isMultirotor())
    {
        QLOG_INFO() << "APMCopter: setting WPNAV_SPEED: " << ui.speedDoubleSpinBox->value() * 100.0f;
        m_uas->setParameter(1,"WPNAV_SPEED",
                            QVariant((static_cast<float>(ui.speedDoubleSpinBox->value() * 100.0f))));
        return;
    }
    else if (m_uas->isFixedWing())
    {
        // [TODO} need to add AirSpeed/GroundSpeed options here as well
        QVariant variant;
        if (m_uas->getParamManager()->getParameterValue(1,"ARSPD_ENABLE",variant))
        {
            if (variant.toInt() == 1)
            {
                QLOG_INFO() << "APMPlane: ARSPD_ENABLED setting TRIM_ARSPD_CN";
                m_uas->setParameter(1,"TRIM_ARSPD_CN",
                                    QVariant((static_cast<float>(ui.speedDoubleSpinBox->value() * 100.0f))));
                return;
            }

        }
        QLOG_INFO() << "APMPlane: setting TRIM_ARSPD_CN";
        m_uas->setParameter(1,"TRIM_ARSPD_CN",
                            QVariant((static_cast<float>(ui.speedDoubleSpinBox->value() * 100.0f))));

    } else if (m_uas->isGroundRover()) {
        QLOG_INFO() << "APMCopter: setting CRUISE_SPEED: " << ui.speedDoubleSpinBox->value() * 100.0f;
        m_uas->setParameter(1, "CRUISE_SPEED",
                            QVariant((static_cast<float>(ui.speedDoubleSpinBox->value() * 100.0f))));

    }
}

void UASActionsWidget::setMode()
{
    QLOG_INFO() << "    UASActionsWidget::setMode()";

    if(!activeUas())
        return;

    QLOG_INFO() << "Set Mode to "
                << ui.modeComboBox->itemData(ui.modeComboBox->currentIndex()).toInt();

    m_uas->setMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                   ui.modeComboBox->itemData(ui.modeComboBox->currentIndex()).toInt());
}

void UASActionsWidget::setShortcutMode()
{
    QLOG_INFO() << "    UASActionsWidget::setShortcutMode()";

    if(!activeUas())
        return;

    QLOG_INFO() << "Set Mode to "
                << ui.shortcutButtonGroup->checkedButton()->text();
    int index = ui.modeComboBox->findText(ui.shortcutButtonGroup->checkedButton()->text());
    QLOG_DEBUG() << "index: "
                << index;
    ui.modeComboBox->setCurrentIndex(index);
    m_uas->setMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                   ui.modeComboBox->itemData(ui.modeComboBox->currentIndex()).toInt());
}


void UASActionsWidget::setAction()
{
    QLOG_INFO() << "UASActionsWidget::setAction()";

    if(!activeUas())
        return;

    QLOG_INFO() << "Set Action to " << ui.actionComboBox->currentIndex();

    int currentIndex = ui.actionComboBox->currentIndex();
    MAV_CMD currentCommand = static_cast<MAV_CMD>(ui.actionComboBox->itemData(currentIndex).toInt());

    switch (m_uas->getAutopilotType()) {
        case MAV_AUTOPILOT_ARDUPILOTMEGA: {
            if (m_uas->isFixedWing()){
                sendApmPlaneCommand(currentCommand);

            } else if (m_uas->isMultirotor()){
                sendApmCopterCommand(currentCommand);

            } else if (m_uas->isGroundRover()){
                sendApmRoverCommand(currentCommand);

            } else {
                QLOG_WARN() << "UASActionWidget: Unsupported System Type" << m_uas->getSystemType();
            }

            } break;

        case MAV_AUTOPILOT_PX4: {
            // [TODO] PX4 flight controller go here
        }

        case MAV_AUTOPILOT_GENERIC:
        default: {
            // [TODO] Generic, and other flight controllers here (or own sections as above)
        }
    }
}


int UASActionsWidget::preFlightWarningBox()
{
    QLOG_INFO() << "Display Pre-Flight Warning Check";
    return QMessageBox::critical(this,tr("Warning"),tr("This action must be done when on the gorund. If vehicle is in the air the this action will result in a crash!"),
                         QMessageBox::Ok,QMessageBox::Abort);
}

void UASActionsWidget::sendApmPlaneCommand(MAV_CMD command)
{
    switch(command) {

    case MAV_CMD_NAV_LOITER_UNLIM: {
        // Loiter around this MISSION an unlimited amount of time
        Q_ASSERT(command == MAV_CMD_NAV_LOITER_UNLIM);
        QLOG_INFO() << "MAV_CMD_NAV_LOITER_UNLIM";

        int confirm = 1;    // [TODO] Verify This is what ArduPlane Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_NAV_RETURN_TO_LAUNCH: {
        /* Return to launch location |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
        Q_ASSERT(command == MAV_CMD_NAV_RETURN_TO_LAUNCH);
        QLOG_INFO() << "MAV_CMD_NAV_RETURN_TO_LAUNCH";

        int confirm = 1;    // [TODO] Verify This is what ArduPlane Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_CALIBRATION: {
        // Trigger calibration. This command will be only accepted if in pre-flight mode.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_CALIBRATION);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_CALIBRATION";

        if (preFlightWarningBox() == QMessageBox::Abort)
            return;

        int confirm = 1;
        float param1 = 1.0; // Gyro calibration: 0: no, 1: yes
        float param2 = 0.0; // Magnetometer calibration: 0: no, 1: yes
        float param3 = 0.0; // Ground pressure: 0: no, 1: yes
        float param4 = 0.0; // Radio calibration: 0: no, 1: yes
        float param5 = 0.0; // Accelerometer calibration: 0: no, 1: yes
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_MISSION_START: {
        // start running a mission last_item:
        Q_ASSERT(command == MAV_CMD_MISSION_START);
        QLOG_INFO() << "MAV_CMD_MISSION_START";

        int confirm = 1;
        float param1 = 1.0; // first_item: the first mission item to run
        float param2 = 0.0; // the last mission item to run (after this item is run, the mission ends)|
        float param3 = 0.0; // | Empty|
        float param4 = 0.0; // | Empty|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN: {
        // Request the reboot or shutdown of system components.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN";

        if (preFlightWarningBox() == QMessageBox::Abort)
            return;

        int confirm = 1;
        float param1 = 1.0; // | 0: Do nothing for autopilot 1: Reboot autopilot, 2: Shutdown autopilot.
        float param2 = 1.0; // | 0: Do nothing for onboard computer, 1: Reboot onboard computer, 2: Shutdown onboard computer.
        float param3 = 0.0; // | Reserved|
        float param4 = 0.0; // | Reserved|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    default:
        QLOG_INFO() << "sendApmPlaneCommand: Unknown Command " << command;
    }
}

void UASActionsWidget::sendApmCopterCommand(MAV_CMD command)
{
    switch(command) {

    case MAV_CMD_NAV_LOITER_UNLIM: {
        // Loiter around this MISSION an unlimited amount of time
        Q_ASSERT(command == MAV_CMD_NAV_LOITER_UNLIM);
        QLOG_INFO() << "MAV_CMD_NAV_LOITER_UNLIM";

        int confirm = 1;    // [TODO] Verify This is what ArduCopter Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_NAV_RETURN_TO_LAUNCH: {
        /* Return to launch location |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
        Q_ASSERT(command == MAV_CMD_NAV_RETURN_TO_LAUNCH);
        QLOG_INFO() << "MAV_CMD_NAV_RETURN_TO_LAUNCH";

        int confirm = 1;    // [TODO] Verify This is what ArduCopter Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_CALIBRATION: {
        // Trigger calibration. This command will be only accepted if in pre-flight mode.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_CALIBRATION);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_CALIBRATION";

        if (preFlightWarningBox() == QMessageBox::Abort)
            return;

        int confirm = 1;
        float param1 = 1.0; // Gyro calibration: 0: no, 1: yes
        float param2 = 0.0; // Magnetometer calibration: 0: no, 1: yes
        float param3 = 0.0; // Ground pressure: 0: no, 1: yes
        float param4 = 0.0; // Radio calibration: 0: no, 1: yes
        float param5 = 0.0; // Accelerometer calibration: 0: no, 1: yes
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_MISSION_START: {
        // start running a mission last_item:
        Q_ASSERT(command == MAV_CMD_MISSION_START);
        QLOG_INFO() << "MAV_CMD_MISSION_START";

        int confirm = 1;
        float param1 = 1.0; // first_item: the first mission item to run
        float param2 = 0.0; // the last mission item to run (after this item is run, the mission ends)|
        float param3 = 0.0; // | Empty|
        float param4 = 0.0; // | Empty|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN: {
        // Request the reboot or shutdown of system components.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN";

        if (preFlightWarningBox() == QMessageBox::Abort)
            return;

        int confirm = 1;
        float param1 = 1.0; // | 0: Do nothing for autopilot 1: Reboot autopilot, 2: Shutdown autopilot.
        float param2 = 1.0; // | 0: Do nothing for onboard computer, 1: Reboot onboard computer, 2: Shutdown onboard computer.
        float param3 = 0.0; // | Reserved|
        float param4 = 0.0; // | Reserved|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    default:
        QLOG_INFO() << "sendApmCopterCommand: Unknown Command " << command;
    }
}

void UASActionsWidget::sendApmRoverCommand(MAV_CMD command)
{
    // [TODO] :(
    QLOG_INFO() << "UASActionWidget::sendApmRoverCommand";
    QLOG_INFO() << "to be implemented";
}

void UASActionsWidget::setRTLMode()
{
    if(!activeUas())
        return;

    QLOG_INFO() << "UASActionsWidget::setRTLMode()";

    switch (m_uas->getAutopilotType()) {
        case MAV_AUTOPILOT_ARDUPILOTMEGA: {

            int idx = ui.modeComboBox->findText("RTL");
            int mode = ui.modeComboBox->itemData(idx).toInt();
            m_uas->setMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                           mode);
        } break;

        case MAV_AUTOPILOT_PX4: {
            // [TODO] PX4 flight controller go here
        }

        case MAV_AUTOPILOT_GENERIC:
        default: {
            // [TODO] Generic, and other flight controllers here (or own sections as above)
        }
    }
}

bool UASActionsWidget::activeUas()
{
    if (m_uas == NULL) {
        QLOG_ERROR() << "UASActionsWidget: Error: No Active UAS, please connnect";
        return false;
    }

    return true;
}

void UASActionsWidget::parameterChanged(int uas, int component, int parameterCount,
                                        int parameterId, QString parameterName, QVariant value)
{
    if((parameterName == "WPNAV_SPEED")|| (parameterName == "TRIM_ARSPD_CN")
        || (parameterName == "CRUISE_SPEED")){
        QLOG_DEBUG() << "UASAction setting speed spin box from " << parameterName;
        ui.speedDoubleSpinBox->setValue(value.toDouble()/100.0f);

    }
}

void UASActionsWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QLOG_DEBUG() << "contextMenuEvent";

    if(m_uas == NULL)
        return;

    APMShortcutModesDialog* dialog = new APMShortcutModesDialog();

    if(m_uas->isMultirotor()){
        setupApmCopterComboBox(dialog->opt1ComboBox());
        setupApmCopterComboBox(dialog->opt2ComboBox());
        setupApmCopterComboBox(dialog->opt3ComboBox());
        setupApmCopterComboBox(dialog->opt4ComboBox());
    } else if (m_uas->isFixedWing()) {
        setupApmPlaneComboBox(dialog->opt1ComboBox());
        setupApmPlaneComboBox(dialog->opt2ComboBox());
        setupApmPlaneComboBox(dialog->opt3ComboBox());
        setupApmPlaneComboBox(dialog->opt4ComboBox());
    } else if (m_uas->isGroundRover()){
        setupApmRoverComboBox(dialog->opt1ComboBox());
        setupApmRoverComboBox(dialog->opt2ComboBox());
        setupApmRoverComboBox(dialog->opt3ComboBox());
        setupApmRoverComboBox(dialog->opt4ComboBox());
    } else {
        // Do nothing.
        delete dialog;
        dialog = NULL;
        return;
    }

    dialog->opt1ComboBox()->setCurrentIndex(dialog->opt1ComboBox()->findText(ui.opt1ModeButton->text()));
    dialog->opt2ComboBox()->setCurrentIndex(dialog->opt2ComboBox()->findText(ui.opt2ModeButton->text()));
    dialog->opt3ComboBox()->setCurrentIndex(dialog->opt3ComboBox()->findText(ui.opt3ModeButton->text()));
    dialog->opt4ComboBox()->setCurrentIndex(dialog->opt4ComboBox()->findText(ui.opt4ModeButton->text()));

    if(dialog->exec() == QDialog::Accepted) {
        // Changes where made and OK'd
        ui.opt1ModeButton->setText(dialog->opt1ComboBox()->currentText());
        ui.opt2ModeButton->setText(dialog->opt2ComboBox()->currentText());
        ui.opt3ModeButton->setText(dialog->opt3ComboBox()->currentText());
        ui.opt4ModeButton->setText(dialog->opt4ComboBox()->currentText());
        configureModeButtonEnableDisable();
        saveApmSettings();
     } else {
        QLOG_DEBUG() << "No Shortcut mode changes made";
     }
}

void UASActionsWidget::configureModeButtonEnableDisable()
{
    if (ui.opt1ModeButton->text().compare("none",Qt::CaseInsensitive)==0){
        ui.opt1ModeButton->setVisible(false);
    } else {
        ui.opt1ModeButton->setVisible(true);
    }

    if (ui.opt2ModeButton->text().compare("none",Qt::CaseInsensitive)==0){
        ui.opt2ModeButton->setVisible(false);
    } else {
        ui.opt2ModeButton->setVisible(true);
    }

    if (ui.opt3ModeButton->text().compare("none",Qt::CaseInsensitive)==0){
        ui.opt3ModeButton->setVisible(false);
    } else {
        ui.opt3ModeButton->setVisible(true);
    }

    if (ui.opt4ModeButton->text().compare("none",Qt::CaseInsensitive)==0){
        ui.opt4ModeButton->setVisible(false);
    } else {
        ui.opt4ModeButton->setVisible(true);
    }
}

void UASActionsWidget::saveApmSettings()
{
    // Store settings
    QSettings settings;

    if(m_uas->isMultirotor()){
        settings.setValue("APM_MULTI_SHORTCUT_OPT1",ui.opt1ModeButton->text());
        settings.setValue("APM_MULTI_SHORTCUT_OPT2",ui.opt2ModeButton->text());
        settings.setValue("APM_MULTI_SHORTCUT_OPT3",ui.opt3ModeButton->text());
        settings.setValue("APM_MULTI_SHORTCUT_OPT4",ui.opt4ModeButton->text());

    } else if (m_uas->isFixedWing()){
        settings.setValue("APM_FIXED_SHORTCUT_OPT1",ui.opt1ModeButton->text());
        settings.setValue("APM_FIXED_SHORTCUT_OPT2",ui.opt2ModeButton->text());
        settings.setValue("APM_FIXED_SHORTCUT_OPT3",ui.opt3ModeButton->text());
        settings.setValue("APM_FIXED_SHORTCUT_OPT4",ui.opt4ModeButton->text());

    } else if (m_uas->isGroundRover()) {
        settings.setValue("APM_GROUND_SHORTCUT_OPT1",ui.opt1ModeButton->text());
        settings.setValue("APM_GROUND_SHORTCUT_OPT2",ui.opt2ModeButton->text());
        settings.setValue("APM_GROUND_SHORTCUT_OPT3",ui.opt3ModeButton->text());
        settings.setValue("APM_GROUND_SHORTCUT_OPT4",ui.opt4ModeButton->text());

    } else {
       // Nothing here to save
    }

    settings.sync();
}

void UASActionsWidget::loadApmSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();

    if(m_uas->isMultirotor()){
       if (settings.contains("APM_MULTI_SHORTCUT_OPT1")){
           QLOG_DEBUG() << "Mutli: opt1Mode: " << settings.value("APM_MULTI_SHORTCUT_OPT1").toString();
           QLOG_DEBUG() << "Mutli: opt2Mode: " << settings.value("APM_MULTI_SHORTCUT_OPT2").toString();
           QLOG_DEBUG() << "Mutli: opt3Mode: " << settings.value("APM_MULTI_SHORTCUT_OPT3").toString();
           QLOG_DEBUG() << "Mutli: opt4Mode: " << settings.value("APM_MULTI_SHORTCUT_OPT4").toString();
           ui.opt1ModeButton->setText(settings.value("APM_MULTI_SHORTCUT_OPT1").toString());
           ui.opt2ModeButton->setText(settings.value("APM_MULTI_SHORTCUT_OPT2").toString());
           ui.opt3ModeButton->setText(settings.value("APM_MULTI_SHORTCUT_OPT3").toString());
           ui.opt4ModeButton->setText(settings.value("APM_MULTI_SHORTCUT_OPT4").toString());
        }

    } else if (m_uas->isFixedWing()){
       if (settings.contains("APM_FIXED_SHORTCUT_OPT1")){
           QLOG_DEBUG() << "Fixed: opt1Mode: " << settings.value("APM_FIXED_SHORTCUT_OPT1").toString();
           QLOG_DEBUG() << "Fixed: opt2Mode: " << settings.value("APM_FIXED_SHORTCUT_OPT2").toString();
           QLOG_DEBUG() << "Fixed: opt3Mode: " << settings.value("APM_FIXED_SHORTCUT_OPT3").toString();
           QLOG_DEBUG() << "Fixed: opt4Mode: " << settings.value("APM_FIXED_SHORTCUT_OPT4").toString();
           ui.opt1ModeButton->setText(settings.value("APM_FIXED_SHORTCUT_OPT1").toString());
           ui.opt2ModeButton->setText(settings.value("APM_FIXED_SHORTCUT_OPT2").toString());
           ui.opt3ModeButton->setText(settings.value("APM_FIXED_SHORTCUT_OPT3").toString());
           ui.opt4ModeButton->setText(settings.value("APM_FIXED_SHORTCUT_OPT4").toString());
       }

     } else if(m_uas->isMultirotor()){
       if (settings.contains("APM_GROUND_SHORTCUT_OPT1")){
           QLOG_DEBUG() << "Ground: opt1Mode: " << settings.value("APM_GROUND_SHORTCUT_OPT1").toString();
           QLOG_DEBUG() << "Ground: opt2Mode: " << settings.value("APM_GROUND_SHORTCUT_OPT2").toString();
           QLOG_DEBUG() << "Ground: opt3Mode: " << settings.value("APM_GROUND_SHORTCUT_OPT3").toString();
           QLOG_DEBUG() << "Ground: opt4Mode: " << settings.value("APM_GROUND_SHORTCUT_OPT4").toString();

           ui.opt1ModeButton->setText(settings.value("APM_GROUND_SHORTCUT_OPT1").toString());
           ui.opt2ModeButton->setText(settings.value("APM_GROUND_SHORTCUT_OPT2").toString());
           ui.opt3ModeButton->setText(settings.value("APM_GROUND_SHORTCUT_OPT3").toString());
           ui.opt4ModeButton->setText(settings.value("APM_GROUND_SHORTCUT_OPT4").toString());
       }
     }
}

