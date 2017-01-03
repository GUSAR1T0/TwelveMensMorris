#include "algorithm.h"
#include <ctime>
#include <QDebug>

Algorithm::Algorithm()
{
    StateField = new Algorithm::Player *[8];
    for (int i = 0; i < 8; i++)
        StateField[i] = new Algorithm::Player[3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            StateField[i][j] = Algorithm::Player::Unknown;

    OldStateField = new Algorithm::Player *[8];
    for (int i = 0; i < 8; i++)
        OldStateField[i] = new Algorithm::Player[3];

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            OldStateField[i][j] = Algorithm::Player::Unknown;

    EnemyField = new bool *[2];
    for (int i = 0; i < 2; i++)
        EnemyField[i] = new bool[20];

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 20; j++)
            EnemyField[i][j] = true;

    CountSteps = new int[2];
    CountSteps[0] = CountSteps[1] = 0;
}

Algorithm::~Algorithm()
{
    for (int i = 0; i < 8; i++)
    {
        delete StateField[i];
        StateField[i] = nullptr;
    }

    delete StateField;
    StateField = nullptr;

    for (int i = 0; i < 8; i++)
    {
        delete OldStateField[i];
        OldStateField[i] = nullptr;
    }

    delete OldStateField;
    OldStateField = nullptr;

    for (int i = 0; i < 2; i++)
    {
        delete EnemyField[i];
        EnemyField[i] = nullptr;
    }

    delete EnemyField;
    EnemyField = nullptr;

    delete CountSteps;
    CountSteps = nullptr;
}

