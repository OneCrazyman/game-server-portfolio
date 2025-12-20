///////////////////////////////////////////////
// BinaryTree 그리기 GDI
// gpt와 perplexity를 활용해 작성하였다
///////////////////////////////////////////////

#include "framework.h"
#include "Tree-binary-tree.h"
#include <Windowsx.h>
#include <Windows.h>
#include "BinaryTree.h"
#define MAX_LOADSTRING 100

// 선분 정보 저장 구조체
struct LineSegment
{
    POINT start;
    POINT end;
};

// 노드 정보 저장 구조체
struct NodeInfo
{
    int x;
    int y;
    int key;
    int color;
};

/////////////////////
// 전역 변수
/////////////////////
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING] = L"Binary Tree";                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING] = L"BinaryTreeClass";            // 기본 창 클래스 이름입니다.
// 사용자 변수
BinaryTree<int> gTree;
//
std::vector<LineSegment> g_lines;
std::vector<NodeInfo> g_nodes;
enum ControlBoxID
{
    IDC_INSERT_TEXT = 1001,
    IDC_INSERT_BTN,
    IDC_DELETE_TEXT,
    IDC_DELETE_BTN
};

using Node = BinaryTree<int>::Node;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// ----------------
// 사용자 정의 함수 선언
// ----------------

