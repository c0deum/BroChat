#ifndef QACESSETTINGSDIALOG_H
#define QACESSETTINGSDIALOG_H
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

class QAcesSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QAcesSettingsDialog(QWidget *parent = nullptr);

signals:
    void                acesChannelChanged();
    void                acesChannelDisabled();
    void                acesAliasesChanged( const QString & );
    void                acesSupportersListChanged( const QString & );
    void                acesBlackListChanged( const QString & );
    //void              acesOriginalColorsChanged( bool originalColors );
    void                acesRemoveBlackListUsersChanged( bool );

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
    QCheckBox *         acesChannelCheckBox;
    QLineEdit *         acesChannelEdit;
    //QCheckBox *       acesOriginalColorsCheckBox;
    QLineEdit *         acesAliasesEdit;
    QTextEdit *         acesSupportersListEdit;
    QTextEdit *         acesBlackListEdit;
    QCheckBox *         acesRemoveBlackListUsersCheckBox;
};

#endif // QACESSETTINGSDIALOG_H