void Algorithm::createDTree(Algorithm::Player Player, DecisionTree *&TemporaryPointer, int ReserveField[2][2],
                            int X1, int Y1, int X2, int Y2, int Level)
{
    /* Создание дерева решений */

    DecisionTree *tmp = new DecisionTree();

    tmp->Player = Player;
    tmp->PosOnScene[0][0] = X1;
    tmp->PosOnScene[0][1] = Y1;
    tmp->PosOnScene[1][0] = X2;
    tmp->PosOnScene[1][1] = Y2;

    if (ReserveField[Player][0] >= 1)   //Если шашки еще имеются в запасе...
    {
        ReserveField[Player][0]--;

        if (Level <= 2)
        {
            int count = 0;

            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 3; j++)
                    if (StateField[i][j] == Algorithm::Player::Unknown) count++;

            tmp->CountChilds = count;
            tmp->Child = new DecisionTree *[tmp->CountChilds];

            bool f = false;
            int i = 0, j = 0;

            for (int k = 0; k < tmp->CountChilds; k++)
            {
                while (StateField[i][j] != Algorithm::Player::Unknown)
                {
                    if (j + 1 != 3)
                        j++;
                    else
                        if (i + 1 != 8)
                        {
                            i++;
                            j = 0;
                        }
                        else
                        {
                            f = true;
                            break;
                        }
                }

                if (!f)
                {
                    StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);

                    createDTree((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White), tmp->Child[k],
                                ReserveField, -1, -1, i, j, Level + 1);

                    StateField[i][j] = Algorithm::Player::Unknown;
                }

                if (j + 1 != 3)
                    j++;
                else
                    if (i + 1 != 8)
                    {
                        i++;
                        j = 0;
                    }
                    else
                        break;
            }
        }
        else
        {
            tmp->CountChilds = 0;
            tmp->Child = nullptr;
        }

        ReserveField[Player][0]++;
    }
    else
    {
        if (ReserveField[Player][1] > 3)    //Если в запасе не имеются, но у игрока на поле больше трех шашек...
        {
            if (Level <= 2)
            {
                int count = 0;

                for (int i = 0; i < 8; i++)
                    for (int j = 0; j < 3; j++)
                        if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                        {
                            makeAvailableField(i, j, StateField);

                            StateField[i][j] = Algorithm::Player::Unknown;

                            for (int k = 0; k < 8; k++)
                                for (int l = 0; l < 3; l++)
                                    if (StateField[k][l] == Algorithm::Player::Available)
                                    {
                                        count++;
                                        StateField[k][l] = Algorithm::Player::Unknown;
                                    }

                            StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);
                        }

                tmp->CountChilds = count;
                tmp->Child = new DecisionTree *[tmp->CountChilds];

                count = 0;

                for (int i = 0; i < 8; i++)
                    for (int j = 0; j < 3; j++)
                        if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                        {
                            makeAvailableField(i, j, this->StateField);

                            this->StateField[i][j] = Algorithm::Player::Unknown;

                            Algorithm::Player StateField[8][3];

                            for (int a = 0; a < 8; a++)
                                for (int b = 0; b < 3; b++)
                                    StateField[a][b] = this->StateField[a][b];

                            for (int k = 0; k < 8; k++)
                                for (int l = 0; l < 3; l++)
                                    if (this->StateField[k][l] == Algorithm::Player::Available)
                                    {
                                        this->StateField[k][l] = (Player == Algorithm::Player::White ? Algorithm::Player::Black :
                                                                                                       Algorithm::Player::White);

                                        for (int a = 0; a < 8; a++)
                                            for (int b = 0; b < 3; b++)
                                                if (this->StateField[a][b] == Algorithm::Player::Available)
                                                    this->StateField[a][b] = Algorithm::Player::Unknown;

                                        createDTree((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White),
                                                    tmp->Child[count], ReserveField, i, j, k, l, Level + 1);

                                        count++;

                                        for (int a = 0; a < 8; a++)
                                            for (int b = 0; b < 3; b++)
                                                this->StateField[a][b] = StateField[a][b];
                                    }

                            this->StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);

                            for (int a = 0; a < 8; a++)
                                for (int b = 0; b < 3; b++)
                                    if (this->StateField[a][b] == Algorithm::Player::Available)
                                        this->StateField[a][b] = Algorithm::Player::Unknown;
                        }
            }
            else
            {
                tmp->CountChilds = 0;
                tmp->Child = nullptr;
            }
        }
        else        //Если на поле три шашки и меньше...
        {
            if (Level <= 2)
            {
                int count = 0;

                for (int i = 0; i < 8; i++)
                    for (int j = 0; j < 3; j++)
                        if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                        {
                            for (int k = 0; k < 8; k++)
                                for (int l = 0; l < 3; l++)
                                    if (StateField[k][l] == Algorithm::Player::Unknown)
                                        StateField[k][l] = Algorithm::Player::Available;

                            StateField[i][j] = Algorithm::Player::Unknown;

                            for (int k = 0; k < 8; k++)
                                for (int l = 0; l < 3; l++)
                                    if (StateField[k][l] == Algorithm::Player::Available)
                                    {
                                        StateField[k][l] = Algorithm::Player::Unknown;
                                        count++;
                                    }

                            StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);
                        }

                tmp->CountChilds = count;
                tmp->Child = new DecisionTree *[tmp->CountChilds];

                count = 0;

                for (int i = 0; i < 8; i++)
                    for (int j = 0; j < 3; j++)
                        if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                        {
                            for (int k = 0; k < 8; k++)
                                for (int l = 0; l < 3; l++)
                                    if (StateField[k][l] == Algorithm::Player::Unknown)
                                        StateField[k][l] = Algorithm::Player::Available;

                            this->StateField[i][j] = Algorithm::Player::Unknown;

                            Algorithm::Player StateField[8][3];

                            for (int a = 0; a < 8; a++)
                                for (int b = 0; b < 3; b++)
                                    StateField[a][b] = this->StateField[a][b];

                            for (int k = 0; k < 8; k++)
                                for (int l = 0; l < 3; l++)
                                    if (this->StateField[k][l] == Algorithm::Player::Available)
                                    {
                                        this->StateField[k][l] = (Player == Algorithm::Player::White ? Algorithm::Player::Black :
                                                                                                       Algorithm::Player::White);

                                        for (int a = 0; a < 8; a++)
                                            for (int b = 0; b < 3; b++)
                                                if (this->StateField[a][b] == Algorithm::Player::Available)
                                                    this->StateField[a][b] = Algorithm::Player::Unknown;

                                        createDTree((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White),
                                                    tmp->Child[count], ReserveField, i, j, k, l, Level + 1);

                                        count++;

                                        for (int a = 0; a < 8; a++)
                                            for (int b = 0; b < 3; b++)
                                                this->StateField[a][b] = StateField[a][b];
                                    }

                            this->StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);

                            for (int a = 0; a < 8; a++)
                                for (int b = 0; b < 3; b++)
                                    if (this->StateField[a][b] == Algorithm::Player::Available)
                                        this->StateField[a][b] = Algorithm::Player::Unknown;
                        }
            }
            else
            {
                tmp->CountChilds = 0;
                tmp->Child = nullptr;
            }
        }
    }

    TemporaryPointer = tmp;
}

