# wpy++ Installer

!include "MUI2.nsh"

# Define installation page options
!define MUI_INSTFILESPAGE_FINISHHEADER_TEXT "Installation Complete"
!define MUI_INSTFILESPAGE_FINISHHEADER_SUBTEXT "wpy++ has been successfully installed on your computer. Click Close to exit the installer."
!define MUI_INSTFILESPAGE_ABORTHEADER_TEXT "Installation Aborted"
!define MUI_INSTFILESPAGE_ABORTHEADER_SUBTEXT "Installation was not completed successfully."

# Define installer pages
!insertmacro MUI_PAGE_LICENSE "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\LICENSE.md"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

# Language
!insertmacro MUI_LANGUAGE "English"

# Custom function to ensure proper button behavior
Function .onInstSuccess
  # This function is called when installation succeeds
  # It ensures the Next button appears instead of Close
FunctionEnd

Outfile "wpy++-1.0.1-win64.exe"
InstallDir "C:\WICC\Python++"
VIProductVersion "1.0.1.0"
VIAddVersionKey "ProductName" "wpy++ Python++ Compiler"
VIAddVersionKey "CompanyName" "WNU-Project"
VIAddVersionKey "LegalCopyright" "© 2025 WNU-Project"
VIAddVersionKey "FileDescription" "wpy++ Python++ Compiler Installer"
VIAddVersionKey "FileVersion" "1.0.1.0"

Section "Install wpy++"
  SetOutPath "$INSTDIR"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\wpy++.exe"
SectionEnd

Section "Install wpy++ Source Code"
  SetOutPath "$INSTDIR\source code"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppgraphics.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppintoasm.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppintoasm.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppintoasm.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppintoasm32.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppintoasm32.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppintoasm32.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\tokens.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\asmintoobject.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\asmintoobject.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\asmintoobject.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\asmintoobject32.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\asmintoobject32.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\asmintoobject32.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra.asmintoobject.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra.asmintoobject.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra.objectintoexe.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra.objectintoexe.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra.pyppintoasm.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra.pyppintoasm.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra_asmintoobject.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra_objectintoexe.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\gra_pyppintoasm.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\lexer.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\lexer.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\main.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\main.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\Makefile"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\objectintoexe.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\objectintoexe.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\objectintoexe.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\objectintoexe32.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\objectintoexe32.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\objectintoexe32.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\parser.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\parser.h"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\parser.o"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppgraphics.c"
  File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\pyppgraphics.h"
SectionEnd

Section "Install Important Documents"
    SetOutPath "$INSTDIR\docs"
    File "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++\LICENSE.md"
SectionEnd
