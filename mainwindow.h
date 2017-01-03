#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "algorithm.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QStack>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum Situation
    {
        Turn, Win
    };

    struct HistoryStruct
    {
        int OwnCh[2][2], EnemyCh[2];
        Algorithm::Player **StateField;
        Algorithm::Player **OldStateField;
        bool **EnemyField;
        int *CountSteps;
    };

    bool Flag[4];
    int OwnCh[2][2], EnemyCh[2];
    QString Font = "Microsoft YaHei UI Light";
    HistoryStruct *History;

    QPushButton *Btn_NewGame;
    QPushButton *Btn_Settings;
    QPushButton ***Field = nullptr;
    QLabel ***Result;
    QLabel *Text = nullptr;
    QPixmap *sheetImage_white;
    QPixmap *sheetImage_black;
    QLabel *sheetLabel_white;
    QLabel *sheetLabel_black;
    QStack <HistoryStruct> UndoHistory;

    Algorithm *Alg;
    Algorithm::Player Player;
    Algorithm::Level Level = Algorithm::Level::Easy;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setMainPage();
    void createScene();
    void deleteScene();
    void setBackgroundScene();
    void showResults();
    void showText(Algorithm::Player Player, Situation Flag);
    int makeAction(int i, int j);
    int setColorField(int i, int j);
    void setAvailableField(int i, int j);
    void removeEnemyField(Algorithm::Player Player, int i, int j);
    void setFirstFieldPC(int i, int j);
    void setColorOnScene(Algorithm::Player Player, int i, int j);
    void setResultPC();
    bool showWinner();
    void changeTurn();
    void setHistory(HistoryStruct *History, int OwnCh[2][2], int EnemyCh[2], Algorithm::Player **StateField, Algorithm::Player **OldStateField,
                    bool **EnemyField, int *CountSteps);
    void getHistory(HistoryStruct *History);

private slots:
    void on_actionNewGame_triggered();
    void on_actionSettings_triggered();
    void on_actionMain_Page_triggered();
    void on_actionRules_triggered();
    void on_actionUndo_triggered();
    void Field_clicked();
    void makePCStep(int i, int j);

protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QBrush *Background;
    QPalette Palette;
};

#endif // MAINWINDOW_H
