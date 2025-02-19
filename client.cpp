#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error al inicializar Winsock" << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Error al crear socket" << endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección IP del servidor
    serverAddress.sin_port = htons(12345);

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Error al conectar con el servidor" << endl;
        return 1;
    }

    string nombreJugador;
    cout << "Introduce tu nombre: ";
    getline(cin, nombreJugador);

    send(clientSocket, nombreJugador.c_str(), nombreJugador.length(), 0);

    char buffer[1024];
    while (true) {
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << buffer << endl;

        if (strstr(buffer, "Tu turno")) {
            char jugada;
            cin >> jugada;
            send(clientSocket, &jugada, sizeof(jugada), 0);
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}