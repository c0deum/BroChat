#ifndef QBEAMPROSETTINGSDIALOG_H
#define QBEAMPROSETTINGSDIALOG_H
#include "../core/virtualsettingstab.h"

class QIcon;
class QString;
class QVBoxLayout;
class QCheckBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QGroupBox;
class QWidget;

class QBeamproSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QBeamproSettingsDialog(QWidget *parent = 0);

signals:    
    void                beamproChannelChanged();
    void                beamproChannelDisabled();
    void                beamproAliasesChanged( const QString & );
    void                beamproSupportersListChanged( const QString & );
    void                beamproBlackListChanged( const QString & );
    void                beamproRemoveBlackListUsersChanged( bool );

public slots:

    // VirtualSettingsTab interface
public:
    virtual QWidget *createLayout(QWidget *parent, QSettings &settings);
    virtual void saveSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual void connectDialogToChat(QObject *chat);
    virtual QIcon getIcon();
    virtual QString getName();
    virtual QObject *getAdditionalInfo();

private:
    QCheckBox *         beamproChannelCheckBox;
    QLineEdit *         beamproChannelEdit;
    QLineEdit *         beamproAliasesEdit;
    QTextEdit *         beamproSupportersListEdit;
    QTextEdit *         beamproBlackListEdit;
    QCheckBox *         beamproRemoveBlackListUsersCheckBox;
};

#endif // QBEAMPROSETTINGSDIALOG_H
