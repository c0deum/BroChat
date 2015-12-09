#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QListWidget>

#include <QMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QSettings>

#include <QStringList>

#include "settingsconsts.h"

#include "qpollsettingsdialog.h"

QPollSettingsDialog::QPollSettingsDialog( QWidget * parent )
: QDialog( parent )
, questionEdit_( new QLineEdit( this ) )
, pollTimeSpinBox_( new QSpinBox( this ) )
, multiChoiceCheckBox_( new QCheckBox( tr( "Multi Choice" ), this ) )
, answersListWidget_( new QListWidget( this ) )
, answerEdit_( new QLineEdit( this ) )
, addAnswerButton_( new QPushButton( tr( "Add Answer" ), this ) )
, modifyAnswerButton_( new QPushButton( tr ( "Modify Answer" ), this ) )
, removeAnswerButton_( new QPushButton( tr( "Remove Answer" ), this ) )
{
    setWindowTitle( tr( "Poll Settings" ) );
    createWidgets();
}

QPollSettingsDialog::~QPollSettingsDialog()
{
}

void QPollSettingsDialog::createWidgets()
{
    QSettings settings;

    QVBoxLayout * mainLayout = new QVBoxLayout();

    QHBoxLayout * questionLayout = new QHBoxLayout();

    QLabel * questionLabel = new QLabel( tr( "Question:" ), this );

    questionEdit_->setText( settings.value( LAST_POLL_QUESTION_SETTING_PATH, QString() ).toString() );

    questionLayout->addWidget( questionLabel );
    questionLayout->addWidget( questionEdit_ );

    mainLayout->addLayout( questionLayout );


    QHBoxLayout * pollTimeLayout = new QHBoxLayout();

    QLabel * pollTimeLabel = new QLabel( tr( "Poll Time(sec ):" ), this );
    pollTimeSpinBox_->setRange( 0, 3600 );
    pollTimeSpinBox_->setValue( settings.value( LAST_POLL_TIME_SETTING_PATH, DEFAULT_POLL_TIME ).toInt() );

    pollTimeLayout->addWidget( pollTimeLabel );
    pollTimeLayout->addWidget( pollTimeSpinBox_ );

    mainLayout->addLayout( pollTimeLayout );

    multiChoiceCheckBox_->setChecked( settings.value( LAST_POLL_MULTI_CHOICE_SETTING_PATH, DEFAULT_POLL_MULTI_CHOICE ).toBool() );

    mainLayout->addWidget( multiChoiceCheckBox_ );

    answersListWidget_->addItems( settings.value( LAST_POLL_ENTRIES_ANSWERS_SETTING_PATH, QStringList() ).toStringList() );
    QObject::connect( answersListWidget_, SIGNAL( itemSelectionChanged() ), this, SLOT( onAnswersListSelectionChanged() ) );

    mainLayout->addWidget( answersListWidget_ );

    QHBoxLayout * answerLayout = new QHBoxLayout();

    QLabel * answerLabel = new QLabel( tr( "Answer:" ), this );

    QObject::connect( answerEdit_, SIGNAL( textChanged( QString ) ), this, SLOT( onAnswerEditChanged() ) );

    answerLayout->addWidget( answerLabel );
    answerLayout->addWidget( answerEdit_ );

    mainLayout->addLayout( answerLayout );


    QHBoxLayout * answersButtonsLayout = new QHBoxLayout();

    addAnswerButton_->setEnabled( false );
    QObject::connect( addAnswerButton_, SIGNAL( clicked() ), this, SLOT( onAddAnswerButtonClick() ) );

    modifyAnswerButton_->setEnabled( false );
    QObject::connect( modifyAnswerButton_, SIGNAL( clicked() ), this, SLOT( onModifyAnswerButtonClick() ) );

    removeAnswerButton_->setEnabled( false );
    QObject::connect( removeAnswerButton_, SIGNAL( clicked() ), this, SLOT( onRemoveAnswerButtonClick() ) );

    answersButtonsLayout->addWidget( addAnswerButton_ );
    answersButtonsLayout->addWidget( modifyAnswerButton_ );
    answersButtonsLayout->addWidget( removeAnswerButton_ );

    mainLayout->addLayout( answersButtonsLayout );


    QHBoxLayout * buttonsLayout = new QHBoxLayout();

    QPushButton * buttonOk = new QPushButton( tr( "&Ok" ), this );

    QObject::connect( buttonOk, SIGNAL( clicked() ), this, SLOT( saveSettings() ) );
    QObject::connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );

    QPushButton * buttonCancel = new QPushButton( tr( "&Cancel" ), this );

    QObject::connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    buttonsLayout->addStretch( 1 );
    buttonsLayout->addWidget( buttonOk );
    buttonsLayout->addWidget( buttonCancel );

    mainLayout->addLayout( buttonsLayout );


    setLayout( mainLayout );
}

