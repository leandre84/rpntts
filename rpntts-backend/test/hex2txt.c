#include <stdio.h>
#include <string.h>

int hex_to_int(char c) {
    int first = c / 16 - 3;
    int second = c % 16;
    int result = first*10 + second;

    if (result > 9) {
        result--;
    }
    return result;
}

int hex_to_ascii(char c, char d) {
    int high = hex_to_int(c) * 16;
    int low = hex_to_int(d);

    return high+low;
}

int main(int argc, char **argv) {
    int length = 0;
    char buf = 0;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Need exactly one argument!\n");
        return 1;
    }

    length = strlen(argv[1]);

    for(i = 0; i < length; i++){
        if(i % 2 != 0){
            printf("%c", hex_to_ascii(buf, argv[1][i]));
        }
        else {
            buf = argv[1][i];
        }
    }

    return 0;
}
