
// OpenCVAppGUIDlg.h: 헤더 파일
//

#pragma once

#include <iostream>
#include <map>
#include <vector>

#define OPENCV_470

#include <opencv2/opencv.hpp>
#ifdef OPENCV_470
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#endif // OPENCV_470

#ifdef _DEBUG
#pragma comment(lib,"opencv_world470d.lib")
#else	//RELEASE
#pragma comment(lib,"opencv_world470.lib")
#endif

using namespace std;
using namespace cv;

typedef int(*InspMethod)(void*);	//Function Pointer -> 반환형(int) 함수명(InspMethod) 매개변수(void*)

enum { eImgSrcColor = 0, eImgSrcGray, eImgDebugGray, eImgDebugColor,eImgDrawColor, eImgResultColor, eImgBufferMax};

// COpenCVAppGUIDlg 대화 상자
class COpenCVAppGUIDlg : public CDialogEx
{
// 생성입니다.
public:
	COpenCVAppGUIDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENCVAPPGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private : 
	std::map<string, InspMethod> _mInsps;
	std::map<int, cv::Mat> _mMatBuff;// { {"src", 10}, { "GPU", 15 }, { "RAM", 20 } };
	std::map<int, CRect> _mWndImageView;// { {1016, (10,10,300,400)}, { 2, (301,10,300,400) }, { "RAM", 20 } };
	Point pt_begin, pt_end;
	vector<Point> _vLinePoints;
	CRect _rtImageView;//image view area
	cv::Mat _SourceImage, _DrawImage; // 이미지 정보를 담고 있는 객체.
	//_Draw ... display ... original+draw rect/line/circle ...
	//_Frame/_SourceBuffer  ... original
	//agorithm...function (input, params)...input-constant, params-variables
	//_ProcessImage ... 중간/결과 이미지 buffer
	
	bool _bShowResult = false;
	bool _bShowDebug = false;
	bool _bShowClear = false;
	double _dSrcScale = 1.0, _dNewScale = 1.0;

public:
	void UpdateDispSrc();

private:
	int OnAllocateBuffer(int cols, int rows);
	int OnInspection(InputArray src, OutputArray dst);
	int OnInspection(Mat src, Mat dst);
	int OnInspection(uchar* pSrc, size_t cols, size_t rows, uchar* pDst);
	static int CallInspFindcontourSample(void* lpUserData);
	static int CallInspFindShape(void* lpUserData);
	static int CallInspFillCircle(void* lpUserData);
	static int CallInspFillTriangle(void* lpUserData);
	static int CallInspFillRect(void* lpUserData);
	static int CallInspFillRectTri(void* lpUserData);
	static int CallInspArea(void* lpUserData);
	static int CallInspSegmentColor(void* lpUserData);
	static int CallInspMorpology(void* lpUserData);
	static int CallInspCheckUsingMorpology(void* lpUserData);
	static int CallInspHistogram(void* lpUserData);
	static int CallInspGamma(void* lpUserData);
	static int CallInspSearchingContour(void* lpUserData);
	static int CallInspMatching(void* lpUserData);
	int OnInspMatching();
	int OnInspSearchingContour();
	int OnDrawHistogram(const Mat&, Mat&);
	int OnInspGamma();
	int OnInspHistogram();
	int OnInspCheckUsingMorpology();
	int OnInspSegmentColor();
	int UpdateInspList();
	int OnInspFindcontourSample();
	int OnInspFindShapes();
	int OnInspFillCircle();
	int OnInspFillTriangle();
	int OnInspFillRect();
	int OnInspFillRectTri();
	int OnInspArea();
	int OnInspMorpology();

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnBnClickedBtnInspectioncv();
	afx_msg void OnBnClickedBtnSamplecode();

	afx_msg LRESULT OnAddString(WPARAM wParam, LPARAM lParam);

	CListBox _ListBoxLog;
	void AddString(LPCSTR lpszLog);
};