void Algorithm::createExtraDTree(Algorithm::Player Player, int ReserveField[2][2])
{
    /* Поиск такой шашки, которую наиболее выгодно убрать с поля */

    int count = 0;
    int amount = 0;
    DecisionTree *tmp;

    if (ReserveField[(Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White)][0] >= 1)
    {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
                if (StateField[i][j] == Algorithm::Player::Unknown) count++;

        tmp = new DecisionTree[count];
        amount = count;

        bool f = false;
        int i = 0, j = 0;

        for (int k = 0; k < count; k++)
        {
            while (StateField[i][j] != Algorithm::Player::Unknown)
            {
                if (j + 1 != 3)
                    j++;
                else
                    if (i + 1 != 8)
                    {
                        i++;
                        j = 0;
                    }
                    else
                    {
                        f = true;
                        break;
                    }
            }

            if (!f)
            {
                bool EnemyField[2][20];

                for (int n = 0; n < 2; n++)
                    for (int m = 0; m < 20; m++)
                        EnemyField[n][m] = this->EnemyField[n][m];

                StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);

                tmp[k].PosOnScene[0][0] = tmp[k].PosOnScene[0][1] = -1;
                tmp[k].PosOnScene[1][0] = i;
                tmp[k].PosOnScene[1][1] = j;
                tmp[k].Value = threeInRow((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White), StateField, this->EnemyField);

                StateField[i][j] = Algorithm::Player::Unknown;

                for (int n = 0; n < 2; n++)
                    for (int m = 0; m < 20; m++)
                        this->EnemyField[n][m] = EnemyField[n][m];

            }
        }
    }
    else
    {
        if (ReserveField[(Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White)][1] > 3)
        {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 3; j++)
                    if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                    {
                        makeAvailableField(i, j, StateField);

                        StateField[i][j] = Algorithm::Player::Unknown;

                        for (int k = 0; k < 8; k++)
                            for (int l = 0; l < 3; l++)
                                if (StateField[k][l] == Algorithm::Player::Available)
                                {
                                    count++;
                                    StateField[k][l] = Algorithm::Player::Unknown;
                                }

                        StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);
                    }

            tmp = new DecisionTree[count];
            amount = count;

            count = 0;

            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 3; j++)
                    if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                    {
                        makeAvailableField(i, j, this->StateField);

                        this->StateField[i][j] = Algorithm::Player::Unknown;

                        Algorithm::Player StateField[8][3];

                        for (int a = 0; a < 8; a++)
                            for (int b = 0; b < 3; b++)
                                StateField[a][b] = this->StateField[a][b];

                        for (int k = 0; k < 8; k++)
                            for (int l = 0; l < 3; l++)
                                if (this->StateField[k][l] == Algorithm::Player::Available)
                                {
                                    bool EnemyField[2][20];

                                    for (int n = 0; n < 2; n++)
                                        for (int m = 0; m < 20; m++)
                                            EnemyField[n][m] = this->EnemyField[n][m];

                                    this->StateField[k][l] = (Player == Algorithm::Player::White ? Algorithm::Player::Black :
                                                                                                   Algorithm::Player::White);

                                    for (int a = 0; a < 8; a++)
                                        for (int b = 0; b < 3; b++)
                                            if (this->StateField[a][b] == Algorithm::Player::Available)
                                                this->StateField[a][b] = Algorithm::Player::Unknown;

                                    tmp[count].PosOnScene[0][0] = i;
                                    tmp[count].PosOnScene[0][1] = j;
                                    tmp[count].PosOnScene[1][0] = k;
                                    tmp[count].PosOnScene[1][1] = l;
                                    tmp[count].Value = threeInRow((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White),
                                                                   this->StateField, this->EnemyField);

                                    count++;

                                    for (int a = 0; a < 8; a++)
                                        for (int b = 0; b < 3; b++)
                                            this->StateField[a][b] = StateField[a][b];

                                    for (int n = 0; n < 2; n++)
                                        for (int m = 0; m < 20; m++)
                                            this->EnemyField[n][m] = EnemyField[n][m];
                                }

                        this->StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);

                        for (int a = 0; a < 8; a++)
                            for (int b = 0; b < 3; b++)
                                if (this->StateField[a][b] == Algorithm::Player::Available)
                                    this->StateField[a][b] = Algorithm::Player::Unknown;
                    }
        }
        else
        {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 3; j++)
                    if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                    {
                        for (int k = 0; k < 8; k++)
                            for (int l = 0; l < 3; l++)
                                if (StateField[k][l] == Algorithm::Player::Unknown)
                                    StateField[k][l] = Algorithm::Player::Available;

                        StateField[i][j] = Algorithm::Player::Unknown;

                        for (int k = 0; k < 8; k++)
                            for (int l = 0; l < 3; l++)
                                if (StateField[k][l] == Algorithm::Player::Available)
                                {
                                    StateField[k][l] = Algorithm::Player::Unknown;
                                    count++;
                                }

                        StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);
                    }

            tmp = new DecisionTree[count];
            amount = count;

            count = 0;

            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 3; j++)
                    if (StateField[i][j] == (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
                    {
                        for (int k = 0; k < 8; k++)
                            for (int l = 0; l < 3; l++)
                                if (StateField[k][l] == Algorithm::Player::Unknown)
                                    StateField[k][l] = Algorithm::Player::Available;

                        this->StateField[i][j] = Algorithm::Player::Unknown;

                        Algorithm::Player StateField[8][3];

                        for (int a = 0; a < 8; a++)
                            for (int b = 0; b < 3; b++)
                                StateField[a][b] = this->StateField[a][b];

                        for (int k = 0; k < 8; k++)
                            for (int l = 0; l < 3; l++)
                                if (this->StateField[k][l] == Algorithm::Player::Available)
                                {
                                    bool EnemyField[2][20];

                                    for (int n = 0; n < 2; n++)
                                        for (int m = 0; m < 20; m++)
                                            EnemyField[n][m] = this->EnemyField[n][m];

                                    this->StateField[k][l] = (Player == Algorithm::Player::White ? Algorithm::Player::Black :
                                                                                                   Algorithm::Player::White);

                                    for (int a = 0; a < 8; a++)
                                        for (int b = 0; b < 3; b++)
                                            if (this->StateField[a][b] == Algorithm::Player::Available)
                                                this->StateField[a][b] = Algorithm::Player::Unknown;

                                    tmp[count].PosOnScene[0][0] = i;
                                    tmp[count].PosOnScene[0][1] = j;
                                    tmp[count].PosOnScene[1][0] = k;
                                    tmp[count].PosOnScene[1][1] = l;
                                    tmp[count].Value = threeInRow((Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White),
                                                                   this->StateField, this->EnemyField);

                                    count++;

                                    for (int a = 0; a < 8; a++)
                                        for (int b = 0; b < 3; b++)
                                            this->StateField[a][b] = StateField[a][b];

                                    for (int n = 0; n < 2; n++)
                                        for (int m = 0; m < 20; m++)
                                            this->EnemyField[n][m] = EnemyField[n][m];
                                }

                        this->StateField[i][j] = (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White);

                        for (int a = 0; a < 8; a++)
                            for (int b = 0; b < 3; b++)
                                if (this->StateField[a][b] == Algorithm::Player::Available)
                                    this->StateField[a][b] = Algorithm::Player::Unknown;
                    }
        }
    }

    for (int i = 0; i < amount; i++)
        if (tmp[i].Value != 0)
        {
            if (tmp[i].PosOnScene[0][0] == -1)
            {
                StateField[tmp[i].PosOnScene[1][0]][tmp[i].PosOnScene[1][1]] = Algorithm::Player::Unknown;
                qDebug() << "createExtraDTree(1): " << tmp[i].PosOnScene[1][0] << tmp[i].PosOnScene[1][1];
                return;
            }
            else
            {
                StateField[tmp[i].PosOnScene[0][0]][tmp[i].PosOnScene[0][1]] = Algorithm::Player::Unknown;
                qDebug() << "createExtraDTree(2): " << tmp[i].PosOnScene[0][0] << tmp[i].PosOnScene[0][1];
                return;
            }
        }

    srand(unsigned int(time(NULL)));

    int i = rand() % 8;
    int j = rand() % 3;

    while (StateField[i][j] != (Player == Algorithm::Player::White ? Algorithm::Player::Black : Algorithm::Player::White))
    {
        i = rand() % 8;
        j = rand() % 3;
    }

    StateField[i][j] = Algorithm::Player::Unknown;
    qDebug() << "createExtraDTree(3): " << i << j;
}

