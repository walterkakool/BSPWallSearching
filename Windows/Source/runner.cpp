// BSPMaze.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "bsp_maze.h"

#include <stdio.h>
#include <time.h>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <iomanip>
#include <typeinfo>
#include <assert.h>   
#include <list>
#include <string>
#include <cmath>
#include <iterator>
#include <cstddef>
#include <array>
#include <math.h>
#include <type_traits>
#include <cstdint>
#include <boost/intrusive/rbtree.hpp>
#include <boost/multi_array.hpp>
#include <exception>
#include <typeinfo>
#include <functional>
#include <stdexcept>
#include <memory>
#include <algorithm>  
#include <utility>
#include <set>
#include <numbers>
#include <complex>
#include <numbers>
#include <iterator> 
#include <condition_variable> 
#include <thread>         
#include <mutex>  
#include <cstdlib>
#include <fstream>
#include <d2d1.h>

#pragma comment(lib, "d2d1")

#include "components.hpp"
#include "program_properties.cpp"

#define MAX_LOADSTRING 128
#define DWORD_MAX      4294967295 

#define _USE_MATH_DEFINES
//Abandoned personal constant
//#define MAX_BUFF_SIZE 10240

using namespace std;
using namespace BSP;
using namespace Common;
using namespace ProgramProperties;

int Common::Location::north_wall = 0;
int Common::Location::south_wall = 0;
int Common::Location::east_wall  = 0;
int Common::Location::west_wall  = 0;

std::complex<int> Location::north_wall_point(0, 0);
std::complex<int> Location::south_wall_point(0, 0);
std::complex<int> Location::east_wall_point(0, 0);
std::complex<int> Location::west_wall_point(0, 0);

std::complex<int> Location::point(0, 0);


// Global Variables:
HINSTANCE        hInst;                                // current instance
WCHAR            szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR            szWindowClass[MAX_LOADSTRING];            // the main window class name
StateProperties* pStateProperties;
HANDLE           hDIB;
char*            lpBmp;
BITMAPINFOHEADER bi;
DWORD            dwBmpSize;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//For F2 (reading file)
HANDLE              hFile;
OVERLAPPED          lpOverlapped;
RECT                rc;
  
VOID CALLBACK AsynRoutine(
                            __in  DWORD dwErrorCode,
                            __in  DWORD dwNumberOfBytesTransfered,
                            __in  LPOVERLAPPED lpOverlapped
);

VOID CALLBACK refresh(
                       LPVOID lpArg,               // Data value
                       DWORD dwTimerLowValue,      // Timer low value
                       DWORD dwTimerHighValue     // Timer high value
);

VOID CALLBACK AsynRoutine(
                                __in  DWORD dwErrorCode,
                                __in  DWORD transferred,
                                __in  LPOVERLAPPED lpOverlapped ) {
    
    if(pStateProperties) {  //Does nothing if it's NULL
    
        if(transferred) {

            if( lpOverlapped->Offset+transferred < lpOverlapped->Offset ){
            
                ++lpOverlapped->OffsetHigh;
                  lpOverlapped->Offset = lpOverlapped->OffsetHigh
                                                      ? 
                                                      transferred + lpOverlapped->Offset
                                                      :
                                                      0;
            }
            else lpOverlapped->Offset += transferred;

        }
        else {
        
            pStateProperties->itrCounts,
            lpOverlapped->OffsetHigh, 
            lpOverlapped->Offset            = 0;        //No transferred 

            rstIterator(*pStateProperties);
        }
                 

    }
    
}

