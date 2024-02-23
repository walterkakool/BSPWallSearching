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

#define MAX_LOADSTRING 100

#define _USE_MATH_DEFINES

#define MAX_BUFF_SIZE 10240

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

void run(int a, int b);
int main(int argc, char** argv );

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
   StateProperties* pStateProperties = new (std::nothrow) StateProperties;
   pStateProperties->started = false;
   pStateProperties->TABed = false;
   pStateProperties->F2ed = false;
   pStateProperties->cleaned = false;

   //RECT rect;
   HWND hWnd = CreateWindowEx( 
                                0,                      // no extended styles 
                                szWindowClass,           // scroll bar control class 
                                szTitle,                // no window text 
                                WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,   // window styles 
                                SM_CXSCREEN,         // leftmost horizontal position 
                                SM_CYSCREEN,         // topest vertical position 
                                GetSystemMetrics( SM_CXSCREEN ),   // maximizing width 
                                GetSystemMetrics( SM_CYSCREEN ),   // maximizing height 
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

    PAINTSTRUCT                ps;
    HDC                        hdc;
    HRESULT                    hr = S_OK;
    RECT                       rc;
    D2D1_RECT_F                rectangle;
    D2D1_SIZE_U                paint_sz;

    LONG_PTR                   ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
    StateProperties*           pStateProperties = reinterpret_cast<StateProperties*>(ptr);
    HANDLE                     hFile;
    OVERLAPPED                 rOverlap = {0};

    TCHAR                      scroll_bar[]     = L"Scroll bar called";
    TCHAR                      text_buffer[ MAX_BUFF_SIZE ] = { '\0' };
    string                     strbuff          = "";
    char                       fMaze[ MAX_BUFF_SIZE ] = { '\0' };
    
    if ( !(pStateProperties == nullptr) ) {

        bool statesChk = 
                pStateProperties->F2ed || 
                pStateProperties->started || 
                pStateProperties->TABed ? 
                    true : false;

        if ( statesChk ) 
            strbuff = pStateProperties->F2Mssg;       
        else {
        
            SYSTEMTIME local_time_obj;
            GetLocalTime(&local_time_obj);

            strbuff += "----"; 
            strbuff += to_string( local_time_obj.wHour );
            strbuff += " : ";
            strbuff += to_string( local_time_obj.wMinute );
            strbuff += " : ";
            strbuff += to_string( local_time_obj.wSecond );
            strbuff += "---\n";

            for( int i = 0; i < strbuff.length(); ++i ) 
                text_buffer[ i ] = strbuff [ i ];            
        }
    }

    std::function<void( int, int, int, int )> set_rectangle_location = [ &rectangle ] (  

      int left, int top, int right, int bottom ) { rectangle = D2D1::RectF( left, top, right, bottom );
    };  
    
    std::function<void( int )> properties_messages_init = [ &pStateProperties ]( int size ) {

        pStateProperties->F2Mssg = '\0';  
        for ( int i = 0; i< size - 1; ++i  )  pStateProperties->F2Mssg += '\0';
    };
    
    std::function<void()> load_strbuff = [ &strbuff, &text_buffer ]() {
        
        for ( int i = 0; i< strbuff.length(); ++i  ) text_buffer [ i ] = strbuff [ i ];
    };

    std::function<void()> read_maze = [ &fMaze, &text_buffer ]() {
        
        for ( int i = 0; i< MAX_BUFF_SIZE; ++i  ) text_buffer [ i ] = fMaze [ i ];
    };

    std::function<void( D2D1::ColorF )> fill_rectangle_color = [ &pRenderTarget, &pBrush ]
                                                                        ( D2D1_COLOR_F color) { 
                                               pRenderTarget->CreateSolidColorBrush( color,&pBrush );
    };

     std::function<void()> scroll_screen =  [ &hWnd ] () {
                                                             ScrollWindowEx(

                                                                                hWnd, 
                                                                                000, 
                                                                                1000, 
                                                                                (CONST RECT *) NULL, 
                                                                                (CONST RECT *) NULL, 
                                                                                (HRGN) NULL, 
                                                                                (PRECT) NULL, 
                                                                                SW_INVALIDATE
                                                            ); 
    };    

     std::function<void()>
         load_maze = [ 
             
                        &pRenderTarget, 
                        &rectangle, 
                        &pBrush, 
                        &set_rectangle_location, 
                        &fill_rectangle_color ]() {

                                                    pRenderTarget->BeginDraw();
                                                    pRenderTarget->Clear( 

                                                        D2D1::ColorF( D2D1::ColorF::White ) 
                                                    ); 

                                                    set_rectangle_location( 

                                                                            0, 
                                                                            0, 
                                                                            GetSystemMetrics( SM_CXSCREEN ), 
                                                                            GetSystemMetrics( SM_CYSCREEN ) 
                                                    );

                                                    fill_rectangle_color( D2D1::ColorF::White );
                                                    pRenderTarget->FillRectangle( rectangle, pBrush);
                                                    pRenderTarget->EndDraw();
     };

     std::function<void()> clean_field = [ 
             
                                            &pRenderTarget, 
                                            &rectangle, 
                                            &pBrush, 
                                            &set_rectangle_location, 
                                            &fill_rectangle_color,
                                            &hWnd,
                                            &rc,
                                            &paint_sz,
                                            &pFactory ]() {

                                                    GetClientRect(hWnd, &rc);
                                                    paint_sz = D2D1::SizeU( rc.right, rc.bottom );

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

                                                    set_rectangle_location( 

                                                                            0, 
                                                                            0, 
                                                                            GetSystemMetrics( SM_CXSCREEN ), 
                                                                            GetSystemMetrics( SM_CYSCREEN ) 
                                                    );

                                                    fill_rectangle_color( D2D1::ColorF::White );
                                                    pRenderTarget->FillRectangle( rectangle, pBrush);
                                                    pRenderTarget->EndDraw();
     };

     std::function<void( 

                    std::complex<int>, 
                    std::complex<int>, 
                    std::complex<int>, 
                    std::complex<int> 
                   )> render4walls = [ 

                        &pRenderTarget, 
                        &rectangle, 
                        &pBrush, 
                        &set_rectangle_location, 
                        &fill_rectangle_color ] ( 

                            std::complex<int> n, 
                            std::complex<int> s, 
                            std::complex<int> w, 
                            std::complex<int> e ) {         

                                    pRenderTarget->BeginDraw();
                                    pRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White ) ); 

                                    //filling north
                                    set_rectangle_location( 

                                                                n.real(), 
                                                                n.imag(), 
                                                                n.real() + 15, 
                                                                n.imag() + 15 
                                    );

                                    fill_rectangle_color( D2D1::ColorF( D2D1::ColorF::Red ) );
                                    pRenderTarget->FillRectangle( rectangle, pBrush);

                                    //filling south
                                    set_rectangle_location( 

                                                                s.real(), 
                                                                s.imag(), 
                                                                s.real() + 15, 
                                                                s.imag() + 15 
                                    );

                                    fill_rectangle_color( D2D1::ColorF( D2D1::ColorF::Blue ) );
                                    pRenderTarget->FillRectangle( rectangle, pBrush);

                                    //filling west
                                    set_rectangle_location( 

                                                                w.real(), 
                                                                w.imag(), 
                                                                w.real() + 15, 
                                                                w.imag() + 15
                                    );

                                    fill_rectangle_color( D2D1::ColorF( D2D1::ColorF::Brown ) );
                                    pRenderTarget->FillRectangle( rectangle, pBrush );

                                    //fill east
                                    set_rectangle_location( 

                                                                e.real(), 
                                                                e.imag(), 
                                                                e.real() + 15, 
                                                                e.imag() + 15 
                                    );

                                    fill_rectangle_color( D2D1::ColorF( D2D1::ColorF::Green ) );
                                    pRenderTarget->FillRectangle( rectangle, pBrush );
                                    pRenderTarget->EndDraw();
     };

    switch (message) {

        case WM_COMMAND: {

                int wmId = LOWORD(wParam);
                // Parse the menu selections:
                switch (wmId) {

                    case IDM_ABOUT:

                        DialogBox(

                            hInst, 
                            MAKEINTRESOURCE(IDD_ABOUTBOX), 
                            hWnd, 
                            About
                        );

                break;

                    case IDM_EXIT:

                        DestroyWindow(hWnd);
                        break;

                default:

                    return DefWindowProc(

                                            hWnd, 
                                            message, 
                                            wParam, 
                                            lParam
                    );
                }
        }

        break;

        case WM_PAINT: {
            
            bool states_check = 
                pStateProperties->F2ed || pStateProperties->started || pStateProperties->TABed ? 
                   true : false;

            hdc = BeginPaint(hWnd, &ps);
            load_strbuff();
                
            if ( !( pStateProperties->cleaned ) ) 
                TextOut(hdc,0, 0, text_buffer, strbuff.length() );
            else {
                    
                pStateProperties->cleaned = false;
                SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties );  
            }
         
            EndPaint(hWnd, &ps);   
            break;
        }      

        case WM_VSCROLL: {

            hdc = BeginPaint(hWnd, &ps);

            scroll_screen();

            //TextOut needs to follow render_walls; otherwise, it gets vanished.
            TextOut(hdc,0, 150, scroll_bar, _tcslen( scroll_bar ) );

            EndPaint(hWnd, &ps);
            break;
        }
    
        case WM_KEYDOWN:{

            switch( wParam ) {
                
                default:
                    break;

                //loads maze.pnm
                case VK_F2:

                    
                    pStateProperties->F2ed = true;

                    SetWindowLongPtr(
                        hWnd, 
                        GWLP_USERDATA, 
                        (LONG_PTR)pStateProperties
                    );

                    GetCurrentDirectory( MAX_BUFF_SIZE, text_buffer );

                    properties_messages_init( MAX_BUFF_SIZE );

                    for ( int i = 0; i< MAX_BUFF_SIZE; ++i  ) 
                        pStateProperties->F2Mssg [ i ] = text_buffer [i];

                    strbuff = "\\maze.pnm";
                    //strbuff += '\0';
                    
                    for ( int i = 0; i < MAX_BUFF_SIZE; ++i ) {

                        if ( pStateProperties->F2Mssg [ i ] != '\0' ) 
                            text_buffer [i] = pStateProperties->F2Mssg [ i ];
                        else 
                            for ( int j = 0; strbuff[j] != '\0' && i < MAX_BUFF_SIZE; ++j, ++i )
                                text_buffer [i] = strbuff[j];                           
                    }
                    
                    hFile = CreateFile(

                                        text_buffer,           // file to open
                                        GENERIC_READ,          // open for reading
                                        FILE_SHARE_READ,       // share for reading
                                        NULL,                  // default security
                                        OPEN_EXISTING,         // existing file only
                                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                                        NULL                   // no attr. template
                    );                 
                    
                    for ( int i = 0; i < MAX_BUFF_SIZE; ++i ) fMaze [i] = '\0';

                    for ( int i = 0; i < MAX_BUFF_SIZE; ++i ) {
                    
                        if ( ReadFileEx(hFile, fMaze, MAX_BUFF_SIZE, &rOverlap, NULL) ) {
                    
                            properties_messages_init( MAX_BUFF_SIZE );
                            for (int i=0; i<MAX_BUFF_SIZE; ++i ) pStateProperties->F2Mssg [ i ] = fMaze [i];                    
                        }
                        else
                            break;
                            //pStateProperties->F2Mssg = "F2 failed";                    
                    }

                      
                    scroll_screen();
                    clean_field();

                    //SleepEx(INFINITE, TRUE);
                    CloseHandle(hFile);

                    break;
                
                case VK_F4:

                    pStateProperties->cleaned = true;

                    SetWindowLongPtr(
                        hWnd, 
                        GWLP_USERDATA, 
                        (LONG_PTR)pStateProperties
                    );

                    scroll_screen();
                    clean_field();
                    break;

                //will toggle location
                case VK_TAB: 
                    
                    //updating StateProperties
                    //TAB's been pressed; setting it to true
                    pStateProperties->TABed = true;
                    SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pStateProperties );
                    
                    scroll_screen();
                    
                    GetClientRect(hWnd, &rc);
                    paint_sz = D2D1::SizeU( rc.right, rc.bottom );
                    D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory );         

                    //pRenderTarget will hold the address
                    pFactory->CreateHwndRenderTarget ( 

                                                        D2D1::RenderTargetProperties(),  
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

            break;
        }        

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:

            return DefWindowProc(hWnd, message, wParam, lParam);
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

