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
    virtual QWidget *createLayout(QWidget *parent, QSettings &settings);
    virtual void saveSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual void connectDialogToChat(QObject *chat);
    virtual QIcon getIcon();
    virtual QString getName();
    virtual QObject *getAdditionalInfo();

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
