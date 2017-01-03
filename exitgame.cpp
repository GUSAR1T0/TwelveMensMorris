#include "exitgame.h"
#include "ui_exitgame.h"

ExitGame::ExitGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExitGame)
{
    ui->setupUi(this);
    setWindowTitle("Exit");
}

ExitGame::~ExitGame()
{
    delete ui;
}

void ExitGame::on_buttonBox_accepted()
{
    accept();
}

void ExitGame::on_buttonBox_rejected()
{
    reject();
}
