#ifndef QTWITCHSETTINGSDIALOG_H
#define QTWITCHSETTINGSDIALOG_H


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

class QTwitchSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    QTwitchSettingsDialog(QWidget *parent = nullptr);

signals:
    void                twitchChannelChanged();
    void                twitchChannelDisabled();
    void                twitchAliasesChanged( const QString & );
    void                twitchSupportersListChanged( const QString & );
    void                twitchBlackListChanged( const QString & );
    void                twitchRemoveBlackListUsersChanged( bool );
    void                twitchOriginalColorsChanged( bool );
    void                twitchBadgesChanged( bool );

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
    QCheckBox *         twitchChannelCheckBox;
    QLineEdit *         twitchChannelEdit;
    QCheckBox *         twitchOriginalColorsCheckBox;
    QCheckBox *         twitchBadgesCheckBox;
    QLineEdit *         twitchAliasesEdit;
    QTextEdit *         twitchSupportersListEdit;
    QTextEdit *         twitchBlackListEdit;
    QCheckBox *         twitchRemoveBlackListUsersCheckBox;
};

#endif // QTWITCHSETTINGSDIALOG_H
