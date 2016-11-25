#ifndef QGAMERSTVSETTINGSDIALOG_H
#define QGAMERSTVSETTINGSDIALOG_H
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

class QGamerstvSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QGamerstvSettingsDialog(QWidget *parent = 0);

signals:
    void                gamerstvChannelChanged();
    void                gamerstvChannelDisabled();
    void                gamerstvAliasesChanged( const QString & );
    void                gamerstvSupportersListChanged( const QString & );
    void                gamerstvBlackListChanged( const QString & );
    void                gamerstvRemoveBlackListUsersChanged( bool );
    void                gamerstvBadgesChanged( bool );

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
    QCheckBox *         gamerstvChannelCheckBox;
    QLineEdit *         gamerstvChannelEdit;
    QCheckBox *         gamerstvBadgesCheckBox;
    QLineEdit *         gamerstvAliasesEdit;
    QTextEdit *         gamerstvSupportersListEdit;
    QTextEdit *         gamerstvBlackListEdit;
    QCheckBox *         gamerstvRemoveBlackListUsersCheckBox;
};

#endif // QGAMERSTVSETTINGSDIALOG_H