bool Algorithm::step1(DecisionTree *&tmp, Algorithm::Player Player, int Level)
{
    /* Поиск возможного хода соперника, тем самым помешать ему сделать этот ход */

    if ((tmp->Value == 1) && (Level == 1)) return false;

    if ((tmp->Child) && (Level < 2))
    {
        for (int i = 0; i < tmp->CountChilds; i++)
            if (step1(tmp->Child[i], Player, Level + 1)) return true;
    }
    else
    {
        if (tmp->Value == -1)
        {
            if (CountSteps[Player] < 9)
            {
                if (StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] == Algorithm::Player::Unknown)
                {
                    StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = Player;
                    qDebug() << "func1(1): " << tmp->PosOnScene[1][0] << tmp->PosOnScene[1][1];
                    return true;
                }
            }
            else
            {
                if (StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] == Algorithm::Player::Unknown)
                {
                    for (int i = 0; i < 8; i++)
                        for (int j = 0; j < 3; j++)
                            if (StateField[i][j] == Player)
                            {
                                makeAvailableField(i, j, StateField);

                                if (StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] == Algorithm::Player::Available)
                                {
                                    StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = Player;
                                    StateField[i][j] = Algorithm::Player::Unknown;

                                    for (int k = 0; k < 8; k++)
                                        for (int l = 0; l < 3; l++)
                                            if (StateField[k][l] == Algorithm::Player::Available)
                                                StateField[k][l] = Algorithm::Player::Unknown;

                                    qDebug() << "func1(2): " << tmp->PosOnScene[1][0] << tmp->PosOnScene[1][1];

                                    return true;
                                }

                                for (int k = 0; k < 8; k++)
                                    for (int l = 0; l < 3; l++)
                                        if (StateField[k][l] == Algorithm::Player::Available)
                                            StateField[k][l] = Algorithm::Player::Unknown;
                            }
                }
            }
        }
    }

    return false;
}