void QPollSettingsDialog::saveSettings()
{
    QSettings settings;

    bool isPollChanged = false;

    QString oldQuestion = settings.value( LAST_POLL_QUESTION_SETTING_PATH, QString() ).toString();
    if( oldQuestion != questionEdit_->text() )
    {
        settings.setValue( LAST_POLL_QUESTION_SETTING_PATH, questionEdit_->text() );
        isPollChanged = true;
    }

    int oldPollTime = settings.value( LAST_POLL_TIME_SETTING_PATH, DEFAULT_POLL_TIME ).toInt();

    if( oldPollTime != pollTimeSpinBox_->value() )
    {
        settings.setValue( LAST_POLL_TIME_SETTING_PATH, pollTimeSpinBox_->value() );
        isPollChanged = true;
    }

    bool oldMultiChoice = settings.value( LAST_POLL_MULTI_CHOICE_SETTING_PATH, DEFAULT_POLL_MULTI_CHOICE ).toBool();

    if( oldMultiChoice != multiChoiceCheckBox_->isChecked() )
    {
        settings.setValue( LAST_POLL_MULTI_CHOICE_SETTING_PATH, multiChoiceCheckBox_->isChecked() );
        isPollChanged = true;
    }

    QStringList oldAnswersList = settings.value( LAST_POLL_ENTRIES_ANSWERS_SETTING_PATH, QStringList() ).toStringList();
    QStringList answersList;


    for( int i = 0; i < answersListWidget_->count(); ++i )
    {
        answersList << answersListWidget_->item( i )->text();

    }

    if( oldAnswersList != answersList )
    {
        settings.setValue( LAST_POLL_ENTRIES_ANSWERS_SETTING_PATH, answersList );

        QStringList votesList;
        for( int i = 0; i < answersList.size(); ++i )
        {
            votesList << "0";
        }

        settings.setValue( LAST_POLL_ENTRIES_VOTES_SETTING_PATH, votesList );

        isPollChanged = true;
    }

    if( isPollChanged )
        emit pollChanged();
}

void QPollSettingsDialog::onAddAnswerButtonClick()
{
    if( !answersListWidget_->findItems( answerEdit_->text() , Qt::MatchCaseSensitive ).isEmpty() )
    {
        QMessageBox::information( this, tr( "Error" ), tr( "Answer [" ) + answerEdit_->text() + tr( "] is already exists" ) );
        return;
    }
    answersListWidget_->addItem( answerEdit_->text() );
}

void QPollSettingsDialog::onModifyAnswerButtonClick()
{
    answersListWidget_->currentItem()->setText( answerEdit_->text() );
}

void QPollSettingsDialog::onRemoveAnswerButtonClick()
{
    answersListWidget_->removeItemWidget( answersListWidget_->takeItem( answersListWidget_->currentRow() ) );
}

void QPollSettingsDialog::onAnswerEditChanged()
{
    addAnswerButton_->setEnabled( !answerEdit_->text().isEmpty() );
    modifyAnswerButton_->setEnabled( addAnswerButton_->isEnabled() && !answersListWidget_->selectedItems().isEmpty() );
}

void QPollSettingsDialog::onAnswersListSelectionChanged()
{
    removeAnswerButton_->setEnabled( !answersListWidget_->selectedItems().isEmpty() );
    modifyAnswerButton_->setEnabled( removeAnswerButton_->isEnabled() && !answerEdit_->text().isEmpty() );

    if( answersListWidget_->selectedItems().isEmpty() )
        return;

    answerEdit_->setText( answersListWidget_->currentItem()->text() );
}
