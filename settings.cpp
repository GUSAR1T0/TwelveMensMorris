#include "settings.h"
#include "ui_settings.h"
#include "algorithm.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    setWindowTitle("Settings");

    connect(ui->PC, SIGNAL(pressed()), this, SLOT(PC_pressed()));
    connect(ui->Player, SIGNAL(pressed()), this, SLOT(Player_pressed()));
}

Settings::~Settings()
{
    delete ui;
}

Algorithm::Level Settings::getLevel()
{
    if (!Flag) return Algorithm::Level::Opponent;

    if (ui->Easy->isChecked()) return Algorithm::Level::Easy;
    else
        if (ui->Middle->isChecked()) return Algorithm::Level::Middle;
        else return Algorithm::Level::Hard;
}

void Settings::on_buttonBox_accepted()
{
    accept();
}

void Settings::on_buttonBox_rejected()
{
    reject();
}

void Settings::Player_pressed()
{
    ui->Level->setEnabled(false);
    Flag = false;
}

void Settings::PC_pressed()
{
    ui->Level->setEnabled(true);
    Flag = true;
}
