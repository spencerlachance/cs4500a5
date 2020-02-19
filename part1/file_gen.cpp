#include <stdio.h>

int main(int argc, char** argv) {
    FILE* file = fopen("datafile.txt", "w");
    for (int i = 0; i < 866666; i++) {
        fputs("<+1><0><1.0><10><1><7.7><88><0>\n", file);
        fputs("<2><0><6767.239302><10990><0><2344.9><7232><1>\n", file);
        fputs("<3><1><11.11><666><0><234.0000001><7890><1>\n", file);
    }
    return 0;
}