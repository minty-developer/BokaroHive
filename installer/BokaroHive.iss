[Setup]
; --- 프로그램 기본 정보 ---
AppName=Bokaro Hive
AppVersion=1.0.0
AppPublisher=Bokaro Dev Team
AppPublisherURL=https://github.com/minty-developer/BokaroHive
AppSupportURL=https://github.com/minty-developer/BokaroHive/issues
AppUpdatesURL=https://github.com/minty-developer/BokaroHive/releases
AppMutex=BokaroHiveMutex

; [수정] DefaultDirName은 URL이 아니라 실제 설치될 경로(C:\Program Files\...)여야 합니다.
DefaultDirName={autopf}\Bokaro Hive
DefaultGroupName=Bokaro Hive

; [권장] 상대 경로 사용 (iss 파일이 installer 폴더에 있다면 ..\ 로 상위 폴더 참조)
SetupIconFile=..\assets\Logo\icon.ico
UninstallDisplayIcon={app}\BokaroHive.exe

; --- 빌드 및 압축 ---
Compression=lzma2/ultra64
SolidCompression=yes
OutputDir=..\Output
OutputBaseFilename=BokaroHive_Setup_v1.0.0
PrivilegesRequired=admin

; [수정] 라이선스 파일 연결 (마크다운 대신 txt 권장)
LicenseFile=..\License.txt

[Languages]
Name: "korean"; MessagesFile: "compiler:Languages\Korean.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "firewall"; Description: "윈도우 방화벽 예외 등록"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
; [수정] 모든 경로를 프로젝트 폴더 구조에 맞춰 상대 경로로 변경
Source: "..\bin\BokaroHive.exe"; DestDir: "{app}"; Flags: ignoreversion
;Source: "..\bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion

; 폰트 자동 시스템 등록
Source: "..\assets\fonts\*.ttf"; DestDir: "{fonts}"; FontInstall: "Bokaro Custom Font"; Flags: onlyifdestfileexists uninsneveruninstall

; 리소스 폴더
Source: "..\assets\fonts\*"; DestDir: "{app}\assets\fonts"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\assets\images\*"; DestDir: "{app}\assets\images"; Flags: ignoreversion recursesubdirs createallsubdirs

; 읽어보기(Readme) 파일
Source: "..\README.md"; DestDir: "{app}"; DestName: "README.txt"; Flags: isreadme

[Icons]
; 아이콘 경로 수정
Name: "{group}\Bokaro Hive"; Filename: "{app}\BokaroHive.exe"; IconFilename: "{app}\assets\images\icon.ico"
Name: "{group}\Bokaro Hive 프로그램 삭제"; Filename: "{app}\unins000.exe"
Name: "{autodesktop}\Bokaro Hive"; Filename: "{app}\BokaroHive.exe"; Tasks: desktopicon; IconFilename: "{app}\assets\images\icon.ico"

[Registry]
Root: HKLM; Subkey: "Software\BokaroDev\BokaroHive"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\BokaroDev\BokaroHive"; ValueType: string; ValueName: "Version"; ValueData: "1.0.0"; Flags: uninsdeletekey

[Run]
; 방화벽 등록
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall add rule name=""Bokaro Hive"" dir=in action=allow program=""{app}\BokaroHive.exe"" enable=yes"; Tasks: firewall; Flags: runhidden
; 설치 완료 후 실행
Filename: "{app}\BokaroHive.exe"; Description: "{cm:LaunchProgram,Bokaro Hive}"; Flags: nowait postinstall skipifsilent
; 마크다운 대신 메모장으로 읽을 수 있게 변환된 파일 열기
Filename: "{win}\notepad.exe"; Parameters: "{app}\README.txt"; Description: "사용 방법 읽어보기"; Flags: postinstall shellexec unchecked

[UninstallRun]
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall delete rule name=""Bokaro Hive"""; Flags: runhidden

[Code]
// 프로그램 중복 실행 방지
function InitializeSetup(): Boolean;
begin
  Result := True;
end;