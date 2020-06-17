#include <string>
#include <unistd.h>

using namespace std;

int my_menu(){
    while (1) {
        char line[4096];
        if (isatty(0))
            putchar('>');
        char *message = fgets(line, 4095, stdin);
        if (message == NULL)
                   return 0;
        if (strcmp(line, "\n") != 0){
            size_t i = 0;
            for (; line[i] != '\n'; i++);
            line[i] = '\0';
            char *approx = strtok(line, " \t");
            char *distance = strtok(NULL, " \t");
            char *word = strtok(NULL, " \t");
            printf("%s", approx);
            printf("%s", " ");
            printf("%s", distance);
            printf("%s", " ");
            printf("%s", word);
            printf("%s", "\n");
        }
    }
}

int main() {
    my_menu();
}
