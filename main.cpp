#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <conio.h>
#include <windows.h>

//get cursor to x,y
void gotoxy(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

using namespace std;

//creating a structure
struct Tetromino {
    int size;
    int shape[2][2];
    int x, y;
};

//fulfiling the buffer
void fillBuffer(char** buffer, int** field, int height, int width, Tetromino t) {
    for (int i = 0; i<height; i++) {
        for (int j = 0; j<width; j++) {
            buffer[i][j] = (field[i][j] ? '#' : ' ');
        }
    }

    //adding Tetromino to buffer
    for (int i = 0; i<t.size; i++) {
        for (int j = 0; j<t.size; j++) {
            if (t.shape[i][j]) {
                buffer[t.y + i][t.x + j] = '#';
            }
        }
    }
}

//adding the buffer to screen
void drawBuffer(char** buffer, int height, int width) {
    for (int i = 0; i<height; i++) {
        for (int j = 0; j<width; j++) {
            cout<<buffer[i][j];
        }
        cout<<'\n';
    }
}


//fulfilling the field
void initField(int** field, int height, int width) {
    for (int i = 0; i<height; i++) {
        for (int j = 0; j<width; j++) {
            if (j == 0||j == width - 1||i == height - 1) {
                field[i][j] = 1;
            }
            else {
                field[i][j] = 0;
            }
        }
    }
}
//data output
void drawField(int** field, int height, int width) {
    for (int i = 0; i<height; i++) {
        for (int j = 0; j<width; j++) {
            cout<<(field[i][j] ? "#" : " ");
        }
        cout<<"\n";
    }
}

void drawFieldwithTetromino(int** field, int height, int width, Tetromino t) {
    for (int i = 0; i<height; i++) {
        for (int j = 0; j<width; j++) {
            if (i>=t.y && i<t.y + t.size&&
                j>=t.x && j<t.x + t.size&&
                t.shape[i-t.y][j-t.x] == 1) {
                cout<<"#";
            }
            else {
                cout<<(field[i][j] ? "#" : " ");
            }
        }
        cout<<"\n";
    }
}

bool canMoveDown(int** field, Tetromino t, int height, int width) {
    for (int i = 0; i<t.size; i++) {
        for (int j = 0; j<t.size; j++) {
            if (t.shape[i][j] == 1) {
                int newY = t.y + i + 1;
                int newX = t.x + j;
                if (newY >= height || field[newY][newX]) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool canMoveLeft(int** field, Tetromino t) {
    for (int i = 0; i<t.size; i++) {
        for (int j = 0; j<t.size; j++) {
            if (t.shape[i][j]) {
                if (field[t.y+i][t.x+j-1]) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool canMoveRight(int** field, Tetromino t) {
    for (int i = 0; i<t.size; i++) {
        for (int j = 0; j<t.size; j++) {
            if (t.shape[i][j]) {
                if (field[t.y+i][t.x+j+1]) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool canSpawn(int** field, Tetromino t) {
    for (int i = 0; i<t.size; i++) {
        for (int j = 0; j<t.size; j++) {
            if (t.shape[i][j]) {
                if (field[t.y+i][t.x+j]) {
                    return false;
                }
            }
        }
    }
    return true;
}

void fixTetromino(int** field, Tetromino t) {
    for (int i = 0; i<t.size; i++) {
        for (int j = 0; j<t.size; j++) {
            if (t.shape[i][j] == 1) {
                field[t.y+i][t.x+j] = 1;
            }
        }
    }
}

Tetromino newTetromino(int width) {
    Tetromino t;  //creating an object
    t.size = 2;
    t.shape[0][0] = t.shape[0][1] = 1;
    t.shape[1][0] = t.shape[1][1] = 1;
    t.x = 1 + rand() % (width - t.size - 2); //random horizontal position
    t.y = 0;
    return t;
}

void clearFullLines(int** field, int height, int width) {
    for (int i = height -2; i>0; i--) {
        bool full = true;
        for (int j = 1; j<width-1; j++) {
            if (field[i][j] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int k = i; k>1; k--) {
                for (int j = 1; j<width-1; j++) {
                    field[k][j] = field[k-1][j];
                }
            }
            for (int j = 1; j<width-1; j++) field[1][j] = 0;

            i++;
        }
    }
}

int main() {
    srand(time(0));

    int height = 10;
    int width = 18;

    //creating the field and the buffer
    int** field = new int*[height];
    char** buffer = new char*[height];

    for (int i = 0; i < height; i++) {
        field[i] = new int[width];
        buffer[i] = new char[width];
    }

    initField(field, height, width);

    while (true) {
        Tetromino t = newTetromino(width);

        if (!canSpawn(field, t)) {
            gotoxy(4, height/2 -1 );
            cout<<"GAME OVER!"<<endl;
            _getch();
            break;
        }

        auto lastFall = chrono::steady_clock::now();

        while (true){
            gotoxy(0, 0);
            //drawFieldwithTetromino(field, height, width, t); //для отладки

            fillBuffer(buffer, field, height, width, t);
            drawBuffer(buffer, height, width);


            //keyboard input
            if (_kbhit()) {
                char key(_getch());

                if ((key=='a' || key==char(228) || key==char(148)) && canMoveLeft(field, t)) {
                    t.x--;
                }
                if ((key=='d' || key==char(162) || key==char(130) ) && canMoveRight(field, t)) {
                    t.x++;
                }
                if ((key=='s' || key==char(235) || key==char(155)) && canMoveDown(field, t, height, width)) {
                    t.y++;
                    lastFall = chrono::steady_clock::now();//for faster movement
                }
                if (key==' ') {
                    while (canMoveDown(field, t, height, width)) {
                        t.y++;
                    }
                }

            }
            //auto fall
            auto now = chrono::steady_clock::now();
            if (chrono::duration_cast<chrono::milliseconds>(now-lastFall).count()>300) {
                if (canMoveDown(field, t, height, width)) {
                    t.y++;

                }else {
                    break;
                }
                lastFall = now;
            }
            this_thread::sleep_for(chrono::milliseconds(30));

        }
        fixTetromino(field, t);
        clearFullLines(field, height, width);
    }


    drawField(field, height, width); //для отладки


    //clearing the field and the buffer
    for (int i = 0; i < height; i++) {
        delete[] field[i];
        delete[] buffer[i];
    }
    delete[] field;
    delete[] buffer;


    return 0;
}
//TODO добавить рестарт на R
//TODO добавить управление на стрелочки
//TODO (Позже) Добавить счётчик очков
//TODO (Позже) Вынести таймер падения отдельно от input(?)
//TODO (Опционально) Подготовить логику под графику
//TODO вывести программу в графику