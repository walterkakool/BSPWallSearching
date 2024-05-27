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

#define BUFF_MAX        1024
#define MAX_MAZE_WIDTH  211  
#define DWORD_MAX       4294967295 

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
HINSTANCE           hInst;                                // current instance
WCHAR               szTitle[BUFF_MAX];                  // The title bar text
WCHAR               szWindowClass[BUFF_MAX];            // the main window class name
StateProperties*    pStateProperties;
HANDLE              hDIB;
char*               lpBmp;
BITMAPINFOHEADER    bi;
DWORD               dwBmpSize;
CRITICAL_SECTION    crtSec;

TCHAR                      tcharBuff[BUFF_MAX]      = { '\0' };
char                       buffMaze[BUFF_MAX]       = { '\0' };
string                     strBuff;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//For F2 (reading file)
HANDLE              hFile;
OVERLAPPED          lpOverlapped;
RECT                rc;

void clnBuffs(TCHAR tcharBuff[], char buffMaze[], string &strBuff );

VOID CALLBACK ContentRoutine(
                            __in  DWORD dwErrorCode,
                            __in  DWORD dwNumberOfBytesTransfered,
                            __in  LPOVERLAPPED lpOverlapped
);

VOID CALLBACK TitleRoutine(
                            __in  DWORD dwErrorCode,
                            __in  DWORD dwNumberOfBytesTransfered,
                            __in  LPOVERLAPPED lpOverlapped
);

void clnBuffs( TCHAR tcharBuff[], char buffMaze[], string &strBuff  ) {

   for(int i = 0; i < BUFF_MAX; ++i)        
        tcharBuff[i], buffMaze[i] = '\0';

   strBuff = '\0';
   for(int i = 0; i < BUFF_MAX; ++i) strBuff += '\0';
   
}

VOID CALLBACK ContentRoutine(
                                __in  DWORD dwErrorCode,
                                __in  DWORD transferred,
                                __in  LPOVERLAPPED lpOverlapped ) {
    
    if(pStateProperties) {  //Does nothing if it's NULL
    
        if(transferred) {

            if( lpOverlapped->Offset+transferred + 2 < lpOverlapped->Offset ){
            
                ++lpOverlapped->OffsetHigh;
                  lpOverlapped->Offset = lpOverlapped->OffsetHigh
                                                      ? 
                                                      transferred + lpOverlapped->Offset + 2
                                                      :
                                                      0;
            }
            else lpOverlapped->Offset += transferred + 2;

        }
        else {
        
            lpOverlapped->OffsetHigh, 
            lpOverlapped->Offset            = 0;        //No transferred 

            //rstIterator(*pStateProperties);
        }
                 

    }
    
}

