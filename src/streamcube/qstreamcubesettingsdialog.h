#ifndef QSTREAMCUBESETTINGSDIALOG_H
#define QSTREAMCUBESETTINGSDIALOG_H

#include <QObject>
#include <QWidget>
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



class QStreamCubeSettingsDialog : public QWidget,public VirtualSettingsTab
{
    Q_OBJECT
public:
    explicit QStreamCubeSettingsDialog(QWidget *parent = 0);

signals:
    void                streamcubeChannelChanged();
    void                streamcubeChannelDisabled();
    void                streamcubeAliasesChanged( const QString & );
    void                streamcubeSupportersListChanged( const QString & );
    void                streamcubeBlackListChanged( const QString & );
    void                streamcubeRemoveBlackListUsersChanged( bool );
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
    QCheckBox *         streamcubeChannelCheckBox;
    QLineEdit *         streamcubeChannelEdit;
    QLineEdit *         streamcubeAliasesEdit;
    QTextEdit *         streamcubeSupportersListEdit;
    QTextEdit *         streamcubeBlackListEdit;
    QCheckBox *         streamcubeRemoveBlackListUsersCheckBox;
};

#endif // QSTREAMCUBESETTINGSDIALOG_H
