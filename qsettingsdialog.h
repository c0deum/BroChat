#ifndef QSETTINGSDIALOG_H
#define QSETTINGSDIALOG_H

#include <QDialog>

class QLineEdit;
class QTextEdit;
class QComboBox;
class QSlider;
class QCheckBox;
class QSpinBox;
class QLabel;

class QColorDialog;

class QSettingsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit QSettingsDialog( QWidget *parent = 0 );
    virtual ~QSettingsDialog();
private:
    void setColorButtonStyle( QPushButton *button, QRgb color, int fontSize = 16 );
private slots:
    void nicknamesColorSelection();
    void messagesColorSelection();
    void viewersColorSelection();
    void aliasesColorSelection();
public slots:
    void saveSettings();
signals:
    void opacityChanged();
    void styleChanged();
    void flagsOrAttributesChanged();

    void languageChanged();

    void showSystemMessagesChanged();
    void showImagesChanged();
    void useServerStateChanged();
    void saveToFileStateChanged();

    void acesChannelChanged();
    void acesChannelDisabled();
    void acesAliasesChanged( const QString & );
    void acesSupportersListChanged( const QString & );
    void acesBlackListChanged( const QString & );
    //void acesOriginalColorsChanged( bool originalColors );
    void acesRemoveBlackListUsersChanged( bool );

    void cyberGameChannelChanged();
    void cyberGameChannelDisabled();
    void cyberGameAliasesChanged( const QString & );
    void cyberGameSupportersListChanged( const QString & );
    void cyberGameBlackListChanged( const QString & );
    void cyberGameRemoveBlackListUsersChanged( bool );

    void funstreamChannelChanged();
    void funstreamChannelDisabled();
    void funstreamAliasesChanged( const QString & );
    void funstreamSupportersListChanged( const QString & );
    void funstreamBlackListChanged( const QString & );
    void funstreamRemoveBlackListUsersChanged( bool );

    void gamerstvChannelChanged();
    void gamerstvChannelDisabled();
    void gamerstvAliasesChanged( const QString & );
    void gamerstvSupportersListChanged( const QString & );
    void gamerstvBlackListChanged( const QString & );
    void gamerstvRemoveBlackListUsersChanged( bool );

    void gipsyteamChannelChanged();
    void gipsyteamChannelDisabled();
    void gipsyteamAliasesChanged( const QString & );
    void gipsyteamSupportersListChanged( const QString & );
    void gipsyteamBlackListChanged( const QString & );
    void gipsyteamRemoveBlackListUsersChanged( bool );

    void goodGameChannelChanged();
    void goodGameChannelDisabled();
    void goodGameUseAnimatedSmilesChanged( bool );
    void goodGameAliasesChanged( const QString & );
    void goodGameSupportersListChanged( const QString & );
    void goodGameBlackListChanged( const QString & );
    void goodGameRemoveBlackListUsersChanged( bool );

    void hitboxChannelChanged();
    void hitboxChannelDisabled();
    void hitboxAliasesChanged( const QString & );
    void hitboxSupportersListChanged( const QString & );
    void hitboxBlackListChanged( const QString & );
    void hitboxRemoveBlackListUsersChanged( bool );
    void hitboxOriginalColorsChanged( bool );


    void livecodingChannelChanged();
    void livecodingChannelDisabled();
    void livecodingAliasesChanged( const QString & );
    void livecodingSupportersListChanged( const QString & );
    void livecodingBlackListChanged( const QString & );
    void livecodingRemoveBlackListUsersChanged( bool );


    void realltvChannelChanged();
    void realltvChannelDisabled();
    void realltvAliasesChanged( const QString & );
    void realltvSupportersListChanged( const QString & );
    void realltvBlackListChanged( const QString & );
    void realltvRemoveBlackListUsersChanged( bool );


    void sc2tvChannelChanged();
    void sc2tvChannelDisabled();
    void sc2tvAliasesChanged( const QString & );
    void sc2tvSupportersListChanged( const QString & );
    void sc2tvBlackListChanged( const QString & );
    void sc2tvRemoveBlackListUsersChanged( bool );
    void sc2tvOriginalColorsChanged( bool );

    void streamboxChannelChanged();
    void streamboxChannelDisabled();
    void streamboxAliasesChanged( const QString & );
    void streamboxSupportersListChanged( const QString & );
    void streamboxBlackListChanged( const QString & );
    void streamboxRemoveBlackListUsersChanged( bool );

    void twitchChannelChanged();
    void twitchChannelDisabled();
    void twitchAliasesChanged( const QString & );
    void twitchSupportersListChanged( const QString & );
    void twitchBlackListChanged( const QString & );
    void twitchRemoveBlackListUsersChanged( bool );

    void youtubeChannelChanged();
    void youtubeChannelDisabled();
    void youtubeAliasesChanged( const QString & );
    void youtubeSupportersListChanged( const QString & );
    void youtubeBlackListChanged( const QString & );
    void youtubeRemoveBlackListUsersChanged( bool );
