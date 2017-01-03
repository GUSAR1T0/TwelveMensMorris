#ifndef SETTINGS_H
#define SETTINGS_H

#include "algorithm.h"
#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

    bool Flag = true;

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    Algorithm::Level getLevel();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void Player_pressed();
    void PC_pressed();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
