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
int Common::Location::east_wall = 0;
int Common::Location::west_wall = 0;

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
bool             f2ed;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//For F2 (reading file)
HANDLE              hFile;
OVERLAPPED          lpOverlapped = {0};
//int                 itrCounts = 0;

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
            else lpOverlapped->Offset = transferred + lpOverlapped->Offset;

        }
        else {
            
            pStateProperties->itrCounts = 0;
            lpOverlapped->OffsetHigh, lpOverlapped->Offset = 0;        //No transferred         
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

   hInst = hInstance; // Store instance handle in our global variable

   //initializing program state propoerties
   
   Common::Maze::load_maze("");

   pStateProperties = new (std::nothrow) StateProperties;
   ProgramProperties::initPropertiesStates( *pStateProperties );
   ProgramProperties::initPropertiesF2( *pStateProperties, MAX_LOADSTRING );
   ProgramProperties::rstIterator( *pStateProperties ); 
   ProgramProperties::rstMv( *pStateProperties ); 
   
   //RECT rect;
   
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

    ID2D1Factory*              pFactory = NULL;
    ID2D1HwndRenderTarget*     pRenderTarget_maze = NULL;
    ID2D1HwndRenderTarget*     pRenderTarget = NULL;
    ID2D1HwndRenderTarget*     pRenderTarget_south = NULL;
    ID2D1SolidColorBrush*      pBrush = NULL;

                               f2ed = false;

    PAINTSTRUCT                ps;
    HDC                        hdc;
    HRESULT                    hr = S_OK;
    RECT                       fullRc;
    RECT                       curRc;
    D2D1_RECT_F                rectangle;
    D2D1_SIZE_U                paint_sz;

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


    std::function<void()> endScreen = [&pRenderTarget] () { 

        if( pRenderTarget ) {
                
            pRenderTarget->PopAxisAlignedClip();
            pRenderTarget->EndDraw();
        }            
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
                        GetSystemMetrics( SM_CYSCREEN  ) - 35, 
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
        render4walls = [ 
                        &pStateProperties,
                        &pRenderTarget, 
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

        GetClientRect(hWnd, &fullRc);
        paint_sz = D2D1::SizeU( fullRc.right, fullRc.bottom );

        D2D1CreateFactory(
                            D2D1_FACTORY_TYPE_SINGLE_THREADED, 
                            &pFactory
        );         

        //pRenderTarget will hold the address
        pFactory->CreateHwndRenderTarget ( 
                                            D2D1::RenderTargetProperties(),  
                                            D2D1::HwndRenderTargetProperties( hWnd, paint_sz), 
                                            &pRenderTarget
        );

        pRenderTarget->BeginDraw();
        pRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White ) ); 

        pRenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( 
                                                                    pStateProperties->mvX,
                                                                    pStateProperties->mvY                   
                                     )
        );

        //Filling north
        set_rectangle_location( 
                                n.real(), 
                                n.imag(), 
                                n.real() + 15, 
                                n.imag() + 15 
        );

        colorBrush( D2D1::ColorF( D2D1::ColorF::Red ) );
        pRenderTarget->FillRectangle( rectangle, pBrush);
        
        pBrush->Release();
        pBrush = NULL;
        
        //Filling south
        set_rectangle_location( 
                                s.real(), 
                                s.imag(), 
                                s.real() + 15, 
                                s.imag() + 15 
        );

        colorBrush( D2D1::ColorF( D2D1::ColorF::Blue ) );
        pRenderTarget->FillRectangle( rectangle, pBrush);
        
        pBrush->Release();
        pBrush = NULL;
        
        //Filling west
        set_rectangle_location( 
                                w.real(), 
                                w.imag(), 
                                w.real() + 15, 
                                w.imag() + 15
        );

        colorBrush( D2D1::ColorF( D2D1::ColorF::Brown ) );
        pRenderTarget->FillRectangle( rectangle, pBrush );
        
        pBrush->Release();
        pBrush = NULL;
        
        //Fill east
        set_rectangle_location( 
                                e.real(), 
                                e.imag(), 
                                e.real() + 15, 
                                e.imag() + 15 
        );

        colorBrush( D2D1::ColorF( D2D1::ColorF::Green ) );
        pRenderTarget->FillRectangle( rectangle, pBrush );

        pRenderTarget->EndDraw();
        
        pBrush->Release();
        pBrush = NULL;
        pFactory->Release();
        pFactory = NULL;
        pRenderTarget->Release();
        pRenderTarget = NULL;

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
                                       srllScreen
                            ]() {

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

                                            tcharBuff,                // file to open
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
                    }
                    else {                   

                        initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );
                        pStateProperties->f2Mssg = "No input file.";

                        InvalidateRect( hWnd, NULL, TRUE );    
                    }
                    
     };
     
    if ( lpOverlapped.OffsetHigh || lpOverlapped.Offset ) {

        message = WM_KEYDOWN;
        wParam  = VK_F2;
    }
    
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

            if( pStateProperties->started ) goto donePaint;

            pStateProperties->started = true;
            SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties ); 
           
            hdc = BeginPaint(hWnd, &ps );

            
            render4walls( 
                            std::complex<int>(50, 50), 
                            std::complex<int>(50, 100), 
                            std::complex<int>(25, 75), 
                            std::complex<int>(75, 75) 
            );

            loadTimeStrBuff();
            loadTcharBuff();          
            TextOut(hdc,0, 0, tcharBuff, strBuff.length() );

            strBuff = std::to_string( pStateProperties->itrCounts );
            loadTcharBuff();          
            TextOut(hdc,0, 270 , tcharBuff, strBuff.length() ); 

            strBuff = pStateProperties->f2Mssg;
            loadTcharBuff();          
            TextOut(hdc,0, 300 + pStateProperties->itrCounts * 25, tcharBuff, strBuff.length() ); 

            EndPaint(hWnd, &ps );
            DeleteDC(hdc);

            pStateProperties->started = false;

            donePaint:

                SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties ); 
                break;
        }      

        case WM_VSCROLL: {

            hdc = BeginPaint(hWnd, &ps );

            pStateProperties->started = true;
            SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties ); 

            EndPaint(hWnd, &ps );  
            InvalidateRect( hWnd, NULL, TRUE );

            break;
        }
    
        case WM_KEYDOWN:{

            switch( wParam ) {
                
                default:
                    break;

                case VK_UP:{
                    

                    pStateProperties->mvY -= 10;

                    SetWindowLongPtr(

                                        hWnd, 
                                        GWLP_USERDATA, 
                                        (LONG_PTR)pStateProperties
                    );

                    //pStateProperties->started = true;

                    loadTimeStrBuff();
                    loadTcharBuff();          

                    InvalidateRect( hWnd, NULL, TRUE );

                    break;                
                }

                case VK_DOWN:{
                    
                    pStateProperties->mvY += 10;

                    SetWindowLongPtr(

                                        hWnd, 
                                        GWLP_USERDATA, 
                                        (LONG_PTR)pStateProperties
                    );

                    //pStateProperties->started = true;

                    loadTimeStrBuff();
                    loadTcharBuff();          

                    InvalidateRect( hWnd, NULL, TRUE );

                    break;                
                }

                case VK_LEFT:{

                    pStateProperties->mvX -= 10;

                    SetWindowLongPtr(

                                        hWnd, 
                                        GWLP_USERDATA, 
                                        (LONG_PTR)pStateProperties
                    );

                    //pStateProperties->started = true;

                    loadTimeStrBuff();
                    loadTcharBuff();      

                    InvalidateRect( hWnd, NULL, TRUE );
                    break;                
                }

                case VK_RIGHT:{

                    pStateProperties->mvX += 10;

                    SetWindowLongPtr(

                                        hWnd, 
                                        GWLP_USERDATA, 
                                        (LONG_PTR)pStateProperties
                    );

                    //pStateProperties->started = true;
                    InvalidateRect( hWnd, NULL, TRUE);

                    break;                
                }

                //loads maze.pnm
                case VK_F2:{
                    
                    ++pStateProperties->itrCounts;
                    SetWindowLongPtr(
                                    hWnd, 
                                    GWLP_USERDATA, 
                                    (LONG_PTR)pStateProperties
                    );
                    
                    initPropertiesF2 ( *pStateProperties, MAX_LOADSTRING );
                    GetCurrentDirectory( MAX_LOADSTRING, tcharBuff );

                    for ( int i = 0; i< MAX_LOADSTRING; ++i  ) 
                        pStateProperties->f2path [i] = tcharBuff [i];

                    strBuff = "\\maze.pnm";                  
                    loadMaze();

                    InvalidateRect(hWnd, NULL, TRUE);

                    break;                
                }

                
                case VK_F4: {               
                    
                    pStateProperties->cleaned = true;

                    SetWindowLongPtr(
                                        hWnd, 
                                        GWLP_USERDATA, 
                                        (LONG_PTR)pStateProperties
                    );


                    break;
                }

                //will toggle location
                case VK_TAB: {
                               
                    //updating StateProperties
                    //TAB's been pressed; setting it to true
                    pStateProperties->TABed = true;
                    SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties );
                    
                    //srllScreen();
                    
                    GetClientRect(hWnd, &fullRc);
                    paint_sz = D2D1::SizeU( fullRc.right, fullRc.bottom );
                    D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory );         
                    
                    pFactory->CreateHwndRenderTarget (  //pRenderTarget will hold the address
                                                        D2D1::RenderTargetProperties( ),  
                                                        D2D1::HwndRenderTargetProperties(hWnd, paint_sz), 
                                                        &pRenderTarget
                    );

                    render4walls( 
                                    std::complex<int>(50, 50), 
                                    std::complex<int>(50, 100), 
                                    std::complex<int>(25, 75), 
                                    std::complex<int>(75, 75) 
                    );

                    break;                
                }

            }

            break;
        }        

        case WM_DESTROY: {
        
            PostQuitMessage(0);
            delete(pStateProperties);
            //InvalidateRect(hWnd, NULL, TRUE);
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

