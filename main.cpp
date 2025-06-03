#define UNICODE
#include <windows.h>
#include <cstdint>

const int WIDTH = 800;
const int HEIGHT = 600;
uint32_t framebuffer[WIDTH * HEIGHT];


// Forward declaration of the window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


void RenderTestImage() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            uint8_t r = (x * 256) / WIDTH;
            uint8_t g = (y * 256) / HEIGHT;
            uint8_t b = (r+g) / 2; // Simple gradient effect

            framebuffer[y * WIDTH + x] = (255 << 24) | (r << 16) | (g << 8) | b;  // ARGB
        }
    }
}


// Entry point of a Windows application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"SimpleRendererWindow";

    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        L"My CPU Renderer",              // Window text
        WS_OVERLAPPEDWINDOW,           // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    RenderTestImage();  // Fill the framebuffer with test data

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window procedure: handles messages sent to the window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = WIDTH;
            bmi.bmiHeader.biHeight = -HEIGHT; // Negative height for top-down bitmap
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            StretchDIBits(
                hdc,
                0, 0, WIDTH, HEIGHT,
                0, 0, WIDTH, HEIGHT,
                framebuffer,
                &bmi,
                DIB_RGB_COLORS,
                SRCCOPY
            );

            EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