VOID CALLBACK TitleRoutine(
                                __in  DWORD dwErrorCode,
                                __in  DWORD transferred,
                                __in  LPOVERLAPPED lpOverlapped ) {
    
    std::string  strTmp = "";
    //pStateProperties->width = stoi(strTmp); 

    if(pStateProperties) {  //Does nothing if it's NULL
    
        if(transferred) {

            //
            for(int i = 0; i < MAX_MAZE_WIDTH; ++i){
            
                if(pStateProperties->height) break;

                ++lpOverlapped->Offset;
                if(buffMaze[i] == ' ' ){
                   
                    strTmp = "";
                    
                    for(++i; buffMaze[i] != ' '; ++i){
                    
                        ++lpOverlapped->Offset;
                        strTmp += buffMaze[i];
                        
                    }
                    --i;
                    
                    if(!pStateProperties->width)
                        pStateProperties->width  = stoi(strTmp);
                    else
                        pStateProperties->height = stoi(strTmp);
                        
                }
                       
            }

            lpOverlapped->Offset += 3;
            pStateProperties->mazeStart = lpOverlapped->Offset;
            //lpOverlapped->Offset = contentStart;
        }

                 

    }
    
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
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, BUFF_MAX);
    LoadStringW(hInstance, IDC_BSPMAZE, szWindowClass, BUFF_MAX);
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

   clnBuffs( tcharBuff, buffMaze, strBuff );
   /*
   for(int i = 0; i < 10240; ++i) {
        for(int j = 0; j < 10240; ++j) {
            
            Common::Maze::maze[i][j] = 0;
        }
   
   }
   */
   pStateProperties = new (std::nothrow) StateProperties;
   ProgramProperties::initPropertiesStates( *pStateProperties );
   ProgramProperties::initPropertiesF2( *pStateProperties, BUFF_MAX );
   ProgramProperties::rstIterator( *pStateProperties ); 
   ProgramProperties::rstMazeStart( *pStateProperties );    
   ProgramProperties::rstLoc( *pStateProperties ); 
   ProgramProperties::rstMv( *pStateProperties ); 
   ProgramProperties::rstDimensions( *pStateProperties ); 

   InitializeCriticalSection(&crtSec);

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

    curRc.left   = 0;
    curRc.top    = 0;
    curRc.right  = 0;
    curRc.bottom = 0;

    std::function<void(int,int,int,int)> set_rectangle_location=[&rectangle]( 
                                                                              int left, 
                                                                              int top,
                                                                              int right,
                                                                              int bottom ) { 

        rectangle = D2D1::RectF( left, top, right, bottom );
    };  
   
    
    std::function<void()> loadTcharBuff = []() {
        
        for ( int i = 0; i< strBuff.length(); ++i  ) tcharBuff [ i ] = strBuff [ i ];
    };

    std::function<void()> read_maze = []() {
        
        for ( int i = 0; i< BUFF_MAX; ++i  ) tcharBuff [ i ] = buffMaze [ i ];
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
        
        for(int i = 0; i < rc.bottom/10; ++i)
            for(int j = 0; j < rc.right/10; ++j){
            
                set_rectangle_location( 
                                        j*10, 
                                        i*10, 
                                        j*10 + 10, 
                                        i*10 + 10 
                );                


                if( Common::Maze::maze[i + pStateProperties->verticalPg* (rc.bottom)/10 ][j + pStateProperties->horizontalPg*rc.right/10 ] ){
                
                    pBitmapRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &pBrush );

                    pBitmapRenderTarget->FillRectangle( rectangle, pBrush);   

                    pBrush->Release();
                    pBrush = NULL;
                }
            
            }

        pBitmapRenderTarget->EndDraw();

        pBitmapRenderTarget->GetBitmap(&pMaze);
    };

    std::function<void( std::complex<int> )> renderWalls = [ 
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
                                                            &pFactory] ( std::complex<int> location ) { 

        int localX = location.real() * 10;
        int localY = (((pStateProperties->height-1)-location.imag()) %( (rc.bottom) /10)) *10;

        pRenderTarget->BeginDraw();
        pRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White ) ); 

        pRenderTarget->DrawBitmap(
                                pMaze,
                                D2D1::RectF(rc.left, rc.top, rc.right, (rc.bottom)),
                                1.0,
                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR

        );

        if( pStateProperties->isLocation ){
        
            pRenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( 
                                                                        pStateProperties->mvX,
                                                                        pStateProperties->mvY + 5                  
                                         )
            );

            pRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Purple ), &pBrush );

            set_rectangle_location( 
                                    localX, 
                                    localY, 
                                    localX + 10, 
                                    localY + 10 
            ); 

            pRenderTarget->FillRectangle( rectangle, pBrush); 

            pBrush->Release();
            pBrush = NULL;


            pRenderTarget->SetTransform( D2D1::Matrix3x2F::Identity() );
        
        }

        pRenderTarget->EndDraw();
        
    };

     std::function<void( )> loadTimeStrBuff=[]() {    

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
                                       //&strBuff, 
                                       //&tcharBuff, 
                                       &pStateProperties,
                                       //&buffMaze,
                                       clnField, 
                                       srllScreen]() {
                    
                    for ( int i = 0; 1; ++i ) {

                        tcharBuff [i] = pStateProperties->f2path [i];

                        if ( !pStateProperties->f2path [i] ) {
                            
                            for ( int j = 0; strBuff[j]; ++i, ++j ) {
                                
                                if( i < BUFF_MAX ) tcharBuff [i] = strBuff[j];
                                else {
                                
                                    pStateProperties->path2long = true;
                                    goto openFile;
                                }

                            }

                            goto openFile;
                        }
                    }
                    
                    openFile:

                        if ( !pStateProperties->height ){
                    
                            hFile = CreateFile(
                                                tcharBuff,                  // file to open
                                                GENERIC_READ,               // open for reading
                                                FILE_SHARE_READ,            // share for reading
                                                NULL,                       // default security
                                                OPEN_EXISTING,              // existing file only
                                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                                                NULL                        // no attr. template
                                    );                         


                            for ( int i = 0; i < BUFF_MAX; ++i ) buffMaze [i] = '\0';

                            
                            if ( ReadFileEx( hFile, buffMaze, MAX_MAZE_WIDTH, &lpOverlapped, TitleRoutine)){

                                try{
                                SleepEx(INFINITE, TRUE); //Sleep for AsynRoutine
                                }
                                catch(exception& e){
                        
                                }
                                
                                initPropertiesF2 ( *pStateProperties, BUFF_MAX );

                                for (int i=0; i<MAX_MAZE_WIDTH; ++i) pStateProperties->f2Mssg[i] = buffMaze[i]; 
                                
                                CloseHandle(hFile);         //next open will start at the offset

                            }
                            else {                   

                                initPropertiesF2 ( *pStateProperties, BUFF_MAX );
                                CloseHandle(hFile);    
                            }
                            
                        }
                        else if(!pStateProperties->itrCounts)
                            lpOverlapped.Offset = pStateProperties->mazeStart;
                        
                        hFile = CreateFile(
                                            tcharBuff,                  
                                            GENERIC_READ,               
                                            FILE_SHARE_READ,           
                                            NULL,                       
                                            OPEN_EXISTING,              
                                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                                            NULL                        
                                );            

                        for ( int i = 0; i < BUFF_MAX; ++i ) buffMaze [i] = '\0';

                        if ( ReadFileEx( hFile, buffMaze, MAX_MAZE_WIDTH, &lpOverlapped, ContentRoutine)){

                            try{
                            SleepEx(INFINITE, TRUE); //Sleep for AsynRoutine
                            }
                            catch(exception& e){
                        
                            }
                            ProgramProperties::initPropertiesF2 ( *pStateProperties, BUFF_MAX );

                            for (int i=0; i<MAX_MAZE_WIDTH; ++i) pStateProperties->f2Mssg[i] = buffMaze[i]; 

                            CloseHandle(hFile);         //next open will start at the offset

                            //Cuz BitBlt scans from the bottom, stores the content in a reverse order
                            for ( int i = 0; i < MAX_MAZE_WIDTH; ++i )

                                if(pStateProperties->itrCounts < pStateProperties->height){
                                
                                    Common::Maze::maze[ pStateProperties->height - pStateProperties->itrCounts - 1][i] = 
                                        pStateProperties->f2Mssg[i] == '1' 
                                                                    ?
                                                                    1
                                                                    :
                                                                    0;                                
                                }



                        }
                        else {                   

                            ProgramProperties::initPropertiesF2 ( *pStateProperties, BUFF_MAX );
                            CloseHandle(hFile);    
                        }
                        
                    
     };
     
     std::function<void( )> outMaze=[ 
                                       &hWnd,
                                       //&strBuff, 
                                       //&tcharBuff, 
                                       &pStateProperties,
                                       //&buffMaze,
                                       clnField, 
                                       srllScreen
                            ]() {

                    for ( int i = 0; 1; ++i ) {

                        tcharBuff [i] = pStateProperties->f2path [i];

                        if ( !pStateProperties->f2path [i] ) {
                            
                            for ( int j = 0; strBuff[j]; ++i, ++j ) {
                                
                                if( i < BUFF_MAX ) tcharBuff [i] = strBuff[j];
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

                    for ( int i = 0; i < BUFF_MAX; ++i ) buffMaze [i] = '\0';

                    if ( WriteFile( hFile, (LPSTR)lpBmp, dwBmpSize, NULL, &lpOverlapped)){
                        try{
                        SleepEx(INFINITE, TRUE); //Sleep for AsynRoutine
                        }
                        catch(exception& e){
                        
                        }
                        CloseHandle(hFile);         //next open will start at the offset
                    }
                    else {                   

                        ProgramProperties::initPropertiesF2 ( *pStateProperties, BUFF_MAX );
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
                        DeleteCriticalSection(&crtSec);
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
            /*
            if( pStateProperties->isF4 ){
                
                pStateProperties->isF4 = false;
                goto keyTAB;
            }
            */
            if( lpOverlapped.OffsetHigh || lpOverlapped.Offset ) {
            
                SendMessage( hWnd, WM_KEYDOWN, VK_F2, lParam );
                break;
            }
            
            if( hDIB && !pStateProperties->isF5 ){
            
                pStateProperties->isF5 = false;

                GlobalUnlock(hDIB);
                GlobalFree(hDIB);

                hDIB = NULL;
            }

            if( pStateProperties->isF5 ){
            
                pStateProperties->isF5 = false;
                break;
            }
                
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED , &pFactory);

            pFactory->CreateHwndRenderTarget(
                    D2D1::RenderTargetProperties(),  
                    D2D1::HwndRenderTargetProperties(
                                                        hWnd, 
                                                        D2D1::SizeU(rc.right, (rc.bottom) )
                    ), 
                    &pRenderTarget
            );


            if( !TryEnterCriticalSection(&crtSec) ) goto donePaint;

            pStateProperties->started = true;
           
            pRenderTarget->CreateCompatibleRenderTarget(
                                                        D2D1::SizeF(rc.right, (rc.bottom) ),  
                                                        &pBitmapRenderTarget
            );

            hdc = BeginPaint(hWnd, &ps );            

            setBitmap( 
                        std::complex<int>(rc.right, (rc.bottom) + 0), 
                        std::complex<int>(rc.right, (rc.bottom) + 50), 
                        std::complex<int>(rc.right-25, (rc.bottom) + 25), 
                        std::complex<int>(rc.right+25, (rc.bottom) + 25) 
            );

            pStateProperties->isLocation = false;
            renderWalls( std::complex<int>(pStateProperties->locX, pStateProperties->locY) );
            /*
            strBuff = "currentPgX:  ";
            strBuff += std::to_string( (pStateProperties->locX + pStateProperties->mvX/10)/(rc.right/10)  );
            loadTcharBuff();          
            TextOut(hdc,100, 270 , tcharBuff, strBuff.length() ); 

            strBuff = "currentPgY:  ";
            strBuff += std::to_string( (pStateProperties->locY + (-pStateProperties->mvY/10))/(rc.bottom/10)    );
            loadTcharBuff();          
            TextOut(hdc,100, 370 , tcharBuff, strBuff.length() ); 

            strBuff = "pageX:  ";
            strBuff += std::to_string( pStateProperties->horizontalPg  );
            loadTcharBuff();          
            TextOut(hdc,250, 370 , tcharBuff, strBuff.length() ); 

            strBuff = "pageY:  ";
            strBuff += std::to_string( pStateProperties->verticalPg  );
            loadTcharBuff();          
            TextOut(hdc,400, 370 , tcharBuff, strBuff.length() ); 
            
            strBuff = "locY:  ";
            strBuff += std::to_string( pStateProperties->locY  );
            loadTcharBuff();          
            TextOut(hdc,550, 370 , tcharBuff, strBuff.length() ); 

            strBuff = "pStateProperties->itrCounts:  ";
            strBuff += std::to_string( pStateProperties->itrCounts );
            loadTcharBuff();          
            TextOut(hdc,350, 270 , tcharBuff, strBuff.length() ); 

            strBuff = "pStateProperties->width:  ";
            strBuff += std::to_string( pStateProperties->width );
            loadTcharBuff();          
            TextOut(hdc,750, 270 , tcharBuff, strBuff.length() ); 

            strBuff = "pStateProperties->height:  ";
            strBuff += std::to_string( pStateProperties->height );
            loadTcharBuff();          
            TextOut(hdc,1050, 270 , tcharBuff, strBuff.length() ); 
            
            strBuff = std::to_string( lpOverlapped.Offset );
            //strBuff = std::to_string( pMaze->GetPixelFormat().format );
            loadTcharBuff();          
            TextOut(hdc,150, 270 , tcharBuff, strBuff.length() );             
        
            strBuff = pStateProperties->f2Mssg;
            loadTcharBuff();    
            
            TextOut(
                    hdc,
                    0, 
                    0,
                    tcharBuff, 
                    strBuff.length()
            );
            */
            DeleteDC(hdc);
            hdc = NULL;
            EndPaint(hWnd, &ps );

            //pStateProperties->started = false;
            LeaveCriticalSection( &crtSec );

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
                                                (rc.bottom)
                         );

                SelectObject( hdcMem, hBmp );
                
                if (!BitBlt(hdcMem,0 , 0,  rc.right, (rc.bottom), hdc, 0, 0,SRCCOPY) )
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
                            (UINT)(rc.bottom),
                            lpBmp,
                            (BITMAPINFO*)&bi, 
                            DIB_RGB_COLORS
                );

                ReleaseDC(hWnd, hdc);
                DeleteDC(hdc);
                DeleteObject( hBmp );
                DeleteDC(hdcMem);

                hdc = NULL;
                
                //if( lpOverlapped.OffsetHigh || lpOverlapped.Offset ) goto loadF2;
                SendMessage( hWnd, WM_KEYDOWN, VK_SPACE, lParam );

                break;
        }      

        case WM_VSCROLL: {

            //srllScreen();

            break;
        }

        case WM_SYSKEYDOWN:{
        
            switch( wParam ) {

                default:
                    break;

                case VK_UP:{

                    if( !TryEnterCriticalSection(&crtSec) ) 
                        goto doneUp;

                    //pStateProperties->started = true;

                    pStateProperties->verticalPg = pStateProperties->verticalPg < pStateProperties->height/( (rc.bottom)/10)+1 
                                                                    ?
                                                                    ++pStateProperties->verticalPg
                                                                    :
                                                                    pStateProperties->verticalPg;

                    //pStateProperties->started = false;
                    LeaveCriticalSection(&crtSec);
                    goto refreshMaze;
                    break;
                }

                case VK_DOWN:{

                    if( !TryEnterCriticalSection(&crtSec) ) 
                        goto doneDown;
                    //pStateProperties->started = true;

                    pStateProperties->verticalPg = pStateProperties->verticalPg
                                                                    ?
                                                                    --pStateProperties->verticalPg
                                                                    :
                                                                    0;
                    //pStateProperties->started = false;
                    LeaveCriticalSection(&crtSec);
                    goto refreshMaze;
                    break;
                }

                case VK_LEFT:{

                    if( !TryEnterCriticalSection(&crtSec) ) 
                        goto doneLeft;
                    //pStateProperties->started = true;

                    pStateProperties->horizontalPg = pStateProperties->horizontalPg
                                                                    ?
                                                                    --pStateProperties->horizontalPg
                                                                    :
                                                                    0;

                    //pStateProperties->started = false;
                    LeaveCriticalSection(&crtSec);
                    goto refreshMaze;
                    break;
                }

                case VK_RIGHT:{

                    if( !TryEnterCriticalSection(&crtSec) ) 
                        goto doneRight;
                    //pStateProperties->started = true;

                    pStateProperties->horizontalPg = pStateProperties->horizontalPg < pStateProperties->width/(rc.right/10)+1 
                                                                    ?
                                                                    ++pStateProperties->horizontalPg
                                                                    :
                                                                    pStateProperties->horizontalPg;

                    //pStateProperties->started = false;
                    LeaveCriticalSection(&crtSec);
                    goto refreshMaze;
                    break;
                }

            }
            break;
        }

        case WM_KEYDOWN:{

            switch( wParam ) {
                
                default:
                    break;

                case VK_UP:{

                    if( !TryEnterCriticalSection(&crtSec) )
                        goto doneUp;

                        //pStateProperties->started = true; 
                    
                        pStateProperties->mvY = Common::Maze::maze
                                                [ pStateProperties->locY + (-pStateProperties->mvY/10) + 1]
                                                [ pStateProperties->locX + pStateProperties->mvX/10       ]
                                                                           ?
                                                                            pStateProperties->mvY
                                                                            :
                                                                            pStateProperties->mvY-10;
                    

                        LeaveCriticalSection(&crtSec);
                        SendMessage( hWnd, WM_KEYDOWN, VK_SPACE, lParam );

                    doneUp:
                        break;                
                }

                case VK_DOWN:{

                    if( !TryEnterCriticalSection(&crtSec) )
                        goto doneDown;

                        //pStateProperties->started = true;
                        
                        pStateProperties->mvY = Common::Maze::maze
                                                [ pStateProperties->locY + (-pStateProperties->mvY/10) - 1]
                                                [ pStateProperties->locX + pStateProperties->mvX/10       ]
                                                                           ?
                                                                            pStateProperties->mvY
                                                                            :
                                                                            pStateProperties->mvY+10;
                         
                      
                        LeaveCriticalSection(&crtSec);
                        SendMessage( hWnd, WM_KEYDOWN, VK_SPACE, lParam );

                    doneDown:
                        break;

                }

                case VK_LEFT:{

                    if( !TryEnterCriticalSection(&crtSec) )
                        goto doneLeft;

                        //pStateProperties->started = true;
                        pStateProperties->mvX = Common::Maze::maze
                                                [ pStateProperties->locY + (-pStateProperties->mvY/10)]
                                                [ pStateProperties->locX + pStateProperties->mvX/10 -1]
                                                                           ?
                                                                            pStateProperties->mvX
                                                                            :
                                                                            pStateProperties->mvX-10;

                        LeaveCriticalSection(&crtSec);
                        SendMessage( hWnd, WM_KEYDOWN, VK_SPACE, lParam );

                    doneLeft:
                        break;                
                }

                case VK_RIGHT:{

                    if( !TryEnterCriticalSection(&crtSec) )
                        goto doneRight;

                        pStateProperties->mvX = Common::Maze::maze
                                                [ pStateProperties->locY + (-pStateProperties->mvY/10)]
                                                [ pStateProperties->locX + pStateProperties->mvX/10 +1]
                                                                           ?
                                                                            pStateProperties->mvX
                                                                            :
                                                                            pStateProperties->mvX+10;

                        LeaveCriticalSection(&crtSec);
                        SendMessage( hWnd, WM_KEYDOWN, VK_SPACE, lParam );

                    doneRight:

                        break;                
                }

                //loads maze.pnm
                case VK_F2:{
                
                    loadF2:

                        //if(pStateProperties->started) goto doneF2;

                        //pStateProperties->started = true;

                        ProgramProperties::initPropertiesF2 ( *pStateProperties, BUFF_MAX );
                        GetCurrentDirectory( MAX_MAZE_WIDTH, tcharBuff );

                        for ( int i = 0; i< BUFF_MAX; ++i  ) 
                            pStateProperties->f2path [i] = tcharBuff [i];

                        //clnBuffs( tcharBuff, buffMaze, strBuff );
                        strBuff = "\\maze.pnm";
                        if(TryEnterCriticalSection(&crtSec) ){

                            loadMaze();
                            LeaveCriticalSection(&crtSec);
                        }
                        
                        //pStateProperties->started = false;
                        

                        if(pStateProperties->f2Mssg[0]){
                        
                            ++pStateProperties->itrCounts;
                            InvalidateRect( hWnd, NULL, TRUE );
                            UpdateWindow( hWnd );
                            /*
                            if( lpOverlapped.OffsetHigh || lpOverlapped.Offset ) 
                                SendMessage( hWnd, WM_KEYDOWN, VK_F2, lParam );
                            else
                                goto refresh;
                            */
                        }

                    doneF2:
                            
                            if( lpOverlapped.OffsetHigh || lpOverlapped.Offset ) 
                                break;
                            else
                                goto refreshMaze;                        
                            
                        break;                
                }

                case VK_F5: {               
                    
                    refreshMaze:

                        if( !TryEnterCriticalSection(&crtSec) ) goto doneF5;

                            //pStateProperties->started = true;

                            ProgramProperties::rstIterator(*pStateProperties);
                            lpOverlapped = {0}; 

                            //pStateProperties->started = false;
                            pStateProperties->isF5    = true;

                            LeaveCriticalSection( &crtSec );
                            InvalidateRect( hWnd, NULL, TRUE );
                            UpdateWindow( hWnd );

                    doneF5:
                        break;  
                }

                case VK_F8: {               
                    
                    glancingMode:

                        if( !TryEnterCriticalSection(&crtSec) ) goto doneF8;

                            ++pStateProperties->glancing %= 2;

                            LeaveCriticalSection( &crtSec );
                            InvalidateRect( hWnd, &curRc, TRUE);
                            UpdateWindow( hWnd );
                            
                    doneF8:
                        SendMessage( hWnd, WM_KEYDOWN, VK_F5, lParam );    
                        break;  
                } 

                //will toggle location
                case VK_SPACE: {
                               
                    keySpace:
                    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED , &pFactory);

                    pFactory->CreateHwndRenderTarget ( 
                                D2D1::RenderTargetProperties(),  
                                D2D1::HwndRenderTargetProperties(
                                                                    hWnd, 
                                                                    D2D1::SizeU(rc.right, (rc.bottom))
                                ), 
                                &pRenderTarget
                    );

                    pRenderTarget->CreateCompatibleRenderTarget(
                                                                D2D1::SizeF(rc.right, (rc.bottom)),  
                                                                &pBitmapRenderTarget
                    );

                    pBitmapRenderTarget->GetBitmap( &pMaze );
                    
                    pMaze->CopyFromMemory( 
                                           NULL,
                                           lpBmp,
                                           (bi.biWidth ) * 4 
                    );
                    

                    hdc = BeginPaint(hWnd, &ps );

                    pStateProperties->isLocation = true;

                    if ( pStateProperties->height )                        
                        renderWalls( 
                                    std::complex<int>(
                                                        pStateProperties->locX, 
                                                        pStateProperties->locY
                                    )
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
                    
                    if( !pStateProperties->glancing && !ProgramProperties::pageMatched( *pStateProperties, (rc.bottom), rc.right ) ){
                        
                        CloseHandle(hFile);

                        pStateProperties->horizontalPg = (pStateProperties->locX + pStateProperties->mvX/10)/(rc.right/10);
                        pStateProperties->verticalPg   = (pStateProperties->locY + (-pStateProperties->mvY/10))/( (rc.bottom) /10);

                        SendMessage( hWnd, WM_KEYDOWN, VK_F5, lParam );
                    }
                    else        
                        CloseHandle(hFile);
                    break;                
                }

            }
            break;

        }        

        case WM_DESTROY: {
        
            PostQuitMessage(0);
            delete(pStateProperties);
            DeleteCriticalSection(&crtSec);
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
