// winmain-grid-paint.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "framework.h"
#include "Tree-RB-tree.h"
#include <Windowsx.h>
#include <Windows.h>
#include <commctrl.h> // SetWindowSubclass 필요
#pragma comment(lib, "comctl32.lib")
#include "RBtree.h"
#include <time.h>
#include <string>
#define MAX_LOADSTRING 100

enum ControlBoxID
{
    IDC_INSERT_TEXT = 1001,
    IDC_INSERT_BTN,
    IDC_DELETE_TEXT,
    IDC_DELETE_BTN,
    IDC_CHECK_SHOWNIL
};

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

using Node = RBtree<int>::Node;

/////////////////////
// 전역 변수
/////////////////////
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING] = L"RB Tree";                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING] = L"RBtreeClass";            // 기본 창 클래스 이름입니다.
HWND hWnd;

//
const int nil_check_color = 100;
const int node_radius = 20;

RBtree<int> gTree;
bool is_dulicate_node = false;
bool show_nil_nodes = false;
int gNode_pos_x;

//
std::vector<LineSegment> g_lines;
std::vector<NodeInfo> g_nodes;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

// ----------------
// 사용자 정의 함수 선언
// ----------------
// 컨트롤 박스 생성
void CreateControlBox(HWND hWnd);
// 전체 Tree를 계산
int CalculateTree(Node* node, int y);
// 노드를 그린다.
void DrawTree(HDC hdc);
void DrawNode(HDC hdc, int x, int y, int key, int color);
void DrawNilNode(HDC hdc, int x, int y);
// 버튼 이벤트 처리
void SwitchCommand(HWND hWnd, WPARAM wParam);
//
int GetSubtreeWidth(Node* node);
// 더미 데이터

void InsertDummyData()
{
    int arr[] = {50,20,60,10,30,55,80 };
    for (int i = 0; i < _countof(arr); i++)
    {
        //gTree.Insert(arr[i], 0);
    }
    int darr[] = { 51, 92, 68, 70, 61};
    for (int i = 0; i < _countof(darr); i++)
    {
        //gTree.Delete(darr[i]);
    }
    InvalidateRect(hWnd, nullptr, TRUE); // 다시 그리기 요청
}
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
    //LoadStringW(hInstance, IDC_TREERBTREE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TREERBTREE));

    MSG msg;

    InsertDummyData();
    srand(time(NULL));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TREERBTREE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TREERBTREE);
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

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
        }
    }
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        break;
    }
    case WM_CREATE:
        CreateControlBox(hWnd);
        break;
    case WM_COMMAND:
    {
        SwitchCommand(hWnd, wParam);
        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        TextOut(hdc, 10, 50, L"랜덤값 삽입(SPACE)", lstrlen(L"랜덤값 삽입(SPACE)"));
        TextOut(hdc, 10, 70, L"삽입,삭제(ENTER)", lstrlen(L"삽입,삭제(ENTER)"));
        if (is_dulicate_node)
            TextOut(hdc, 10, 90, L"노드 중복!", lstrlen(L"노드 중복!"));

        auto root = gTree.GetRootNode();
        if (root)
        {
            // 트리 그리기 전 초기화
            gNode_pos_x = 100;  // 시작 x 좌표
            g_lines.clear();
            g_nodes.clear();

            // 계산 단계
            CalculateTree(root, 120);

            // 렌더링 단계
            DrawTree(hdc);
        }

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

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
        {
            // 포커스된 텍스트박스에 따라 버튼 클릭
            int id = GetDlgCtrlID(hWnd);
            HWND hParent = GetParent(hWnd);

            switch (id)
            {
            case ControlBoxID::IDC_INSERT_TEXT:
                SendMessageW(GetDlgItem(hParent, ControlBoxID::IDC_INSERT_BTN), BM_CLICK, 0, 0);
                break;
            case ControlBoxID::IDC_DELETE_TEXT:
                SendMessageW(GetDlgItem(hParent, ControlBoxID::IDC_DELETE_BTN), BM_CLICK, 0, 0);
                break;
            }
            return 0; 
        }
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
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
    HWND hInsertEdit = CreateControl(L"EDIT", nullptr, WS_BORDER | ES_LEFT,
        10, 10, 100, 25, (HMENU)ControlBoxID::IDC_INSERT_TEXT);
    SetWindowSubclass(hInsertEdit, EditSubclassProc, 1, 0);

    // 삽입 버튼
    CreateControl(L"BUTTON", L"삽입", BS_PUSHBUTTON,
        120, 10, 50, 25, (HMENU)ControlBoxID::IDC_INSERT_BTN);

    // 삭제 텍스트박스
    HWND hDeleteEdit = CreateControl(L"EDIT", nullptr, WS_BORDER | ES_LEFT,
        200, 10, 100, 25, (HMENU)ControlBoxID::IDC_DELETE_TEXT);
    SetWindowSubclass(hDeleteEdit, EditSubclassProc, 2, 0);

    // 삭제 버튼
    CreateControl(L"BUTTON", L"삭제", BS_PUSHBUTTON,
        310, 10, 50, 25, (HMENU)ControlBoxID::IDC_DELETE_BTN);

    // 체크 박스
    CreateControl(L"BUTTON", L"Show Nil Nodes", WS_TABSTOP | BS_AUTOCHECKBOX,
        390, 10, 150, 20, (HMENU)ControlBoxID::IDC_CHECK_SHOWNIL);
}

