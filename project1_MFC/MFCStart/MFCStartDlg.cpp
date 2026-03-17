
// MFCStartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCStart.h"
#include "MFCStartDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCStartDlg 대화 상자



CMFCStartDlg::CMFCStartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSTART_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_isThreadRunning = false;
}

void CMFCStartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_strRadius);
	DDX_Text(pDX, IDC_EDIT_THICK, m_strThickness);
}

BEGIN_MESSAGE_MAP(CMFCStartDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CMFCStartDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_RANDOM, &CMFCStartDlg::OnBnClickedButtonRandom)
END_MESSAGE_MAP()


// CMFCStartDlg 메시지 처리기

BOOL CMFCStartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_isDragging = false;
	m_dragIndex = -1;
	m_clickRadius = 10;
	m_circleThickness = 3;
	m_strRadius = _T("10");
	m_strThickness = _T("3");
	UpdateData(FALSE);

	CDialogEx::OnInitDialog();

	srand((unsigned int)time(NULL));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCStartDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

BOOL CMFCStartDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		CPoint pt(pMsg->pt);
		ScreenToClient(&pt);

		if (m_isDragging && m_dragIndex != -1)
		{
			m_points[m_dragIndex].x = pt.x;
			m_points[m_dragIndex].y = pt.y;

			Invalidate();
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void DrawCircle(CDC* pDC, int cx, int cy, int r)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // 두께 고정

	CPen* oldPen = pDC->SelectObject(&pen);

	const int steps = 360;
	double angleStep = 2.0 * 3.141592 / steps;

	int prevX = cx + r;
	int prevY = cy;

	for (int i = 1; i <= steps; i++)
	{
		double angle = i * angleStep;

		int x = cx + (int)(r * cos(angle));
		int y = cy + (int)(r * sin(angle));

		pDC->MoveTo(prevX, prevY);
		pDC->LineTo(x, y);

		prevX = x;
		prevY = y;
	}

	pDC->SelectObject(oldPen);
}

void DrawFilledCircle(CDC* pDC, int cx, int cy, int r)
{
	for (int y = -r; y <= r; y++)
	{
		for (int x = -r; x <= r; x++)
		{
			if (x * x + y * y <= r * r)
			{
				pDC->SetPixel(cx + x, cy + y, RGB(0, 0, 0));
			}
		}
	}
}

bool GetCircleFrom3Points(MyPoint p1, MyPoint p2, MyPoint p3, double& cx, double& cy, double& r)
{
	double A = p2.x - p1.x;
	double B = p2.y - p1.y;
	double C = p3.x - p1.x;
	double D = p3.y - p1.y;

	double E = A * (p1.x + p2.x) + B * (p1.y + p2.y);
	double F = C * (p1.x + p3.x) + D * (p1.y + p3.y);
	double G = 2 * (A * (p3.y - p2.y) - B * (p3.x - p2.x));

	if (G == 0) return false; // 일직선이면 원 못 만듦

	cx = (D * E - B * F) / G;
	cy = (A * F - C * E) / G;

	r = sqrt((p1.x - cx) * (p1.x - cx) + (p1.y - cy) * (p1.y - cy));

	return true;
}

