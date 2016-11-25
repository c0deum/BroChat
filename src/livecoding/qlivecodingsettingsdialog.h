#ifndef QLIVECODINGSETTINGSDIALOG_H
#define QLIVECODINGSETTINGSDIALOG_H

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

class QLivecodingSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QLivecodingSettingsDialog(QWidget *parent = nullptr);

signals:
    void                livecodingChannelChanged();
    void                livecodingChannelDisabled();
    void                livecodingAliasesChanged( const QString & );
    void                livecodingSupportersListChanged( const QString & );
    void                livecodingBlackListChanged( const QString & );
    void                livecodingRemoveBlackListUsersChanged( bool );
    void                livecodingBadgesChanged( bool );

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
    QCheckBox *         livecodingChannelCheckBox;
    QLineEdit *         livecodingChannelEdit;
    QCheckBox *         livecodingBadgesCheckBox;
    QLineEdit *         livecodingLoginEdit;
    QLineEdit *         livecodingPasswordEdit;
    QLineEdit *         livecodingAliasesEdit;
    QTextEdit *         livecodingSupportersListEdit;
    QTextEdit *         livecodingBlackListEdit;
    QCheckBox *         livecodingRemoveBlackListUsersCheckBox;
};

#endif // QLIVECODINGSETTINGSDIALOG_H
