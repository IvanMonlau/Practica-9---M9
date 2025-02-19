#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib> // Para generar números aleatorios

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct Jugador {
    SOCKET socket;
    string nombre;
};

vector<Jugador> jugadores;

// Función para determinar el ganador
string determinarGanador(char jugador1, char jugador2) {
    if (jugador1 == jugador2) {
        return "Empate";
    } else if (
        (jugador1 == 'P' && (jugador2 == 'T' || jugador2 == 'L')) || // Piedra gana a Tijera y Lagarto
        (jugador1 == 'A' && (jugador2 == 'P' || jugador2 == 'S')) || // Papel gana a Piedra y Spock
        (jugador1 == 'T' && (jugador2 == 'A' || jugador2 == 'L')) || // Tijera gana a Papel y Lagarto
        (jugador1 == 'L' && (jugador2 == 'S' || jugador2 == 'A')) || // Lagarto gana a Spock y Papel
        (jugador1 == 'S' && (jugador2 == 'T' || jugador2 == 'P'))    // Spock gana a Tijera y Piedra
    ) {
        return jugadores[0].nombre;
    } else {
        return jugadores[1].nombre;
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error al inicializar Winsock" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error al crear socket" << endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Error al enlazar socket" << endl;
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Error al escuchar en socket" << endl;
        return 1;
    }

    cout << "Servidor iniciado. Esperando conexiones..." << endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Error al aceptar conexión" << endl;
            continue;
        }

        char buffer[1024];
        recv(clientSocket, buffer, sizeof(buffer), 0);
        string nombreJugador(buffer);

        jugadores.push_back({clientSocket, nombreJugador});

        cout << "Jugador conectado: " << nombreJugador << endl;

        if (jugadores.size() == 2) {
            while (true) { 
                char jugador1 = ' ';
                char jugador2 = ' ';
        
                // Intercambiar jugadas con los clientes
                send(jugadores[0].socket, "Tu turno. Elige (P)iedra, (A)papel, (T)ijera, (L)agarto o (S)pock: ", 70, 0);
                int bytesReceived = recv(jugadores[0].socket, buffer, sizeof(buffer) - 1, 0);
                buffer[bytesReceived] = '\0'; // Null-terminate the buffer
                jugador1 = toupper(buffer[0]); // Convertir a mayúscula para facilitar la comparación
        
                send(jugadores[1].socket, "Tu turno. Elige (P)iedra, (A)papel, (T)ijera, (L)agarto o (S)pock: ", 70, 0);
                bytesReceived = recv(jugadores[1].socket, buffer, sizeof(buffer) - 1, 0);
                buffer[bytesReceived] = '\0'; 
                jugador2 = toupper(buffer[0]); 
        
                // Validar entrada del jugador (opcional, pero recomendado)
                vector<char> validMoves = {'P', 'A', 'T', 'L', 'S'};
                if (find(validMoves.begin(), validMoves.end(), jugador1) == validMoves.end() ||
                    find(validMoves.begin(), validMoves.end(), jugador2) == validMoves.end()) {
                    string mensajeError = "Jugada inválida. Por favor, elige una opción válida.";
                    send(jugadores[0].socket, mensajeError.c_str(), mensajeError.length(), 0);
                    send(jugadores[1].socket, mensajeError.c_str(), mensajeError.length(), 0);
                    continue; // Volver a pedir las jugadas
                }
        
                // Determinar ganador
                string ganador = determinarGanador(jugador1, jugador2);

        
                // Enviar resultados a los clientes
                string resultadoJugador1 = "Tu jugada: " + string(1, jugador1) + "\nJugada del oponente: " + string(1, jugador2) + "\nResultado: " + ganador;
                send(jugadores[0].socket, resultadoJugador1.c_str(), resultadoJugador1.length()+14, 0);
        
                string resultadoJugador2 = "Tu jugada: " + string(1, jugador2) + "\nJugada del oponente: " + string(1, jugador1) + "\nResultado: " + ganador;
                send(jugadores[1].socket, resultadoJugador2.c_str(), resultadoJugador2.length()+14, 0);
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}