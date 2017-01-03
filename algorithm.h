#ifndef ALGORITHM_H
#define ALGORITHM_H

struct DecisionTree;

class Algorithm
{
public: 
    enum Player
    {
        White, Black, Unknown, Available
    };
    enum Level
    {
        Easy, Middle, Hard, Opponent
    };

    Player **StateField;
    Player **OldStateField;
    Level LvlPC;
    bool **EnemyField;
    int *CountSteps;
    DecisionTree *Head = nullptr;

    Algorithm();
    ~Algorithm();
    void createDTree(Algorithm::Player Player, DecisionTree *&TemporaryPointer, int ReserveField[2][2], int X1, int Y1, int X2, int Y2, int Level);
    void createExtraDTree(Algorithm::Player Player, int ReserveField[2][2]);
    void showValue(DecisionTree *&tmp, int Level);
    void getValue(DecisionTree *&tmp, int Level);
    bool step1(DecisionTree *&tmp, Algorithm::Player Player, int Level);
    bool step2(DecisionTree *&tmp, Algorithm::Player Player, int Level);
    bool step3(DecisionTree *&tmp, Algorithm::Player Player);
    void deleteDTree(DecisionTree *&tmp);
    int threeInRow(Algorithm::Player Player, Algorithm::Player **StateField, bool **EnemyField);
    void makeAvailableField(int i, int j, Algorithm::Player **StateField);
    void casheStateField();
    void clearOldStateField();
};

struct DecisionTree
{
    Algorithm::Player Player;
    int PosOnScene[2][2];
    int Value = 0;
    int CountChilds;
    DecisionTree **Child;
};

#endif // ALGORITHM_H
