#define POSTEXT_OF_WHITE 756, 88, 270, 150
#define POSTEXT_OF_BLACK 755, 448, 270, 150

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "goto_mainpage.h"
#include "newgame.h"
#include "settings.h"
#include "rules.h"
#include "exitgame.h"
#include "gameover.h"
#include "algorithm.h"
#include <ctime>
#include <QPushButton>
#include <QCloseEvent>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Twelve Men\'s Morris");

    setMainPage();
}

MainWindow::~MainWindow()
{
    if (Field) deleteScene();
    else
    {
        delete Background;
        Background = nullptr;
    }

    delete ui;
}

void MainWindow::setHistory(HistoryStruct *History, int OwnCh[2][2], int EnemyCh[2], Algorithm::Player **StateField, Algorithm::Player **OldStateField,
                            bool **EnemyField, int *CountSteps)
{
    /* Занесение в историю текущее состояние игры */

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            History->OwnCh[i][j] = OwnCh[i][j];

    History->EnemyCh[0] = EnemyCh[0];
    History->EnemyCh[1] = EnemyCh[1];

    History->StateField = new Algorithm::Player *[8];
    for (int i = 0; i < 8; i++)
        History->StateField[i] = new Algorithm::Player[3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            History->StateField[i][j] = StateField[i][j];

    History->OldStateField = new Algorithm::Player *[8];
    for (int i = 0; i < 8; i++)
        History->OldStateField[i] = new Algorithm::Player[3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            History->OldStateField[i][j] = OldStateField[i][j];

    History->EnemyField = new bool *[2];
    for (int i = 0; i < 2; i++)
        History->EnemyField[i] = new bool[20];

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 20; j++)
            History->EnemyField[i][j] = EnemyField[i][j];

    History->CountSteps = new int[2];
    History->CountSteps[0] = CountSteps[0];
    History->CountSteps[1] = CountSteps[1];
}

void MainWindow::getHistory(HistoryStruct *History)
{
    /* Извлечение состояния из истории */

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            OwnCh[i][j] = History->OwnCh[i][j];

    Result[Algorithm::Player::White][2]->setText(QString::number(OwnCh[Algorithm::Player::White][0]));
    Result[Algorithm::Player::White][3]->setText(QString::number(OwnCh[Algorithm::Player::White][1]));
    Result[Algorithm::Player::Black][2]->setText(QString::number(OwnCh[Algorithm::Player::Black][0]));
    Result[Algorithm::Player::Black][3]->setText(QString::number(OwnCh[Algorithm::Player::Black][1]));

    EnemyCh[0] = History->EnemyCh[0];
    EnemyCh[1] = History->EnemyCh[1];

    Result[Algorithm::Player::White][5]->setText(QString::number(EnemyCh[Algorithm::Player::White]));
    Result[Algorithm::Player::Black][5]->setText(QString::number(EnemyCh[Algorithm::Player::Black]));

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            Alg->StateField[i][j] = History->StateField[i][j];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
        {
            if (Alg->StateField[i][j] == Algorithm::Player::Black)
                Field[i][j]->setIcon(QIcon(":/images/blackfield.png"));

            if (Alg->StateField[i][j] == Algorithm::Player::White)
                Field[i][j]->setIcon(QIcon(":/images/whitefield.png"));

            if (Alg->StateField[i][j] == Algorithm::Player::Available)
                Field[i][j]->setIcon(QIcon(":/images/greenfield.png"));

            if (Alg->StateField[i][j] == Algorithm::Player::Unknown)
                Field[i][j]->setIcon(QIcon(":/images/grayfield.png"));
        }

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            Alg->OldStateField[i][j] = History->OldStateField[i][j];

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 20; j++)
            Alg->EnemyField[i][j] = History->EnemyField[i][j];

    Alg->CountSteps[0] = History->CountSteps[0];
    Alg->CountSteps[1] = History->CountSteps[1];
}

void MainWindow::setMainPage()
{
    /* Отображение главной страницы игры */

    Background = new QBrush(QImage(":/images/start_background.png"));
    Palette = this->palette();
    Palette.setBrush(QPalette::Background, *Background);
    this->setPalette(Palette);

    QFont Font_Start(Font, 30, QFont::Light, false);

    Btn_NewGame = new QPushButton("Press to start game. . .", this);
    Btn_NewGame->setFont(Font_Start);
    Btn_NewGame->setGeometry(10, 640, 510, 100);
    Btn_NewGame->setAutoDefault(true);
    Btn_NewGame->show();

    Btn_Settings = new QPushButton("Press to change settings. . .", this);
    Btn_Settings->setFont(Font_Start);
    Btn_Settings->setGeometry(530, 640, 510, 100);
    Btn_Settings->show();

    connect(Btn_NewGame, SIGNAL(clicked(bool)), this, SLOT(on_actionNewGame_triggered()));
    connect(Btn_Settings, SIGNAL(clicked(bool)), this, SLOT(on_actionSettings_triggered()));
}

