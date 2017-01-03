#ifndef EXITGAME_H
#define EXITGAME_H

#include <QDialog>

namespace Ui {
class ExitGame;
}

class ExitGame : public QDialog
{
    Q_OBJECT

public:
    explicit ExitGame(QWidget *parent = 0);
    ~ExitGame();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ExitGame *ui;
};

#endif // EXITGAME_H
