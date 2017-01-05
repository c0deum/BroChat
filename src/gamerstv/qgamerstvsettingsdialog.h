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
    QWidget *createLayout(QWidget *parent, QSettings &settings) override;
    void saveSettings(QSettings &settings) override;
    void loadSettings(QSettings &settings) override;
    void connectDialogToChat(QObject *chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

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
