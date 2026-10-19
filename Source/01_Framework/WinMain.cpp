// 출처: https://powerclabman.tistory.com/79  [WinAPI] 게임을 위한 메인 루프 만들기
#include "framework.h"
#include "Main.h"

/* 전역 변수 */
HINSTANCE	hInst;		// 핸들 인스턴스
HWND		ghwnd;		// 핸들 윈도우

/* 윈도우 클래스 및 타이틀 */
LPCWSTR		gwinClassName = L"MyAPIGame";
LPCWSTR		gwinTitleName = L"MyAPIGame";

/* 전방 선언 */
ATOM                MyRegisterClass(HINSTANCE hInstance);	// 클래스 등록함수
BOOL                InitInstance(HINSTANCE, int);			// 인스턴스 초기화
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);	// 윈도우 프로시저

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	/* 윈도우 정보를 등록 */
	MyRegisterClass(hInstance);

	/* 핸들 윈도우를 생성 */
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	MSG msg = {};

	// 기본 메시지 루프입니다:
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			/* 메인 루프 */
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWAPISTUDY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = gwinClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	/* 윈도우 사이즈 조절 */
	RECT windowRect = { 0,0,800,600 };

	::AdjustWindowRect(&windowRect, WS_OVERLAPPED, false);

	HWND hWnd = CreateWindowW(gwinClassName, gwinTitleName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	ghwnd = hWnd; /* hwnd는 쓸데가 많으니 전역으로 저장해놓고 사용 */

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
