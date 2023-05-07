
// OpenCVAppGUIDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "OpenCVAppGUI.h"
#include "OpenCVAppGUIDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <format>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NAME_WINDOW_SRC "srcView"
#define NAME_WINDOW_RESULT "srcResult"
#define NAME_WINDOW_DEBUG "srcDebug"
#define WM_ADD_STRING	(WM_USER + 500)

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

// COpenCVAppGUIDlg 대화 상자

COpenCVAppGUIDlg::COpenCVAppGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENCVAPPGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenCVAppGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, _ListBoxLog);
}

BEGIN_MESSAGE_MAP(COpenCVAppGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOAD, &COpenCVAppGUIDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &COpenCVAppGUIDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &COpenCVAppGUIDlg::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_BTN_INSPECTIONCV, &COpenCVAppGUIDlg::OnBnClickedBtnInspectioncv)
	ON_BN_CLICKED(IDC_BTN_SAMPLECODE, &COpenCVAppGUIDlg::OnBnClickedBtnSamplecode)
	ON_MESSAGE(WM_ADD_STRING, &COpenCVAppGUIDlg::OnAddString)
END_MESSAGE_MAP()

// COpenCVAppGUIDlg 메시지 처리기

BOOL COpenCVAppGUIDlg::OnInitDialog()
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

	//ShowWindow(SW_MAXIMIZE);
	//ShowWindow(SW_MINIMIZE);

	//opencv -> window handle
	namedWindow(NAME_WINDOW_SRC, WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(NAME_WINDOW_SRC);
	HWND hParent = (HWND)FindWindow(NULL, NAME_WINDOW_SRC);
	HWND hOrgParent = ::SetParent(hWnd, GetDlgItem(IDC_PICTURE)->m_hWnd);
	::ShowWindow(hOrgParent, SW_HIDE);

	namedWindow(NAME_WINDOW_RESULT, WINDOW_AUTOSIZE);
	hWnd = (HWND)cvGetWindowHandle(NAME_WINDOW_RESULT);
	hParent = (HWND)FindWindow(NULL, NAME_WINDOW_RESULT);
	hOrgParent = ::SetParent(hWnd, GetDlgItem(IDC_PICTURE_RESULT)->m_hWnd);
	::ShowWindow(hOrgParent, SW_HIDE);

	namedWindow(NAME_WINDOW_DEBUG, WINDOW_AUTOSIZE);
	hWnd = (HWND)cvGetWindowHandle(NAME_WINDOW_DEBUG);
	hParent = (HWND)FindWindow(NULL, NAME_WINDOW_DEBUG);
	hOrgParent = ::SetParent(hWnd, GetDlgItem(IDC_PICTURE_DEBUG)->m_hWnd);
	::ShowWindow(hOrgParent, SW_HIDE);

	//GetDlgItem(IDC_PICTURE)->GetWindowRect(_rtImageView);
	//ScreenToClient(_rtImageView);

	_mWndImageView.insert(pair<int, CRect>(IDC_PICTURE, CRect(0,0,0,0)));
	_mWndImageView.insert(pair<int, CRect>(IDC_PICTURE_RESULT, CRect(0, 0, 0, 0)));
	_mWndImageView.insert(pair<int, CRect>(IDC_PICTURE_DEBUG, CRect(0, 0, 0, 0)));

	for (auto iter = _mWndImageView.begin(); iter != _mWndImageView.end(); iter++) {
		int resID = (iter->first);
		CRect rtImgView;
		GetDlgItem(resID)->GetWindowRect(rtImgView);
		ScreenToClient(rtImgView);
		iter->second = rtImgView;
	}
	_mMatBuff.clear();
	_mInsps.clear();

	AddString("System Log :: Start()");

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void COpenCVAppGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COpenCVAppGUIDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	if (IsIconic()){
		//CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

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
	else{
		CBrush brush(HS_DIAGCROSS, RGB(200, 200, 200));
		//dc.FillRect(CRect(rect.left + 1, rect.top + 1, rect.right, rect.bottom), &brush);

		for (auto iter = _mWndImageView.begin(); iter != _mWndImageView.end(); iter++) {
			CRect rt = iter->second;
			dc.FillRect(rt, &brush);
		}

		if (!_mMatBuff[eImgSrcColor].empty()){
			Mat Img = _mMatBuff[eImgSrcColor];
			Mat DrawResize;
			Size size(int(_dNewScale * Img.cols), int(_dNewScale * Img.rows));
			resize(Img, DrawResize, size);

			resizeWindow(NAME_WINDOW_SRC, size.width, size.height);
			imshow(NAME_WINDOW_SRC, DrawResize);
		}

		if (_bShowResult){
			if (!_mMatBuff[eImgDrawColor].empty()){
				Mat Img = _mMatBuff[eImgDrawColor];
				Mat DrawResize;
				Size size(int(_dNewScale * Img.cols), int(_dNewScale * Img.rows));
				resize(Img, DrawResize, size);

				resizeWindow(NAME_WINDOW_DEBUG, size.width, size.height);
				imshow(NAME_WINDOW_DEBUG, DrawResize);
			}
		}
		if (_bShowDebug){
			if (!_mMatBuff[eImgDebugColor].empty()){
				Mat Img = _mMatBuff[eImgDebugColor];
				Mat DrawResize;
				Size size(int(_dNewScale * Img.cols), int(_dNewScale * Img.rows));
				resize(Img, DrawResize, size);

				resizeWindow(NAME_WINDOW_RESULT, size.width, size.height);
				imshow(NAME_WINDOW_RESULT, DrawResize);
			}
		}
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COpenCVAppGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COpenCVAppGUIDlg::UpdateDispSrc()
{
	_bShowResult = false;
	_bShowDebug = false;
	CRect rectSrc;
	::GetWindowRect(GetDlgItem(IDC_PICTURE)->m_hWnd, rectSrc);
	double dScaleX = rectSrc.Width() / (double)_SourceImage.cols;
	double dScaleY = rectSrc.Height() / (double)_SourceImage.rows;
	_dSrcScale = min(dScaleX, dScaleY);
	_dNewScale = _dSrcScale;
	//_bShowResult = FALSE;
	//防止顯示不同比例圖片時DC殘留
	CWnd* pWnd = GetDlgItem(IDC_PICTURE);
	pWnd->GetClientRect(&rectSrc);//제어 클라이언트 영역의 좌표 얻기
	pWnd->ClientToScreen(rectSrc);//제어 클라이언트 영역에서 대화 영역으로 영역 좌표 변환
	this->ScreenToClient(rectSrc); //다이얼로그 영역의 영역 좌표를 다이얼로그의 클라이언트 영역 좌표로 변환
	InvalidateRect(rectSrc);
	//비율이 다른 사진을 표시할 때 DC 잔여물 방지
}

void COpenCVAppGUIDlg::OnBnClickedBtnLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_READONLY, _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();

		//string fileName = path;
		//_SourceImage = cv::imread(fileName, IMREAD_ANYCOLOR);

		//if (_SourceImage.type() == CV_8UC1)
		//	cvtColor(_SourceImage, _DrawImage, COLOR_GRAY2BGR);
		//else
		//	_SourceImage.copyTo(_DrawImage);

		string fileName = path.GetString();
		_SourceImage = cv::imread(fileName, IMREAD_ANYCOLOR);

		OnAllocateBuffer(_SourceImage.cols, _SourceImage.rows);

		UpdateDispSrc();
		UpdateInspList();
		InvalidateRect(_rtImageView, FALSE);
		AfxMessageBox("Image Loaded");
	}
}

void COpenCVAppGUIDlg::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//string fileName = "jetson_clone.bmp";
	//_imageView.Save(fileName.c_str());

	CFileDialog dlgFileSave(FALSE, "*.*", NULL,//화일명 없음
		OFN_FILEMUSTEXIST,
		_T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"), NULL);

	//옵션 선택 부분.
	if (dlgFileSave.DoModal() == IDOK)
	{
		string str = dlgFileSave.GetPathName().GetString();
		imwrite(str, _SourceImage);		
	}
	AfxMessageBox("Image Saved");
}

void COpenCVAppGUIDlg::OnBnClickedBtnInspection(){
//input
	//OnInspection(InputArray src, OutputArray dst);
	Mat src = _mMatBuff[eImgSrcGray];
	
//fucntion
	Mat dst = _mMatBuff[eImgDebugGray];
	
	// using pointer to data
	OnInspection(src.data, src.cols, src.rows, dst.data);
	dst.copyTo(_mMatBuff[eImgDebugGray]);

	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
//result & display

	_bShowResult = true;
	_bShowDebug = true;

	Mat draw = _mMatBuff[eImgDrawColor]; 
	Mat dstde = _mMatBuff[eImgDebugColor];
	//cv::rectangle(draw, Rect(10, 10, 50, 50), Scalar(0, 0, 255), 3);
	int thickness = 3;
	int lineType = LINE_8;
	int w = 100;
	int vSize = _vLinePoints.size();
	line(draw, _vLinePoints[0], _vLinePoints[1], Scalar(0, 0, 255), thickness, lineType);
	line(draw, _vLinePoints[vSize - 2], _vLinePoints[vSize -1], Scalar(0, 0, 255), thickness ,lineType);
	line(draw, _vLinePoints[0], _vLinePoints[vSize - 2], Scalar(0, 0, 255), thickness, lineType);
	line(draw, _vLinePoints[1], _vLinePoints[vSize -1], Scalar(0, 0, 255), thickness, lineType);
	line(dstde, Point(_vLinePoints[0].x, 0), Point(_vLinePoints[0].x, src.rows), Scalar(200, 177, 35), 1, lineType);
	line(dstde, Point(_vLinePoints[1].x, 0), Point(_vLinePoints[1].x, src.rows), Scalar(169, 155, 187), 1, lineType);
	line(dstde, Point(0, _vLinePoints[0].y), Point(src.cols, _vLinePoints[0].y), Scalar(200, 35, 81), 1, lineType);
	line(dstde, Point(0, _vLinePoints[vSize - 1].y), Point(src.cols, _vLinePoints[vSize - 1].y), Scalar(0, 215, 255), 1, lineType);
	circle(dstde, _vLinePoints[0], 6, Scalar(212, 164, 73), 3, lineType,0);
	circle(dstde, _vLinePoints[1], 6, Scalar(212, 164, 73), 3, lineType, 0);
	circle(dstde, _vLinePoints[vSize - 2], 6, Scalar(212, 164, 73), 3, lineType, 0);
	circle(dstde, _vLinePoints[vSize - 1], 6, Scalar(212, 164, 73), 3, lineType, 0);
	Invalidate(FALSE);
}


int COpenCVAppGUIDlg::OnAllocateBuffer(int cols, int rows){
	_mMatBuff.clear();

	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgSrcColor, Mat(rows, cols, CV_8UC3)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgSrcGray, Mat(rows, cols, CV_8UC1)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgDebugGray, Mat(rows, cols, CV_8UC1)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgDrawColor, Mat(rows, cols, CV_8UC3)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgResultColor, Mat(rows, cols, CV_8UC3)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgDebugColor, Mat(rows, cols, CV_8UC3)));

	//source to source of map
	//_SourceImage.copyTo(_mMatBuff[eImgSrc]);
	
	//source to draw of map
	if (_SourceImage.type() == CV_8UC1)
	{
		cvtColor(_SourceImage, _mMatBuff[eImgSrcColor], COLOR_GRAY2BGR); 
		_SourceImage.copyTo(_mMatBuff[eImgSrcGray]);		
	}
	else
	{
		_SourceImage.copyTo(_mMatBuff[eImgSrcColor]);
		cvtColor(_SourceImage, _mMatBuff[eImgSrcGray], COLOR_BGR2GRAY);
	}

	Mat vi = _mMatBuff[eImgSrcColor];

	_mMatBuff[eImgSrcColor].copyTo(_mMatBuff[eImgDrawColor]);

	//debug of map ... just create buffer as initilized value
	_mMatBuff[eImgDebugGray] = 0;

	//result of map ... just create buffer as initilized value
	_mMatBuff[eImgResultColor] = 255;

	return 0;
}

