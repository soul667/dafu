#include <iostream>
#include <vector>

using namespace std;

void printArrow(int Step, int station, int Color) {
    vector<vector<char>> arrow(7, vector<char>(7, ' ')); // 创建一个 7x7 的二维数组表示箭头

    for (int w = 0; w < 7; w++) {
        for (int i = 1; i < 6; i++) { // 0-》1
            int location1, location2, new1;

            if (i < 3) {
                for (int j = 0; j <= i-1; j++) { // i to i-1
                    location1 = j + i * 7 + Step * 7 + station * 7;
                    location1 = (location1 > 237) ? location1 - 238 : location1;
                    location2 = location1 + (6 - 2 * j);

                    arrow[i][j] = '*'; // 将箭头左右两侧标记为 '*'
                    arrow[i][6 - j] = '*';
                }
            } else {
                int number = (i - 2) * 2;
                for (int k = 0; k < (7 - number); k++) {
                    new1 = i * 7 + k + (number / 2) + Step * 7 + station * 7;
                    new1 = (new1 > 237) ? new1 - 238 : new1;
                    if(i==3&&k==2) continue;
                    arrow[i][k + number / 2] = '*'; // 将箭头中间部分标记为 '*'
                }
            }
        }
    }

    // 打印箭头
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            cout << arrow[i][j];
        }
        cout << endl;
    }
}

int main() {
    int Step = 0;      // 箭头起始位置（可根据需要调整）
    int station = 0;   // 箭头类型或方向（可根据需要调整）
    int Color = 1;     // 颜色（这里用 1 表示，实际中可根据 LED 颜色设置）

    printArrow(Step, station, Color);

    return 0;
}