void run(int a, int b){
    
    auto                            start = std::chrono::system_clock::now();
    auto                            end = std::chrono::system_clock::now();
    string                          time_cost_str = "0";
    char*                           time_cost_char = &time_cost_str[0];
    int                             use_BSP = 0; 
    int                             input = 0;
    int                             locay = Common::Maze::rows-b; 
    int                             lasty = locay;
    int                             locax = a; 
    int                             lastx = locax;

    Common::Naive*                  naive_obj = new Common::Naive();
    BSP::Segment*                   current_segment = new BSP::Segment();

    std::pair<bool, BSP::Segment*>  root = current_segment->build_tree(

                                                std::complex<int>( 1, Common::Maze::rows - 2 ), 
                                                std::complex<int>( Common::Maze::cols-2, 1)
    );

    BSP::Segment::upddate_parents(root.second);

    BSP::Segment* leaf_node =  BSP::Segment::to_leaf(
                                                        root.second,
                                                        std::complex<int>(1, 1)
    );

    prints:

        Common::Location::reset();

        if(use_BSP){

            start = std::chrono::system_clock::now();

            leaf_node = 
                BSP::Segment::to_leaf(

                                        root.second, 
                                        std::complex<int>( locax, Common::Maze::rows-locay )
            );

            BSP::Segment::update_location_walls(

                                                    leaf_node, 
                                                    std::complex<int>(

                                                                        locax, 
                                                                        Common::Maze::rows-locay
                                                    ), 
                                                    Common::Maze::rows,
                                                    Common::Maze::cols
            );

            end = std::chrono::system_clock::now();

            time_cost_str = 
                std::to_string(

                    std::chrono::
                        duration_cast<std::chrono::milliseconds>
                            (end - start).count()
            );

            //time_cost_char = &time_cost_str[0];

            /*
            std::cout<<
                "BSP time cost: "<< 
                    time_cost_str<<std::endl;
            */

        }else{
        
            start = std::chrono::system_clock::now();

            Location::point = 
                std::complex<int> ( 

                    locax, 
                    Common::Maze::rows - locay 
                );

            naive_obj->update_walls();
            end = std::chrono::system_clock::now();

            time_cost_str = 
                std::to_string(
                    std::chrono::duration_cast<std::chrono::milliseconds>
                        (end - start).count()
            );

            //time_cost_char = &time_cost_str[0];
            std::cout<<"Naive time cost: "<< time_cost_str<<std::endl;
        }

        delete current_segment;
        delete naive_obj;
        delete root.second;
}

