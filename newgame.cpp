#include "newgame.h"
#include "ui_newgame.h"

NewGame::NewGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGame)
{
    ui->setupUi(this);
    setWindowTitle("New Game");
}

NewGame::~NewGame()
{
    delete ui;
}

void NewGame::on_buttonBox_accepted()
{
    accept();
}

void NewGame::on_buttonBox_rejected()
{
    reject();
}
