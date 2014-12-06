; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "3D_SCAN.h"
LastPage=0

ClassCount=5
Class1=CMy3D_SCANApp
Class2=CMy3D_SCANDoc
Class3=CMy3D_SCANView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Class5=CAboutDlg
Resource3=IDD_DIALOG1

[CLS:CMy3D_SCANApp]
Type=0
HeaderFile=3D_SCAN.h
ImplementationFile=3D_SCAN.cpp
Filter=N
LastObject=CMy3D_SCANApp

[CLS:CMy3D_SCANDoc]
Type=0
HeaderFile=3D_SCANDoc.h
ImplementationFile=3D_SCANDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC

[CLS:CMy3D_SCANView]
Type=0
HeaderFile=3D_SCANView.h
ImplementationFile=3D_SCANView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_EDIT_ScanStart




[CLS:CAboutDlg]
Type=0
HeaderFile=3D_SCAN.cpp
ImplementationFile=3D_SCAN.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_STARTSNAP
Command2=ID_STOPSNAP
Command3=IDC_ScanSet
Command4=ID_EDIT_ScanStart
Command5=ID_EDIT_ScanStop
CommandCount=5

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[DLG:IDD_DIALOG1]
Type=1
Class=CMainFrame
ControlCount=7
Control1=IDC_EDIT_SERIAL,edit,1350631552
Control2=IDC_SERIAL,static,1342308352
Control3=IDC_EDIT_STEP,edit,1350631552
Control4=IDC_SERIAL2,static,1342308352
Control5=IDC_SERIAL3,static,1342308352
Control6=IDC_EDIT_DEGREE,edit,1350631552
Control7=IDC_BUTTON_OK,button,1342242816

