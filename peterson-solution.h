#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int turn;				// Qual é o processo da vez
bool *interested;			// Quais são os interessados à acessar a região crítica

void enter_region(int pid, int N) {	// Coordena a entrada de N processos à região crítica
    for (int i = 0; i < N; i++) {	// Para cada N processos, faça:
        if (i == pid)			// Se eu sou o processo da vez, não devo esperar
            continue;
        interested[pid] = true;		// A
        turn = pid;
        while (turn == pid && interested[i] == true) sleep(1);
    }
}

void leave_region(int pid) {
	interested[pid] = false;
}
