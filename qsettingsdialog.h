#ifndef QSETTINGSDIALOG_H
#define QSETTINGSDIALOG_H

#include <QDialog>

class QLineEdit;
class QTextEdit;
class QComboBox;
class QSlider;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QLabel;

class QColorDialog;

class QScrollArea;

class QTabWidget;

class QVBoxLayout;
class QHBoxLayout;

class QSettingsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit            QSettingsDialog( QWidget * parent = nullptr );
    virtual             ~QSettingsDialog();
private:
    void                loadDialogSettings();
    void                setupWidgets();
    void                setupGeneralTab();
    void                setupDefaultStyleTab();
    void                setupAcesTab();
    void                setupCybergameTab();
    void                setupFunstreamTab();
    void                setupGamerstvTab();
    void                setupGipsyteamTab();
    void                setupGoodgameTab();
    void                setupHitboxTab();
    void                setupIgdcTab();
    void                setupLivecodingTab();
    void                setupRealltvTab();
    void                setupSc2tvTab();
    void                setupStreamboxTab();
    void                setupTwitchTab();
    void                setupYoutubetab();

    void                setColorButtonStyle( QPushButton * button, QRgb color, int fontSize = 16 );
    void                buttonColorSelection( QPushButton * button, QRgb & color );

    void                addLayoutRow( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets );
    void                addLayoutRow( QVBoxLayout * layout, std::initializer_list< QVBoxLayout * > layouts );
    void                addWidgets( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets );
    void                addLayouts( QHBoxLayout * layout, std::initializer_list< QVBoxLayout * > layouts );

    void                setupSpinBox( QSpinBox * spinBox, int min, int max, int defaultValue, const QString & settingPath );
    void                setupColorButton( QPushButton * button, QRgb & color, unsigned int defaultColor, const QString & settingPath );


private slots:
    void                bodyBackgroundColorSelection();

    void                evenMessagesNicknamesColorSelection();
    void                oddMessagesNicknamesColorSelection();
    void                aliasesMessagesNicknamesColorSelection();
    void                supportersMessagesNicknamesColorSelection();
    void                blackListMessagesNicknamesColorSelection();

    void                evenMessagesBackgroundColorSelection();
    void                oddMessagesBackgroundColorSelection();
    void                aliasesBackgroundColorSelection();
    void                supportersBackgroundColorSelection();
    void                blackListBackgroundColorSelection();

    void                evenMessagesTextColorSelection();
    void                oddMessagesTextColorSelection();
    void                aliasesMessagesTextColorSelection();
    void                supportersMessagesTextColorSelection();
    void                blackListMessagesTextColorSelection();

    void                evenMessagesBorderColorSelection();
    void                oddMessagesBorderColorSelection();
    void                aliasesMessagesBorderColorSelection();
    void                supportersMessagesBorderColorSelection();
    void                blackListMessagesBorderColorSelection();

    void                viewersColorSelection();
    void                viewersBackgroundColorSelection();

    void                linksColorSelection();

public slots:
    void                saveSettings();
    void                saveDialogSettings();

