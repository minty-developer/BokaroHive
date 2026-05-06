[Setup]
; --- 프로그램 기본 정보 ---
AppName=Bokaro Hive
AppVersion=1.0.0
AppPublisher=Bokaro Dev Team
AppPublisherURL=https://github.com/minty-developer/BokaroHive
DefaultDirName={autopf}\BokaroHive
DefaultGroupName=Bokaro Hive
SetupIconFile=C:\BOKARO-GAME\Logo\icon.ico
UninstallDisplayIcon={app}\BokaroHive.exe

; --- 빌드 및 압축 ---
Compression=lzma2/ultra64
SolidCompression=yes
OutputDir=.\Output
OutputBaseFilename=BokaroHive_Full_Setup
PrivilegesRequired=admin

; 라이선스 파일 연결
LicenseFile=C:\BOKARO-GAME\license.txt

[Languages]
Name: "korean"; MessagesFile: "compiler:Languages\Korean.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "firewall"; Description: "윈도우 방화벽 예외 등록"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
; 메인 실행 파일
Source: "C:\BOKARO-GAME\BokaroHive.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\BOKARO-GAME\*.dll"; DestDir: "{app}"; Flags: ignoreversion

; [기능 1] 폰트 자동 시스템 등록 (사용자 PC에 폰트 설치)
; 폰트 파일명이 'GameFont.ttf'라고 가정합니다. 실제 파일명으로 수정하세요.
Source: "C:\BOKARO-GAME\fonts\*.ttf"; DestDir: "{fonts}"; FontInstall: "Bokaro Custom Font"; Flags: certainfont onlyifdestfileexists uninsneveruninstall

; 리소스 폴더
Source: "C:\BOKARO-GAME\fonts\*"; DestDir: "{app}\fonts"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\BOKARO-GAME\images\*"; DestDir: "{app}\images"; Flags: ignoreversion recursesubdirs createallsubdirs

; [기능 2] 읽어보기(Readme) 파일
Source: "C:\BOKARO-GAME\readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\Bokaro Hive"; Filename: "{app}\BokaroHive.exe"; IconFilename: "{app}\images\icon.ico"
Name: "{group}\{cm:UninstallProgram,Bokaro Hive}"; Filename: "{uninstaller}"
Name: "{autodesktop}\Bokaro Hive"; Filename: "{app}\BokaroHive.exe"; Tasks: desktopicon; IconFilename: "{app}\images\icon.ico"

[Registry]
; [기능 3] 레지스트리에 설치 경로 및 버전 정보 저장
Root: HKLM; Subkey: "Software\BokaroDev\BokaroHive"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\BokaroDev\BokaroHive"; ValueType: string; ValueName: "Version"; ValueData: "1.0.0"; Flags: uninsdeletekey

[Run]
; [기능 4] 방화벽 예외 등록 실행 (netsh 명령 활용)
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall add rule name=""Bokaro Hive"" dir=in action=allow program=""{app}\BokaroHive.exe"" enable=yes"; Tasks: firewall; Flags: runhidden
; 설치 완료 후 프로그램 실행
Filename: "{app}\BokaroHive.exe"; Description: "{cm:LaunchProgram,Bokaro Hive}"; Flags: nowait postinstall skipifsilent
; 설치 완료 후 메모장으로 읽어보기 표시
Filename: "{win}\notepad.exe"; Parameters: "{app}\readme.txt"; Description: "사용 방법 읽어보기"; Flags: postinstall shellexec unchecked

[UninstallRun]
; 삭제 시 방화벽 규칙도 함께 제거
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall delete rule name=""Bokaro Hive"""; Flags: runhidden

[Code]
// [기능 5] 프로그램 중복 실행 방지 및 구버전 감지 로직
function InitializeSetup(): Boolean;
var
  V: Cardinal;
begin
  Result := True;
  // 실행 중인지 체크
  if IsAppRunning('BokaroHive.exe') then
  begin
    MsgBox('프로그램이 현재 실행 중입니다. 종료 후 다시 시도해 주세요.', mbError, MB_OK);
    Result := False;
  end;
end;

// [기능 6] 설치 완료 시 브라우저 호출 (선택 사항)
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // 설치 성공 후 안내 페이지를 띄우고 싶다면 아래 주석 해제
    // ShellExec('open', 'https://yourwebsite.com/welcome', '', '', SW_SHOW, ewNoWait, V);
  end;
end;