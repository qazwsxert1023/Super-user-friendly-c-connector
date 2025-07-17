#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1 : 0)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void simulateClick(bool isLeftClick) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    if (isLeftClick) {
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
    } else {
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
    }
    SendInput(1, &input, sizeof(INPUT));
}


HWND hModeInput, hFreqInput, hKeyInput, hStartButton;
int mode, f, key;
char k;
int vk_key;
bool isRunning = false; 

void startAutoClicker() {
    if (isRunning) return;

    
    char modeText[10], freqText[10], keyText[10];
    GetWindowText(hModeInput, modeText, 10);
    GetWindowText(hFreqInput, freqText, 10);
    GetWindowText(hKeyInput, keyText, 10);

    mode = atoi(modeText);
    f = atoi(freqText);
    k = keyText[0];
    if ('a' <= k && k <= 'z') k -= 32; 
    vk_key = VkKeyScan(k);

   
    if (mode < 1 || mode > 2 || f <= 0 || (k < 'A' || k > 'Z')) {
        MessageBox(NULL, "请输入正确的参数！", "错误", MB_ICONERROR);
        return;
    }

    isRunning = true;

    
    while (isRunning) {
        if (KEY_DOWN(VK_ESCAPE)) {
            isRunning = false;
            MessageBox(NULL, "已退出连点器。", "提示", MB_OK);
            break;
        }

        if (KEY_DOWN(vk_key)) {
            Sleep(200); 
            while (!KEY_DOWN(vk_key)) {
                if (key == 1) simulateClick(true);
                if (key == 2) simulateClick(false);

                double x = 1000.0 / f;
                if (mode == 2) x += rand() % 50;
                Sleep(static_cast<DWORD>(x));
            }
            Sleep(200);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "AutoClickerWindow";

    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); 

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "窗口注册失败！", "错误", MB_ICONERROR);
        return 0;
    }

    
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "连点器 - 自动点击",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "窗口创建失败！", "错误", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            
            CreateWindow("STATIC", "模式 (1=普通, 2=防检测):", WS_VISIBLE | WS_CHILD, 20, 20, 200, 20, hwnd, NULL, NULL, NULL);
            hModeInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 220, 20, 100, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "频率 (次/秒):", WS_VISIBLE | WS_CHILD, 20, 60, 200, 20, hwnd, NULL, NULL, NULL);
            hFreqInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 220, 60, 100, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "触发键 (如 A):", WS_VISIBLE | WS_CHILD, 20, 100, 200, 20, hwnd, NULL, NULL, NULL);
            hKeyInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 220, 100, 100, 20, hwnd, NULL, NULL, NULL);

            hStartButton = CreateWindow("BUTTON", "启动连点器", WS_VISIBLE | WS_CHILD, 20, 140, 150, 30, hwnd, (HMENU)1, NULL, NULL);
        }
        return 0;

        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) { 
                startAutoClicker();
            }
        }
        return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