int COpenCVAppGUIDlg::OnInspection(InputArray src, OutputArray dst){
	return 0;
}

int COpenCVAppGUIDlg::OnInspection(Mat src, Mat dst){
	return 0;
}

int COpenCVAppGUIDlg::CallInspFindcontourSample(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFindcontourSample();
}

int COpenCVAppGUIDlg::CallInspFindShape(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFindShapes();
}

int COpenCVAppGUIDlg::CallInspFillCircle(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFillCircle();
}

int COpenCVAppGUIDlg::CallInspFillTriangle(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFillTriangle();
}

int COpenCVAppGUIDlg::CallInspFillRect(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFillRect();
}

int COpenCVAppGUIDlg::UpdateInspList(){
	_mInsps.insert(make_pair("OnInspFindcontourSample", &COpenCVAppGUIDlg::CallInspFindcontourSample));
	_mInsps.insert(make_pair("OnInspFindShapes", COpenCVAppGUIDlg::CallInspFindShape));
	_mInsps.insert(make_pair("OnInspFillCircle", COpenCVAppGUIDlg::CallInspFillCircle));
	_mInsps.insert(make_pair("OnInspFillTriangle", COpenCVAppGUIDlg::CallInspFillTriangle));
	_mInsps.insert(make_pair("OnInspFillRect", COpenCVAppGUIDlg::CallInspFillRect));
	_mInsps.insert(make_pair("OnInspFillRectTri", COpenCVAppGUIDlg::CallInspFillRectTri));
	_mInsps.insert(make_pair("OnInspArea", COpenCVAppGUIDlg::CallInspArea));
	_mInsps.insert(make_pair("OnInspSegmentColor", COpenCVAppGUIDlg::CallInspSegmentColor));
	_mInsps.insert(make_pair("OnInspMorpology", COpenCVAppGUIDlg::CallInspMorpology));
	_mInsps.insert(make_pair("OnInspCheckUsingMorpology", COpenCVAppGUIDlg::CallInspCheckUsingMorpology));
	_mInsps.insert(make_pair("OnInspHistogram", COpenCVAppGUIDlg::CallInspHistogram));
	_mInsps.insert(make_pair("OnInspGamma", COpenCVAppGUIDlg::CallInspGamma));
	_mInsps.insert(make_pair("OnInspSearchingContour", COpenCVAppGUIDlg::CallInspSearchingContour));
	_mInsps.insert(make_pair("OnInspMatching", COpenCVAppGUIDlg::CallInspMatching));
	return 1;
}

int COpenCVAppGUIDlg::CallInspFillRectTri(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFillRectTri();
}

int COpenCVAppGUIDlg::CallInspArea(void* lpUserData){
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspArea();
}

int COpenCVAppGUIDlg::CallInspSegmentColor(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspSegmentColor();
}

int COpenCVAppGUIDlg::CallInspMorpology(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspMorpology();
}

int COpenCVAppGUIDlg::CallInspCheckUsingMorpology(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspCheckUsingMorpology();
}

int COpenCVAppGUIDlg::CallInspHistogram(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspHistogram();
}

int COpenCVAppGUIDlg::CallInspGamma(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspGamma();
}

int COpenCVAppGUIDlg::CallInspSearchingContour(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspSearchingContour();
}

int COpenCVAppGUIDlg::CallInspMatching(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspMatching();
}

