#ifndef QAZUBUSETTINGSDIALOG_H
#define QAZUBUSETTINGSDIALOG_H
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

class QAzubuSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QAzubuSettingsDialog(QWidget *parent = nullptr);

signals:
    void                azubuChannelChanged();
    void                azubuChannelDisabled();
    void                azubuAliasesChanged( const QString & );
    void                azubuSupportersListChanged( const QString & );
    void                azubuBlackListChanged( const QString & );
    void                azubuRemoveBlackListUsersChanged( bool );

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
    QCheckBox *         azubuChannelCheckBox;
    QLineEdit *         azubuChannelEdit;
    QLineEdit *         azubuAliasesEdit;
    QTextEdit *         azubuSupportersListEdit;
    QTextEdit *         azubuBlackListEdit;
    QCheckBox *         azubuRemoveBlackListUsersCheckBox;
};

#endif // QAZUBUSETTINGSDIALOG_H
