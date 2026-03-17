
// MFCStartDlg.h: 헤더 파일
//

#pragma once
#include <vector>

struct MyPoint {
	int x;
	int y;
};

// CMFCStartDlg 대화 상자
class CMFCStartDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCStartDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	std::vector<MyPoint> m_points;   // 클릭한 점들 (최대 3개)

	int m_clickRadius;               // 클릭 원 반지름
	int m_circleThickness;           // 정원 두께

	bool m_isDragging;               // 드래그 여부
	int m_dragIndex;                 // 드래그 중인 점 인덱스

	CString m_strRadius;
	CString m_strThickness;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSTART_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonRandom();
private:
	static UINT OnThreadFunc(LPVOID pParam);
	bool m_isThreadRunning;
};
