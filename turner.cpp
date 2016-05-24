#include <cstdio>
#include <cstring>

#include <cstdlib>
#include <ctime>

using namespace std;

int main() {

    int end;

    srand(time(NULL));

    // Read a flag indicating wheter the game has finished
    while (scanf("%d",&end) and !end) {
        // TODO: Read other things (TODO make the game send them)

        // Print the angle the snake will turn (0 = go forward)
        int ang = 30;   // turns 30 degrees
        printf("%d\n", ang);
        fflush(stdout);
    }

    return 0;
}