private:
    QRgb nicknamesTextColor;
    QRgb messagesTextColor;
    QRgb viewersTextColor;
    QRgb aliasesTextColor;

    QComboBox *languageCombo;
    QCheckBox *stayOnTopCheckBox;
    QCheckBox *frameLessWindowCheckBox;
    //QCheckBox *transparentWindowCheckBox;
    QComboBox *styleCombo;
    QSlider   *opacitySlider;

    QCheckBox *showSystemMessagesCheckBox;

    QCheckBox *showImagesCheckBox;

    QCheckBox *useServerCheckBox;
    QSpinBox  *serverPortSpinBox;

    QCheckBox *saveToFileCheckBox;
    QSpinBox  *saveToFileIntervalSpinBox;

    //default styles

    QComboBox *fontNameCombo;

    QSpinBox *nicknamesFontSizeSpinBox;
    QPushButton *nicknamesTextColorButton;

    QSpinBox *messagesFontSizeSpinBox;
    QPushButton *messagesTextColorButton;

    QSpinBox *viewersFontSizeSpinBox;
    QPushButton *viewersTextColorButton;

    QSpinBox *aliasesFontSizeSpinBox;
    QPushButton *aliasesTextColorButton;

    QSpinBox *smilesSizeSpinBox;
    QSpinBox *serviceIconsSizeSpinBox;

    QComboBox *animationTypeCombo;

    //channels

    QCheckBox *acesChannelCheckBox;
    QLineEdit *acesChannelEdit;
    //QCheckBox *acesOriginalColorsCheckBox;
    QLineEdit *acesAliasesEdit;
    QTextEdit *acesSupportersListEdit;
    QTextEdit *acesBlackListEdit;
    QCheckBox *acesRemoveBlackListUsersCheckBox;

    QCheckBox *cyberGameChannelCheckBox;
    QLineEdit *cyberGameChannelEdit;
    QLineEdit *cyberGameAliasesEdit;
    QTextEdit *cyberGameSupportersListEdit;
    QTextEdit *cyberGameBlackListEdit;
    QCheckBox *cyberGameRemoveBlackListUsersCheckBox;

    QCheckBox *funstreamChannelCheckBox;
    QLineEdit *funstreamChannelEdit;
    QLineEdit *funstreamAliasesEdit;
    QTextEdit *funstreamSupportersListEdit;
    QTextEdit *funstreamBlackListEdit;
    QCheckBox *funstreamRemoveBlackListUsersCheckBox;

    QCheckBox *gamerstvChannelCheckBox;
    QLineEdit *gamerstvChannelEdit;
    QLineEdit *gamerstvAliasesEdit;
    QTextEdit *gamerstvSupportersListEdit;
    QTextEdit *gamerstvBlackListEdit;
    QCheckBox *gamerstvRemoveBlackListUsersCheckBox;

    QCheckBox *gipsyteamChannelCheckBox;
    QLineEdit *gipsyteamChannelEdit;
    QLineEdit *gipsyteamAliasesEdit;
    QTextEdit *gipsyteamSupportersListEdit;
    QTextEdit *gipsyteamBlackListEdit;
    QCheckBox *gipsyteamRemoveBlackListUsersCheckBox;

    QCheckBox *goodGameChannelCheckBox;
    QLineEdit *goodGameChannelEdit;
    QCheckBox *goodGameUseAnimatedSmilesCheckBox;
    QLineEdit *goodGameAliasesEdit;
    QTextEdit *goodGameSupportersListEdit;
    QTextEdit *goodGameBlackListEdit;
    QCheckBox *goodGameRemoveBlackListUsersCheckBox;

    QCheckBox *hitboxChannelCheckBox;
    QLineEdit *hitboxChannelEdit;
    QCheckBox *hitboxOriginalColorsCheckBox;
    QLineEdit *hitboxAliasesEdit;
    QTextEdit *hitboxSupportersListEdit;
    QTextEdit *hitboxBlackListEdit;
    QCheckBox *hitboxRemoveBlackListUsersCheckBox;


    QCheckBox *livecodingChannelCheckBox;
    QLineEdit *livecodingChannelEdit;
    QLineEdit *livecodingAliasesEdit;
    QTextEdit *livecodingSupportersListEdit;
    QTextEdit *livecodingBlackListEdit;
    QCheckBox *livecodingRemoveBlackListUsersCheckBox;


    QCheckBox *realltvChannelCheckBox;
    QLineEdit *realltvChannelEdit;
    QLineEdit *realltvAliasesEdit;
    QTextEdit *realltvSupportersListEdit;
    QTextEdit *realltvBlackListEdit;
    QCheckBox *realltvRemoveBlackListUsersCheckBox;


    QCheckBox *sc2tvChannelCheckBox;
    QLineEdit *sc2tvChannelEdit;
    QCheckBox *sc2tvOriginalColorsCheckBox;
    QLineEdit *sc2tvAliasesEdit;
    QTextEdit *sc2tvSupportersListEdit;
    QTextEdit *sc2tvBlackListEdit;
    QCheckBox *sc2tvRemoveBlackListUsersCheckBox;

    QCheckBox *streamboxChannelCheckBox;
    QLineEdit *streamboxChannelEdit;
    QLineEdit *streamboxAliasesEdit;
    QTextEdit *streamboxSupportersListEdit;
    QTextEdit *streamboxBlackListEdit;
    QCheckBox *streamboxRemoveBlackListUsersCheckBox;

    QCheckBox *twitchChannelCheckBox;
    QLineEdit *twitchChannelEdit;
    QLineEdit *twitchAliasesEdit;
    QTextEdit *twitchSupportersListEdit;
    QTextEdit *twitchBlackListEdit;
    QCheckBox *twitchRemoveBlackListUsersCheckBox;

    QCheckBox *youtubeChannelCheckBox;
    QLineEdit *youtubeChannelEdit;
    QLineEdit *youtubeAliasesEdit;
    QTextEdit *youtubeSupportersListEdit;
    QTextEdit *youtubeBlackListEdit;
    QCheckBox *youtubeRemoveBlackListUsersCheckBox;
};

#endif // QSETTINGSDIALOG_H
