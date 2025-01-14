#ifndef SELFINFOWIDGET_H
#define SELFINFOWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

class SelfInfoWidget : public QDialog
{
    Q_OBJECT
public:
    SelfInfoWidget(QWidget* parent);

    void InitSignalSlot();
    void ClickNameSubmitBtn();
    void ClickNameSubmitBtnDone();
    void ClickDescSubmitBtn();
    void ChickDescSubmitBtnDone();
    void ClickGetVerifyCodeBtn();
    void ClickPhoneSubmitBtn();
    void ClickPhoneSubmitBtnDone();
    void ClickAvatarBtn();
    void ClickAvatarBtnDone();

private:
    QGridLayout* layout;

    QPushButton* avatarBtn;
    QLabel* idTag;
    QLabel* idLabel;

    QLabel* nameTag;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QPushButton* nameModifyBtn;
    QPushButton* nameSubmitBtn;

    QLabel* descTag;
    QLabel* descLabel;
    QLineEdit* descEdit;
    QPushButton* descModifyBtn;
    QPushButton* descSubmitBtn;

    QLabel* phoneTag;
    QLabel* phoneLabel;
    QLineEdit* phoneEdit;
    QPushButton* phoneModifyBtn;
    QPushButton* phoneSubmitBtn;

    QLabel* verifyCodeTag;
    QLineEdit* verifyCodeEdit;
    QPushButton* getVerifyCodeBtn;

    QString phoneToChange;

    int leftTime = 30;
};

#endif // SELFINFOWIDGET_H