void MainWindow::createScene()
{
    /* Создание сцены игры */

    //Установка фона игры
    setBackgroundScene();

    //Отобразить информацию по статистике игры
    showResults();
    OwnCh[Algorithm::Player::White][0] = OwnCh[Algorithm::Player::Black][0] = 9;
    OwnCh[Algorithm::Player::White][1] = OwnCh[Algorithm::Player::Black][1] = 0;
    EnemyCh[Algorithm::Player::White] = EnemyCh[Algorithm::Player::Black] = 0;

    //Флаги для обработки ситуаций
    Flag[0] = true;
    Flag[1] = false;
    Flag[2] = false;
    Flag[3] = false;

    //Процесс создания полей для игры
    Field = new QPushButton **[8];
    for (int i = 0; i < 8; i++)
        Field[i] = new QPushButton *[3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            Field[i][j] = new QPushButton(this);

    Field[0][0]->setGeometry(350, 273, 50, 50);
    Field[0][1]->setGeometry(350, 173, 50, 50);
    Field[0][2]->setGeometry(350, 73, 50, 50);
    Field[1][0]->setGeometry(450, 273, 50, 50);
    Field[1][1]->setGeometry(550, 173, 50, 50);
    Field[1][2]->setGeometry(650, 73, 50, 50);
    Field[2][0]->setGeometry(450, 373, 50, 50);
    Field[2][1]->setGeometry(550, 373, 50, 50);
    Field[2][2]->setGeometry(650, 373, 50, 50);
    Field[3][0]->setGeometry(450, 473, 50, 50);
    Field[3][1]->setGeometry(550, 573, 50, 50);
    Field[3][2]->setGeometry(650, 673, 50, 50);
    Field[4][0]->setGeometry(350, 473, 50, 50);
    Field[4][1]->setGeometry(350, 573, 50, 50);
    Field[4][2]->setGeometry(350, 673, 50, 50);
    Field[5][0]->setGeometry(250, 473, 50, 50);
    Field[5][1]->setGeometry(150, 573, 50, 50);
    Field[5][2]->setGeometry(50, 673, 50, 50);
    Field[6][0]->setGeometry(250, 373, 50, 50);
    Field[6][1]->setGeometry(150, 373, 50, 50);
    Field[6][2]->setGeometry(50, 373, 50, 50);
    Field[7][0]->setGeometry(250, 273, 50, 50);
    Field[7][1]->setGeometry(150, 173, 50, 50);
    Field[7][2]->setGeometry(50, 73, 50, 50);

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
        {
            Field[i][j]->show();
            Field[i][j]->setFlat(true);
            Field[i][j]->setIcon(QIcon(":/images/grayfield.png"));
            Field[i][j]->setIconSize(QSize(50, 50));

            connect(Field[i][j], SIGNAL(clicked(bool)), this, SLOT(Field_clicked()));
        }

    //Игра начинается с хода белыми шашками
    Player = Algorithm::Player::White;

    //Вызов конструктора класса "Algorithm" и определение уровня сложности
    Alg = new Algorithm();
    Alg->LvlPC = Level;

    //Разблокировка сигналов
    blockSignals(false);

    //История
    History = new HistoryStruct();
    setHistory(History, OwnCh, EnemyCh, Alg->StateField, Alg->OldStateField, Alg->EnemyField, Alg->CountSteps);
    UndoHistory.push(*History);
}

void MainWindow::deleteScene()
{
    /* Удаление сцены игры */

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            delete Field[i][j];
            Field[i][j] = nullptr;
        }

        delete Field[i];
        Field[i] = nullptr;
    }

    delete Field;
    Field = nullptr;

    delete Alg;
    Alg = nullptr;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            delete Result[i][j];
            Result[i][j] = nullptr;
        }

        if (i == 1)
        {
            delete Result[1][8];
            Result[1][8] = nullptr;
        }

        delete Result[i];
        Result[i] = nullptr;
    }

    delete Result;
    Result = nullptr;

    delete Text;
    Text = nullptr;

    delete sheetImage_white;
    sheetImage_white = nullptr;

    delete sheetImage_black;
    sheetImage_black = nullptr;

    delete sheetLabel_white;
    sheetLabel_white = nullptr;

    delete sheetLabel_black;
    sheetLabel_black = nullptr;

    delete History;
    History = nullptr;
}

