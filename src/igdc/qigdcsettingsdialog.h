#ifndef QIGDCSETTINGSDIALOG_H
#define QIGDCSETTINGSDIALOG_H
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

class QIgdcSettingsDialog:public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    QIgdcSettingsDialog(QWidget *parent = nullptr);

signals:
    void                igdcChannelChanged();
    void                igdcChannelDisabled();
    void                igdcAliasesChanged( const QString & );
    void                igdcSupportersListChanged( const QString & );
    void                igdcBlackListChanged( const QString & );
    void                igdcRemoveBlackListUsersChanged( bool );
    void                igdcBadgesChanged( bool );


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
    QCheckBox *         igdcChannelCheckBox;
    QLineEdit *         igdcChannelEdit;
    QCheckBox *         igdcBadgesCheckBox;
    QLineEdit *         igdcAliasesEdit;
    QTextEdit *         igdcSupportersListEdit;
    QTextEdit *         igdcBlackListEdit;
    QCheckBox *         igdcRemoveBlackListUsersCheckBox;
};

#endif // QIGDCSETTINGSDIALOG_H
