#include "gameover.h"
#include "ui_gameover.h"

GameOver::GameOver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameOver)
{
    ui->setupUi(this);
    setWindowTitle("Game Over");
}

GameOver::~GameOver()
{
    delete ui;
}

void GameOver::setWinner(Algorithm::Player player)
{
    if (player == Algorithm::Player::White)
        ui->label->setText("Game over! White is winner!");
    else
        if (player == Algorithm::Player::Black)
            ui->label->setText("Game over! Black is winner!");
}

void GameOver::on_pushButton_clicked()
{
    close();
}
