# 이 fork 사용 가이드

[deskflow/deskflow](https://github.com/deskflow/deskflow)를 Windows 서버 + macOS 클라이언트 환경에 맞게 고친 fork입니다.
아래 예시에서 서버(Windows) 이름은 `WIN-PC`, 클라이언트(맥) 이름은 `my-mac.local`로 씁니다.
실제 이름으로 바꿔서 쓰세요.

## 이 fork에서 바뀐 점

| 내용 | 위치 |
| --- | --- |
| Windows 서버에서 빠르게 넘어갈 때 클라이언트 커서가 튀거나 서버로 튕기던 문제 수정 (upstream [#10030](https://github.com/deskflow/deskflow/issues/10030)) | `src/lib/platform/MSWindowsComputer.cpp` |
| Home/End를 ⌘←/⌘→(줄 처음/끝)로 보내기, 컴퓨터별 설정 | `src/lib/server/Server.cpp` |
| Ctrl ↔ Win/Command 바꾸기, 컴퓨터별 설정 | 기존 수식키 매핑(`modifierCtrl`/`modifierSuper`) 사용 |
| 위 두 설정을 GUI에서: 서버 구성 → 고급 → **클라이언트 키보드** 표 | `src/lib/gui/dialogs/ServerConfigDialog.*` |
| 외부 설정 파일을 쓸 때 서버 구성 창에서 "확인"을 누르면 컴퓨터 설정이 지워지던 문제 수정 | `ServerConfigDialog.cpp` |
| macOS 클라이언트: 한/영 키(kKeyHangul)를 F19로 보내기 | `src/lib/platform/OSXKeyState.cpp` |

## 설정

### 설정 파일 위치 (Windows)

- 앱 설정: `%APPDATA%\Deskflow\Deskflow.conf`
- 서버 설정 파일(외부 설정 파일을 쓸 때): 경로는 `Deskflow.conf`의 `[server] externalConfigFile`

설정 파일은 Deskflow를 **종료한 뒤** 고치고 다시 실행하세요. 실행 중에 고치면 GUI가 자기 값으로 덮어쓸 수 있습니다.

### Deskflow.conf 예시

```ini
[core]
coreMode=2          ; 2 = 서버
processMode=1       ; 1 = Desktop 모드 (권장, 아래 '보안' 참고)

[server]
externalConfig=true
externalConfigFile=C:/Users/<사용자>/AppData/Roaming/Deskflow/deskflow-server.conf
clipboardSize=50    ; MB. 이미지는 압축 없이 전달되므로 3MB 기본값이면 스크린샷이 잘 안 넘어감

[computer_WIN-PC]
name=WIN-PC

[computer_my-mac.local]
name=my-mac.local
homeEndLineNavigation=true   ; Home/End → ⌘←/⌘→
modifierCtrl=super           ; 서버의 Ctrl  → 맥의 Command
modifierSuper=ctrl           ; 서버의 Win   → 맥의 Ctrl
```

- 컴퓨터 이름은 클라이언트가 보내는 이름과 정확히 같아야 합니다. 맥은 보통 `xxx.local` 형태입니다. 다르면 서버 로그에 `unrecognised client name "..."`이 찍히니 그 이름을 그대로 쓰세요.
- 1.27부터 컴퓨터 목록은 서버 설정 파일의 `section: screens`가 아니라 위의 `[computer_*]` 그룹에서 읽습니다. `section: screens`는 무시됩니다.
- `homeEndLineNavigation`과 Ctrl/Win 바꾸기는 GUI의 서버 구성 → 고급 → **클라이언트 키보드** 표에서도 바꿀 수 있습니다. 외부 설정 파일을 써도 이 표는 쓸 수 있습니다.

### 서버 설정 파일 예시 (가장자리 일부만 연결)

맥을 서버 왼쪽 끝의 **아래쪽 2/5 구간**에만 연결하는 예시입니다.

```
section: links
	WIN-PC:
		left(60,100) = my-mac.local
	my-mac.local:
		right = WIN-PC(60,100)
end

section: options
end
```

- `(60,100)`은 서버 화면 전체(모든 모니터를 합친 영역) 높이의 60%~100% 구간입니다. 0이 맨 위, 100이 맨 아래입니다.
- 돌아올 때도 같은 구간으로 돌아오게 반대쪽 링크에도 같은 범위를 씁니다.
- `section: options` 블록은 비어 있어도 **꼭 있어야** 합니다. 이 블록이 없으면 `Deskflow.conf`의 서버 옵션(전환 지연, 클립보드 등)이 적용되지 않습니다.
- 배치 편집기(서버 구성 → 컴퓨터 탭)는 이런 부분 구간을 표현할 수 없어서, 외부 설정 파일을 켜면 그 탭은 비활성화됩니다.

### 맥 클라이언트 설정

- 시스템 설정 → 개인정보 보호 및 보안 → **손쉬운 사용**, **입력 모니터링**에서 Deskflow 허용
- 한/영 키: 시스템 설정 → 키보드 → 키보드 단축키 → 입력 소스 → **이전 입력 소스 선택**을 F19로 지정
  (한/영 키가 F19로 전달되므로 이 단축키로 입력 소스가 바뀝니다. 맥 클라이언트도 이 fork로 빌드해야 합니다.)

### 보안

- Windows 설치본은 기본으로 SYSTEM 권한 서비스(`Deskflow`)로 서버를 실행합니다. 이 서비스의 IPC 파이프는 모든 로컬 사용자에게 열려 있습니다.
- **Desktop 모드**(`processMode=1`)를 쓰고 서비스는 중지 + 시작 유형 '수동'으로 두는 것을 권장합니다.
  관리자 PowerShell: `Stop-Service Deskflow; Set-Service Deskflow -StartupType Manual`
- TLS(`[security] tlsEnabled=true`)와 상대 인증서 확인(`checkPeerFingerprints=true`)은 켜 두세요. 기본값입니다.

## Windows 빌드

### 준비물

- Visual Studio 2022: "C++를 사용한 데스크톱 개발" 워크로드와 vcpkg 구성 요소. CMake와 Ninja도 함께 들어 있습니다.
- Qt 6.11.3 `msvc2022_64`. 공식 설치본과 같은 버전이어야 설치본의 Qt DLL을 그대로 쓸 수 있습니다.
- 7z 압축을 풀 도구(7-Zip 또는 Python `py7zr`)

### Qt 받기

aqtinstall 3.3은 6.11.x의 새 저장소 구조를 못 찾으므로 아카이브를 직접 받습니다.

```
https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt6_6113/qt6_6113_msvc2022_64/qt.qt6.6113.win64_msvc2022_64/
```

- 이 폴더에서 `qtbase`, `qttools`, `qtsvg`, **`qtdeclarative`** 4개 `.7z`를 받습니다. 받은 뒤에는 각각 옆의 `.sha1` 파일로 체크섬을 확인합니다.
  - `qtdeclarative`가 필요한 이유: 번역 도구 `lupdate.exe`가 `Qt6Qml.dll`을 필요로 합니다.
- `download.qt.io`는 지역 미러로 리다이렉트됩니다. 느리면 같은 경로를 다른 공식 미러에서 받으세요(예: `https://mirrors.ocf.berkeley.edu/qt/online/...`).
- 4개를 한 폴더(예: `C:\qt-6.11.3`)에 풉니다.

### 빌드

빌드는 저장소의 **사본**에서 하세요. 설정 단계가 소스 폴더에 `vcpkg.json`을 만들고, `lupdate`가 `translations/*.ts`를 고쳐 쓰기 때문입니다.

```bat
call "C:\Program Files\Microsoft Visual Studio\2022\<에디션>\VC\Auxiliary\Build\vcvars64.bat"
set "VCPKG_ROOT=C:\Program Files\Microsoft Visual Studio\2022\<에디션>\VC\vcpkg"
set "PATH=C:\qt-6.11.3\bin;%PATH%"

cmake -S <소스 사본> -B <빌드 폴더> -G Ninja ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH=C:\qt-6.11.3 ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
  -DBUILD_TESTS=ON -DSKIP_BUILD_TESTS=ON -DBUILD_INSTALLER=OFF
cmake --build <빌드 폴더>
cmake --install <빌드 폴더> --prefix <스테이지 폴더>
```

- 처음 설정할 때 vcpkg가 OpenSSL을 소스에서 빌드하므로 15분 정도 걸립니다.
- 단위 테스트: 빌드 폴더의 `ServerTests.exe`, `ServerConfigTests.exe`를 Qt `bin`을 PATH에 넣고 실행합니다(예: `ServerTests.exe -o result.txt,txt`).
- 번역(`.ts`)을 고쳤다면 빌드가 `.qm`을 다시 만들지 않을 수 있으니 직접 컴파일합니다.
  `lrelease translations\deskflow_ko.ts -qm <스테이지 폴더>\translations\deskflow_ko.qm`
- 태그 없이(얕게) 클론하면 버전이 `1.27.0.9999`처럼 표시됩니다.

## 설치

1. 공식 설치본을 먼저 설치합니다: `winget install Deskflow.Deskflow`
   Qt/OpenSSL DLL, 시작 메뉴, 방화벽 규칙이 여기서 만들어집니다.
2. Deskflow를 종료합니다(트레이 아이콘 → 종료).
3. 관리자 권한으로 스테이지 폴더의 아래 파일을 `C:\Program Files\Deskflow`에 덮어씁니다.
   - `deskflow.exe`, `deskflow-core.exe`, `deskflow-daemon.exe`
   - `translations\deskflow_*.qm`

   Qt DLL은 같은 6.11.3이고 OpenSSL은 3.x끼리 호환되므로, 설치본의 DLL을 그대로 둡니다.
4. 시작 메뉴로 Deskflow를 실행합니다.

- winget 등으로 Deskflow를 업데이트하면 공식 파일로 덮어써지므로 3번을 다시 해야 합니다.
- 되돌리려면 `winget uninstall Deskflow.Deskflow` 후 `winget install Deskflow.Deskflow`로 공식 버전을 다시 설치합니다. 설정 파일은 그대로 남습니다.

## 문제 해결

- **재시작하면 설정이 예전으로 돌아간다:** 설정을 바꾼 도구가 MSIX 패키지 앱(예: Claude 데스크톱 앱) 안에서 실행됐다면, `%APPDATA%`에 쓴 내용이 그 앱 전용 사본(`%LOCALAPPDATA%\Packages\<앱>\LocalCache\Roaming\...`)으로 들어가 실제 파일에는 반영되지 않습니다. 탐색기나 일반 터미널에서 실제 파일을 고치고, 앱 전용 사본은 지우세요.
- **왼쪽 끝 전체에서 넘어간다:** `externalConfig=false`로 읽힌 경우입니다. 이때 GUI가 배치 편집기 기준으로 `C:\ProgramData\Deskflow\deskflow-server.conf`를 만들어 씁니다. 실제 `Deskflow.conf`의 `externalConfig` 값을 확인하세요.
- **클라이언트로 넘어가자마자 서버로 튕긴다 / 클립보드가 가끔 안 넘어간다:** 이 fork의 수정이 들어간 `deskflow-core.exe`인지 확인하세요(파일 버전 `1.27.0.9999`).
- **이미지 클립보드가 안 넘어간다:** 서버 로그에 `exceeds limit`가 있으면 `clipboardSize`(MB)를 올리세요.