void CMFCStartDlg::OnPaint()
{
	UpdateData(TRUE); // UI → 변수

	// 원 반지름 사용자 입력
	m_clickRadius = _ttoi(m_strRadius);
	if (m_clickRadius <= 0)
		m_clickRadius = 10;
	
	// 정원 두께 사용자 입력
	m_circleThickness = _ttoi(m_strThickness);
	if (m_circleThickness <= 0)
		m_circleThickness = 1;

	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CFont font;
		font.CreatePointFont(100, L"Arial");
		CFont* oldFont = dc.SelectObject(&font);
		CString str;

		for (int i = 0; i < m_points.size(); i++)
		{
			str.Format(L"(%d, %d)", m_points[i].x, m_points[i].y);
			dc.TextOut(m_points[i].x + 20, m_points[i].y - 20, str);
		}

		for (auto& p : m_points)
		{
			DrawFilledCircle(&dc, p.x, p.y, m_clickRadius);
		}
		for (int i = 0; i < m_points.size(); i++)
		{
			str.Format(L"(%d, %d)", m_points[i].x, m_points[i].y);
			dc.TextOut(m_points[i].x + 20, m_points[i].y - 20, str);
		}

		dc.SelectObject(oldFont);

		if (m_points.size() == 3)
		{
			double cx, cy, r;

			if (GetCircleFrom3Points(m_points[0], m_points[1], m_points[2], cx, cy, r))
			{
				for (int t = 0; t < m_circleThickness; t++)
				{
					DrawCircle(&dc, (int)cx, (int)cy, (int)r + t);
				}
			}
		}
	}
}

void CMFCStartDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	OutputDebugString(L"Up\n");

	ReleaseCapture();
	m_isDragging = false;
	m_dragIndex = -1;

	ReleaseCapture();

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CMFCStartDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i = 0; i < m_points.size(); i++)
	{
		int dx = m_points[i].x - point.x;
		int dy = m_points[i].y - point.y;

		int range = m_clickRadius + 20;

		if (dx * dx + dy * dy <= range * range)
		{
			m_isDragging = true;
			m_dragIndex = i;

			SetCapture();
			return;
		}
	}

	if (m_points.size() < 3)
	{
		MyPoint p;
		p.x = point.x;
		p.y = point.y;

		m_points.push_back(p);
		//UpdatePointText();	// EditControl에 좌표 출력하는 함수 -> 비활성
	}

	Invalidate();

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CMFCStartDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	if (m_isDragging && m_dragIndex != -1)
	{
		m_points[m_dragIndex].x = point.x;
		m_points[m_dragIndex].y = point.y;

		//UpdatePointText();
		Invalidate();
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CMFCStartDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
/*
void CMFCStartDlg::UpdatePointText()
{
	CString str;

	// 첫 번째 점
	if (m_points.size() > 0)
	{
		str.Format(L"(%d, %d)", m_points[0].x, m_points[0].y);
		SetDlgItemText(IDC_POINT1, str);
	}

	// 두 번째 점
	if (m_points.size() > 1)
	{
		str.Format(L"(%d, %d)", m_points[1].x, m_points[1].y);
		SetDlgItemText(IDC_POINT2, str);
	}

	// 세 번째 점
	if (m_points.size() > 2)
	{
		str.Format(L"(%d, %d)", m_points[2].x, m_points[2].y);
		SetDlgItemText(IDC_POINT3, str);
	}
}
*/
// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCStartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCStartDlg::OnBnClickedButtonReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_points.clear();
	m_isDragging = false;
	m_dragIndex = -1;

	Invalidate();
}

void CMFCStartDlg::OnBnClickedButtonRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_points.size() < 3)
		return;

	if (m_isThreadRunning)
		return;

	m_isThreadRunning = true;

	AfxBeginThread(OnThreadFunc, this);		// 메인 UI와 구분하여 쓰레드를 구동
}


/* 쓰레드 동작 시 실행 구문 */
UINT CMFCStartDlg::OnThreadFunc(LPVOID pParam)
{
	CMFCStartDlg* pDlg = (CMFCStartDlg*)pParam;

	for (int i = 0; i < 10; i++)
	{
		Sleep(500);

		if (pDlg->m_points.size() < 3)
			continue;

		CRect rect;
		pDlg->GetClientRect(&rect);

		for (int j = 0; j < pDlg->m_points.size(); j++)
		{
			pDlg->m_points[j].x = rand() % rect.Width();
			pDlg->m_points[j].y = rand() % rect.Height();
		}

		pDlg->Invalidate(); // 다시 그리기
	}

	pDlg->m_isThreadRunning = false;

	return 0;
}