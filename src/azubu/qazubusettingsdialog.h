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
    virtual QWidget *createLayout(QWidget *parent, QSettings &settings);
    virtual void saveSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual void connectDialogToChat(QObject *chat);
    virtual QIcon getIcon();
    virtual QString getName();
    virtual QObject *getAdditionalInfo();

private:
    QCheckBox *         azubuChannelCheckBox;
    QLineEdit *         azubuChannelEdit;
    QLineEdit *         azubuAliasesEdit;
    QTextEdit *         azubuSupportersListEdit;
    QTextEdit *         azubuBlackListEdit;
    QCheckBox *         azubuRemoveBlackListUsersCheckBox;
};

#endif // QAZUBUSETTINGSDIALOG_H
