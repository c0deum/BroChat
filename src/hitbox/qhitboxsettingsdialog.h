#ifndef QHITBOXSETTINGSDIALOG_H
#define QHITBOXSETTINGSDIALOG_H
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

class QHitboxSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QHitboxSettingsDialog(QWidget *parent = nullptr);

signals:
    void                hitboxChannelChanged();
    void                hitboxChannelDisabled();
    void                hitboxAliasesChanged( const QString & );
    void                hitboxSupportersListChanged( const QString & );
    void                hitboxBlackListChanged( const QString & );
    void                hitboxRemoveBlackListUsersChanged( bool );
    void                hitboxOriginalColorsChanged( bool );

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
    QCheckBox *         hitboxChannelCheckBox;
    QLineEdit *         hitboxChannelEdit;
    QCheckBox *         hitboxOriginalColorsCheckBox;
    QLineEdit *         hitboxAliasesEdit;
    QTextEdit *         hitboxSupportersListEdit;
    QTextEdit *         hitboxBlackListEdit;
    QCheckBox *         hitboxRemoveBlackListUsersCheckBox;
};

#endif // QHITBOXSETTINGSDIALOG_H