void MainWindow::setBackgroundScene()
{
    /* Установка фона сцены */

    if (Background)
    {
        delete Background;
        Background = nullptr;
    }

    //Создание фона и композиции для игры
    Background = new QBrush(QImage(":/images/background.png"));
    Palette = this->palette();
    Palette.setBrush(QPalette::Background, *Background);
    this->setPalette(Palette);

    sheetImage_white = new QPixmap(":/images/sheet1.png");
    sheetLabel_white = new QLabel(this);
    sheetLabel_white->setPixmap(*sheetImage_white);
    sheetLabel_white->setGeometry(750, 25, 290, 355);
    sheetLabel_white->show();

    sheetImage_black = new QPixmap(":/images/sheet2.png");
    sheetLabel_black = new QLabel(this);
    sheetLabel_black->setPixmap(*sheetImage_black);
    sheetLabel_black->setGeometry(745, 383, 290, 355);
    sheetLabel_black->show();
}

void MainWindow::showResults()
{
    /* Отображения результатов игроков */

    Result = new QLabel **[2];
    Result[Algorithm::Player::White] = new QLabel *[8];
    for (int i = 0; i < 8; i++)
        Result[Algorithm::Player::White][i] = new QLabel(this);

    Result[Algorithm::Player::White][0]->setGeometry(753, 35, 270, 150);
    Result[Algorithm::Player::White][0]->setText("Result of White");
    Result[Algorithm::Player::White][0]->setFont(QFont(Font, 27));
    Result[Algorithm::Player::White][0]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::White][1]->setGeometry(765, 180, 135, 50);
    Result[Algorithm::Player::White][1]->setText("White checkers:");
    Result[Algorithm::Player::White][1]->setFont(QFont(Font, 13));

    Result[Algorithm::Player::White][6]->setGeometry(765, 205, 135, 50);
    Result[Algorithm::Player::White][6]->setText("- In reserve:");
    Result[Algorithm::Player::White][6]->setFont(QFont(Font, 12));

    Result[Algorithm::Player::White][2]->setGeometry(789, 218, 66, 100);
    Result[Algorithm::Player::White][2]->setText("9");
    Result[Algorithm::Player::White][2]->setFont(QFont(Font, 35));
    Result[Algorithm::Player::White][2]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::White][7]->setGeometry(765, 285, 135, 50);
    Result[Algorithm::Player::White][7]->setText("- On scene:");
    Result[Algorithm::Player::White][7]->setFont(QFont(Font, 12));

    Result[Algorithm::Player::White][3]->setGeometry(789, 298, 66, 100);
    Result[Algorithm::Player::White][3]->setText("0");
    Result[Algorithm::Player::White][3]->setFont(QFont(Font, 35));
    Result[Algorithm::Player::White][3]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::White][4]->setGeometry(907, 180, 135, 50);
    Result[Algorithm::Player::White][4]->setText("Black chekers:");
    Result[Algorithm::Player::White][4]->setFont(QFont(Font, 13));

    Result[Algorithm::Player::White][5]->setGeometry(890, 245, 135, 100);
    Result[Algorithm::Player::White][5]->setText("0");
    Result[Algorithm::Player::White][5]->setFont(QFont(Font, 50));
    Result[Algorithm::Player::White][5]->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < 8; i++)
    {
        Result[Algorithm::Player::White][i]->setStyleSheet("color: rgb(128,0,0)");
        Result[Algorithm::Player::White][i]->show();
    }

    Result[Algorithm::Player::Black] = new QLabel *[9];
    for (int i = 0; i < 9; i++)
        Result[Algorithm::Player::Black][i] = new QLabel(this);

    Result[Algorithm::Player::Black][8]->setGeometry(753, 355, 270, 150);

    if (Level == Algorithm::Level::Opponent)
        Result[Algorithm::Player::Black][8]->setText("");
    if (Level == Algorithm::Level::Easy)
        Result[Algorithm::Player::Black][8]->setText("Your opponent is AI (Easy)");
    if (Level == Algorithm::Level::Middle)
        Result[Algorithm::Player::Black][8]->setText("Your opponent is AI (Midlle)");
    if (Level == Algorithm::Level::Hard)
        Result[Algorithm::Player::Black][8]->setText("Your opponent is AI (Hard)");

    Result[Algorithm::Player::Black][8]->setFont(QFont(Font, 13));
    Result[Algorithm::Player::Black][8]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::Black][0]->setGeometry(753, 395, 270, 150);
    Result[Algorithm::Player::Black][0]->setText("Result of Black");
    Result[Algorithm::Player::Black][0]->setFont(QFont(Font, 27));
    Result[Algorithm::Player::Black][0]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::Black][1]->setGeometry(765, 540, 135, 50);
    Result[Algorithm::Player::Black][1]->setText("Black checkers:");
    Result[Algorithm::Player::Black][1]->setFont(QFont(Font, 13));

    Result[Algorithm::Player::Black][6]->setGeometry(765, 565, 135, 50);
    Result[Algorithm::Player::Black][6]->setText("- In reserve:");
    Result[Algorithm::Player::Black][6]->setFont(QFont(Font, 12));

    Result[Algorithm::Player::Black][2]->setGeometry(785, 578, 66, 100);
    Result[Algorithm::Player::Black][2]->setText("9");
    Result[Algorithm::Player::Black][2]->setFont(QFont(Font, 35));
    Result[Algorithm::Player::Black][2]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::Black][7]->setGeometry(765, 645, 135, 50);
    Result[Algorithm::Player::Black][7]->setText("- On scene:");
    Result[Algorithm::Player::Black][7]->setFont(QFont(Font, 12));

    Result[Algorithm::Player::Black][3]->setGeometry(785, 658, 66, 100);
    Result[Algorithm::Player::Black][3]->setText("0");
    Result[Algorithm::Player::Black][3]->setFont(QFont(Font, 35));
    Result[Algorithm::Player::Black][3]->setAlignment(Qt::AlignCenter);

    Result[Algorithm::Player::Black][4]->setGeometry(900, 540, 135, 50);
    Result[Algorithm::Player::Black][4]->setText("White chekers:");
    Result[Algorithm::Player::Black][4]->setFont(QFont(Font, 13));

    Result[Algorithm::Player::Black][5]->setGeometry(885, 605, 135, 100);
    Result[Algorithm::Player::Black][5]->setText("0");
    Result[Algorithm::Player::Black][5]->setFont(QFont(Font, 50));
    Result[Algorithm::Player::Black][5]->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < 9; i++)
    {
        Result[Algorithm::Player::Black][i]->setStyleSheet("color: rgb(128,0,0)");
        Result[Algorithm::Player::Black][i]->show();
    }

    showText(Algorithm::Player::White, Situation::Turn);
}