VOID CALLBACK refresh(
                       LPVOID lpArg,               // Data value
                       DWORD dwTimerLowValue,      // Timer low value
                       DWORD dwTimerHighValue) {     // Timer high value

    
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BSPMAZE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BSPMAZE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BSPMAZE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BSPMAZE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){

   hInst            = hInstance; // Store instance handle in our global variable
   lpOverlapped     = {0};
   dwBmpSize        = 0;
   hDIB             = NULL;
   lpBmp            = NULL;
   hFile            = NULL;


   Common::Maze::load_maze("");

   pStateProperties = new (std::nothrow) StateProperties;
   ProgramProperties::initPropertiesStates( *pStateProperties );
   ProgramProperties::initPropertiesF2( *pStateProperties, MAX_LOADSTRING );
   ProgramProperties::rstIterator( *pStateProperties ); 
   ProgramProperties::rstMv( *pStateProperties ); 
   ProgramProperties::rstTxt( *pStateProperties ); 

   HWND hWnd = CreateWindowEx( 
                                0,                      // no extended styles 
                                szWindowClass,           // scroll bar control class 
                                szTitle,                // no window text 
                                WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,   // window styles 
                                SM_CXSCREEN - 7,         // leftmost horizontal position 
                                SM_CYSCREEN - 1,         // topest vertical position 
                                GetSystemMetrics( SM_CXSCREEN  ) + 14,   // full-screen width 
                                GetSystemMetrics( SM_CYSCREEN  ) - 33,   // full-screen height 
                                (HWND) NULL,           // no parent for overlapped windows 
                                (HMENU) NULL,          // use the window class menu 
                                hInst,               // global instance handle  
                                nullptr           // program pointer 
   );


   SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties);

   if (!hWnd)
   {
      return FALSE;
   }
   
   GetClientRect(hWnd, &rc);
   /*
   fullSrn.left   = 0;
   fullSrn.top    = 0;
   fullSrn.right  = rc.right;
   fullSrn.bottom = rc.bottom;
   */
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    ID2D1Factory*              pFactory                  = NULL;
    ID2D1HwndRenderTarget*     pRenderTarget             = NULL;
    ID2D1BitmapRenderTarget*   pBitmapRenderTarget       = NULL;
    ID2D1SolidColorBrush*      pBrush                    = NULL;
    
    ID2D1Bitmap*               pMaze                     = NULL; 

    HDC                        hdc      = NULL;
    HDC                        hdcMemDC = NULL;
    HDC                        hdcMem   = NULL;
    HBITMAP                    hBmp     = NULL;
    BITMAP                     bmp;


    PAINTSTRUCT                ps;
    HRESULT                    hr                        = S_OK;
    RECT                       fullRc;
    RECT                       curRc;
    D2D1_RECT_F                rectangle;
    D2D1_SIZE_U                paint_sz;

    //BITMAPFILEHEADER           bmfHeader;


    D2D1_RECT_F                full_rectangle = D2D1::RectF( 
                                                            0, 
                                                            0, 
                                                            GetSystemMetrics( SM_CXSCREEN ), 
                                                            GetSystemMetrics( SM_CYSCREEN ) 
                                                );

    D2D1_RECT_F                r_rectangle = D2D1::RectF( 100, 100, 250, 250 );

    LONG_PTR                   ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
    StateProperties*           pStateProperties = reinterpret_cast<StateProperties*>(ptr);

    TCHAR                      scroll_bar[]     = L"Scroll bar called";
    TCHAR                      tcharBuff[ MAX_LOADSTRING ] = { '\0' };
    string                     strBuff          = "";
    char                       fMaze[ MAX_LOADSTRING ] = { '\0' };

    curRc.left   = 0;
    curRc.top    = 0;
    curRc.right  = 0;
    curRc.bottom = 0;


    std::function<void()> rfScreen = [&hWnd, &curRc, &pStateProperties] () { 

        curRc.left    = pStateProperties->txtL;
        curRc.top     = pStateProperties->txtTop;
        curRc.right   = pStateProperties->txtR;
        curRc.bottom  = pStateProperties->txtBottom;

        //InvalidateRect( hWnd, &curRc, TRUE);
        InvalidateRect( hWnd, NULL, TRUE);
        UpdateWindow( hWnd );
    };

    std::function<void(int,int,int,int)> set_rectangle_location=[&rectangle]( 
                                                                              int left, 
                                                                              int top,
                                                                              int right,
                                                                              int bottom ) { 

        rectangle = D2D1::RectF( left, top, right, bottom );
    };  
   
    
    std::function<void()> loadTcharBuff = [ &strBuff, &tcharBuff ]() {
        
        for ( int i = 0; i< strBuff.length(); ++i  ) tcharBuff [ i ] = strBuff [ i ];
    };

    std::function<void()> read_maze = [ &fMaze, &tcharBuff ]() {
        
        for ( int i = 0; i< MAX_LOADSTRING; ++i  ) tcharBuff [ i ] = fMaze [ i ];
    };

    std::function<void( D2D1::ColorF )> colorBrush =
        [ &pRenderTarget, &pBrush ] ( D2D1_COLOR_F color) { 

            pRenderTarget->CreateSolidColorBrush( color,&pBrush );
    };

    std::function<void()> srllScreen = [ &hWnd ] () {  //scroll screen
        
        ScrollWindowEx(
                        hWnd, 
                        000, 
                        -(GetSystemMetrics( SM_CYSCREEN  ) - 35), 
                        (CONST RECT *) NULL, 
                        (CONST RECT *) NULL, 
                        (HRGN) NULL, 
                        (PRECT) NULL, 
                        SW_INVALIDATE
        ); 


    };    


    
    std::function<void()> clnField=[    //clean field
                                        &pStateProperties,
                                        &pRenderTarget, 
                                        &rectangle, 
                                        &pBrush, 
                                        &set_rectangle_location, 
                                        &colorBrush,
                                        &hWnd,
                                        &fullRc,
                                        &paint_sz,
                                        &pFactory]() {
                                            
                                            GetClientRect(hWnd, &fullRc);
                                            paint_sz = D2D1::SizeU( fullRc.right, fullRc.bottom );

                                            D2D1CreateFactory(
                                                                D2D1_FACTORY_TYPE_SINGLE_THREADED, 
                                                                &pFactory
                                            );         

                                            //pRenderTarget will hold the address
                                            pFactory->CreateHwndRenderTarget ( 
                                                            D2D1::RenderTargetProperties(),  
                                                            D2D1::HwndRenderTargetProperties(
                                                                                                hWnd, 
                                                                                                paint_sz
                                                            ), 
                                                            &pRenderTarget
                                            );

                                            pRenderTarget->BeginDraw();
                                            pRenderTarget->Clear(D2D1::ColorF( D2D1::ColorF::White )); 
                                            pRenderTarget->EndDraw();

                                            pFactory->Release();
                                            pFactory = NULL;
                                            pRenderTarget->Release();
                                            pRenderTarget = NULL;
    };

    std::function<void( std::complex<int>,std::complex<int>, std::complex<int>,std::complex<int> )> 
        setBitmap = [ 
                        &pStateProperties,
                        &pBitmapRenderTarget, 
                        &pMaze,
                        &rectangle, 
                        &pBrush, 
                        &set_rectangle_location, 
                        &colorBrush,
                        &hWnd,
                        &fullRc,
                        &paint_sz,
                        &pFactory] ( 
                                    std::complex<int> n, 
                                    std::complex<int> s, 
                                    std::complex<int> w, 
                                    std::complex<int> e ) {         

        pBitmapRenderTarget->BeginDraw(); 
        pBitmapRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White ) ); 
        /*
        pBitmapRenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( 
                                                                    pStateProperties->mvX,
                                                                    pStateProperties->mvY                   
                                     )
        );
        */
        
        for(int i = 0; i < rc.bottom/15; ++i)
            for(int j = 0; j < rc.right/15; ++j){
            
                set_rectangle_location( 
                                        j*15, 
                                        i*15, 
                                        j*15 + 15, 
                                        i*15 + 15 
                );                


                if( Common::Maze::maze[i][j] ){
                
                    pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &pBrush );

                    pBitmapRenderTarget->FillRectangle( rectangle, pBrush);   

                    pBrush->Release();
                    pBrush = NULL;
                }
            
            }
            /*
                set_rectangle_location( 
                                        0*10, 
                                        0*10, 
                                        0*10 + 15, 
                                        0*10 + 15 
                );   

                    pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &pBrush );

                    pBitmapRenderTarget->FillRectangle( rectangle, pBrush);   

                    pBrush->Release();
                    pBrush = NULL;

                set_rectangle_location( 
                                        1*15, 
                                        1*15, 
                                        1*15 + 15, 
                                        1*15 + 15 
                );   

                    pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &pBrush );

                    pBitmapRenderTarget->FillRectangle( rectangle, pBrush);   

                    pBrush->Release();
                    pBrush = NULL;
              */      
        /*
        //Filling north
        set_rectangle_location( 
                                Common::Maze::maze[][], 
                                rc.bottom - n.imag(), 
                                n.real() + 15, 
                                rc.bottom - (n.imag() - 15) 
        );

        pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Red ), &pBrush );
        pBitmapRenderTarget->FillRectangle( rectangle, pBrush);
        
        pBrush->Release();
        pBrush = NULL;
        
        //Filling south
        set_rectangle_location( 
                                s.real(), 
                                rc.bottom - s.imag(), 
                                s.real() + 15, 
                                rc.bottom - (s.imag() - 15) 
        );

        pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Blue ), &pBrush );
        pBitmapRenderTarget->FillRectangle( rectangle, pBrush);
        
        pBrush->Release();
        pBrush = NULL;
        
        //Filling west
        set_rectangle_location( 
                                w.real(), 
                                rc.bottom - w.imag(), 
                                w.real() + 15, 
                                rc.bottom - (w.imag() - 15)
        );

        pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Brown ), &pBrush );
        pBitmapRenderTarget->FillRectangle( rectangle, pBrush );
        
        pBrush->Release();
        pBrush = NULL;
        
        //Fill east
        set_rectangle_location( 
                                e.real(), 
                                rc.bottom - e.imag(), 
                                e.real() + 15, 
                                rc.bottom - (e.imag() - 15) 
        );

        pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Green ), &pBrush );
        pBitmapRenderTarget->FillRectangle( rectangle, pBrush );

        pBitmapRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        */
        pBitmapRenderTarget->EndDraw();
        
        //pBrush->Release();
        //pBrush = NULL;

        pBitmapRenderTarget->GetBitmap(&pMaze);
    };

    std::function<void( std::complex<int>,std::complex<int>, std::complex<int>,std::complex<int> )> 
        render4walls = [ 
                        &pStateProperties,
                        &pRenderTarget, 
                        &rectangle, 
                        &pBrush, 
                        &pMaze,
                        &set_rectangle_location, 
                        &colorBrush,
                        &hWnd,
                        &fullRc,
                        &paint_sz,
                        &pFactory] ( 
                                    std::complex<int> n, 
                                    std::complex<int> s, 
                                    std::complex<int> w, 
                                    std::complex<int> e ) {         

        pRenderTarget->BeginDraw();
        pRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White ) ); 
        /*
        pRenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( 
                                                                    pStateProperties->mvX,
                                                                    pStateProperties->mvY                   
                                     )
        );
        */
        pRenderTarget->DrawBitmap(
                                pMaze,
                                D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom),
                                1.0,
                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR

        );

        pRenderTarget->EndDraw();
        
    };

     std::function<void( )> loadTimeStrBuff=[ &strBuff]() {    

        SYSTEMTIME local_time_obj;
        GetLocalTime(&local_time_obj);

        strBuff += "----"; 
        strBuff += to_string( local_time_obj.wHour );
        strBuff += " : ";
        strBuff += to_string( local_time_obj.wMinute );
        strBuff += " : ";
        strBuff += to_string( local_time_obj.wSecond );
        strBuff += "---\n";
     }; 

     std::function<void( )> loadMaze=[ 
                                       &hWnd,
                                       &strBuff, 
                                       &tcharBuff, 
                                       &pStateProperties,
                                       &fMaze,
                                       clnField, 
                                       srllScreen]() {

                    for ( int i = 0; 1; ++i ) {

                        tcharBuff [i] = pStateProperties->f2path [i];

                        if ( !pStateProperties->f2path [i] ) {
                            
                            for ( int j = 0; strBuff[j]; ++i, ++j ) {
                                
                                if( i < MAX_LOADSTRING ) tcharBuff [i] = strBuff[j];
                                else {
                                
                                    pStateProperties->path2long = true;
                                    goto openFile;
                                }

                            }

                            goto openFile;
                        }
                    }
                    
                    openFile:
                    
                        hFile = CreateFile(
                                            tcharBuff,                  // file to open
                                            GENERIC_READ,               // open for reading
                                            FILE_SHARE_READ,            // share for reading
                                            NULL,                       // default security
                                            OPEN_EXISTING,              // existing file only
                                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                                            NULL                        // no attr. template
                                );            

                    for ( int i = 0; i < MAX_LOADSTRING; ++i ) fMaze [i] = '\0';

                    if ( ReadFileEx( hFile, fMaze, MAX_LOADSTRING - 1, &lpOverlapped, AsynRoutine)){

                        SleepEx(INFINITE, TRUE);    //Sleep for AsynRoutine
                        initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );

                        for (int i=0; i<MAX_LOADSTRING; ++i) pStateProperties->f2Mssg[i] = fMaze[i]; 

                        CloseHandle(hFile);         //next open will start at the offset

                        //Cuz BitBlt scans from the bottom, stores the content in a reverse order
                        for ( int i = 0; i < MAX_LOADSTRING-1; ++i )
                            Common::Maze::maze[ rc.bottom/15 - pStateProperties->itrCounts - 1 ][i] = 
                                pStateProperties->f2Mssg[i] == '1'
                                                ?
                                                1
                                                :
                                                0;
                        Common::Maze::maze[ rc.bottom - pStateProperties->itrCounts ][MAX_LOADSTRING-1] = 1;

                                                                                  
                    }
                    else {                   

                        initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );
                        CloseHandle(hFile);    
                    }
                    
     };
     
     std::function<void( )> outMaze=[ 
                                       &hWnd,
                                       &strBuff, 
                                       &tcharBuff, 
                                       &pStateProperties,
                                       &fMaze,
                                       clnField, 
                                       srllScreen
                            ]() {

                    for ( int i = 0; 1; ++i ) {

                        tcharBuff [i] = pStateProperties->f2path [i];

                        if ( !pStateProperties->f2path [i] ) {
                            
                            for ( int j = 0; strBuff[j]; ++i, ++j ) {
                                
                                if( i < MAX_LOADSTRING ) tcharBuff [i] = strBuff[j];
                                else {
                                
                                    pStateProperties->path2long = true;
                                    goto mkFile;
                                }

                            }

                            goto mkFile;
                        }
                    }
                    
                    mkFile:

                        hFile = CreateFile(

                                            tcharBuff,                // file to open
                                            GENERIC_WRITE,               // open for reading
                                            FILE_SHARE_WRITE,            // share for reading
                                            NULL,                       // default security
                                            CREATE_ALWAYS,              // existing file only
                                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                                            NULL                        // no attr. template
                        );            

                    for ( int i = 0; i < MAX_LOADSTRING; ++i ) fMaze [i] = '\0';

                    if ( WriteFile( hFile, (LPSTR)lpBmp, dwBmpSize, NULL, &lpOverlapped)){

                        SleepEx(INFINITE, TRUE);    //Sleep for AsynRoutine
                        CloseHandle(hFile);         //next open will start at the offset
                    }
                    else {                   

                        //initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );
                        CloseHandle(hFile);    
                    }
                    
     };
    
    switch (message) {

        case WM_COMMAND: {

            int wmId = LOWORD(wParam);
            // Parse the menu selections:
                switch (wmId) {

                    case IDM_ABOUT:{
                    
                        DialogBox(
                                    hInst, 
                                    MAKEINTRESOURCE(IDD_ABOUTBOX), 
                                    hWnd, 
                                    About
                        );

                        break;                    
                    }

                    case IDM_EXIT:{
                    
                        DestroyWindow(hWnd);
                        delete(pStateProperties);

                        break;                    
                    }

                    default:{

                        return DefWindowProc(
                                                hWnd, 
                                                message, 
                                                wParam, 
                                                lParam
                        );

                        break;
                    }

                }

            break;
        }

        case WM_PAINT: {  //For printing context
             
            if( hDIB && !pStateProperties->isF4 ){
            
                pStateProperties->isF4 = false;

                GlobalUnlock(hDIB);
                GlobalFree(hDIB);

                hDIB = NULL;
            }

            if( pStateProperties->isF4 ){
            

                break;
            }
                
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED , &pFactory);

            pFactory->CreateHwndRenderTarget(
                    D2D1::RenderTargetProperties(),  
                    D2D1::HwndRenderTargetProperties(
                                                        hWnd, 
                                                        D2D1::SizeU(rc.right, rc.bottom)
                    ), 
                    &pRenderTarget
            );


            if( pStateProperties->started ) goto donePaint;

            pStateProperties->started = true;
           
            pRenderTarget->CreateCompatibleRenderTarget(
                                                        D2D1::SizeF(rc.right, rc.bottom),  
                                                        &pBitmapRenderTarget
            );

            hdc = BeginPaint(hWnd, &ps );            
            /*

            setBitmap( 
                std::complex<int>(200, rc.bottom + 0), 
                std::complex<int>(200, rc.bottom + 50), 
                std::complex<int>(175, rc.bottom + 25), 
                std::complex<int>(225, rc.bottom + 25) 
            );
            */

            setBitmap( 
                        std::complex<int>(rc.right, rc.bottom + 0), 
                        std::complex<int>(rc.right, rc.bottom + 50), 
                        std::complex<int>(rc.right-25, rc.bottom + 25), 
                        std::complex<int>(rc.right+25, rc.bottom + 25) 
            );

            render4walls( 
                        std::complex<int>(50, 50), 
                        std::complex<int>(50, 100), 
                        std::complex<int>(25, 75), 
                        std::complex<int>(75, 75) 
            );
            
            loadTimeStrBuff();
            loadTcharBuff();          
            TextOut(hdc,0, 50, tcharBuff, strBuff.length() );
            
            strBuff = std::to_string( pStateProperties->itrCounts );
            //strBuff = std::to_string( pMaze->GetPixelFormat().format );
            loadTcharBuff();          
            TextOut(hdc,0, 270 , tcharBuff, strBuff.length() ); 

            strBuff = std::to_string( rc.right );
            //strBuff = std::to_string( pMaze->GetPixelFormat().format );
            loadTcharBuff();          
            TextOut(hdc,50, 270 , tcharBuff, strBuff.length() ); 

            strBuff = std::to_string( rc.bottom );
            //strBuff = std::to_string( pMaze->GetPixelFormat().format );
            loadTcharBuff();          
            TextOut(hdc,100, 270 , tcharBuff, strBuff.length() ); 
            
        
            strBuff = pStateProperties->f2Mssg;
            loadTcharBuff();    
            
            TextOut(
                    hdc,
                    pStateProperties->txtL, 
                    pStateProperties->txtTop,
                    tcharBuff, 
                    strBuff.length()
            );

            DeleteDC(hdc);
            hdc = NULL;
            EndPaint(hWnd, &ps );


            pStateProperties->started = false;

            donePaint:

                pFactory->Release();
                pFactory = NULL;
                pRenderTarget->Release();
                pRenderTarget = NULL;
                
                pBitmapRenderTarget->Release();
                pBitmapRenderTarget = NULL;
                pMaze->Release();
                pMaze = NULL;
            
            //srllScreen();

            loadGlobalBuff:

                hdc = GetDC( hWnd );
                hdcMem = CreateCompatibleDC( hdc );

                hBmp   = CreateCompatibleBitmap( 
                                                hdc,
                                                rc.right,
                                                rc.bottom
                         );

                SelectObject( hdcMem, hBmp );
                
                if (!BitBlt(hdcMem,0 , 0,  rc.right,rc.bottom, hdc, 0, 0,SRCCOPY) )
                    goto loadF2;
                
                GetObject(hBmp, sizeof(BITMAP), &bmp);

                bi.biSize                  = sizeof(BITMAPINFOHEADER);
                bi.biWidth                 = bmp.bmWidth;
                bi.biHeight                = bmp.bmHeight;
                bi.biPlanes                = 1;
                bi.biBitCount              = 32;
                bi.biCompression           = BI_RGB;
                bi.biSizeImage             = 0;
                bi.biXPelsPerMeter         = 0;
                bi.biYPelsPerMeter         = 0;
                bi.biClrUsed               = 0;
                bi.biClrImportant          = 0;
            
                //dwBmpSize = ( (bi.biWidth * bi.biBitCount + 31) / 32) * 4 * bi.biHeight;
                dwBmpSize = (bi.biWidth  ) * 4 * bi.biHeight;

                hDIB  = GlobalAlloc(GHND, dwBmpSize);
                lpBmp = (char*)GlobalLock(hDIB);

                GetDIBits(
                            hdc,
                            hBmp,
                            0,
                            (UINT)rc.bottom,
                            lpBmp,
                            (BITMAPINFO*)&bi, 
                            DIB_RGB_COLORS
                );

                ReleaseDC(hWnd, hdc);
                DeleteDC(hdc);
                DeleteObject( hBmp );
                DeleteDC(hdcMem);

                hdc = NULL;

                if( lpOverlapped.OffsetHigh || lpOverlapped.Offset ) goto loadF2;
                //InvalidateRect(hWnd,NULL,TRUE);

                break;
        }      

        case WM_VSCROLL: {

            srllScreen();

            break;
        }
    
        case WM_KEYDOWN:{

            switch( wParam ) {
                
                default:
                    break;

                case VK_UP:{

                    if(pStateProperties->started) goto doneUp;

                        pStateProperties->started = true;
                        pStateProperties->mvY -= 15;
                    
                        //pStateProperties->started = true;

                        loadTimeStrBuff();
                        loadTcharBuff();          

                        pStateProperties->started = false;
                        InvalidateRect( hWnd, NULL, TRUE );
                        UpdateWindow( hWnd );

                    doneUp:
                        break;                
                }

                case VK_DOWN:{

                    if(pStateProperties->started) goto doneDown;

                        pStateProperties->started = true;
                        pStateProperties->mvY += 15;

                        loadTimeStrBuff();
                        loadTcharBuff();          

                        pStateProperties->started = false;
                        InvalidateRect( hWnd, NULL, TRUE );
                        UpdateWindow( hWnd );

                    doneDown:
                        break;

                }

                case VK_LEFT:{

                    if(pStateProperties->started) goto doneLeft;

                        pStateProperties->started = true;
                        pStateProperties->mvX -= 15;

                        loadTimeStrBuff();
                        loadTcharBuff();      

                        pStateProperties->started = false;
                        InvalidateRect( hWnd, NULL, TRUE );
                        UpdateWindow( hWnd );

                    doneLeft:
                        break;                
                }

                case VK_RIGHT:{

                    if(pStateProperties->started) goto doneRight;

                        pStateProperties->started = true;
                        pStateProperties->mvX += 15;

                        loadTimeStrBuff();
                        loadTcharBuff();      

                        pStateProperties->started = false;
                        InvalidateRect( hWnd, NULL, TRUE );
                        UpdateWindow( hWnd );

                    doneRight:
                        break;                
                }

                //loads maze.pnm
                case VK_F2:{
                
                    loadF2:

                        if(pStateProperties->started) goto doneF2;

                        pStateProperties->started = true;

                    
                        initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );
                        GetCurrentDirectory( MAX_LOADSTRING, tcharBuff );

                        for ( int i = 0; i< MAX_LOADSTRING; ++i  ) 
                            pStateProperties->f2path [i] = tcharBuff [i];

                        strBuff = "\\maze.pnm";   
                        loadMaze();
                        //rstTxt(*pStateProperties);
                        pStateProperties->started = false;

                        if(pStateProperties->f2Mssg[0])
                            ++pStateProperties->itrCounts;   
                        
                        //rfScreen();
                        
                    doneF2:
                        //InvalidateRect(hWnd, NULL, TRUE);
                        break;                
                }

                
                case VK_F4: {               
                    
                    pStateProperties->isF4 = true;
                    InvalidateRect( hWnd, NULL, TRUE);
                    UpdateWindow( hWnd );

                    break;
                }

                //will toggle location
                case VK_TAB: {
                               
                    //updating StateProperties
                    //TAB's been pressed; setting it to true
                    /*
                    printMaze:

                        if(pStateProperties->started) goto outDone;

                        pStateProperties->started = true;

                        initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );
                        GetCurrentDirectory( MAX_LOADSTRING, tcharBuff );

                        for ( int i = 0; i< MAX_LOADSTRING; ++i  ) 
                            pStateProperties->f2path [i] = tcharBuff [i];

                        strBuff = "\\rawBmp.pnm";   
                        //++pStateProperties->itrCounts;   
                        outMaze();
                        rstTxt(*pStateProperties);
                        pStateProperties->started = false;

                        //rfScreen();
                        
                    outDone:
                    */

                    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED , &pFactory);

                    pFactory->CreateHwndRenderTarget ( 
                                D2D1::RenderTargetProperties(),  
                                D2D1::HwndRenderTargetProperties(
                                                                    hWnd, 
                                                                    D2D1::SizeU(rc.right, rc.bottom)
                                ), 
                                &pRenderTarget
                    );

                    pRenderTarget->CreateCompatibleRenderTarget(
                                                                D2D1::SizeF(rc.right, rc.bottom),  
                                                                &pBitmapRenderTarget
                    );

                    pBitmapRenderTarget->GetBitmap( &pMaze );
                    
                    pMaze->CopyFromMemory( 
                                           NULL,
                                           lpBmp,
                                           (bi.biWidth ) * 4 
                    );
                    

                    hdc = BeginPaint(hWnd, &ps );
                    
                    
                    render4walls( 
                                std::complex<int>(50, 50), 
                                std::complex<int>(50, 100), 
                                std::complex<int>(25, 75), 
                                std::complex<int>(75, 75) 
                    );
                    
                    DeleteDC(hdc);
                    hdc = NULL;
                    EndPaint(hWnd, &ps);

                    pFactory->Release();
                    pFactory = NULL;

                    pRenderTarget->Release();
                    pRenderTarget = NULL;
                
                    pBitmapRenderTarget->Release();
                    pBitmapRenderTarget = NULL;
                    
                    pMaze->Release();
                    pMaze = NULL;
                    
                    CloseHandle(hFile);
                    break;                
                }

            }

            break;
        }        

        case WM_DESTROY: {
        
            PostQuitMessage(0);
            delete(pStateProperties);

            break;        
        }


        default: {

            return DefWindowProc(hWnd, message, wParam, lParam);        
        }

    }

    return 0;
}

// Message handler for about box.
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
