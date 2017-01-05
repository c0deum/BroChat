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
    QWidget *createLayout(QWidget *parent, QSettings &settings) override;
    void saveSettings(QSettings &settings) override;
    void loadSettings(QSettings &settings) override;
    void connectDialogToChat(QObject *chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

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