void MainWindow::showText(Algorithm::Player Player, Situation Flag)
{
    /* Сообщение о текущем ходе конкретного игрока или о его победе */

    if (!Text) Text = new QLabel(this);

    if (Flag == Situation::Turn)
    {
        if (Player == Algorithm::Player::White)
        {
            Text->setGeometry(POSTEXT_OF_WHITE);
            Text->setText("Turn of White");
        }
        else
        {
            Text->setGeometry(POSTEXT_OF_BLACK);
            Text->setText("Turn of Black");
        }
    }
    else
    {
        if (Player == Algorithm::Player::White)
        {
            Text->setGeometry(POSTEXT_OF_WHITE);
            Text->setText("White is winner!");
        }
        else
        {
            Text->setGeometry(POSTEXT_OF_BLACK);
            Text->setText("Black is winner!");
        }
    }

    Text->setFont(QFont("Calibri", 20));
    Text->setAlignment(Qt::AlignCenter);
    Text->show();
}

int MainWindow::makeAction(int i, int j)
{
    /* Обработка хода игрока */

    int count = 0;

    if (!Flag[1])
        count = setColorField(i, j);
    else
        if (Player == Algorithm::Player::White) removeEnemyField(Algorithm::Player::Black, i, j);
        else if (Player == Algorithm::Player::Black) removeEnemyField(Algorithm::Player::White, i, j);

    return count;
}