bool Algorithm::step2(DecisionTree *&tmp, Algorithm::Player Player, int Level)
{
    /* Поиск возможного хода компьютера, если первый шаг прошел безрезультатно */

    if ((tmp->Child) && (Level < 1))
    {
        for (int i = 0; i < tmp->CountChilds; i++)
            if (step2(tmp->Child[i], Player, Level + 1)) return true;
    }
    else
    {
        if (tmp->Value == 1)
        {
            if (CountSteps[Player] < 9)
            {
                if (StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] == Algorithm::Player::Unknown)
                {
                    StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = Player;
                    qDebug() << "func2(1): " << tmp->PosOnScene[1][0] << tmp->PosOnScene[1][1];
                    return true;
                }
            }
            else
                if (StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] == Algorithm::Player::Unknown)
                {
                    StateField[tmp->PosOnScene[0][0]][tmp->PosOnScene[0][1]] = Algorithm::Player::Unknown;
                    StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = Player;
                    qDebug() << "func2(2): " << tmp->PosOnScene[1][0] << tmp->PosOnScene[1][1];
                    return true;
                }
        }
    }

    return false;
}

bool Algorithm::step3(DecisionTree *&tmp, Algorithm::Player Player)
{
    /* Поиск возможного хода компьютера, если второй шаг прошел безрезультатно */

    if (tmp->Child)
    {
        for (int i = 0; i < tmp->CountChilds; i++)
            if (tmp->Child[i]->Child)
                for (int j = 0; j < tmp->Child[i]->CountChilds; j++)
                    if (tmp->Child[i]->Child[j]->Child)
                        for (int k = 0; k < tmp->Child[i]->Child[j]->CountChilds; k++)
                            if (tmp->Child[i]->Child[j]->Child[k]->Value == 1)
                            {
                                if (CountSteps[Player] < 9)
                                {
                                    if (StateField[tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][0]][tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][1]]
                                                                                                                                        == Algorithm::Player::Unknown)
                                    {
                                        StateField[tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][0]][tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][1]] = Player;
                                        qDebug() << "func3(1): " << tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][0]
                                                                 << tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][1];
                                        return true;
                                    }
                                }
                                else
                                {
                                    if (StateField[tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][0]][tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][1]]
                                                                                                                                        == Algorithm::Player::Unknown)
                                        {
                                            StateField[tmp->Child[i]->Child[j]->Child[k]->PosOnScene[0][0]][tmp->Child[i]->Child[j]->Child[k]->PosOnScene[0][1]]
                                                                                                                                         = Algorithm::Player::Unknown;
                                            StateField[tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][0]][tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][1]] = Player;
                                            qDebug() << "func3(2): " << tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][0]
                                                                     << tmp->Child[i]->Child[j]->Child[k]->PosOnScene[1][1];
                                            return true;
                                        }
                                }
                            }
    }

    return false;
}

