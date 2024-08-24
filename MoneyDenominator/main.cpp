#include <windows.h>
#include<tchar.h>

// Global variables
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// The name of the window class
static TCHAR szWindowClass[] = _T("win32app");

// The title bar text
static TCHAR szTitle[] = _T("Currency Denomination Counter");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hWnd;
    MSG msg;

    // Initialize global strings
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Win32 Guided Tour"), NULL);
        return 1;
    }

    hInst = hInstance; // Store instance handle in our global variable

    // Create the window
    hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 300,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
    {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Win32 Guided Tour"), NULL);
        return 1;
    }

    // Show the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditAmount, hButtonCalc, hStaticResult;
    static int amount;
    switch (message)
    {
    case WM_CREATE:
        // Create controls
        hEditAmount = CreateWindow(_T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            20, 20, 200, 25,
            hWnd, (HMENU)1, hInst, NULL);

        hButtonCalc = CreateWindow(_T("BUTTON"), _T("Calculate"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            240, 20, 100, 25,
            hWnd, (HMENU)2, hInst, NULL);

        hStaticResult = CreateWindow(_T("STATIC"), _T("Result:"),
            WS_CHILD | WS_VISIBLE,
            20, 60, 320, 100,
            hWnd, (HMENU)3, hInst, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 2)
        {
            // Get amount from edit control
            TCHAR buffer[10];
            GetWindowText(hEditAmount, buffer, 10);
            amount = _ttoi(buffer);

            // Calculate denominations
            int Rs500 = amount / 500;
            amount %= 500;
            int Rs100 = amount / 100;
            amount %= 100;
            int Rs50 = amount / 50;
            amount %= 50;
            int Rs20 = amount / 20;
            amount %= 20;
            int Rs10 = amount / 10;
            amount %= 10;
            int Rs1 = amount;

            // Display result
            TCHAR result[256];
            _stprintf_s(result, _T("500s: %d\n100s: %d\n50s: %d\n20s: %d\n10s: %d\n1s: %d"), Rs500, Rs100, Rs50, Rs20, Rs10, Rs1);
            SetWindowText(hStaticResult, result);
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // Here you can set colors, draw UI, etc.
        SetTextColor(hdc, RGB(0, 0, 255)); // Set text color to blue
        SetBkMode(hdc, TRANSPARENT); // Transparent background
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