signals:
    void                opacityChanged();
    void                styleChanged();
    void                flagsOrAttributesChanged();

    void                languageChanged();

    void                showSystemMessagesChanged();
    void                showImagesChanged();
    void                useServerStateChanged();
    void                saveToFileStateChanged();
    void                saveMessagesToLogChanged();

    void                acesChannelChanged();
    void                acesChannelDisabled();
    void                acesAliasesChanged( const QString & );
    void                acesSupportersListChanged( const QString & );
    void                acesBlackListChanged( const QString & );
    //void              acesOriginalColorsChanged( bool originalColors );
    void                acesRemoveBlackListUsersChanged( bool );

    void                cyberGameChannelChanged();
    void                cyberGameChannelDisabled();
    void                cyberGameAliasesChanged( const QString & );
    void                cyberGameSupportersListChanged( const QString & );
    void                cyberGameBlackListChanged( const QString & );
    void                cyberGameRemoveBlackListUsersChanged( bool );

    void                funstreamChannelChanged();
    void                funstreamChannelDisabled();
    void                funstreamAliasesChanged( const QString & );
    void                funstreamSupportersListChanged( const QString & );
    void                funstreamBlackListChanged( const QString & );
    void                funstreamRemoveBlackListUsersChanged( bool );

    void                gamerstvChannelChanged();
    void                gamerstvChannelDisabled();
    void                gamerstvAliasesChanged( const QString & );
    void                gamerstvSupportersListChanged( const QString & );
    void                gamerstvBlackListChanged( const QString & );
    void                gamerstvRemoveBlackListUsersChanged( bool );

    void                gipsyteamChannelChanged();
    void                gipsyteamChannelDisabled();
    void                gipsyteamAliasesChanged( const QString & );
    void                gipsyteamSupportersListChanged( const QString & );
    void                gipsyteamBlackListChanged( const QString & );
    void                gipsyteamRemoveBlackListUsersChanged( bool );

    void                goodGameChannelChanged();
    void                goodGameChannelDisabled();
    void                goodGameUseAnimatedSmilesChanged( bool );
    void                goodGameAliasesChanged( const QString & );
    void                goodGameSupportersListChanged( const QString & );
    void                goodGameBlackListChanged( const QString & );
    void                goodGameRemoveBlackListUsersChanged( bool );

    void                hitboxChannelChanged();
    void                hitboxChannelDisabled();
    void                hitboxAliasesChanged( const QString & );
    void                hitboxSupportersListChanged( const QString & );
    void                hitboxBlackListChanged( const QString & );
    void                hitboxRemoveBlackListUsersChanged( bool );
    void                hitboxOriginalColorsChanged( bool );

    void                igdcChannelChanged();
    void                igdcChannelDisabled();
    void                igdcAliasesChanged( const QString & );
    void                igdcSupportersListChanged( const QString & );
    void                igdcBlackListChanged( const QString & );
    void                igdcRemoveBlackListUsersChanged( bool );

    void                livecodingChannelChanged();
    void                livecodingChannelDisabled();
    void                livecodingAliasesChanged( const QString & );
    void                livecodingSupportersListChanged( const QString & );
    void                livecodingBlackListChanged( const QString & );
    void                livecodingRemoveBlackListUsersChanged( bool );

    void                realltvChannelChanged();
    void                realltvChannelDisabled();
    void                realltvAliasesChanged( const QString & );
    void                realltvSupportersListChanged( const QString & );
    void                realltvBlackListChanged( const QString & );
    void                realltvRemoveBlackListUsersChanged( bool );

    void                sc2tvChannelChanged();
    void                sc2tvChannelDisabled();
    void                sc2tvAliasesChanged( const QString & );
    void                sc2tvSupportersListChanged( const QString & );
    void                sc2tvBlackListChanged( const QString & );
    void                sc2tvRemoveBlackListUsersChanged( bool );
    void                sc2tvOriginalColorsChanged( bool );

    void                streamboxChannelChanged();
    void                streamboxChannelDisabled();
    void                streamboxAliasesChanged( const QString & );
    void                streamboxSupportersListChanged( const QString & );
    void                streamboxBlackListChanged( const QString & );
    void                streamboxRemoveBlackListUsersChanged( bool );

    void                twitchChannelChanged();
    void                twitchChannelDisabled();
    void                twitchAliasesChanged( const QString & );
    void                twitchSupportersListChanged( const QString & );
    void                twitchBlackListChanged( const QString & );
    void                twitchRemoveBlackListUsersChanged( bool );
    void                twitchOriginalColorsChanged( bool );

    void                youtubeChannelChanged();
    void                youtubeChannelDisabled();
    void                youtubeAliasesChanged( const QString & );
    void                youtubeSupportersListChanged( const QString & );
    void                youtubeBlackListChanged( const QString & );
    void                youtubeRemoveBlackListUsersChanged( bool );