int COpenCVAppGUIDlg::OnInspMatching()
{
	_bShowDebug = _bShowResult = false;

	Mat search_img = imread("../Build/x64/images/6.search_array.png", IMREAD_GRAYSCALE);
	Mat search_img_color = cv::imread("../Build/x64/images/6.search_array.png", IMREAD_ANYCOLOR);

	//Mat matching_img = Mat::zeros(search_img.size(), CV_64F);
	//Mat src_img = imread("../Build/x64/images/search_wally.png", IMREAD_COLOR);
	//pattern matching
	//노가다 코드
	/*if (0) {
	for (size_t row = 0; row <= search_img.rows - search_ptrn.rows; row++) {			//pattern이 차지하는 범위를 뺀 값을 row의 범위로 잡음
		for (size_t col = 0; col <= search_img.cols - search_ptrn.cols; col++) {		//pattern이 차지하는 범위를 뺀 값을 col의 범위로 잡음
			uchar* pSearch = search_img.data;
			uchar* pPtrn = search_ptrn.data;	//5x2
			double* pMatch = (double*)matching_img.data;
			//* * * * *
			//* * * * *
			double TM_SQDIFF = 0.0, TM_SQDIFF_NORMED = 0.;
			double ptrnSQ = 0., searchSQ = 0.;
			for (size_t prow = 0; prow < search_ptrn.rows; prow++) {
				for (size_t pcol = 0; pcol < search_ptrn.cols; pcol++) {
					int ptrn_index = prow * search_ptrn.cols + pcol;
					int search_index = (row + prow) * search_img.cols + (col + pcol);

					double diff = pSearch[search_index] - pPtrn[ptrn_index];
					TM_SQDIFF += (diff * diff);
					searchSQ += pSearch[search_index] * pSearch[search_index];
					ptrnSQ += pPtrn[ptrn_index] * pPtrn[ptrn_index];
					//matching_img.at<double>(row+prow, col+pcol) = TM_SQDIFF;
				}
				TM_SQDIFF_NORMED = TM_SQDIFF / sqrt(ptrnSQ * searchSQ);
				if (TM_SQDIFF != 0)
					matching_img.at<double>(row, col) = 255;
				else
					matching_img.at<double>(row, col) = TM_SQDIFF_NORMED;
			}
		}
	}
	Point pt1, pt2, pt3, pt4;
	vector<Point> contour;
	vector<vector<Point>> contours;

	for (size_t row = 0; row < matching_img.rows; row++) {
		for (size_t col = 0; col < matching_img.cols; col++) {
			int index = row * search_img.cols + col;
			if (matching_img.at<double>(row, col) == 0.0 && row < matching_img.rows - search_ptrn.rows && col < matching_img.cols - search_ptrn.cols) {
				pt1 = Point(col, row);
				pt2 = Point(col + search_ptrn.cols, row);
				pt3 = Point(col + search_ptrn.cols, row + search_ptrn.rows);
				pt4 = Point(col, row + search_ptrn.rows);
				contour.push_back(pt1);
				contour.push_back(pt2);
				contour.push_back(pt3);
				contour.push_back(pt4);
			}
		}
	}
	contours.push_back(contour);
	drawContours(src_img, contours, -1, Scalar(0, 0, 255), 2, 8);

	search_img.copyTo(_mMatBuff[eImgSrcColor]);
	matching_img.copyTo(_mMatBuff[eImgDebugColor]);
	src_img.copyTo(_mMatBuff[eImgDrawColor]);
	_bShowDebug = _bShowResult = true;
	Invalidate(FALSE);
}*/	 
	
	//라이브러리 이용한 똑똑한 코드

	double thresh = 0.9952;
	Mat result_img, search_ptrn;
	vector<Point> contour;
	
	for (double i = 1.0; i < 1.5; i += 0.01){
		search_ptrn = imread("../Build/x64/images/7.search_ptrn_rect_small.png", IMREAD_GRAYSCALE);
		resize(search_ptrn, search_ptrn, Size(search_ptrn.cols *i, search_ptrn.rows * i), INTER_NEAREST);
		matchTemplate(search_img, search_ptrn, result_img, TM_CCORR_NORMED);
		threshold(result_img, result_img, thresh, 1, THRESH_BINARY);
		//result_img.convertTo(result_img, CV_8UC1);
		for (size_t row = 0; row < result_img.rows; row++){
			for (size_t col = 0; col < result_img.cols; col++){
				if (result_img.at<float>(row, col) >= thresh)
					contour.push_back(Point(col, row));
			}
		}
		if (contour.size() > 0)
			break;
	}
	for (size_t i = 0; i < contour.size(); i++){
		rectangle(search_img_color, Rect(contour[i].x, contour[i].y, search_ptrn.cols, search_ptrn.rows), Scalar(50, 50, 255), 2);
	}
		
	result_img.copyTo(_mMatBuff[eImgDebugColor]);
	search_img_color.copyTo(_mMatBuff[eImgDrawColor]);
	
	_bShowDebug = _bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspSearchingContour() {

	//교수님 코드

	//아래 사각형의 Contour를 구하시오
	vector<vector<Point>> Contours;
	vector<Point> Contour;
	Contours.clear();
	Contour.clear();

	const int cols = 10;
	const int rows = 10;
	uchar buff[rows * cols] = {
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,0,0,0,
		0,0,0,1,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
	};

	uchar buff_label[rows * cols] = {
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
	};
	Mat bufImg = Mat(rows, cols, CV_8UC1, buff);
	Mat bufLabel = Mat(rows, cols, CV_8UC1, buff_label);
	bufImg *= 255;

	int label = 0;
	Point ptStart(0, 0);
	bool bStart = false;
	bool bFind = false;

	Contour.clear();
	bFind = false;
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t col = 0; col < cols; col++)
		{
			int index = (row)*cols + (col);
			if (bufImg.data[index] == 255 && bufLabel.data[index] == 0)
			{
				bFind = true;
				bufLabel.data[index] = 255;
				ptStart = Point(col, row);
				Contour.push_back(ptStart);
			}
			if (bFind)
			{
				break;
			}
		}
		if (bFind)
		{
			break;
		}
	}

	//enum eDIR {eEE, eEN,eNN, eWN, eWW, eWS, eSS, eES, eDirMax};
	enum eDIR { eEE, eES, eSS, eWS, eWW, eWN, eNN, eEN, eDirMax };
	vector<Point> vDir(eDirMax);
	vDir[(int)eEE] = Point(1, 0);
	vDir[(int)eEN] = Point(1, -1);
	vDir[(int)eNN] = Point(0, -1);
	vDir[(int)eWN] = Point(-1, -1);
	vDir[(int)eWW] = Point(-1, 0);
	vDir[(int)eWS] = Point(-1, 1);
	vDir[(int)eSS] = Point(0, 1);
	vDir[(int)eES] = Point(1, 1);

	int dir = eEE;
	Point ptCur = ptStart;

	do
	{
		bFind = false;
		Point ptNext = ptCur + vDir[dir];
		if (ptStart == ptNext)
			break;
		//search cross
		if (bufImg.data[(ptNext.y) * cols + (ptNext.x)] == 255 &&
			bufLabel.data[(ptNext.y) * cols + (ptNext.x)] != 255)
		{
			bufLabel.data[(ptNext.y) * cols + (ptNext.x)] = 255;
			bFind = true;
			Contour.push_back(ptNext);
		}
		else
		{
			dir++;
		}
		if (bFind)
		{
			ptCur = ptNext;
			//dir = eEE;
		}


	} while (true);

	for (size_t i = 1; i < Contour.size(); i++)
	{
		Point pre = Contour[i - 1];
		Point cur = Contour[i];
		double diff_pre = sqrt((pre.x - cur.x) * (pre.x - cur.x) + (pre.y - cur.y) * (pre.y - cur.y));
		if (diff_pre == 1 && pre.y == cur.y)
		{
			Contour.erase(Contour.begin() + i);
		}
		if (diff_pre == 1 && pre.x == cur.x)
		{
			Contour.erase(Contour.begin() + i);
		}
	}

	Contours.push_back(Contour);

	for (auto& contour : Contours)
	{
		for (auto& iter : contour)
		{
			string msg = "";
			msg = "x, y = ";
			msg += std::format("{:d}, {:d}", iter.x, iter.y);
			AddString(msg.c_str());
		}
	}
	resize(bufLabel, bufLabel, Size(40 * cols, 40 * rows), 0, 0, INTER_NEAREST);
	bufLabel.copyTo(_mMatBuff[eImgDrawColor]);
	_bShowDebug = _bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

	/*_bShowDebug = _bShowResult = false;

	const int rows = 10, cols = 10;
	uchar buff[cols * rows] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	Mat con(rows, cols, CV_8UC1, buff);

	vector<vector<Point>> contours;
	vector<Point> contour;
	vector<Vec4i> hierarchy;
	uchar check[cols * rows] = { 0, };
	Mat concheck(rows, cols, CV_8UC1, check);
	bool checkFlag = false;*/
	/*
		for (size_t row = 0; row < rows; row++) {
			for (size_t col = 0; col < cols; col++) {
				int index = row * cols + col;
				if (buff[index] == 1 && contour.size() == 0) {
					contour.push_back(Point(col, row));
					check[index] = 1;
				}
				else if (buff[index] == 1 && check[index] == 0) {
					contour.push_back(Point(col, row));
					check[index] = 1;
				}
				else if (buff[index] == 0 && contour.size() > 0) {
					if (buff[index - 1 + rows] == 1 && check[index - 1 + rows] == 0) {
						contour.push_back(Point(col - 1, row + 1));
						col -= 1;
						row += 1;
						check[index - 1 + rows] = 1;
					}
					else if (index > 2 && buff[index - 2] == 1 && check[index - 2] == 0) {
						contour.push_back(Point(col - 2, row));
						col -= 3;
						check[index - 2] = 1;
					}
				}
				else if (buff[index] == 1 && Point(col, row) != contour[0]) {
					if (buff[index - 1 + rows] == 1 && check[index - 1 + rows] == 0) {
						contour.push_back(Point(col - 1, row + 1));
						col -= 2;
						row += 1;
						check[index - 1 + rows] = 1;
					}
					else if (buff[index - 1] == 1 && check[index - 1] == 0) {
						contour.push_back(Point(col - 1, row));
						col -= 2;
						check[index - 1] = 1;
					}
					else if (buff[index - 1 - rows] == 1 && check[index - 1 - rows] == 0) {
						contour.push_back(Point(col - 1, row - 1));
						col -= 2;
						row -= 1;
						check[index - 1 - rows] = 1;
					}
					else if (buff[index - rows] == 1 && check[index - rows] == 0) {
						contour.push_back(Point(col, row - 1));
						col -= 1;
						row -= 1;
						check[index - rows] = 1;
					}
					else if (buff[index - rows] == 1 && check[index - rows] == 1) {
						row -= 2;
						break;
					}
				}
				else if (buff[index] == 1 && Point(col, row) == contour[0]) {
					checkFlag = true;
					break;
				}
			}
			if (checkFlag)
				break;
		}
		contours.push_back(contour);*/
