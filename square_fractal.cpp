#include <iostream>
#include <math.h>
#include <cstring>
// Сделать фрактальную квадратную матрицу из символов, вводится с клавиатуры размер и "сложность фрактала" - параметр,
// описанный буквой n на рисунке примера Снежинки Коха (I11-J11).
// Нужно проверить, реально ли сделать такой фрактал или требуется вывод сообщения об ошибке. 

using namespace std;

enum class dir {none, right, left, top, bottom};

struct square {
    long x1, y1, x2, y2;
    dir obsolete;
    square() {
        x1 = 0;
        y1 = 0;
        x2 = 0;
        y2 = 0;
        obsolete = dir::none;
    }
    square(long _x1, long _y1, long _x2, long _y2, dir _obsolete) {
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        obsolete = _obsolete;
    }
};

const square null_square = square(-1, -1, -1, -1, dir::none);

square *derive(square parent) {
    square *children = new square[4];
    long ln = (parent.x2 - parent.x1) / 3;
    if(parent.obsolete != dir::left)
        children[0] = square(parent.x1 + ln, parent.y1 - ln, parent.x2 - ln, parent.y1, dir::right);
    else
        children[0] = null_square;
    if(parent.obsolete != dir::top)
        children[1] = square(parent.x1 - ln, parent.y1 + ln, parent.x1, parent.y2 - ln, dir::bottom);
    else
        children[1] = null_square;
    if(parent.obsolete != dir::right)
        children[2] = square(parent.x1 + ln, parent.y2, parent.x2 - ln, parent.y2 + ln, dir::left);
    else
        children[2] = null_square;
    if(parent.obsolete != dir::bottom)
        children[3] = square(parent.x2, parent.y1 + ln, parent.x2 + ln, parent.y2 - ln, dir::top);
    else
        children[3] = null_square;
    return children;
}

bool valid(square sq) {
    return sq.x1 != -1;
}

void fill(char **matrix, square sq) {
    for(long x = sq.x1; x < sq.x2; x++)
        for(long y = sq.y1; y < sq.y2; y++)
            matrix[x][y] = '#';
}

void fractal(char **matrix, long size, square *blocks, long num, long it) {
    if(it == 0) {
        delete[] blocks;
        return;
    }
    square *new_blocks = new square[num * 4];
    long cur_size = 0;
    for(long i = 0; i < num; i++) {
        square *cur_branches = derive(blocks[i]);
        for(long j = 0; j < 4; j++) {
            if(valid(cur_branches[j])) {
                new_blocks[cur_size++] = cur_branches[j];
                fill(matrix, cur_branches[j]);
            }
        }
    }
    delete[] blocks;
    square *shrink = new square[cur_size];
    memcpy(shrink, new_blocks, cur_size * sizeof(square));
    delete[] new_blocks;
    fractal(matrix, size, shrink, cur_size, it - 1);
}

int main(void) {
    long size, complexity;
    bool gothic_mode;
    cout << "Specify the size of the matrix: ";
    cin >> size;
    cout << "Specify the fractal complexity: ";
    cin >> complexity;
    cout << "Gothic mode on/off (only works in a terminal with color enabled): [1/0]: ";
    cin >> gothic_mode;
    long init_block = size / 2;
    if(init_block % 2 != size % 2)
        cout << "Impossible to place initial block in the center, off-center placement would not fit, increasing size to "
         << ++size << "." << endl;
    if(size <= 0 || complexity < 0) {
        cout << "Impossible: wrong input." << endl;
        return 0;
    }
    if(init_block / long(pow(3, complexity)) == 0) {
        cout << "Impossible: unable to draw last iteration due to small resolution." << endl;
        return 0;
    }
    if(init_block % long(pow(3, complexity))) {
        cout << "Impossible: unable to place last iteration blocks correctly.";
        return 0;
    }
    char **matrix = new char*[size];
    for(long x = 0; x < size; x++) {
        matrix[x] = new char[size];
        for(long y = 0; y < size; y++)
            matrix[x][y] = '.';
    }
    square *blocks = new square[1];
    blocks[0] = square(size / 2 - init_block / 2,
        size / 2 - init_block / 2,
        size / 2 + ceil(float(init_block) / 2.), 
        size / 2 + ceil(float(init_block) / 2.), 
        dir::none);
    fill(matrix, blocks[0]);
    fractal(matrix, size, blocks, 1, complexity);
    for(long x = 0; x < size; x++) {
        for(long y = 0; y < size; y++) {
            if(gothic_mode && matrix[x][y] == '.')
                cout << "\033[30m" << matrix[x][y];
            else if(gothic_mode && (matrix[x - 1][y] == '.' 
            || matrix[x + 1][y] == '.' || matrix[x][y - 1] == '.'
            || matrix[x][y + 1] == '.'))
                cout << "\033[31m" << matrix[x][y] << "\033[37m";
            else
                cout << matrix[x][y];
        }
        cout << endl;
    }
    for(long x = 0; x < size; x++)
        delete[] matrix[x];
    delete[] matrix;
    return 0;
}