int CalculateTree(Node* node, int y)
{
    if (node == gTree.GetNilNodePtr()) return -1;
    
    const int x_width = 25;
    int childY = y + 75;
    int left_x = -1, right_x = -1;

    // 왼쪽 자식 계산
    if (node->Left != gTree.GetNilNodePtr())
    {
        left_x = CalculateTree(node->Left, childY);
    }
    else if (show_nil_nodes)
    {
        // 닐 노드 정보 저장
        g_nodes.push_back({ gNode_pos_x, childY, NULL, nil_check_color });
        left_x = gNode_pos_x;
        gNode_pos_x += x_width;
    }

    // 현재 노드 위치 기록
    int curr_x = gNode_pos_x;
    g_nodes.push_back({ curr_x, y, node->Key, node->Color });
    gNode_pos_x += x_width;

    // 오른쪽 자식 계산
    if (node->Right != gTree.GetNilNodePtr())
    {
        right_x = CalculateTree(node->Right, childY);
    }
    else if (show_nil_nodes)
    {
        g_nodes.push_back({ gNode_pos_x, childY, NULL, nil_check_color });
        right_x = gNode_pos_x;
        gNode_pos_x += x_width;
    }

    // 자식 연결선 저장
    if (left_x != -1)
    {
        LineSegment line_seg;
        line_seg.start = POINT{ left_x,childY };
        line_seg.end = POINT{ curr_x,y };
        g_lines.push_back(line_seg);
    }
    if (right_x != -1)
    {
        LineSegment line_seg;
        line_seg.start = POINT{ right_x,childY };
        line_seg.end = POINT{ curr_x,y };
        g_lines.push_back(line_seg);
    }

    return curr_x;
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
    for (const auto& nodeinfo : g_nodes)
    {
        if (nodeinfo.color == nil_check_color)
        { // 닐 노드
            DrawNilNode(hdc, nodeinfo.x, nodeinfo.y);
        }
        else
        {
            DrawNode(hdc, nodeinfo.x, nodeinfo.y, nodeinfo.key, nodeinfo.color);
        }
    }
}

void DrawNode(HDC hdc, int x, int y, int key, int color)
{
    // 노드 배경 색
    COLORREF rgb;
    if (color == gTree.BLACK)
    {
        rgb = RGB(0, 0, 0);
    }
    else
    {
        rgb = RGB(255, 0, 0);
    }
    HBRUSH hBrush = CreateSolidBrush(rgb);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Ellipse(hdc, x - node_radius, y - node_radius, x + node_radius, y + node_radius);

    wchar_t buffer[16];
    wsprintf(buffer, L"%d", key);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    RECT rect = { x - node_radius, y - node_radius, x + node_radius, y + node_radius };
    DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 리소스 정리
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

void DrawNilNode(HDC hdc, int x, int y)
{
    HBRUSH grayBrush = CreateSolidBrush(RGB(50,50,50)); // 연회색
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, grayBrush);

    Rectangle(hdc, x - node_radius, y - node_radius/2, x + node_radius, y + node_radius/2);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));
    TextOut(hdc, x - 10, y - 7, L"NIL", 3);

    SelectObject(hdc, oldBrush);
    DeleteObject(grayBrush);
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

        is_dulicate_node = gTree.Find(key);
        gTree.Insert(key, 0); // 이진트리에 삽입

        InvalidateRect(hWnd, nullptr, TRUE); // 다시 그리기 요청
        SetWindowTextW(GetDlgItem(hWnd, IDC_INSERT_TEXT), L"");
        HWND hInsertEdit = GetDlgItem(hWnd, (int)ControlBoxID::IDC_INSERT_TEXT);
        SetFocus(hInsertEdit);
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
        HWND hDeleteEdit = GetDlgItem(hWnd, (int)ControlBoxID::IDC_DELETE_TEXT);
        SetFocus(hDeleteEdit);
        break;
    }
    case ControlBoxID::IDC_CHECK_SHOWNIL:
    {
        LRESULT state = SendMessage(GetDlgItem(hWnd, ControlBoxID::IDC_CHECK_SHOWNIL), BM_GETCHECK, 0, 0);
        show_nil_nodes = (state == BST_CHECKED);
        InvalidateRect(hWnd, NULL, TRUE);       // 다시 그리기
        break;
    }
    default:
        break;
    }
}

int GetSubtreeWidth(Node* node)
{
    if (!node) return 0;
    int leftWidth = GetSubtreeWidth(node->Left);
    int rightWidth = GetSubtreeWidth(node->Right);

    return max(1, leftWidth + rightWidth);
}

void InsertRandomValue(int range)
{
    int key;
    for (int i=0;i<range;++i)
    {
        key = rand() % range;
        if (gTree.Find(key) == false)
            break;
    }
    gTree.Insert(key, 0);
    is_dulicate_node = false;
}