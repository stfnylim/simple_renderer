#define UNICODE
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <iostream>  // Add this line!
#include <io.h>      // Add this for freopen_s
#include <fcntl.h> 

const int CWIDTH = 1920;
const int CHEIGHT = 1080;
uint32_t* framebuffer = nullptr;

void render_sphere(); // Forward declaration of the rendering function


// Forward declaration of the window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


// void RenderTestImage() {
//     for (int y = 0; y < CHEIGHT; ++y) {
//         for (int x = 0; x < CWIDTH; ++x) {
//             uint8_t r = (x * 256) / CWIDTH;
//             uint8_t g = (y * 256) / CHEIGHT;
//             uint8_t b = (r+g) / 2; // Simple gradient effect
            
//             int pos_x = x + CWIDTH / 2; // Center the image
//             int pos_y = y + CHEIGHT / 2; // Center the image
//             framebuffer[pos_y * CWIDTH + pos_x] = (255 << 24) | (r << 16) | (g << 8) | b;  // ARGB
//         }
//     }
// }



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {

    // Allocate a console for this GUI application
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    FILE* pCin;
    freopen_s(&pCin, "CONIN$", "r", stdin);
    FILE* pCerr;
    freopen_s(&pCerr, "CONOUT$", "w", stderr);
    std::ios::sync_with_stdio(true);
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();

    std::cout << "Console allocated successfully!\n";
    const wchar_t CLASS_NAME[] = L"SimpleRendererWindow";
    
    // Allocate framebuffer dynamically
    framebuffer = new uint32_t[CWIDTH * CHEIGHT];
    
    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
    
    // Create the window
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"My CPU Renderer", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CWIDTH, CHEIGHT,
        NULL, NULL, hInstance, NULL);
    
    if (hwnd == NULL) {
        delete[] framebuffer;
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    render_sphere();  // Render after window is shown
    
    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    delete[] framebuffer;  // Clean up
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = CWIDTH;
            bmi.bmiHeader.biHeight = -CHEIGHT;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            
            StretchDIBits(hdc, 0, 0, CWIDTH, CHEIGHT, 0, 0, CWIDTH, CHEIGHT,
                framebuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
            
            EndPaint(hwnd, &ps);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}