int MainWindow::setColorField(int i, int j)
{
    /* Установка цвета поля, вывод результатов на сцене игры */

    int count = 0;

    if ((Alg->StateField[i][j] == Player) && (Flag[0]))
    {
        OwnCh[Player][0]++;
        OwnCh[Player][1]--;
        Alg->StateField[i][j] = Algorithm::Player::Unknown;

        Field[i][j]->setIcon(QIcon(":/images/grayfield.png"));

        Result[Player][2]->setText(QString::number(OwnCh[Player][0]));
        Result[Player][3]->setText(QString::number(OwnCh[Player][1]));

        Flag[0] = false;

        Field[i][j]->setIcon(QIcon(":/images/greenfield.png"));
        Alg->StateField[i][j] = Algorithm::Player::Available;

        setAvailableField(i, j);
    }
    else
    {
        if (Flag[0])
        {
            if (Alg->StateField[i][j] == Algorithm::Player::Unknown)
            {
                if (OwnCh[Player][0] > 0)
                {
                    OwnCh[Player][0]--;
                    OwnCh[Player][1]++;
                    Alg->StateField[i][j] = Player;

                    if (Player == Algorithm::Player::White)
                        Field[i][j]->setIcon(QIcon(":/images/whitefield.png"));
                    else
                        Field[i][j]->setIcon(QIcon(":/images/blackfield.png"));

                    Result[Player][2]->setText(QString::number(OwnCh[Player][0]));
                    Result[Player][3]->setText(QString::number(OwnCh[Player][1]));

                    EnemyCh[Player] += count = Alg->threeInRow(Player, Alg->StateField, Alg->EnemyField);
                    Result[Player][5]->setText(QString::number(EnemyCh[Player]));

                    if (Player == Algorithm::Player::White)
                    {
                        OwnCh[Algorithm::Player::Black][1] -= count;
                        Result[Algorithm::Player::Black][3]->
                            setText(QString::number(OwnCh[Algorithm::Player::Black][1]));
                    }
                    else
                    {
                        OwnCh[Algorithm::Player::White][1] -= count;
                        Result[Algorithm::Player::White][3]->
                            setText(QString::number(OwnCh[Algorithm::Player::White][1]));
                    }

                    Flag[0] = true;
                }

                Flag[2] = true;
            }
        }
        else
        {
            if (Alg->StateField[i][j] == Algorithm::Player::Available)
            {
                OwnCh[Player][0]--;
                OwnCh[Player][1]++;
                Alg->StateField[i][j] = Player;

                setColorOnScene(Player, i, j);

                Result[Player][2]->setText(QString::number(OwnCh[Player][0]));
                Result[Player][3]->setText(QString::number(OwnCh[Player][1]));

                EnemyCh[Player] += count = Alg->threeInRow(Player, Alg->StateField, Alg->EnemyField);
                Result[Player][5]->setText(QString::number(EnemyCh[Player]));

                if (Player == Algorithm::Player::White)
                {
                    OwnCh[Algorithm::Player::Black][1] -= count;
                    Result[Algorithm::Player::Black][3]->
                        setText(QString::number(OwnCh[Algorithm::Player::Black][1]));
                }
                else
                {
                    OwnCh[Algorithm::Player::White][1] -= count;
                    Result[Algorithm::Player::White][3]->
                        setText(QString::number(OwnCh[Algorithm::Player::White][1]));
                }

                Flag[0] = true;

                for (int k = 0; k < 8; k++)
                    for (int l = 0; l < 3; l++)
                        if (Alg->StateField[k][l] == Algorithm::Player::Available)
                        {
                            Alg->StateField[k][l] = Algorithm::Player::Unknown;
                            Field[k][l]->setIcon(QIcon(":/images/grayfield.png"));
                        }

                Flag[2] = true;
            }
        }
    }

    return count;
}

void MainWindow::setAvailableField(int i, int j)
{
    /* Отображение на сцене допустимых к выбору полей */

    if (OwnCh[Player][0] > 1) return;

    if (OwnCh[Player][0] + OwnCh[Player][1] == 3)
    {
        for (int k = 0; k < 8; k++)
            for (int l = 0; l < 3; l++)
                if (Alg->StateField[k][l] == Algorithm::Player::Unknown)
                {
                    Field[k][l]->setIcon(QIcon(":/images/greenfield.png"));
                    Alg->StateField[k][l] = Algorithm::Player::Available;
                }
    }
    else
    {
        Alg->makeAvailableField(i, j, Alg->StateField);

        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
                if (Alg->StateField[i][j] == Algorithm::Player::Available)
                    Field[i][j]->setIcon(QIcon(":/images/greenfield.png"));
    }
}

void MainWindow::removeEnemyField(Algorithm::Player Player, int i, int j)
{
    /* Удаление шашки соперника */

    if ((EnemyCh[Algorithm::Player::White] > 6) || (EnemyCh[Algorithm::Player::Black] > 6))
        return;

    if (!Flag[1])
    {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
                if (Alg->StateField[i][j] == Player)
                {
                    Field[i][j]->setIcon(QIcon(":/images/greenfield.png"));
                    Alg->StateField[i][j] = Algorithm::Player::Available;
                }

        Flag[1] = true;
        Flag[2] = false;
    }
    else
        if (Alg->StateField[i][j] == Algorithm::Player::Available)
        {
            Field[i][j]->setIcon(QIcon(":/images/grayfield.png"));
            Alg->StateField[i][j] = Algorithm::Player::Unknown;

            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 3; j++)
                    if (Alg->StateField[i][j] == Algorithm::Player::Available)
                        if (Player == Algorithm::Player::White)
                        {
                            Field[i][j]->setIcon(QIcon(":/images/whitefield.png"));
                            Alg->StateField[i][j] = Algorithm::Player::White;
                        }
                        else
                            if (Player == Algorithm::Player::Black)
                            {
                                Field[i][j]->setIcon(QIcon(":/images/blackfield.png"));
                                Alg->StateField[i][j] = Algorithm::Player::Black;
                            }

            Flag[1] = false;
            Flag[2] = true;
        }
}

