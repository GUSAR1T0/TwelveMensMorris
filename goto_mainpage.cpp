#include "goto_mainpage.h"
#include "ui_goto_mainpage.h"

GoTo_MainPage::GoTo_MainPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoTo_MainPage)
{
    ui->setupUi(this);
}

GoTo_MainPage::~GoTo_MainPage()
{
    delete ui;
}

void GoTo_MainPage::on_buttonBox_accepted()
{
    accept();
}

void GoTo_MainPage::on_buttonBox_rejected()
{
    reject();
}