void Algorithm::getValue(DecisionTree *&tmp, int Level)
{
    /* Получение оценки для каждой вершины и каждого листа дерева решений */

    if (Level > 0)
    {
        if ((tmp->PosOnScene[0][0] == -1) && (tmp->PosOnScene[0][1] == -1))
        {
            bool EnemyField[2][20];

            for (int n = 0; n < 2; n++)
                for (int m = 0; m < 20; m++)
                    EnemyField[n][m] = this->EnemyField[n][m];

            StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = tmp->Player;

            tmp->Value = (Level % 2 == 0 ? -threeInRow(tmp->Player, StateField, this->EnemyField) :
                                           +threeInRow(tmp->Player, StateField, this->EnemyField));

            if (Level <= 2)
                for (int k = 0; k < tmp->CountChilds; k++)
                    getValue(tmp->Child[k], Level + 1);

            StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = Algorithm::Player::Unknown;

            for (int n = 0; n < 2; n++)
                for (int m = 0; m < 20; m++)
                    this->EnemyField[n][m] = EnemyField[n][m];
        }
        else
        {
            bool EnemyField[2][20];

            for (int n = 0; n < 2; n++)
                for (int m = 0; m < 20; m++)
                    EnemyField[n][m] = this->EnemyField[n][m];

            StateField[tmp->PosOnScene[0][0]][tmp->PosOnScene[0][1]] = Algorithm::Player::Unknown;
            StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = tmp->Player;

            tmp->Value = (Level % 2 == 0 ? -threeInRow(tmp->Player, StateField, this->EnemyField) :
                                           +threeInRow(tmp->Player, StateField, this->EnemyField));

            if (Level <= 2)
                for (int k = 0; k < tmp->CountChilds; k++)
                    getValue(tmp->Child[k], Level + 1);

            StateField[tmp->PosOnScene[0][0]][tmp->PosOnScene[0][1]] = tmp->Player;
            StateField[tmp->PosOnScene[1][0]][tmp->PosOnScene[1][1]] = Algorithm::Player::Unknown;

            for (int n = 0; n < 2; n++)
                for (int m = 0; m < 20; m++)
                    this->EnemyField[n][m] = EnemyField[n][m];
        }
    }
    else
        for (int k = 0; k < tmp->CountChilds; k++)
            getValue(tmp->Child[k], Level + 1);
}

void Algorithm::showValue(DecisionTree *&tmp, int Level)
{
    qDebug() << Level << ": " << tmp->Player << " - " << tmp->PosOnScene[0][0] << ", " << tmp->PosOnScene[0][1] << "; "
             << tmp->PosOnScene[1][0] << ", " << tmp->PosOnScene[1][1] << "; " << tmp->Value << "; " << tmp->CountChilds;

    if (tmp->Child)
        for (int i = 0; i < tmp->CountChilds; i++)
            showValue(tmp->Child[i], Level + 1);
}

void Algorithm::deleteDTree(DecisionTree *&tmp)
{
    /* Удаление дерева решений */

    if (tmp->Child)
    {
        for (int i = 0; i < tmp->CountChilds; i++)
        {
            deleteDTree(tmp->Child[i]);

            delete tmp->Child[i];
            tmp->Child[i] = nullptr;
        }

        delete tmp->Child;
        tmp->Child = nullptr;
    }
}

int Algorithm::threeInRow(Algorithm::Player Player, Algorithm::Player **StateField, bool **EnemyField)
{
    /* Проверка выстроения трех фише в ряд */

    for (int i = 0; i < 8; i++)
        if (((StateField[i][0] == Player) &&
            (StateField[i][1] == Player) &&
            (StateField[i][2] == Player)) && (EnemyField[Player][i] == true))
        {
            EnemyField[Player][i] = false;
            return 1;
        }

    for (int j = 0; j < 3; j++)
    {
        if (((StateField[7][j] == Player) &&
            (StateField[0][j] == Player) &&
            (StateField[1][j] == Player)) && (EnemyField[Player][8 + j * 4] == true))
        {
            EnemyField[Player][8 + j * 4] = false;
            return 1;
        }
        if (((StateField[1][j] == Player) &&
            (StateField[2][j] == Player) &&
            (StateField[3][j] == Player)) && (EnemyField[Player][9 + j * 4] == true))
        {
            EnemyField[Player][9 + j * 4] = false;
            return 1;
        }
        if (((StateField[3][j] == Player) &&
            (StateField[4][j] == Player) &&
            (StateField[5][j] == Player)) && (EnemyField[Player][10 + j * 4] == true))
        {
            EnemyField[Player][10 + j * 4] = false;
            return 1;
        }
        if (((StateField[5][j] == Player) &&
            (StateField[6][j] == Player) &&
            (StateField[7][j] == Player)) && (EnemyField[Player][11 + j * 4] == true))
        {
            EnemyField[Player][11 + j * 4] = false;
            return 1;
        }
    }

    return 0;
}

