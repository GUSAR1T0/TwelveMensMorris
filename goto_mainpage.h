#ifndef GOTO_MAINPAGE_H
#define GOTO_MAINPAGE_H

#include <QDialog>

namespace Ui {
class GoTo_MainPage;
}

class GoTo_MainPage : public QDialog
{
    Q_OBJECT

public:
    explicit GoTo_MainPage(QWidget *parent = 0);
    ~GoTo_MainPage();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GoTo_MainPage *ui;
};

#endif // GOTO_MAINPAGE_H
