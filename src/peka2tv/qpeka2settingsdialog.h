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
    QWidget *createLayout(QWidget *parent, QSettings &settings) override;
    void saveSettings(QSettings &settings) override;
    void loadSettings(QSettings &settings) override;
    void connectDialogToChat(QObject *chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

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
