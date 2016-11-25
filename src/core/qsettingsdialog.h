#ifndef QSETTINGSDIALOG_H
#define QSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include "chattypeenum.h"
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
class VirtualSettingsTab;

class QSettingsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit            QSettingsDialog( QWidget * parent = nullptr );
    virtual             ~QSettingsDialog();

    //link tab dialog to its chat
    void                connectDialogToChat(ChatTypeEnum chatType,QObject* chat);
private:
    void                loadDialogSettings();
    void                setupWidgets();
    void                setupGeneralTab();
    void                setupDefaultStyleTab();

    void                setColorButtonStyle( QPushButton * button, QRgb color, int fontSize = 16 );
    void                buttonColorSelection( QPushButton * button, QRgb & color );

    void                addLayoutRow( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets );
    void                addLayoutRow( QVBoxLayout * layout, std::initializer_list< QVBoxLayout * > layouts );
    void                addWidgets( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets );
    void                addLayouts( QHBoxLayout * layout, std::initializer_list< QVBoxLayout * > layouts );

    void                setupSpinBox( QSpinBox * spinBox, int min, int max, int defaultValue, const QString & settingPath );
    void                setupColorButton( QPushButton * button, QRgb & color, unsigned int defaultColor, const QString & settingPath );

    //generate tabs and attach them to tab host
    void                populateTabs(QTabWidget* tabHost, QSettings &settings);
    //save each tab's settings
    void                saveChatTabsSettings(QSettings &settings);




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

    void                onExportDefaultThemeClick();

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

private:
    QMap<ChatTypeEnum,VirtualSettingsTab*> tabs_;

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

    QPushButton *       exportDefaultTheme;


};

#endif // QSETTINGSDIALOG_H
