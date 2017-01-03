#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "algorithm.h"
#include <QDialog>

namespace Ui {
class GameOver;
}

class GameOver : public QDialog
{
    Q_OBJECT

public:
    explicit GameOver(QWidget *parent = 0);
    ~GameOver();
    void setWinner(Algorithm::Player player);

private slots:
    void on_pushButton_clicked();

private:
    Ui::GameOver *ui;
};

#endif // GAMEOVER_H