void Algorithm::makeAvailableField(int i, int j, Algorithm::Player **StateField)
{
    /* Обособление подходящих фишек */

    if ((i - 1 >= 0) && (StateField[i - 1][j] == Algorithm::Player::Unknown))
        StateField[i - 1][j] = Algorithm::Player::Available;
    else
        if ((i - 1 == -1) && (StateField[7][j] == Algorithm::Player::Unknown))
            StateField[7][j] = Algorithm::Player::Available;

    if (i - 2 >= 0)
    {
        if (((StateField[i - 1][j] == Algorithm::Player::White) ||
            (StateField[i - 1][j] == Algorithm::Player::Black)) &&
            (StateField[i - 2][j] == Algorithm::Player::Unknown))
            StateField[i - 2][j] = Algorithm::Player::Available;
    }
    else
    {
        if (i - 2 == -1)
        {
            if (((StateField[0][j] == Algorithm::Player::White) ||
                (StateField[0][j] == Algorithm::Player::Black)) &&
                (StateField[7][j] == Algorithm::Player::Unknown))
                StateField[7][j] = Algorithm::Player::Available;
        }
        else
            if (i - 2 == -2)
                if (((StateField[7][j] == Algorithm::Player::White) ||
                    (StateField[7][j] == Algorithm::Player::Black)) &&
                    (StateField[6][j] == Algorithm::Player::Unknown))
                    StateField[6][j] = Algorithm::Player::Available;
    }

    if ((i + 1 <= 7) && (StateField[i + 1][j] == Algorithm::Player::Unknown))
        StateField[i + 1][j] = Algorithm::Player::Available;
    else
        if ((i + 1 == 8) && (StateField[0][j] == Algorithm::Player::Unknown))
            StateField[0][j] = Algorithm::Player::Available;

    if (i + 2 <= 7)
    {
        if (((StateField[i + 1][j] == Algorithm::Player::White) ||
            (StateField[i + 1][j] == Algorithm::Player::Black)) &&
            (StateField[i + 2][j] == Algorithm::Player::Unknown))
            StateField[i + 2][j] = Algorithm::Player::Available;
    }
    else
    {
        if (i + 2 == 8)
        {
            if (((StateField[7][j] == Algorithm::Player::White) ||
                (StateField[7][j] == Algorithm::Player::Black)) &&
                (StateField[0][j] == Algorithm::Player::Unknown))
                StateField[0][j] = Algorithm::Player::Available;
        }
        else
            if (i + 2 == 9)
                if (((StateField[0][j] == Algorithm::Player::White) ||
                    (StateField[0][j] == Algorithm::Player::Black)) &&
                    (StateField[1][j] == Algorithm::Player::Unknown))
                    StateField[1][j] = Algorithm::Player::Available;
    }

    if ((j - 1 >= 0) && (StateField[i][j - 1] == Algorithm::Player::Unknown))
        StateField[i][j - 1] = Algorithm::Player::Available;

    if (j - 2 >= 0)
    {
        if (((StateField[i][j - 1] == Algorithm::Player::White) ||
            (StateField[i][j - 1] == Algorithm::Player::Black)) &&
            (StateField[i][j - 2] == Algorithm::Player::Unknown))
            StateField[i][j - 2] = Algorithm::Player::Available;
    }

    if ((j + 1 <= 2) && (StateField[i][j + 1] == Algorithm::Player::Unknown))
        StateField[i][j + 1] = Algorithm::Player::Available;

    if (j + 2 <= 2)
    {
        if (((StateField[i][j + 1] == Algorithm::Player::White) ||
            (StateField[i][j + 1] == Algorithm::Player::Black)) &&
            (StateField[i][j + 2] == Algorithm::Player::Unknown))
            StateField[i][j + 2] = Algorithm::Player::Available;
    }

    if (i - 1 >= 0)
    {
        if ((StateField[i - 1][j] == Algorithm::Player::White) ||
            (StateField[i - 1][j] == Algorithm::Player::Black))
        {
            if ((j - 1 >= 0) && (StateField[i - 1][j - 1] == Algorithm::Player::Unknown))
                StateField[i - 1][j - 1] = Algorithm::Player::Available;
            if ((j + 1 <= 2) && (StateField[i - 1][j + 1] == Algorithm::Player::Unknown))
                StateField[i - 1][j + 1] = Algorithm::Player::Available;
        }
    }
    else
        if (i - 1 == -1)
        {
            if ((StateField[7][j] == Algorithm::Player::White) ||
                (StateField[7][j] == Algorithm::Player::Black))
            {
                if ((j - 1 >= 0) && (StateField[7][j - 1] == Algorithm::Player::Unknown))
                    StateField[7][j - 1] = Algorithm::Player::Available;
                if ((j + 1 <= 2) && (StateField[7][j + 1] == Algorithm::Player::Unknown))
                    StateField[7][j + 1] = Algorithm::Player::Available;
            }
        }

    if (i + 1 <= 7)
    {
        if ((StateField[i + 1][j] == Algorithm::Player::White) ||
            (StateField[i + 1][j] == Algorithm::Player::Black))
        {
            if ((j - 1 >= 0) && (StateField[i + 1][j - 1] == Algorithm::Player::Unknown))
                StateField[i + 1][j - 1] = Algorithm::Player::Available;
            if ((j + 1 <= 2) && (StateField[i + 1][j + 1] == Algorithm::Player::Unknown))
                StateField[i + 1][j + 1] = Algorithm::Player::Available;
        }
    }
    else
        if (i + 1 == 8)
        {
            if ((StateField[0][j] == Algorithm::Player::White) ||
                (StateField[0][j] == Algorithm::Player::Black))
            {
                if ((j - 1 >= 0) && (StateField[0][j - 1] == Algorithm::Player::Unknown))
                    StateField[0][j - 1] = Algorithm::Player::Available;
                if ((j + 1 <= 2) && (StateField[0][j + 1] == Algorithm::Player::Unknown))
                    StateField[0][j + 1] = Algorithm::Player::Available;
            }
        }

    if (j - 1 >= 0)
    {
        if ((StateField[i][j - 1] == Algorithm::Player::White) ||
            (StateField[i][j - 1] == Algorithm::Player::Black))
        {
            if ((i - 1 >= 0) && (StateField[i - 1][j - 1] == Algorithm::Player::Unknown))
                StateField[i - 1][j - 1] = Algorithm::Player::Available;
            if ((i - 1 == -1) && (StateField[7][j - 1] == Algorithm::Player::Unknown))
                StateField[7][j - 1] = Algorithm::Player::Available;
            if ((i + 1 <= 7) && (StateField[i + 1][j - 1] == Algorithm::Player::Unknown))
                StateField[i + 1][j - 1] = Algorithm::Player::Available;
            if ((i + 1 == 8) && (StateField[0][j - 1] == Algorithm::Player::Unknown))
                StateField[0][j - 1] = Algorithm::Player::Available;
        }
    }

    if (j + 1 <= 2)
    {
        if ((StateField[i][j + 1] == Algorithm::Player::White) ||
            (StateField[i][j + 1] == Algorithm::Player::Black))
        {
            if ((i - 1 >= 0) && (StateField[i - 1][j + 1] == Algorithm::Player::Unknown))
                StateField[i - 1][j + 1] = Algorithm::Player::Available;
            if ((i - 1 == -1) && (StateField[7][j + 1] == Algorithm::Player::Unknown))
                StateField[7][j + 1] = Algorithm::Player::Available;
            if ((i + 1 <= 7) && (StateField[i + 1][j + 1] == Algorithm::Player::Unknown))
                StateField[i + 1][j + 1] = Algorithm::Player::Available;
            if ((i + 1 == 8) && (StateField[0][j + 1] == Algorithm::Player::Unknown))
                StateField[0][j + 1] = Algorithm::Player::Available;
        }
    }
}

void Algorithm::casheStateField()
{
    /* Сохранение предыдущего хода */

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            OldStateField[i][j] = StateField[i][j];
}

void Algorithm::clearOldStateField()
{
    /* Очистка истории предыдущих ходов */

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            OldStateField[i][j] = Algorithm::Player::Unknown;
}
