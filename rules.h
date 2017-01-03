#ifndef RULES_H
#define RULES_H

#include <QDialog>
#include <QFile>

namespace Ui {
class Rules;
}

class Rules : public QDialog
{
    Q_OBJECT

public:
    QString RulesText;
    QFile *RulesFile;

    explicit Rules(QWidget *parent = 0);
    ~Rules();
    void readRulesFile();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Rules *ui;
};

#endif // RULES_H
