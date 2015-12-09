#ifndef QPOLLSETTINGSDIALOG
#define QPOLLSETTINGSDIALOG

#include <QDialog>


class QLineEdit;
class QSpinBox;
class QPushButton;
class QCheckBox;
class QListWidget;

class QPollSettingsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit QPollSettingsDialog( QWidget * parent = 0 );
    virtual ~QPollSettingsDialog();
private:
    void createWidgets();

private slots:
    void saveSettings();

    void onAddAnswerButtonClick();
    void onModifyAnswerButtonClick();
    void onRemoveAnswerButtonClick();

    void onAnswerEditChanged();

    void onAnswersListSelectionChanged();
signals:
    void pollChanged();
private:
    QLineEdit * questionEdit_;
    QSpinBox * pollTimeSpinBox_;
    QCheckBox * multiChoiceCheckBox_;
    QListWidget * answersListWidget_;
    QLineEdit * answerEdit_;
    QPushButton * addAnswerButton_;
    QPushButton * modifyAnswerButton_;
    QPushButton * removeAnswerButton_;
};

#endif // QPOLLSETTINGSDIALOG