/*	Mat result = Mat::zeros(rows, cols, CV_8UC1);
	findContours(con, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	drawContours(result, contours, -1, Scalar(255), 1, 8, hierarchy, 0);
	resize(result, result, Size(256, 256), 0, 0, INTER_NEAREST);
	result.copyTo(_mMatBuff[eImgDrawColor]);

	_bShowDebug = _bShowResult = true;
	Invalidate(FALSE);

	return 0;
}*/

int COpenCVAppGUIDlg::OnDrawHistogram(const Mat& src_gray, Mat& draw_color)
{
	const int histo_max = 256;
	int histo[histo_max] = { 0, };

	//get histogram :: 히스토그램
	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++) histo[src_gray.data[i]]++;

	//drawing...histo debug color area
	double thickness = 1;
	int lineType = LINE_8;

	Mat color_draw = draw_color;
	//draw   |------>
	int height = src_gray.rows;
	for (size_t col = 0; col < histo_max; col++)
	{
		//draw histo
		Point pt1 = Point(col * 2, height - 1);//start
		Point pt2 = Point(col * 2, height - 1 - histo[col] / 10);//end
		line(color_draw, pt1, pt2, Scalar(0, 255, 128), thickness, lineType);
	}
	return 1;
}

int COpenCVAppGUIDlg::OnInspGamma()
{
	_bShowDebug = _bShowResult = false;
	_ListBoxLog.ResetContent();
	double minV, maxV;
	Mat src_gray = _mMatBuff[eImgSrcGray];//얕은복사...주소값이 같다

	Mat src_Color = _mMatBuff[eImgSrcColor];
	OnDrawHistogram(src_gray, src_Color);

	putText(src_Color, string("Original"), Point(10, 50), CV_FONT_HERSHEY_TRIPLEX, 0.6, Scalar(0, 255, 255));

	string msg = "";

	msg = "[ Left ] Original Histogram";
	cv::minMaxLoc(src_gray, &minV, &maxV);
	msg += std::format("  min Value = {:0.3f}", minV);
	msg += std::format("  max Value = {:0.3f}", maxV);

	AddString(msg.c_str());

	//1.각 픽셀 밝기를 + 30 만큼 증가하시오
	//새로운 이미지 버퍼를 생성
	Mat src_gray_add = cv::Mat(src_gray.size(), src_gray.type()/*CV_8UC1*/);
	src_gray_add = 0;
	//초기값이 0인 버퍼가 생성

	//src_gray_add = src_gray.clone();//깊은복사...주소값이 달라짐. 데이타는 같다

	int add_value = 30;
	uchar* pData = src_gray.data;//원본
	for (size_t row = 0; row < src_gray.rows; row++){
		for (size_t col = 0; col < src_gray.cols; col++){
			uchar* pDataAdd = src_gray_add.data;//목적
			pDataAdd[row * src_gray.cols + col] = pData[row * src_gray.cols + col];
			//1   =   1
			if (pDataAdd[row * src_gray.cols + col] + add_value > 255)
				pDataAdd[row * src_gray.cols + col] = 255;
			else
				pDataAdd[row * src_gray.cols + col] += add_value;//255보다 크다?  256->1 300->45
			//a = b
			//a += 30
			//uchar ... 용량...0~255 [256] 
		}
	}

	for (size_t i = 0; i < src_gray.total(); i++){
		uchar* pDataAdd = src_gray_add.data;//목적
		pDataAdd[i] = pData[i];
		if (pDataAdd[i] + add_value > 255)
			pDataAdd[i] = 255;
		else
			pDataAdd[i] += add_value;//255보다 크다?  256->1 300->45
	}

	Mat src_gray_matrix_add = cv::Mat(src_gray.size(), src_gray.type()/*CV_8UC1*/);
	//src_gray.copyTo(src_gray_matrix_add);
	//src_gray_matrix_add += add_value;
	src_gray_matrix_add = src_gray * 1 + add_value;

	Mat debugColor = _mMatBuff[eImgDebugColor];
	cvtColor(src_gray_matrix_add, debugColor, COLOR_GRAY2BGR);
	OnDrawHistogram(src_gray_matrix_add, debugColor);
	putText(debugColor, string("Math Add + 30"), Point(10, 50), CV_FONT_HERSHEY_TRIPLEX, 0.6, Scalar(0, 255, 255));

	msg = "";
	msg = "[ Center ] Add 30 Histogram";
	cv::minMaxLoc(src_gray_matrix_add, &minV, &maxV);
	msg += std::format("  min Value = {:0.3f}", minV);
	msg += std::format("  max Value = {:0.3f}", maxV);

	AddString(msg.c_str());

	//2.각 픽셀 밝기를 1.5배만큼 증가하시오
	//3.각 픽셀 밝기를 1.24배하고 20만큼 증가하시오
	Mat src_gray_matrix_op = cv::Mat(src_gray.size(), src_gray.type()/*CV_8UC1*/);
	double a = 1.24;
	int b = 20;
	src_gray_matrix_op = src_gray * a + b;

	//Gamma Correction
	// LUT : Look Up Table
	//int histo[256] = { 0, };
	int LUT[256] = { 0, };
	// .... 0 -> LUT[0] -> 10
	// .... 2 -> LUT[2] -> 12
	//수식변환공식의 생략 ... LUT
	//𝑠 = 255×(𝑟 / 255)^𝛾
	double gamma = 0.4;
	for (size_t i = 0; i < 256; i++){
		LUT[i] = std::pow(i / 255.0, gamma) * 255.0;
	}

	//Mat src_gray_gamma = cv::Mat(src_gray.size(), src_gray.type()/*CV_8UC1*/);
	//src_gray_gamma = 0;

	Mat src_gray_gamma = cv::Mat::zeros(src_gray.size(), src_gray.type()/*CV_8UC1*/);
	for (size_t i = 0; i < src_gray.total(); i++){
		uchar* pDataGamma = src_gray_gamma.data;//목적
		pDataGamma[i] = LUT[pData[i]];
	}

	Mat drawColor = _mMatBuff[eImgDrawColor];
	cvtColor(src_gray_gamma, drawColor, COLOR_GRAY2BGR);
	OnDrawHistogram(src_gray_gamma, drawColor);
	putText(drawColor, string("Gamma r 0.4"), Point(10, 50), CV_FONT_HERSHEY_TRIPLEX, 0.6, Scalar(0, 255, 255));

	msg = "";
	msg = "[ Right ] gamma 0.4 Histogram";
	cv::minMaxLoc(src_gray_gamma, &minV, &maxV);
	msg += std::format("  min Value = {:0.3f}", minV);
	msg += std::format("  max Value = {:0.3f}", maxV);

	AddString(msg.c_str());

	_bShowDebug = _bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspHistogram()
{
	//교수님 코드
	/*_bShowDebug = _bShowResult = false;

	Mat src_gray = _mMatBuff[eImgSrcGray];

	const int histo_max = 256;
	int histo[histo_max] = { 0, };
	int cumulative_freq[histo_max] = { 0, };
	int new_gray_value[histo_max] = { 0, };
	//get histogram :: 히스토그램
	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++) histo[src_gray.data[i]]++;
	//                                                      밝기 값[i번째 밝기 값]
	//get cumulative frequence :: 누적분포
	cumulative_freq[0] = histo[0];
	//for (size_t i = 1; i < histo_max; i++) cumulative_freq[i]=histo[i] + cumulative_freq[i-1];

	//get new value
	int acc = 0;
	for (size_t i = 0; i < histo_max; i++)
	{
		acc += histo[i];
		cumulative_freq[i] = acc;
		//new_gray_value[i] = round((((float)cumulative_freq[i]) * 255) / src_gray.cols * src_gray.rows);
		new_gray_value[i] = round((((float)acc) * 255) / (src_gray.cols * src_gray.rows));
		//0~255                         밝기 최대값 / 전체크기
	}

	//drawing...histo debug color area
	int thickness = 1;
	int lineType = LINE_8;

	cvtColor(_mMatBuff[eImgSrcGray], _mMatBuff[eImgDebugColor], ColorConversionCodes::COLOR_GRAY2BGR);

	Mat debug_draw = _mMatBuff[eImgDebugColor];
	//draw   |------>
	bool view_horizontal = false;//세로로
	bool view_vertical = true;//가로로
	if (view_horizontal)
	{
		for (size_t row = 0; row < histo_max; row++)
		{
			line(debug_draw, Point(0, row * 2), Point(histo[row] / 10, row * 2), Scalar(255, 0, 255), thickness, lineType);
		}
	}

	if (view_vertical)//아래->위
	{
		int height = src_gray.rows;
		for (size_t col = 0; col < histo_max; col++)
		{
			//draw histo
			Point pt1 = Point(col * 2, height - 1);//start
			Point pt2 = Point(col * 2, height - 1 - histo[col] / 10);//end
			line(debug_draw, pt1, pt2, Scalar(0, 255, 128), thickness, lineType);
		}
		//선?
		int max_cumulate = cumulative_freq[histo_max - 1];
		for (size_t col = 0; col < histo_max; col++)
		{
			if (col == 0)
			{
				int scaled_v = (cumulative_freq[col] / max_cumulate) * height;
				Point pt1 = Point(col * 2, scaled_v);//start
				Point pt2 = Point(col * 2, scaled_v);//end
				line(debug_draw, pt1, pt2, Scalar(0, 255, 255), thickness, lineType);
				continue;
			}
			//draw cumulate
			int scaled_v1 = (cumulative_freq[col - 1] * 1.0 / max_cumulate) * height;
			int scaled_v2 = (cumulative_freq[col] * 1.0 / max_cumulate) * height;
			Point pt1 = Point(col * 2, height - scaled_v1);//start
			Point pt2 = Point(col * 2, height - scaled_v2);//end
			line(debug_draw, pt1, pt2, Scalar(0, 255, 255), thickness, lineType);
			//그래프
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//src
	Mat src_gray_histoEqual = _mMatBuff[eImgSrcGray].clone();
	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++)
	{
		src_gray_histoEqual.data[i] = new_gray_value[src_gray.data[i]];
	}//                                              [원본]
	cvtColor(src_gray_histoEqual, _mMatBuff[eImgDrawColor], ColorConversionCodes::COLOR_GRAY2BGR);
	//밝은부분은 더 밝게, 어두운곳은 더 어둡게


	//get histogram :: 히스토그램
	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++) histo[src_gray_histoEqual.data[i]]++;

	//get cumulative frequence :: 누적분포
	cumulative_freq[0] = histo[0];
	//for (size_t i = 1; i < histo_max; i++) cumulative_freq[i]=histo[i] + cumulative_freq[i-1];

	//get new value
	acc = 0;
	for (size_t i = 0; i < histo_max; i++)
	{
		acc += histo[i];
		cumulative_freq[i] = acc;
		//new_gray_value[i] = round((((float)cumulative_freq[i]) * 255) / src_gray.cols * src_gray.rows);
		new_gray_value[i] = round((((float)acc) * 255) / (src_gray.cols * src_gray.rows));
	}

	//drawing...histo debug color area
	thickness = 1;
	lineType = LINE_8;


	Mat color_draw = _mMatBuff[eImgDrawColor];
	//draw   |------>
	view_horizontal = false;
	view_vertical = true;
	if (view_horizontal)
	{
		for (size_t row = 0; row < histo_max; row++)
		{
			line(color_draw, Point(0, row * 2), Point(histo[row] / 10, row * 2), Scalar(255, 0, 255), thickness, lineType);
		}
	}

	if (view_vertical)
	{
		int height = src_gray.rows;
		for (size_t col = 0; col < histo_max; col++)
		{
			//draw histo
			Point pt1 = Point(col * 2, height - 1);//start
			Point pt2 = Point(col * 2, height - 1 - histo[col] / 13);//end
			line(color_draw, pt1, pt2, Scalar(0, 255, 128), thickness, lineType);
		}

		int max_cumulate = cumulative_freq[histo_max - 1];
		for (size_t col = 0; col < histo_max; col++)
		{
			if (col == 0)
			{
				int scaled_v = (cumulative_freq[col] / max_cumulate) * height;
				Point pt1 = Point(col * 2, scaled_v);//start
				Point pt2 = Point(col * 2, scaled_v);//end
				line(color_draw, pt1, pt2, Scalar(0, 255, 255), thickness, lineType);
				continue;
			}
			//draw cumulate
			int scaled_v1 = (cumulative_freq[col - 1] * 1.0 / max_cumulate) * height;
			int scaled_v2 = (cumulative_freq[col] * 1.0 / max_cumulate) * height;
			Point pt1 = Point(col * 2, height - scaled_v1);//start
			Point pt2 = Point(col * 2, height - scaled_v2);//end
			line(color_draw, pt1, pt2, Scalar(0, 255, 255), thickness, lineType);

		}
	}

	//SNR
	Mat enhanced_img = src_gray_histoEqual;
	Mat src_img = src_gray;

	Mat abs_diff;
	absdiff(enhanced_img, src_img, abs_diff);
	abs_diff.convertTo(abs_diff, CV_32F);
	abs_diff = abs_diff.mul(abs_diff);

	Scalar s_sum = cv::sum(abs_diff);
	double sum = s_sum.val[0];
	double MSE = 0, PSNR = 0;
	if (sum <= 1e-10)
	{
		MSE = PSNR = 0;
	}
	else
	{
		MSE = sum / (double)(src_img.channels() * src_img.total());
		PSNR = 10.0 * log10((255 * 255) / MSE);
	}
	string msg = "";
	msg = "MSE = " + to_string(MSE) + " PSNR = " + to_string(PSNR);
	AddString(msg.c_str());



	_bShowDebug = _bShowResult = true;

	Invalidate(FALSE);
	return 0;
}*/
	Mat src_gray = _mMatBuff[eImgSrcGray];
	const int maxbit = 256;
	int val[maxbit] = { 0 };

	for (size_t i = 0; i < src_gray.rows * src_gray.cols; i++) {
		val[src_gray.data[i]]++;
	}

	int cumuVal[maxbit] = { 0 };
	int newVal[maxbit] = { 0 };
	int sumVal = 0;
	int enhanVal[maxbit] = { 0 };

	cumuVal[0] = val[0];
	for (size_t i = 0; i < maxbit; i++) {
		sumVal += val[i];
		cumuVal[i] = sumVal;
		newVal[i] = round((((float)cumuVal[i]) * 255) / (src_gray.rows * src_gray.cols));
	}
	src_gray.copyTo(_mMatBuff[eImgDebugGray]);
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	for (size_t i = 0; i < maxbit; i++) {
		line(_mMatBuff[eImgDebugColor], Point(2 * i, src_gray.rows), Point(2 * i, src_gray.rows - (int)(((double)val[i]) / 10)), Scalar(170, 220, 130), 1, LINE_8);
	}
	Mat src_enhance = src_gray.clone();
	for (size_t i = 0; i < src_enhance.rows * src_enhance.cols; i++) {
		src_enhance.data[i] = newVal[src_enhance.data[i]];
	}
	for (size_t i = 0; i < src_gray.rows * src_gray.cols; i++) {
		enhanVal[src_enhance.data[i]]++;
	}
	cvtColor(src_enhance, src_enhance, COLOR_GRAY2BGR);
	src_enhance.copyTo(_mMatBuff[eImgDrawColor]);
	for (size_t i = 0; i < maxbit; i++) {
		line(_mMatBuff[eImgDrawColor], Point(2 * i, _mMatBuff[eImgDrawColor].rows), Point(2 * i, _mMatBuff[eImgDrawColor].rows - (int)(((double)enhanVal[i] / 13))), Scalar(170, 220, 130), 1, LINE_8);
	}

	long long pM = 0;
	long double PSNR = 0, MSE = 0;
	for (size_t i = 0; i < src_gray.rows * src_gray.cols; i++) {
		int diff = src_enhance.data[i]-src_gray.data[i];
		pM += diff*diff;
	}
	int totalind = src_enhance.total();
	MSE = (long double)(pM) / (/*src_enhance.channels() **/ src_enhance.total());
	PSNR = 10.0 * log((255 * 255) / MSE) ;

	ostringstream ss;
	ss << std::fixed;
	ss << " MSE = ";
	ss << std::setprecision(3) << MSE;
	ss << " PSNR = ";
	ss << std::setprecision(3) << PSNR;
	string ssTxt = ss.str();
	AddString(ssTxt.c_str());

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);
	
	return 0;
}

