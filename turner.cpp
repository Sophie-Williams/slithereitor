#include <cstdio>
#include <cstring>

#include <cstdlib>
#include <ctime>

using namespace std;

int main() {

    int fim;

    srand(time(NULL));
    // Recebe do jogo uma flag indicando se o jogo acabou
    while (scanf("%d",&fim) and !fim) {
        // TODO: Receber outras coisas (TODO fazer o jogo envia-las tb)

        // Imprime o angulo o qual andar em graus. 0 = frente
        int ang = 30;   // gira 30 graus
        printf("%d\n", ang);

        // garantir
        fflush(stdout);
    }

    return 0;
}