void MainWindow::setResultPC()
{
    /* Реализация хода компьютера */

    srand(unsigned int(time(NULL)));

    int percent = rand() % 101;
    bool f = false;

    if (((percent >= 0) && (percent < 25)) && (Alg->LvlPC == Algorithm::Level::Easy)) f = true;
    if (((percent >= 0) && (percent < 50)) && (Alg->LvlPC == Algorithm::Level::Middle)) f = true;
    if (Alg->LvlPC == Algorithm::Level::Hard) f = true;

    if (f)
    {
        Alg->createDTree((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White), Alg->Head, OwnCh,
                         -1, -1, -1, -1, 0);

        Alg->getValue(Alg->Head, 0);

        if (!Alg->step1(Alg->Head, Player, 0))
            if (!Alg->step2(Alg->Head, Player, 0))
                if (!Alg->step3(Alg->Head, Player))
                {
                    if (OwnCh[Player][0] > 0)
                    {
                        srand(unsigned int(time(NULL)));

                        int i = rand() % 8;
                        int j = rand() % 3;

                        while (Alg->StateField[i][j] != Algorithm::Player::Unknown)
                        {
                            i = rand() % 8;
                            j = rand() % 3;
                        }

                        qDebug() << "rand(2): " << i << j;

                        Alg->StateField[i][j] = Player;
                    }
                    else
                    {
                        if (OwnCh[Player][1] > 3)
                        {
                            srand(unsigned int(time(NULL)));

                            int i = rand() % 8;
                            int j = rand() % 3;

                            while (Alg->StateField[i][j] != Player)
                            {
                                i = rand() % 8;
                                j = rand() % 3;
                            }

                            Alg->StateField[i][j] = Algorithm::Player::Unknown;

                            Alg->makeAvailableField(i, j, Alg->StateField);

                            int k = rand() % 8;
                            int l = rand() % 3;

                            while (Alg->StateField[k][l] != Algorithm::Player::Available)
                            {
                                k = rand() % 8;
                                l = rand() % 3;
                            }

                            Alg->StateField[k][l] = Player;

                            for (int i = 0; i < 8; i++)
                                for (int j = 0; j < 3; j++)
                                    if (Alg->StateField[i][j] == Algorithm::Player::Available)
                                        Alg->StateField[i][j] = Algorithm::Player::Unknown;

                            qDebug() << "rand(3): " << i << j << "/" << k << l;
                        }
                        else
                        {
                            srand(unsigned int(time(NULL)));

                            int i = rand() % 8;
                            int j = rand() % 3;

                            while (Alg->StateField[i][j] != Player)
                            {
                                i = rand() % 8;
                                j = rand() % 3;
                            }

                            Alg->StateField[i][j] = Algorithm::Player::Unknown;

                            int k = rand() % 8;
                            int l = rand() % 3;

                            while (Alg->StateField[k][l] != Algorithm::Player::Unknown)
                            {
                                if ((k == i) && (l == j)) continue;

                                k = rand() % 8;
                                l = rand() % 3;
                            }

                            Alg->StateField[k][l] = Player;

                            qDebug() << "rand(4): " << i << j << "/" << k << l;
                        }
                    }
                }

        Alg->deleteDTree(Alg->Head);
    }
    else
    {
        if (OwnCh[Player][0] > 0)
        {
            srand(unsigned int(time(NULL)));

            int i = rand() % 8;
            int j = rand() % 3;

            while (Alg->StateField[i][j] != Algorithm::Player::Unknown)
            {
                i = rand() % 8;
                j = rand() % 3;
            }

            qDebug() << "rand(2): " << i << j;

            Alg->StateField[i][j] = Player;
        }
        else
        {
            if (OwnCh[Player][1] > 3)
            {
                srand(unsigned int(time(NULL)));

                int i = rand() % 8;
                int j = rand() % 3;

                while (Alg->StateField[i][j] != Player)
                {
                    i = rand() % 8;
                    j = rand() % 3;
                }

                Alg->StateField[i][j] = Algorithm::Player::Unknown;

                Alg->makeAvailableField(i, j, Alg->StateField);

                int k = rand() % 8;
                int l = rand() % 3;

                while (Alg->StateField[k][l] != Algorithm::Player::Available)
                {
                    k = rand() % 8;
                    l = rand() % 3;
                }

                Alg->StateField[k][l] = Player;

                for (int i = 0; i < 8; i++)
                    for (int j = 0; j < 3; j++)
                        if (Alg->StateField[i][j] == Algorithm::Player::Available)
                            Alg->StateField[i][j] = Algorithm::Player::Unknown;

                qDebug() << "rand(3): " << i << j << "/" << k << l;
            }
            else
            {
                srand(unsigned int(time(NULL)));

                int i = rand() % 8;
                int j = rand() % 3;

                while (Alg->StateField[i][j] != Player)
                {
                    i = rand() % 8;
                    j = rand() % 3;
                }

                Alg->StateField[i][j] = Algorithm::Player::Unknown;

                int k = rand() % 8;
                int l = rand() % 3;

                while (Alg->StateField[k][l] != Algorithm::Player::Unknown)
                {
                    if ((k == i) && (l == j)) continue;

                    k = rand() % 8;
                    l = rand() % 3;
                }

                Alg->StateField[k][l] = Player;

                qDebug() << "rand(4): " << i << j << "/" << k << l;
            }
        }
    }

    if (OwnCh[Player][0] >= 1)
    {
        OwnCh[Player][0]--;
        OwnCh[Player][1]++;
        Result[Player][2]->setText(QString::number(OwnCh[Player][0]));
        Result[Player][3]->setText(QString::number(OwnCh[Player][1]));
    }

    int count = 0;

    EnemyCh[Player] += count = Alg->threeInRow(Player, Alg->StateField, Alg->EnemyField);
    Result[Player][5]->setText(QString::number(EnemyCh[Player]));

    if (Player == Algorithm::Player::White)
    {
        OwnCh[Algorithm::Player::Black][1] -= count;
        Result[Algorithm::Player::Black][3]->
            setText(QString::number(OwnCh[Algorithm::Player::Black][1]));
    }
    else
    {
        OwnCh[Algorithm::Player::White][1] -= count;
        Result[Algorithm::Player::White][3]->
            setText(QString::number(OwnCh[Algorithm::Player::White][1]));
    }

    if (count > 0) Alg->createExtraDTree(Player, OwnCh);

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
        {
            if (Alg->StateField[i][j] == Algorithm::Player::Black)
                Field[i][j]->setIcon(QIcon(":/images/blackfield.png"));

            if (Alg->StateField[i][j] == Algorithm::Player::White)
                Field[i][j]->setIcon(QIcon(":/images/whitefield.png"));

            if (Alg->StateField[i][j] == Algorithm::Player::Available)
                Field[i][j]->setIcon(QIcon(":/images/greenfield.png"));

            if (Alg->StateField[i][j] == Algorithm::Player::Unknown)
                Field[i][j]->setIcon(QIcon(":/images/grayfield.png"));
        }
}

