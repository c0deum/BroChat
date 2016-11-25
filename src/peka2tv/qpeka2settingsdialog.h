#ifndef QPEKA2SETTINGSDIALOG_H
#define QPEKA2SETTINGSDIALOG_H

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

class QPeka2SettingsDialog:public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    QPeka2SettingsDialog(QWidget *parent = nullptr);

signals:
    void                peka2ChannelChanged();
    void                peka2ChannelDisabled();
    void                peka2AliasesChanged( const QString & );
    void                peka2SupportersListChanged( const QString & );
    void                peka2BlackListChanged( const QString & );
    void                peka2RemoveBlackListUsersChanged( bool );
    void                peka2OriginalColorsChanged( bool );
    void                peka2BadgesChanged( bool );

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
    QCheckBox *         peka2ChannelCheckBox;
    QLineEdit *         peka2ChannelEdit;
    QCheckBox *         peka2OriginalColorsCheckBox;
    QCheckBox *         peka2BadgesCheckBox;
    QLineEdit *         peka2AliasesEdit;
    QTextEdit *         peka2SupportersListEdit;
    QTextEdit *         peka2BlackListEdit;
    QCheckBox *         peka2RemoveBlackListUsersCheckBox;
};

#endif // QPEKA2SETTINGSDIALOG_H