int COpenCVAppGUIDlg::OnInspCheckUsingMorpology()
{
	Mat src_gray = _mMatBuff[eImgSrcGray];
	Mat src_color = _mMatBuff[eImgSrcColor];
	RNG rng(12345);

	Mat thr_img;
	threshold(src_gray, thr_img, 130, 255, THRESH_BINARY_INV);
	src_color.copyTo(_mMatBuff[eImgDebugColor]);

	int kernel_type = MORPH_RECT;
	int kernel_size = 2;
	Mat element = getStructuringElement(kernel_type, Size(2 * kernel_size+1, 2 * kernel_size+1), Point(kernel_size, kernel_size));
	Mat src_bin = thr_img, src_erode, src_dilate;
	erode(src_bin, src_erode, element, Point(-1, -1),1);
	kernel_size = 4;
	element = getStructuringElement(MORPH_ELLIPSE, Size(2 * kernel_size + 1, 2 * kernel_size + 1), Point(kernel_size, kernel_size));
	dilate(src_erode, src_dilate, element, Point(-1, -1),1);
	kernel_size = 3;
	element = getStructuringElement(MORPH_RECT, Size(2 * kernel_size + 1, 2 * kernel_size + 1), Point(kernel_size, kernel_size));
	dilate(src_erode, src_dilate, element, Point(-1, -1), 1);
	kernel_size = 2;
	element = getStructuringElement(MORPH_RECT, Size(2 * kernel_size +1, 2 * kernel_size+1), Point(kernel_size, kernel_size));
	erode(src_dilate, src_dilate, element, Point(-1, -1), 1);
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(src_dilate, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	cvtColor(src_dilate, src_dilate, COLOR_GRAY2BGR);
	
	for (size_t i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		double perimeter = arcLength(contours[i], true);
		double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
		int width = 0,height = 0;
		int max_W = 0, min_W = contours[i][0].x;
		int max_H = 0, min_H = contours[i][0].x;
		Point center;
		if (area < 50)
			continue;
		else {
			for (size_t j = 0; j < contours[i].size(); j++) {
				max_W = max(max_W, contours[i][j].x);
				min_W = min(min_W, contours[i][j].x);		
				max_H = max(max_H, contours[i][j].y);		
				min_H = min(min_H, contours[i][j].y);
			}
			width = max_W - min_W;
			height = max_H - min_H;
			center.x = min_W + (int)((double)width / 2);
			center.y = min_H + (int)((double)height / 2);
			ostringstream ss;
			ss << std::fixed;
			ss << "[";
			ss << to_string(i + 1);
			ss << "]";
			ss << " area = ";
			ss << std::setprecision(3) << area;
			ss << " perimeter = ";
			ss << std::setprecision(3) << perimeter;
			ss << " width = ";
			ss << std::setprecision(3) << width;
			ss << " height = ";
			ss << std::setprecision(3) << height;
			ss << " center = ";
			ss << std::setprecision(3) << center;
			string ssTxt = ss.str();
			AddString(ssTxt.c_str());
			string index_str = "[" + to_string(i + 1) + "]";
			putText(src_dilate, index_str, contours[i][0], cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
			putText(_mMatBuff[eImgDebugColor], index_str, contours[i][0], cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
			Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
			drawContours(src_dilate, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
		}
	}
	src_dilate.copyTo(_mMatBuff[eImgDrawColor]);
	src_dilate.copyTo(_mMatBuff[eImgDebugColor]);

	Mat mask = _mMatBuff[eImgDrawColor].clone();
	Mat drawing = _mMatBuff[eImgDrawColor];
	mask = 0;
	for (size_t i = 0; i < contours.size(); i++){
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(mask, contours, (int)i, color, CV_FILLED, 8, hierarchy);
	}
	drawing = drawing & mask;
	for (size_t i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		string index_str = "[" + to_string(i + 1) + "]";
		if (area < 50)
			continue;
		else
			putText(drawing, index_str, contours[i][0], cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
	}

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspSegmentColor()	//HSV로 도형 분별
{
	Mat src_color = _mMatBuff[eImgSrcColor];
	Mat src_hsv;
	cvtColor(src_color, src_hsv, COLOR_BGR2HSV);

	const int max_value_H = 360 / 2;
	const int max_value = 255;
	int low_H = 80, low_S = 0, low_V = 0;	//red = hue값이 160 이상, green = hue값이 20~60, blue = 80 ~ 110
	int high_H = max_value_H, high_S = max_value, high_V = max_value;
	
	Mat src_hsv_red, src_hsv_green, src_hsv_blue;
	low_H = 160; high_H = 180;
	inRange(src_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), src_hsv_red);
	low_H = 80; high_H = 110;
	inRange(src_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), src_hsv_blue);
	low_H = 20; high_H = 60;
	inRange(src_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), src_hsv_green);
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(src_hsv_blue, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	cvtColor(src_hsv_blue, _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);

	vector<int> circle_index, triangle_index, rectangle_index;

	for(size_t i = 0; i<contours.size();i++){
		double area = contourArea(contours[i]);
		double perimeter = arcLength(contours[i], true);
		double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
		ostringstream ss;
		ss << std::fixed;
		ss << "[";
		ss << to_string(i + 1);
		ss << "]";
		ss << " area = ";
		ss << std::setprecision(3) << area;
		ss << " perimeter = ";
		ss << std::setprecision(3) << perimeter;
		ss << " circularity = ";
		ss << std::setprecision(3) << circularity;
		string ssTxt = ss.str();
		AddString(ssTxt.c_str());
		if (circularity <= 0.7) {
			triangle_index.push_back(i);
			continue;
		}
		else if (circularity > 0.7 && circularity <= 0.8) {
			rectangle_index.push_back(i);
			continue;
		}
		else if (circularity > 0.8) {
			circle_index.push_back(i);
			continue;
		}
	}
	for (size_t i = 0; i < contours.size(); i++) {
		string index_str = "[" + to_string(i + 1) + "]";
		putText(_mMatBuff[eImgDebugColor], index_str, contours[i][0], cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
	}
	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);
	return 0;
}

int COpenCVAppGUIDlg::OnInspFindcontourSample(){
	if (0) {
		Mat src_gray = _mMatBuff[eImgSrcGray];
		int thresh = 128;

		RNG rng(12345);

		//Mat canny_output;
		//Canny(src_gray, canny_output, thresh, thresh * 2);

		Mat thr_img;
		threshold(src_gray, thr_img, thresh, 255, THRESH_BINARY);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(thr_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		//canny_output.copyTo(_mMatBuff[eImgDebugGray]);
		cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
		Mat drawing = _mMatBuff[eImgDebugColor];

		for (size_t i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
			drawContours(drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
		}
		drawing = _mMatBuff[eImgDrawColor];

		for (size_t i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
			drawContours(drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
		}

		int thickness = 5;
		int lineType = LINE_8;
		for (size_t i = 0; i < contours.size(); i++)
		{
			for (size_t k = 0; k < contours[i].size(); k++)
			{
				Point pt = contours[i][k];
				drawing = _mMatBuff[eImgDrawColor];
				line(drawing, Point(pt.x - 10, pt.y - 10), Point(pt.x + 10, pt.y + 10), Scalar(255, 255, 0), thickness, lineType);
				line(drawing, Point(pt.x + 10, pt.y - 10), Point(pt.x - 10, pt.y + 10), Scalar(255, 255, 0), thickness, lineType);
				drawing = _mMatBuff[eImgDebugColor];
				line(drawing, Point(pt.x - 10, pt.y - 10), Point(pt.x + 10, pt.y + 10), Scalar(255, 255, 0), thickness, lineType);
				line(drawing, Point(pt.x + 10, pt.y - 10), Point(pt.x - 10, pt.y + 10), Scalar(255, 255, 0), thickness, lineType);
			}
		}
		//imshow("Contours", drawing);
		_bShowDebug = true;
		_bShowResult = true;
		Invalidate(FALSE);

		return 0;
	}
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 50;

	for (size_t i = 0; i < src_gray.cols* src_gray.rows; i++){
		uchar* pData = src_gray.data;
		if (pData[i] == 91 || pData[i] == 122 || pData[i] == 192)
			continue;
		else
			pData[i] = 0;
	}
	
	Mat thr_img;
	threshold(src_gray, thr_img, 99, 255, THRESH_BINARY);
	Mat img_cir;
	inRange(src_gray, 90, 100, img_cir);
	Mat img_tri;
	inRange(src_gray, 121, 123, img_tri);
	Mat img_rect;
	inRange(src_gray, 190, 200, img_rect);

	vector<vector<Point> > contours1, contours2, contours3;
	vector<Vec4i> hierarchy;

	findContours(img_cir, contours1, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	findContours(img_tri, contours2, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	findContours(img_rect, contours3, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];

	for (size_t i = 0; i < contours1.size(); i++){
		auto minArea = contourArea(contours1[i]);
		if (minArea < 20)
			continue;
		drawContours(drawing, contours1, (int)i, Scalar(20, 60, 195), 2, LINE_8, hierarchy, 0);
	}
	for (size_t i = 0; i < contours2.size(); i++){
		auto minArea = contourArea(contours2[i]);

		if (minArea < 20)
			continue;
		drawContours(drawing, contours2, (int)i, Scalar(200,149,121), 2, LINE_8, hierarchy, 0);
	}
	for (size_t i = 0; i < contours3.size(); i++){
		auto minArea = contourArea(contours3[i]);

		if (minArea < 20)
			continue;
		drawContours(drawing, contours3, (int)i, Scalar(70, 155, 140), 2, LINE_8, hierarchy, 0);
	}

	drawing = _mMatBuff[eImgDrawColor];

	for (size_t i = 0; i < contours1.size(); i++){
		auto minArea = contourArea(contours1[i]);
		if (minArea < 20)
			continue;
		drawContours(drawing, contours1, (int)i, Scalar(20, 60, 195), 2, LINE_8, hierarchy, 0);
	}
	for (size_t i = 0; i < contours2.size(); i++){
		auto minArea = contourArea(contours2[i]);

		if (minArea < 20)
			continue;
		drawContours(drawing, contours2, (int)i, Scalar(200, 149, 121), 2, LINE_8, hierarchy, 0);
	}
	for (size_t i = 0; i < contours3.size(); i++){
		auto minArea = contourArea(contours3[i]);
		if (minArea < 20)
			continue;
		drawContours(drawing, contours3, (int)i, Scalar(70, 155, 140), 2, LINE_8, hierarchy, 0);
	}

	int thickness = 5;
	int lineType = LINE_8;

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);
	return 0;
}

int COpenCVAppGUIDlg::OnInspFindShapes(){
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 50;

	Mat thr_img;
	threshold(src_gray, thr_img, thresh, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(thr_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	int circle_index = 0, triangle_index = 0, rectangle_index = 0;

	for (size_t i = 0; i < contours.size(); i++){
		if (contours[i].size() >= 100 && contours[i].size() <= 150){
			circle_index = i;
			continue;
		}
		else if (contours[i].size() == 4){
			rectangle_index = i;
			continue;
		}
		else if (contours[i].size() >= 150){
			triangle_index = i;
			continue;
		}
	}
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];
	drawing = _mMatBuff[eImgDrawColor];
	
	drawContours(drawing, contours, (int)circle_index, Scalar(0, 0, 255), 2, LINE_8, hierarchy, 0);
	drawContours(drawing, contours, (int)rectangle_index, Scalar(255, 0, 0), 2, LINE_8, hierarchy, 0);
	drawContours(drawing, contours, (int)triangle_index, Scalar(0, 255, 0), 2, LINE_8, hierarchy, 0);
	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspFillCircle()	//색을 기반으로 도형 분별
{
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 150;

	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++){
		uchar* pData = src_gray.data;
		if (pData[i] == 91 || pData[i] == 122 || pData[i] == 192)
			continue;
		else
			pData[i] = 0;
	}

	Mat cir_img;
	inRange(src_gray, 90, 92, cir_img);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(cir_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<int> shape_index;

	for (size_t i = 0; i < contours.size(); i++)
	{
		auto minArea = contourArea(contours[i]);

		if (minArea < 20)
			continue;
		else {
			shape_index.push_back(i);
			continue;
		}
	}
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];
	drawing = _mMatBuff[eImgDrawColor];

	Mat mask = _mMatBuff[eImgDrawColor].clone();
	mask = 0;
	for (size_t i = 0; i < shape_index.size(); i++)
		drawContours(mask, contours, shape_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	drawing = drawing & mask;

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspFillTriangle(){
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 50;

	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++)
	{
		uchar* pData = src_gray.data;
		if (pData[i] == 91 || pData[i] == 122 || pData[i] == 192)
			continue;
		else
			pData[i] = 0;
	}

	Mat tri_img;
	inRange(src_gray, 120, 122, tri_img);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(tri_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<int> shape_index;

	for (size_t i = 0; i < contours.size(); i++){
		auto minArea = contourArea(contours[i]);

		if (minArea < 20)
			continue;
		else {
			shape_index.push_back(i);
			continue;
		}
	}
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];
	drawing = _mMatBuff[eImgDrawColor];

	Mat mask = _mMatBuff[eImgDrawColor].clone();
	mask = 0;
	for(size_t i = 0; i< shape_index.size();i++)
		drawContours(mask, contours, shape_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	drawing = drawing & mask;

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspFillRect(){
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 50;

	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++){
		uchar* pData = src_gray.data;
		if (pData[i] == 91 || pData[i] == 122 || pData[i] == 192)
			continue;
		else
			pData[i] = 0;
	}

	Mat rect_img;
	inRange(src_gray, 190, 192, rect_img);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(rect_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<int> shape_index;

	for (size_t i = 0; i < contours.size(); i++){
		auto minArea = contourArea(contours[i]);

		if (minArea < 20)
			continue;
		else {
			shape_index.push_back(i);
			continue;
		}
	}
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];
	drawing = _mMatBuff[eImgDrawColor];

	Mat mask = _mMatBuff[eImgDrawColor].clone();
	mask = 0;
	for (size_t i = 0; i < shape_index.size(); i++)
		drawContours(mask, contours, shape_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	drawing = drawing & mask;

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspFillRectTri()
{
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 50;

	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++)
	{
		uchar* pData = src_gray.data;
		if (pData[i] == 91 || pData[i] == 122 || pData[i] == 192)
			continue;
		else
			pData[i] = 0;
	}

	Mat shape_img;
	inRange(src_gray, 120, 192, shape_img);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(shape_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<int> shape_index;

	for (size_t i = 0; i < contours.size(); i++)
	{
		auto minArea = contourArea(contours[i]);

		if (minArea < 20)
			continue;
		else {
			shape_index.push_back(i);
			continue;
		}
	}
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];
	drawing = _mMatBuff[eImgDrawColor];

	Mat mask = _mMatBuff[eImgDrawColor].clone();
	mask = 0;
	for (size_t i = 0; i < shape_index.size(); i++)
		drawContours(mask, contours, shape_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	drawing = drawing & mask;

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspArea() //feature기반으로 3가지 도형 분별 -> Circularity 값으로 삼각형,사각형,원을 나눔
{
	Mat src_gray = _mMatBuff[eImgSrcGray];
	int thresh = 50;
	for (size_t i = 0; i < src_gray.cols * src_gray.rows; i++){
		uchar* pData = src_gray.data;
		if (pData[i] == 91 || pData[i] == 122 || pData[i] == 192)
			continue;
		else
			pData[i] = 0;
	}
	Mat thr_img;
	threshold(src_gray, thr_img, thresh, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(thr_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<int> circle_index, triangle_index, rectangle_index;
	for (size_t i = 0; i < contours.size(); i++){
		double area = contourArea(contours[i]);
		double perimeter = arcLength(contours[i],true);
		double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
		ostringstream ss;
		ss << std::fixed;
		ss << "[";
		ss << to_string(i + 1);
		ss << "]";
		ss << " area = ";
		ss << std::setprecision(3) << area;
		ss << " perimeter = ";
		ss << std::setprecision(3) << perimeter;
		ss << " circularity = ";
		ss << std::setprecision(3) << circularity;
		string ssTxt = ss.str();
		AddString(ssTxt.c_str());
		if (circularity <= 0.7){
			triangle_index.push_back(i);
			continue;
		}
		else if (circularity > 0.7  && circularity <=0.8){
			rectangle_index.push_back(i);
			continue;
		}
		else if (circularity > 0.8){
			circle_index.push_back(i);
			continue;
		}
	}
	cvtColor(_mMatBuff[eImgDebugGray], _mMatBuff[eImgDebugColor], COLOR_GRAY2BGR);
	Mat drawing = _mMatBuff[eImgDebugColor];
	drawing = _mMatBuff[eImgDrawColor];
	drawing.copyTo(_mMatBuff[eImgDebugColor]);
	for (size_t i = 0; i < contours.size(); i++) {
		string index_str = "[" + to_string(i+1) + "]";
		putText(_mMatBuff[eImgDebugColor], index_str, contours[i][0], cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
	}

	Mat mask = _mMatBuff[eImgDrawColor].clone();
	mask = 0;
//	for (size_t i = 0; i < rectangle_index.size(); i++)
//		drawContours(mask, contours, rectangle_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
//	for (size_t i = 0; i < triangle_index.size(); i++)
//		drawContours(mask, contours, triangle_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	for (size_t i = 0; i < circle_index.size(); i++)
		drawContours(mask, contours, circle_index[i], Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	drawing = drawing & mask;

	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);
	return 0;
}

int COpenCVAppGUIDlg::OnInspMorpology()
{
	/*Mat src_color = _mMatBuff[eImgSrcColor];
	Mat src_hsv;
	cvtColor(src_color, src_hsv, COLOR_BGR2HSV);

	const int max_value_H = 360 / 2;
	const int max_value = 255;
	int low_H = 80, low_S = 0, low_V = 0;	//red = hue값이 160 이상, green = hue값이 20~60, blue = 80 ~ 110
	int high_H = max_value_H, high_S = max_value, high_V = max_value;

	Mat src_hsv_red, src_hsv_green, src_hsv_blue;
//	low_H = 160; high_H = 180;
//	inRange(src_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), src_hsv_red);
	low_H = 80; high_H = 110;
	inRange(src_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), src_hsv_blue);
//	low_H = 20; high_H = 60;
//	inRange(src_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), src_hsv_green);
*/
	Mat src_gray = _mMatBuff[eImgSrcGray];
	resize(src_gray, src_gray, Size(src_gray.cols * 2, src_gray.rows * 2));
	Mat src_inv = ~src_gray;
	threshold(src_inv, src_inv, 120, 255, THRESH_BINARY);
	int kernel_type = MORPH_CROSS;		//MORPH_CROSS
	//	0	0	0			0				0	0
	//	0	0	0		0	0	0		0	0	0
	//	0	0	0			0			0	0

	int kernel_size = 1;
	Mat element = getStructuringElement(kernel_type, Size(2 * kernel_size , 2 * kernel_size), Point(kernel_size, kernel_size));
	Mat src_bin = src_inv, src_erode, src_dilate;
	erode(src_bin, src_erode, element);
	//Dilation -> 팽창, Erode -> 침식
	//Mat diff = src_bin - src_erode;	//edge filter효과 같이 경계선(1pixel짜리)만 남음 
	Mat result = ~src_erode;
	resize(result, result, Size((int)(src_gray.cols / 2), (int)(src_gray.rows / 2)));
	result.copyTo(_mMatBuff[eImgDrawColor]);
//	src_bin = src_color;
//	dilate(src_bin, src_dilate, element);
	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);

	return 0;
}

int COpenCVAppGUIDlg::OnInspection(uchar* pSrc, size_t cols, size_t rows, uchar* pDst)
{
	for (size_t row = 0; row < rows; row++){
		for(size_t col = 0; col <cols; col++){
			int i = (row * cols + col);
			pSrc[i] > 120 ? pDst[i] = 255 : pDst[i] = 0;
		}
	}
	for (size_t row = 0; row < rows; row++) {
		for (size_t col = 0; col < cols; col++) {
			int i = (row * cols + col);
			if (pDst[i] != pDst[i + 1] && i < rows * cols - 1) {
				_vLinePoints.push_back(Point(col, row));
			}
		}
	}
	return 0;
}

void COpenCVAppGUIDlg::OnBnClickedBtnInspectioncv()
{
	auto f = _mInsps["OnInspMatching"];
	auto ret = f(this);
}

void COpenCVAppGUIDlg::OnBnClickedBtnSamplecode()
{
	vector<Point> contour;	//single object
	vector<vector<Point>> contours;		//multiple objects

	//contour #1 ..... object1
	contour.clear();
	contour.push_back(Point(10, 10));
	contour.push_back(Point(10, 20));
	contour.push_back(Point(20, 10));
	contour.push_back(Point(20, 20));
	contours.push_back(contour);

	//contour #2..... object2
	contour.clear();	
	contour.push_back(Point(110, 110));
	contour.push_back(Point(110, 120));
	contour.push_back(Point(120, 110));
	contour.push_back(Point(120, 120));
	contours.push_back(contour);

	Point a = contours[0][0];
	Point b = contours[1][0];
}

LRESULT COpenCVAppGUIDlg::OnAddString(WPARAM wParam, LPARAM lParam)
{
	if (!GetSafeHwnd()) return 0;

	if (_ListBoxLog.GetCount() > 500)
	{
		_ListBoxLog.ResetContent();
	}

	LPCTSTR lpszLog = reinterpret_cast <LPCTSTR> (lParam);
	SYSTEMTIME t;
	GetLocalTime(&t);
	CString nStrMsg = _T("");
	nStrMsg += lpszLog;
	int nIdx = _ListBoxLog.AddString(nStrMsg);
	_ListBoxLog.SetTopIndex(_ListBoxLog.GetCount() - 1);

	stringstream ssTime;
	time_t const now_c = time(NULL);
	//ssTime << put_time(localtime(&now_c), "%a %d %b %Y - %I_%M_%S%p");
	ssTime << put_time(localtime(&now_c), "%a %d %b %Y-%I_%M");
	string time_c = ssTime.str();

	ofstream file;
	string fileName;
	fileName += "log";
	fileName += ssTime.str();
	fileName += ".txt";

	file.open(fileName, ios::out | ios::app);
	file << nStrMsg << endl;
	file.close();

	return 0;
}

void COpenCVAppGUIDlg::AddString(LPCSTR lpszLog)
{
	SendMessage(WM_ADD_STRING, 0, (LPARAM)lpszLog);
}
