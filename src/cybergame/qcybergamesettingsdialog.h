#ifndef QCYBERGAMESETTINGSDIALOG_H
#define QCYBERGAMESETTINGSDIALOG_H
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

class QCybergameSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QCybergameSettingsDialog(QWidget *parent = 0);

signals:
    void                cyberGameChannelChanged();
    void                cyberGameChannelDisabled();
    void                cyberGameAliasesChanged( const QString & );
    void                cyberGameSupportersListChanged( const QString & );
    void                cyberGameBlackListChanged( const QString & );
    void                cyberGameRemoveBlackListUsersChanged( bool );

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
    QCheckBox *         cyberGameChannelCheckBox;
    QLineEdit *         cyberGameChannelEdit;
    QLineEdit *         cyberGameAliasesEdit;
    QTextEdit *         cyberGameSupportersListEdit;
    QTextEdit *         cyberGameBlackListEdit;
    QCheckBox *         cyberGameRemoveBlackListUsersCheckBox;

};

#endif // QCYBERGAMESETTINGSDIALOG_H
