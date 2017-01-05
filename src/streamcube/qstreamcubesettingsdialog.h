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
    QWidget *createLayout(QWidget *parent, QSettings &settings) override;
    void saveSettings(QSettings &settings) override;
    void loadSettings(QSettings &settings) override;
    void connectDialogToChat(QObject *chat) override;
    QIcon getIcon() override;
    QString getName() override;
    QObject *getAdditionalInfo() override;

private:
    QCheckBox *         streamcubeChannelCheckBox;
    QLineEdit *         streamcubeChannelEdit;
    QLineEdit *         streamcubeAliasesEdit;
    QTextEdit *         streamcubeSupportersListEdit;
    QTextEdit *         streamcubeBlackListEdit;
    QCheckBox *         streamcubeRemoveBlackListUsersCheckBox;
};

#endif // QSTREAMCUBESETTINGSDIALOG_H
