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
    virtual QWidget *createLayout(QWidget *parent, QSettings &settings);
    virtual void saveSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual void connectDialogToChat(QObject *chat);
    virtual QIcon getIcon();
    virtual QString getName();
    virtual QObject *getAdditionalInfo();
private:
    QCheckBox *         vidiChannelCheckBox;
    QLineEdit *         vidiChannelEdit;
    QLineEdit *         vidiAliasesEdit;
    QTextEdit *         vidiSupportersListEdit;
    QTextEdit *         vidiBlackListEdit;
    QCheckBox *         vidiRemoveBlackListUsersCheckBox;
};

#endif // QVIDISETTINGSDIALOG_H
