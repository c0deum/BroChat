#ifndef QTWITCHSETTINGSDIALOG_H
#define QTWITCHSETTINGSDIALOG_H


#include "../core/qvirtualsettingstab.h"

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
    virtual QWidget *createLayout(QWidget *parent, QSettings &settings);
    virtual void saveSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual void connectDialogToChat(QObject *chat);
    virtual QIcon getIcon();
    virtual QString getName();
    virtual QObject *getAdditionalInfo();
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