// 컨트롤 박스 생성
void CreateControlBox(HWND hWnd);
// 전체 Tree를 계산
void CalculateTree(Node* node, int x, int y, int width);
// 노드를 그린다.
void DrawTree(HDC hdc);
void DrawNode(HDC hdc, int x, int y, int key);
// 버튼 이벤트 처리
void SwitchCommand(HWND hWnd, WPARAM wParam);
// 랜덤 밸 가져오기
void InsertRandomValue(int);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_TREEBINARYTREE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TREEBINARYTREE));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TREEBINARYTREE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TREEBINARYTREE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {
    case WM_KEYDOWN:
    {
        if (wParam == VK_SPACE)
        {
            InsertRandomValue(100);
            InvalidateRect(hWnd, nullptr, TRUE); // 다시 그리기 요청
            break;
        }
        break;
    }
    case WM_LBUTTONDOWN:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
        }
        break;
    case WM_CREATE:
        CreateControlBox(hWnd);
        break;
    case WM_COMMAND: 
    {
        SwitchCommand( hWnd,  wParam);
        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        
        RECT rc;
        GetClientRect(hWnd, &rc);
        int centerX = (rc.right - rc.left) / 2;  // 윈도우 중앙 x좌표

        TextOut(hdc, 10, 50, L"랜덤값 삽입(SPACE)", lstrlen(L"랜덤값 삽입(SPACE)"));
        auto root = gTree.GetRootNode();
        if (root)
        {
            g_lines.clear();
            g_nodes.clear();

            // 계산 단계    
            CalculateTree(root, centerX, 50,330);

            // 렌더링 단계
            DrawTree(hdc);
        }

        //
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    // 윈도우 크기가 변경될때마다 세팅된 dc를 다시 세팅한다
    case WM_SIZE:
    {
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void CreateControlBox(HWND hWnd)
{
    auto CreateControl = [&](LPCWSTR className, LPCWSTR text, DWORD style,
        int x, int y, int width, int height, HMENU id) {
        return CreateWindowW(className, text,
            WS_CHILD | WS_VISIBLE | style,
            x, y, width, height,
            hWnd, id, hInst, nullptr);
    };

    // 삽입 텍스트박스
    CreateControl(L"EDIT", nullptr, WS_BORDER | ES_LEFT,
        10, 10, 100, 25, (HMENU)ControlBoxID::IDC_INSERT_TEXT);

    // 삽입 버튼
    CreateControl(L"BUTTON", L"삽입", BS_PUSHBUTTON,
        120, 10, 50, 25, (HMENU)ControlBoxID::IDC_INSERT_BTN);

    // 삭제 텍스트박스
    CreateControl(L"EDIT", nullptr, WS_BORDER | ES_LEFT,
        200, 10, 100, 25, (HMENU)ControlBoxID::IDC_DELETE_TEXT);

    // 삭제 버튼
    CreateControl(L"BUTTON", L"삭제", BS_PUSHBUTTON,
        310, 10, 50, 25, (HMENU)ControlBoxID::IDC_DELETE_BTN);

}

// 재귀적 트리 계산 함수 (깊이 제한 버전)
void CalculateTree(Node* node, int x, int y, int width)
{
    if (node == nullptr) return;

    // 현재 노드 저장
    g_nodes.push_back({ x, y, node->Key, -1 });

    // 자식 노드 간격 계산 (깊이에 따라 절반씩 감소)
    int childY = y + 75;
    int childWidth = width / 2;

    // 왼쪽 자식 위치 계산
    if (node->Left != nullptr)
    {
        int leftX = x - width;
        CalculateTree(node->Left, leftX, childY, childWidth);
        g_lines.push_back({ {x, y}, {leftX, childY} }); // 연결선 저장
    }

    // 오른쪽 자식 위치 계산
    if (node->Right != nullptr)
    {
        int rightX = x + width;
        CalculateTree(node->Right, rightX, childY, childWidth);
        g_lines.push_back({ {x, y}, {rightX, childY} }); // 연결선 저장
    }
}

void DrawTree(HDC hdc)
{
    // 1. 모든 선분 그리기
    for (const auto& line : g_lines)
    {
        MoveToEx(hdc, line.start.x, line.start.y, nullptr);
        LineTo(hdc, line.end.x, line.end.y);
    }

    // 2. 모든 노드 그리기 (선 위에 덮어씀)
    for (const auto& node : g_nodes)
    {
        DrawNode(hdc, node.x, node.y, node.key);
    }
}

void DrawNode(HDC hdc, int x, int y, int key)
{
    const int node_radius = 20;

    // 노드 배경 색
    HBRUSH hBrush = CreateSolidBrush(RGB(175, 238, 238));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Ellipse(hdc, x - node_radius, y - node_radius, x + node_radius, y + node_radius);

    wchar_t buffer[16];
    wsprintf(buffer, L"%d", key);

    SetBkMode(hdc, TRANSPARENT);
    RECT rect = { x - node_radius, y - node_radius, x + node_radius, y + node_radius };
    DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // 리소스 정리
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}


void SwitchCommand(HWND hWnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
        case ControlBoxID::IDC_INSERT_BTN:
        {
            wchar_t buffer[128];
            GetWindowTextW(GetDlgItem(hWnd, IDC_INSERT_TEXT), buffer, 128);
            int key = _wtoi(buffer); // 문자열 -> 정수

            gTree.Insert(key, 0); // 이진트리에 삽입
            InvalidateRect(hWnd, nullptr, TRUE); // 다시 그리기 요청
            SetWindowTextW(GetDlgItem(hWnd, IDC_INSERT_TEXT), L"");
            break;
        }
        case ControlBoxID::IDC_DELETE_BTN:
        {
            wchar_t buffer[128];
            GetWindowTextW(GetDlgItem(hWnd, IDC_DELETE_TEXT), buffer, 128);
            int key = _wtoi(buffer);

            gTree.Delete(key); // 이진트리에서 삭제
            InvalidateRect(hWnd, nullptr, TRUE); // 다시 그리기 요청
            SetWindowTextW(GetDlgItem(hWnd, IDC_DELETE_TEXT), L"");
            break;
        }
        default:
            break;
    }
}

void InsertRandomValue(int range)
{
    int key =0;
    for (int i = 0;i < range;++i)
    {
        key = rand() % range;
        if (gTree.Find(key) == false)
            break;
    }
    gTree.Insert(key, 0);
}