int main(int argc, char** argv )
{
    std::string     valve = "1";
    int             no_maze = 0;
    std::string     input_path = "";

    //input_path += argv[1];

    char*           argv_chars = argv[1];
    std::string     stra = argv_chars;
    std::string     strb = "a";

    input_path += "./maze.pnm";

    if( !stra.empty() ){

        argv_chars = argv[2];
        strb = argv_chars;
    }

    std::function<std::complex<int>()> initial_point = [](){
    
        int random_num = 
            std::chrono::duration<double>
                ( std::chrono::system_clock::now().
                    time_since_epoch() ).
                        count();

        if( Common::Maze::maze[ random_num%3 ][ random_num%7 ] == 0 ) 
            return std::complex<int> ( random_num%7, random_num%3 );        

        for(int i=0; i< Common::Maze::rows; ++i) 

            for(int j=0; j< Common::Maze::cols; ++j)

                 if( Common::Maze::maze[i][j] == 0 ) 
                    
                     return std::complex<int> (j,i);

        return std::complex<int> (0,0);
    };

    no_maze = Common::Maze::load_maze( input_path );

    if( no_maze ){  

        std::cout<<"no maze.pnm found"<<std::endl;
        return 0;   
    }
    
    try{      

        int a = stoi(stra);
        int b = stoi(strb);

        if(a<1||a>Common::Maze::cols-2 || b<1 || b>Common::Maze::cols-2) 
            goto initial;

        if(Common::Maze::maze[b][a]==0) run(a,b);

        else{
        
            initial:

                std::complex<int> point = initial_point();

                if(point.real() == 0 && point.imag() ==0 ){
            
                    std::cout<<
                        "the input maze is bad cuz no standable point; exiting..."<< std::endl;

                    return 1;
                }
                else{
            
                    run( point.real(), point.imag() );
                }               
        }
        
    }
    catch(exception& e) {
    
        std::complex<int> point = initial_point();

        if(point.real() == 0 && point.imag() ==0 ){
            
            std::cout<<
                "the input maze is bad cuz no standable point; exiting..."<< std::endl;

            return 1;
        }
        else{
            
            run( point.real(), point.imag() );
        }              
    }    

    return 0;
}