private:
    QRgb                bodyBackgroundColor;

    QRgb                evenMessagesNicknamesColor;
    QRgb                oddMessagesNicknamesColor;
    QRgb                aliasesNicknamesColor;
    QRgb                supportersNicknamesColor;
    QRgb                blackListNicknamesColor;

    QRgb                evenMessagesBackgroundColor;
    QRgb                oddMessagesBackgroundColor;
    QRgb                aliasesBackgroundColor;
    QRgb                supportersBackgroundColor;
    QRgb                blackListBackgroundColor;

    QRgb                evenMessagesTextColor;
    QRgb                oddMessagesTextColor;
    QRgb                aliasesMessagesTextColor;
    QRgb                supportersMessagesTextColor;
    QRgb                blackListMessagesTextColor;

    QRgb                evenMessagesBorderColor;
    QRgb                oddMessagesBorderColor;
    QRgb                aliasesMessagesBorderColor;
    QRgb                supportersMessagesBorderColor;
    QRgb                blackListMessagesBorderColor;

    QRgb                viewersTextColor;
    QRgb                viewersBackgroundColor;

    QRgb                linksColor;

    QTabWidget *        tabSettings;

    QComboBox *         languageCombo;
    QCheckBox *         stayOnTopCheckBox;
    QCheckBox *         frameLessWindowCheckBox;
    //QCheckBox *       transparentWindowCheckBox;
    QComboBox *         styleCombo;
    QSlider   *         opacitySlider;
    QCheckBox *         showSystemMessagesCheckBox;
    QCheckBox *         showImagesCheckBox;
    QCheckBox *         saveMessagesToLogCheckBox;
    QCheckBox *         useServerCheckBox;
    QSpinBox  *         serverPortSpinBox;
    QCheckBox *         saveToFileCheckBox;
    QSpinBox  *         saveToFileIntervalSpinBox;

    QComboBox *         fontNameCombo;
    QPushButton *       bodyBackgroundColorButton;    

    QPushButton *       evenMessagesNicknamesColorButton;
    QPushButton *       oddMessagesNicknamesColorButton;
    QPushButton *       aliasesNicknamesColorButton;
    QPushButton *       supportersNicknamesColorButton;
    QPushButton *       blackListNicknamesColorButton;

    QSpinBox *          evenMessagesNicknamesFontSizeSpinBox;
    QSpinBox *          oddMessagesNicknamesFontSizeSpinBox;
    QSpinBox *          aliasesNicknamesFontSizeSpinBox;
    QSpinBox *          supportersNicknamesFontSizeSpinBox;
    QSpinBox *          blackListNicknamesFontSizeSpinBox;

    QPushButton *       evenMessagesBackgroundColorButton;
    QPushButton *       oddMessagesBackgroundColorButton;
    QPushButton *       aliasesBackgroundColorButton;
    QPushButton *       supportersBackgroundColorButton;
    QPushButton *       blackListBackgroundColorButton;

    QPushButton *       evenMessagesTextColorButton;
    QPushButton *       oddMessagesTextColorButton;
    QPushButton *       aliasesMessagesTextColorButton;
    QPushButton *       supportersMessagesTextColorButton;
    QPushButton *       blackListMessagesTextColorButton;

    QSpinBox *          evenMessagesFontSizeSpinBox;
    QSpinBox *          oddMessagesFontSizeSpinBox;
    QSpinBox *          aliasesMessagesFontSizeSpinBox;
    QSpinBox *          supportersMessagesFontSizeSpinBox;
    QSpinBox *          blackListMessagesFontSizeSpinBox;

    QPushButton *       evenMessagesBorderColorButton;
    QPushButton *       oddMessagesBorderColorButton;
    QPushButton *       aliasesMessagesBorderColorButton;
    QPushButton *       supportersMessagesBorderColorButton;
    QPushButton *       blackListMessagesBorderColorButton;       

    QSpinBox *          messagesLeftBorderSizeSpinBox;
    QSpinBox *          messagesRightBorderSizeSpinBox;
    QSpinBox *          messagesTopBorderSizeSpinBox;
    QSpinBox *          messagesBottomBorderSizeSpinBox;

    QSpinBox *          messagesTopLeftBorderRadiusSizeSpinBox;
    QSpinBox *          messagesTopRightBorderRadiusSizeSpinBox;
    QSpinBox *          messagesBottomLeftBorderRadiusSizeSpinBox;
    QSpinBox *          messagesBottomRightBorderRadiusSizeSpinBox;

    QSpinBox *          messagesMarginBottomSizeSpinBox;

    QSpinBox *          messagesIndentSizeSpinBox;

    QSpinBox *          viewersFontSizeSpinBox;
    QPushButton *       viewersTextColorButton;
    QPushButton *       viewersBackgroundColorButton;

    QPushButton *       linksColorButton;
    QSpinBox *          smilesSizeSpinBox;
    QSpinBox *          serviceIconsSizeSpinBox;
    QSpinBox *          maxImagesHeightSpinBox;
    QComboBox *         animationTypeCombo;
    QDoubleSpinBox *    animationDurationSpinBox;

    QCheckBox *         acesChannelCheckBox;
    QLineEdit *         acesChannelEdit;
    //QCheckBox *       acesOriginalColorsCheckBox;
    QLineEdit *         acesAliasesEdit;
    QTextEdit *         acesSupportersListEdit;
    QTextEdit *         acesBlackListEdit;
    QCheckBox *         acesRemoveBlackListUsersCheckBox;

    QCheckBox *         cyberGameChannelCheckBox;
    QLineEdit *         cyberGameChannelEdit;
    QLineEdit *         cyberGameAliasesEdit;
    QTextEdit *         cyberGameSupportersListEdit;
    QTextEdit *         cyberGameBlackListEdit;
    QCheckBox *         cyberGameRemoveBlackListUsersCheckBox;

    QCheckBox *         funstreamChannelCheckBox;
    QLineEdit *         funstreamChannelEdit;
    QLineEdit *         funstreamAliasesEdit;
    QTextEdit *         funstreamSupportersListEdit;
    QTextEdit *         funstreamBlackListEdit;
    QCheckBox *         funstreamRemoveBlackListUsersCheckBox;

    QCheckBox *         gamerstvChannelCheckBox;
    QLineEdit *         gamerstvChannelEdit;
    QLineEdit *         gamerstvAliasesEdit;
    QTextEdit *         gamerstvSupportersListEdit;
    QTextEdit *         gamerstvBlackListEdit;
    QCheckBox *         gamerstvRemoveBlackListUsersCheckBox;

    QCheckBox *         gipsyteamChannelCheckBox;
    QLineEdit *         gipsyteamChannelEdit;
    QLineEdit *         gipsyteamAliasesEdit;
    QTextEdit *         gipsyteamSupportersListEdit;
    QTextEdit *         gipsyteamBlackListEdit;
    QCheckBox *         gipsyteamRemoveBlackListUsersCheckBox;

    QCheckBox *         goodGameChannelCheckBox;
    QLineEdit *         goodGameChannelEdit;
    QCheckBox *         goodGameUseAnimatedSmilesCheckBox;
    QLineEdit *         goodGameAliasesEdit;
    QTextEdit *         goodGameSupportersListEdit;
    QTextEdit *         goodGameBlackListEdit;
    QCheckBox *         goodGameRemoveBlackListUsersCheckBox;

    QCheckBox *         hitboxChannelCheckBox;
    QLineEdit *         hitboxChannelEdit;
    QCheckBox *         hitboxOriginalColorsCheckBox;
    QLineEdit *         hitboxAliasesEdit;
    QTextEdit *         hitboxSupportersListEdit;
    QTextEdit *         hitboxBlackListEdit;
    QCheckBox *         hitboxRemoveBlackListUsersCheckBox;

    QCheckBox *         igdcChannelCheckBox;
    QLineEdit *         igdcChannelEdit;
    QLineEdit *         igdcAliasesEdit;
    QTextEdit *         igdcSupportersListEdit;
    QTextEdit *         igdcBlackListEdit;
    QCheckBox *         igdcRemoveBlackListUsersCheckBox;

    QCheckBox *         livecodingChannelCheckBox;
    QLineEdit *         livecodingChannelEdit;
    QLineEdit *         livecodingLoginEdit;
    QLineEdit *         livecodingPasswordEdit;
    QLineEdit *         livecodingAliasesEdit;
    QTextEdit *         livecodingSupportersListEdit;
    QTextEdit *         livecodingBlackListEdit;
    QCheckBox *         livecodingRemoveBlackListUsersCheckBox;

    QCheckBox *         realltvChannelCheckBox;
    QLineEdit *         realltvChannelEdit;
    QLineEdit *         realltvAliasesEdit;
    QTextEdit *         realltvSupportersListEdit;
    QTextEdit *         realltvBlackListEdit;
    QCheckBox *         realltvRemoveBlackListUsersCheckBox;

    QCheckBox *         sc2tvChannelCheckBox;
    QLineEdit *         sc2tvChannelEdit;
    QCheckBox *         sc2tvOriginalColorsCheckBox;
    QLineEdit *         sc2tvAliasesEdit;
    QTextEdit *         sc2tvSupportersListEdit;
    QTextEdit *         sc2tvBlackListEdit;
    QCheckBox *         sc2tvRemoveBlackListUsersCheckBox;

    QCheckBox *         streamboxChannelCheckBox;
    QLineEdit *         streamboxChannelEdit;
    QLineEdit *         streamboxAliasesEdit;
    QTextEdit *         streamboxSupportersListEdit;
    QTextEdit *         streamboxBlackListEdit;
    QCheckBox *         streamboxRemoveBlackListUsersCheckBox;

    QCheckBox *         twitchChannelCheckBox;
    QLineEdit *         twitchChannelEdit;
    QCheckBox *         twitchOriginalColorsCheckBox;
    QLineEdit *         twitchAliasesEdit;
    QTextEdit *         twitchSupportersListEdit;
    QTextEdit *         twitchBlackListEdit;
    QCheckBox *         twitchRemoveBlackListUsersCheckBox;

    QCheckBox *         youtubeChannelCheckBox;
    QLineEdit *         youtubeChannelEdit;
    QLineEdit *         youtubeAliasesEdit;
    QTextEdit *         youtubeSupportersListEdit;
    QTextEdit *         youtubeBlackListEdit;
    QCheckBox *         youtubeRemoveBlackListUsersCheckBox;
};

#endif // QSETTINGSDIALOG_H
