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
    QWidget *createLayout(QWidget *parent, QSettings &settings) override;
    void saveSettings(QSettings &settings) override;
    void loadSettings(QSettings &settings) override;
    void connectDialogToChat(QObject *chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

private:
    QCheckBox *         beamproChannelCheckBox;
    QLineEdit *         beamproChannelEdit;
    QLineEdit *         beamproAliasesEdit;
    QTextEdit *         beamproSupportersListEdit;
    QTextEdit *         beamproBlackListEdit;
    QCheckBox *         beamproRemoveBlackListUsersCheckBox;
};

#endif // QBEAMPROSETTINGSDIALOG_H
