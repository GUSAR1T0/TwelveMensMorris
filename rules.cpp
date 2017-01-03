#include "rules.h"
#include "ui_rules.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>

Rules::Rules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rules)
{
    ui->setupUi(this);
    setWindowTitle("Rules");

    readRulesFile();
    ui->label->setText(RulesText);
}

Rules::~Rules()
{
    delete ui;
}

void Rules::readRulesFile()
{
    RulesFile = new QFile(":/text/rules.htm");
    if (RulesFile->open(QIODevice::ReadOnly))
    {
        QTextStream stream(RulesFile);
        RulesText = stream.readAll();

        RulesFile->close();
    }
    else return;
}

void Rules::on_pushButton_clicked()
{
    reject();
}
