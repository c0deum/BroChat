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
    QWidget *createLayout(QWidget *parent, QSettings &settings) override;
    void saveSettings(QSettings &settings) override;
    void loadSettings(QSettings &settings) override;
    void connectDialogToChat(QObject *chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;
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
