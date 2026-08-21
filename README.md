# Ergonude
자체 설계 일체형 에르고노믹 키보드의 [ZMK](https://zmk.dev) 펌웨어 설정 저장소.

<img width="1273" height="385" alt="스크린샷 2026-08-21 090238" src="https://github.com/user-attachments/assets/2ca57165-14b6-41d3-aba3-f4a2405d6b8b" />

## 하드웨어

| 항목 | 사양 |
|------|------|
| 컨트롤러 | nRFmicro 1.3 (Nordic nRF52840) |
| 폼팩터 | 일체형 스플릿 배치 (좌우 블록 + 중앙 열), 약 74키 |
| 매트릭스 | 5행 × 19열, col2row 다이오드 |
| 연결 | USB / Bluetooth LE 듀얼 |
| RGB | WS2812 언더글로우 12구 (SPI 구동) |
| 전원 | USB 유선 전원 (배터리 미장착) |

## 특징

- **원터치 OS 전환** — Windows(USB)와 Mac(BLE)을 매크로 하나로 전환.
  출력 채널, 기본 레이어, RGB 색상(Windows 주황 / Mac 파랑)이 함께 바뀌어
  LED 색만 봐도 현재 모드를 알 수 있다.
- **OS별 대칭 레이어** — fn/nav 레이어의 단축키가 OS에 맞게 번역돼 있다
  (예: 복사가 Win 레이어에선 `Ctrl+C`, Mac 레이어에선 `Cmd+C`).
- **Caps Lock RGB 인디케이터** — Caps가 켜지면 언더글로우가 브리딩으로 전환.
  호스트 OS가 보내는 HID indicator(Caps LED 신호)를 구독하는 커스텀 모듈이라
  어떤 경로로 Caps를 바꿔도 LED가 실제 상태와 항상 일치한다.
- **키보드 마우스** — nav 레이어에서 포인터 이동·클릭 지원 (`CONFIG_ZMK_POINTING`).
- **한/영 전환 다중 경로** — `Shift+Space` 조합을 콤보·mod-tap·레이어 매핑으로
  겹겹이 배치해 어떤 타이밍으로 눌러도 전환이 씹히지 않는다.
- **NKRO + 1ms USB 폴링**, ZMK Studio 지원 준비(현재 비활성).

## 레이어

| # | 이름 | 설명 |
|---|------|------|
| 0 | `default_win` | Windows 기본 |
| 1 | `fn_win` | F1–F12, 숫자패드, Windows 단축키 |
| 2 | `nav_win` | 화살표·PgUp/Dn·클립보드, 마우스 키 |
| 3 | `default_mac` | Mac 기본 |
| 4 | `fn_mac` | F1–F12, 숫자패드, Mac 단축키 |
| 5 | `nav_mac` | nav_win의 Mac 버전 |
| 6 | `bt-rgb` | OS 전환, 블루투스 관리, 리셋/부트로더 |

- 왼엄지 스페이스: 탭 = Space, 홀드 = nav 레이어 (`&lt`, retro-tap)
- 오른엄지: 탭 = 한/영(`Shift+Space`), 홀드 = Ctrl
- 레이어 6 진입: 양 엄지 바깥쪽 키 홀드

## 저장소 구조

```
config/
  ergonude.keymap # 키맵·매크로·콤보 정의
  ergonude.conf # 기능 설정 (RGB, BLE, HID indicator 등)
  west.yml # ZMK 버전 고정 (v0.3)
boards/shields/ergonude/
  ergonude.overlay # 매트릭스 핀맵, transform
  ergonude.dtsi # WS2812 SPI 설정
  ergonude-layouts.dtsi # 물리 레이아웃 (ZMK Studio용)
modules/
  behaviors/caps_indicator_rgb.c # Caps 인디케이터 RGB 커스텀 모듈
```



## 빌드

푸시하면 GitHub Actions가 자동으로 빌드한다 (ZMK 공식 `build-user-config` 워크플로우). 결과물은 Actions 탭 → 해당 런의 아티팩트:

- `ergonude nrfmicro_13-zmk.uf2` — 본 펌웨어
- `settings_reset nrfmicro_13-zmk.uf2` — 설정(본딩 포함) 초기화용


## 플래싱
1. 리셋 버튼 더블탭 → 부트로더 진입 (USB 드라이브로 마운트)
2. uf2 파일을 드라이브에 복사 → 자동 재부팅


BLE 페어링 정보가 꼬였거나 HID 구성이 바뀐 펌웨어를 올린 뒤에는
호스트에서 기기를 제거하고 재페어링한다 (키보드 쪽은 레이어 6의 `BT_CLR_ALL`).


## 커스터마이징
- **키맵**: `config/ergonude.keymap`
- **Caps 브리딩 속도**: `modules/behaviors/caps_indicator_rgb.c`의 `BREATHE_PERIOD_MS` (기본 800ms/사이클)
- **RGB 기본 색/밝기**: `config/ergonude.conf`의 `CONFIG_ZMK_RGB_UNDERGLOW_*` 

