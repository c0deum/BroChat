#ifndef QGOODGAMESETTINGSDIALOG_H
#define QGOODGAMESETTINGSDIALOG_H
#include "..\core\QVirtualSettingsTab.h"


class QIcon;
class QString;
class QVBoxLayout;
class QCheckBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QGroupBox;
class QWidget;

class QGoodGameSettingsDialog:public QObject,public VirtualSettingsTab
{
    Q_OBJECT
public:
    QGoodGameSettingsDialog(QObject *parent = nullptr);
    virtual ~QGoodGameSettingsDialog(){}

    // QVirtualSettingsTab interface
public:
    virtual QWidget *createLayout(QWidget *parent,QSettings& settings);
    virtual void saveSettings(QSettings&settings);
    virtual void loadSettings(QSettings&settings);
    virtual QIcon getIcon();
    virtual QString getName();
    virtual QObject *getAdditionalInfo();

private:
    //tab control
    QVBoxLayout* goodgameLayout;
    QCheckBox *goodGameChannelCheckBox;
    QLineEdit *goodGameChannelEdit;
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

};

#endif // QGOODGAMESETTINGSDIALOG_H


