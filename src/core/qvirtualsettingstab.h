#ifndef QVIRTUALSETTINGSTAB_H
#define QVIRTUALSETTINGSTAB_H

#include <QIcon>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

class QSettings;
class QWidget;


class VirtualSettingsTab
{

public:
    VirtualSettingsTab(){}
    virtual ~VirtualSettingsTab() = 0;

    virtual QWidget* createLayout(QWidget * parent,QSettings& settings) = 0;
    virtual void saveSettings(QSettings& settings) = 0;
    virtual void loadSettings(QSettings& settings) = 0;

    void addWidgets( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets )
    {
        for( auto widget : widgets )
        {
            layout->addWidget( widget );
        }
    }


    //tab properties, icon and name and
    //additional info (for possible future usage)
    virtual QIcon getIcon() = 0;
    virtual QString getName() = 0;
    virtual QObject* getAdditionalInfo() = 0;

};
inline VirtualSettingsTab::~VirtualSettingsTab(){}

#endif // QVIRTUALSETTINGSTAB_H
