#ifndef QGOODGAMESETTINGSDIALOG_H
#define QGOODGAMESETTINGSDIALOG_H
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

class QGoodGameSettingsDialog:public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    QGoodGameSettingsDialog(QWidget *parent = nullptr);
    ~QGoodGameSettingsDialog() override{}

    // QVirtualSettingsTab interface
public:
    QWidget *createLayout(QWidget *parent,QSettings& settings) override;
    void saveSettings(QSettings&settings) override;
    void loadSettings(QSettings&settings) override;
    void connectDialogToChat(QObject* chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

private:
    //tab control
    QVBoxLayout* goodgameLayout;
    QCheckBox *goodGameChannelCheckBox;
    QLineEdit *goodGameChannelEdit;
    QCheckBox *goodGameBadgesCheckBox;
    QCheckBox *goodGameUseAnimatedSmilesCheckBox;
    QLineEdit *goodGameAliasesEdit;
    QTextEdit *goodGameSupportersListEdit;
    QTextEdit *goodGameBlackListEdit;
    QCheckBox *goodGameRemoveBlackListUsersCheckBox;



signals:
    void goodGameChannelChanged();
    void goodGameChannelDisabled();
    void goodGameUseAnimatedSmilesChanged( bool );
    void goodGameAliasesChanged( const QString & );
    void goodGameSupportersListChanged( const QString & );
    void goodGameBlackListChanged( const QString & );
    void goodGameRemoveBlackListUsersChanged( bool );
    void goodGameBadgesChanged( bool );

};

#endif // QGOODGAMESETTINGSDIALOG_H


