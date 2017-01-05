#ifndef QVIDISETTINGSDIALOG_H
#define QVIDISETTINGSDIALOG_H

class QIcon;
class QString;
class QVBoxLayout;
class QCheckBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QGroupBox;
class QWidget;


#include "../core/virtualsettingstab.h"

class QVidiSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QVidiSettingsDialog(QWidget *parent = 0);

signals:
    void                vidiChannelChanged();
    void                vidiChannelDisabled();
    void                vidiAliasesChanged( const QString & );
    void                vidiSupportersListChanged( const QString & );
    void                vidiBlackListChanged( const QString & );
    void                vidiRemoveBlackListUsersChanged( bool );

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
    QCheckBox *         vidiChannelCheckBox;
    QLineEdit *         vidiChannelEdit;
    QLineEdit *         vidiAliasesEdit;
    QTextEdit *         vidiSupportersListEdit;
    QTextEdit *         vidiBlackListEdit;
    QCheckBox *         vidiRemoveBlackListUsersCheckBox;
};

#endif // QVIDISETTINGSDIALOG_H