bool MainWindow::showWinner()
{
    /* Вывод победителя на экран */

    if (EnemyCh[Algorithm::Player::White] > 6)
    {
        blockSignals(true);

        GameOver go(this);
        go.setWinner(Algorithm::Player::White);
        go.exec();

        showText(Algorithm::Player::White, Situation::Win);

        return true;
    }
    else
        if (EnemyCh[Algorithm::Player::Black] > 6)
        {
            blockSignals(true);

            GameOver go(this);
            go.setWinner(Algorithm::Player::Black);
            go.exec();

            showText(Algorithm::Player::Black, Situation::Win);

            return true;
        }

    return false;
}

void MainWindow::Field_clicked()
{
    /* Обработка нажатия клавиши */

    int n, m;
    int count = 0;

    if (!signalsBlocked())
    {
        QPushButton *pField = static_cast <QPushButton *> (sender());

        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
                if (pField == this->Field[i][j])
                {
                    n = i;
                    m = j;
                }

        if (OwnCh[Player][0] > 0)
            count = makeAction(n, m);
        else
            if ((Alg->StateField[n][m] == Player) || (Alg->StateField[n][m] == Algorithm::Player::Available))
                count = makeAction(n, m);
            else
                Flag[2] = false;

        if (count > 0)
        {
            if (Player == Algorithm::Player::White) removeEnemyField(Algorithm::Player::Black, n, m);
            else if (Player == Algorithm::Player::Black) removeEnemyField(Algorithm::Player::White, n, m);
        }

        if (Alg->OldStateField[n][m] == Alg->StateField[n][m])
            Flag[2] = false;

        if (showWinner()) return;

        if (Flag[2])
        {
            changeTurn();

            Flag[2] = false;
            Flag[3] = true;
        }
    }

    if (Alg->LvlPC == Algorithm::Level::Opponent)
    {
        setHistory(History, OwnCh, EnemyCh, Alg->StateField, Alg->OldStateField, Alg->EnemyField, Alg->CountSteps);
        UndoHistory.push(*History);

        return;
    }
    else
        if (Flag[3])
        {
            makePCStep(n, m);
            Flag[3] = false;

            if (showWinner()) return;

//            setHistory(History, OwnCh, EnemyCh, Alg->StateField, Alg->OldStateField, Alg->EnemyField, Alg->CountSteps);
//            UndoHistory.push(*History);
        }
}

