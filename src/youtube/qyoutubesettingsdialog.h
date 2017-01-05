#ifndef QYOTUBESETTINGSDIALOG_H
#define QYOTUBESETTINGSDIALOG_H


#include "../core/virtualsettingstab.h"
#include "youtubecontroller.h"

class QIcon;
class QString;
class QVBoxLayout;
class QCheckBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QGroupBox;
class QWidget;


class QYoutubeSettingsDialog:public QObject,public VirtualSettingsTab
{
    Q_OBJECT
public:
    QYoutubeSettingsDialog(QObject *parent = nullptr);
    ~QYoutubeSettingsDialog() override{}

    // QVirtualSettingsTab interface
public:
    QWidget *createLayout(QWidget *parent,QSettings& settings) override;
    void saveSettings(QSettings&settings) override;
    void loadSettings(QSettings&settings) override;

    //connect signals from chat' dialog to chat class' slots
    void connectDialogToChat(QObject* chat) override;

    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

private:
    //tab control
    QVBoxLayout* layout;
    //controls
    QCheckBox *channelCheckBox;
    QPushButton *youtubeLoginWebsitePushButton;
     QPushButton *youtubeDeloginWebsitePushButton;
    QLineEdit *channelEdit;
    QLineEdit *aliasesEdit;
    QTextEdit *supportersListEdit;
    QTextEdit *blackListEdit;
    QCheckBox *removeBlackListUsersCheckBox;
    QGroupBox *groupBox;   
private slots:
    void changeChannelAvailability(bool);



signals:

    void supportersListChanged( const QString & );
    void blackListChanged( const QString & );
    void removeBlackListUsersChanged( bool );
    void loginClicked();
    void deloginClicked();


};

#endif // QYOTUBESETTINGSDIALOG_H
