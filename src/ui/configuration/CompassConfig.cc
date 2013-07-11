#include "CompassConfig.h"


CompassConfig::CompassConfig(QWidget *parent) : QWidget(parent)
{
    m_uas=0;
    ui.setupUi(this);
    ui.autoDecCheckBox->setEnabled(false);
    ui.enableCheckBox->setEnabled(false);
    ui.orientationComboBox->setEnabled(false);
    ui.declinationLineEdit->setEnabled(false);
    connect(ui.enableCheckBox,SIGNAL(clicked(bool)),this,SLOT(enableClicked(bool)));
    connect(ui.autoDecCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoDecClicked(bool)));
    connect(ui.orientationComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(orientationComboChanged(int)));

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());

}
CompassConfig::~CompassConfig()
{
}
void CompassConfig::activeUASSet(UASInterface *uas)
{
    if (!uas) return;
    if (!m_uas)
    {
        disconnect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),this,SLOT(parameterChanged(int,int,QString,QVariant)));
    }
    m_uas = uas;
    connect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),this,SLOT(parameterChanged(int,int,QString,QVariant)));
}
void CompassConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName == "MAG_ENABLE")
    {
        if (value.toInt() == 0)
        {
            ui.enableCheckBox->setChecked(false);
            ui.autoDecCheckBox->setEnabled(false);
            ui.declinationLineEdit->setEnabled(false);
        }
        else
        {
            ui.enableCheckBox->setChecked(true);
            ui.autoDecCheckBox->setEnabled(true);
            ui.declinationLineEdit->setEnabled(true);
        }
        ui.enableCheckBox->setEnabled(true);
    }
    else if (parameterName == "COMPASS_AUTODEC")
    {
        if (value.toInt() == 0)
        {
            ui.autoDecCheckBox->setChecked(false);
        }
        else
        {
            ui.autoDecCheckBox->setChecked(true);
        }
    }
    else if (parameterName == "COMPASS_DEC")
    {
        ui.declinationLineEdit->setText(QString::number(value.toDouble()));
    }
}

void CompassConfig::enableClicked(bool enabled)
{
    if (m_uas)
    {
        if (enabled)
        {
            m_uas->getParamManager()->setParameter(1,"MAG_ENABLE",QVariant(1));
            ui.autoDecCheckBox->setEnabled(true);
            if (!ui.autoDecCheckBox->isChecked())
            {
                ui.declinationLineEdit->setEnabled(true);
            }
        }
        else
        {
            m_uas->getParamManager()->setParameter(1,"MAG_ENABLE",QVariant(0));
            ui.autoDecCheckBox->setEnabled(false);
            ui.declinationLineEdit->setEnabled(false);
        }
    }
}

void CompassConfig::autoDecClicked(bool enabled)
{
    if (m_uas)
    {
        if (enabled)
        {
            m_uas->getParamManager()->setParameter(1,"COMPASS_AUTODEC",QVariant(1));
        }
        else
        {
            m_uas->getParamManager()->setParameter(1,"COMPASS_AUTODEC",QVariant(0));
        }
    }
}

void CompassConfig::orientationComboChanged(int index)
{

}