void MainWindow::makePCStep(int i, int j)
{
    /* Начало обработки хода компьютера */

    blockSignals(true);

    if (OwnCh[0][1] + OwnCh[1][1] == 1)
        setFirstFieldPC(i, j);
    else
        setResultPC();

    changeTurn();

    blockSignals(false);
}

void MainWindow::changeTurn()
{
    /* Смена хода */

    Alg->CountSteps[Player]++;

    if (Player == Algorithm::Player::White)
        Player = Algorithm::Player::Black;
    else
        Player = Algorithm::Player::White;

    showText(Player, Situation::Turn);

    Alg->casheStateField();
}

void MainWindow::setFirstFieldPC(int i, int j)
{
    /* Случайный выбор постановки первой шашки компьютера (вблизости от шашки соперника) */

    srand(unsigned int(time(NULL)));

    int vector = rand()%4 + 1;

    if (vector == 1)
    {
        if (j + 1 != 3)
        {
            Alg->StateField[i][j + 1] = Player;
            setColorOnScene(Player, i, j + 1);
        }
        else
        {
            Alg->StateField[i][0] = Player;
            setColorOnScene(Player, i, 0);
        }
    }

    if (vector == 2)
    {
        if (j - 1 != -1)
        {
            Alg->StateField[i][j - 1] = Player;
            setColorOnScene(Player, i, j - 1);
        }
        else
        {
            Alg->StateField[i][2] = Player;
            setColorOnScene(Player, i, 2);
        }
    }

    if (vector == 3)
    {
        if (i + 1 != 8)
        {
            Alg->StateField[i + 1][j] = Player;
            setColorOnScene(Player, i + 1, j);
        }
        else
        {
            Alg->StateField[0][j] = Player;
            setColorOnScene(Player, 0, j);
        }
    }

    if (vector == 4)
    {
        if (i - 1 != -1)
        {
            Alg->StateField[i - 1][j] = Player;
            setColorOnScene(Player, i - 1, j);
        }
        else
        {
            Alg->StateField[7][j] = Player;
            setColorOnScene(Player, 7, j);
        }
    }

    OwnCh[Player][0]--;
    OwnCh[Player][1]++;
    Result[Player][2]->setText(QString::number(OwnCh[Player][0]));
    Result[Player][3]->setText(QString::number(OwnCh[Player][1]));
}

void MainWindow::setColorOnScene(Algorithm::Player Player, int i, int j)
{
    /* Расстановка шашек на поле */

    if (Player == Algorithm::Player::Black)
        Field[i][j]->setIcon(QIcon(":/images/blackfield.png"));
    else
        Field[i][j]->setIcon(QIcon(":/images/whitefield.png"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    /* Переопределенная функция, обрабатывающая события "close" */

    ExitGame eg(this);
    int res = eg.exec();

    if (res == ExitGame::Accepted)
        event->accept();
    else
        event->ignore();
}

void MainWindow::on_actionNewGame_triggered()
{
    /* Запуск новой игры */

    NewGame st(this);
    int res = st.exec();
    if (res == NewGame::Rejected) return;

    if (Btn_NewGame)
    {
        delete Btn_NewGame;
        Btn_NewGame = nullptr;

        delete Btn_Settings;
        Btn_Settings = nullptr;
    }

    if (Field) deleteScene();

    createScene();
    Alg->clearOldStateField();
}

void MainWindow::on_actionSettings_triggered()
{
    /* Открытие настроек */

    Settings setgame(this);
    int res = setgame.exec();
    if (res == Settings::Rejected) return;
    else
    {
        if (Btn_NewGame)
        {
            delete Btn_NewGame;
            Btn_NewGame = nullptr;

            delete Btn_Settings;
            Btn_Settings = nullptr;
        }

        if (Field) deleteScene();

        Level = setgame.getLevel();

        createScene();
        Alg->clearOldStateField();
    }
}

void MainWindow::on_actionRules_triggered()
{
    /* Открытие правил игры */

    Rules rdialog(this);
    int res = rdialog.exec();
    if (res == Rules::Rejected) return;
}

void MainWindow::on_actionMain_Page_triggered()
{
    GoTo_MainPage mp(this);
    int res = mp.exec();
    if (res == GoTo_MainPage::Rejected) return;
    else
        if (Field)
        {
            deleteScene();
            setMainPage();
        }
}

void MainWindow::on_actionUndo_triggered()
{
    /* Отмена хода */

    if (!UndoHistory.isEmpty())
    {
        (*History) = UndoHistory.pop();
        getHistory(History);
    }
}
