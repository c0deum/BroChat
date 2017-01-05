#ifndef QGIPSYTEAMSETTINGSDIALOG_H
#define QGIPSYTEAMSETTINGSDIALOG_H
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

class QGipsyteamSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QGipsyteamSettingsDialog(QWidget *parent = nullptr);

signals:
    void                gipsyteamChannelChanged();
    void                gipsyteamChannelDisabled();
    void                gipsyteamAliasesChanged( const QString & );
    void                gipsyteamSupportersListChanged( const QString & );
    void                gipsyteamBlackListChanged( const QString & );
    void                gipsyteamRemoveBlackListUsersChanged( bool );
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
    QCheckBox *         gipsyteamChannelCheckBox;
    QLineEdit *         gipsyteamChannelEdit;
    QLineEdit *         gipsyteamAliasesEdit;
    QTextEdit *         gipsyteamSupportersListEdit;
    QTextEdit *         gipsyteamBlackListEdit;
    QCheckBox *         gipsyteamRemoveBlackListUsersCheckBox;
};

#endif // QGIPSYTEAMSETTINGSDIALOG_H
