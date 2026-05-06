#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "raylib.h"

// [ 상수 정의 ]
#define GAME_TITLE "BOKARO GAME!!"
#define GAME_VERSION "v0.1.0(alpha)"

// [ 전역 변수 ]
Font nanumFont;

// [ 열거형 및 구조체 정의 ]
// 게임 화면 상태
typedef enum {
    SCREEN_MENU,
    SCREEN_SETTINGS,
    SCREEN_GAMES,
    SCREEN_GAMES_MAIN,
    SCREEN_GAMES_MAIN_BAG,
    SCREEN_GAMES_MAIN_CREATE,
    SCREEN_GAMES_MAIN_ADVENTURE,
    SCREEN_EXIT
} SCREENS;

// 엔티티(캐릭터/오브젝트) 구조체
typedef struct {
    Vector2 position;
    Texture2D texture;
} Entity;

// 캐릭터 종류
typedef enum {
    PLAYER,
    HATSUNE_MIKU,
    KASANE_TETO
} CHARACTERS;

// 아이템 ID 리스트
typedef enum {
    ITEM_BLANK,
    ITEM_LEEK,
    // 기초 재료
    ITEM_MIKU_FRAGMENT, ITEM_TETO_FRAGMENT, ITEM_BROKEN_CIRCUIT,
    ITEM_RUSTY_METAL, ITEM_MYSTERIOUS_LIQUID, ITEM_OLD_FABRIC, ITEM_BLUE_FLARE,
    // 일반 및 스토리 아이템
    ITEM_BAGUETTE, ITEM_FLUOROBONG, ITEM_HYPNOSIS_COIN, ITEM_BAUMKUCHEN,
    ITEM_MELT_ICECREAM, ITEM_EXECUTION_GLOVES, ITEM_SUNGLASSES, ITEM_MIC_TETORIS,
    // 미쿠의 사랑(흉기) 시리즈
    ITEM_MAGIC_WAND, ITEM_HAMMER, ITEM_SCISSOR, ITEM_BASEBALL_BAT,
    ITEM_AXE, ITEM_STUN_GUN, ITEM_SHOVEL, ITEM_CHAIN_CUTTER,
    ITEM_CUTTER_KNIFE, ITEM_KITCHEN_KNIFE, ITEM_KATANA_JAPAN, ITEM_PICKAXE
} ITEMS;

// 인벤토리 조작 옵션
typedef enum { BAG_ADD, BAG_SUB } BAG_OPTION;

// 아이템 정보 상세 구조체
typedef struct {
    ITEMS id;
    char name[60];
    char desc[400];
    bool IsCanUse;
} ItemInfo;

// 인벤토리 슬롯 구조체
typedef struct {
    ITEMS item_id;
    int count;
} InventorySlot;

// 제작 레시피 구조체
typedef struct {
    ITEMS material1; int count1;
    ITEMS material2; int count2;
    ITEMS result;
} CraftRecipe;

// 저장 데이터 구조체
typedef struct {
    InventorySlot bag[100];
    float playerHP;
} SaveData;

// [ 데이터베이스 설정 ]
// 아이템 도감 데이터
ItemInfo ItemDatabase[] = {
    {ITEM_BLANK, "빈 칸", "", false},
    {ITEM_LEEK, "신선한 대파", "누군가의 소중한 상징이었던 대파입니다.", false},
    {ITEM_MIKU_FRAGMENT, "미쿠의 데이터 조각", "전자 가희의 목소리가 미세하게 울리는 푸른 파편입니다.", false},
    {ITEM_TETO_FRAGMENT, "테토의 데이터 조각", "장난스러운 에너지가 느껴지는 붉은색 데이터 파편입니다.", false},
    {ITEM_BROKEN_CIRCUIT, "망가진 회로", "복원에 사용 가능한 전자기기 부품입니다.", false},
    {ITEM_RUSTY_METAL, "녹슨 금속", "도구 제작에 적합한 금속입니다.", false},
    {ITEM_MYSTERIOUS_LIQUID, "정체불명의 액체", "달콤한 향이 나는 분홍색 액체입니다.", false},
    {ITEM_OLD_FABRIC, "오래된 천", "부드러운 옷가지 파편입니다.", false},
    {ITEM_BLUE_FLARE, "푸른 인광", "데이터가 실체화된 신비로운 에너지입니다.", false},
    {ITEM_BAGUETTE, "바게트", "누군가가 가장 좋아했던 아이템이라고 전해집니다.", false},
    {ITEM_FLUOROBONG, "형광봉", "보카로 하이브 잔해에서 발견된 노래의 파편. 누군가와 미쿠가 함께 부른 노래 같습니다.", false},
    {ITEM_HYPNOSIS_COIN, "최면 동전", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 기억이 조금 남아있는 듯합니다.", false},
    {ITEM_BAUMKUCHEN, "바움쿠헨", "보카로 하이브 잔해에서 발견된 노래의 파편. 누군가의 슬픈 기억이 서려 있습니다.", false},
    {ITEM_MELT_ICECREAM, "녹아버린 아이스크림", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠가 들고 있었던 것 같습니다.", false},
    {ITEM_EXECUTION_GLOVES, "처형 박수 장갑", "보카로 하이브 잔해에서 발견된 노래의 파편. 왠지 모르게 붉은 빛이 감돕니다.", false},
    {ITEM_SUNGLASSES, "선글라스", "보카로 하이브 잔해에서 발견된 노래의 파편. 너무 어두워 아무것도 보이지 않습니다.", false},
    {ITEM_MIC_TETORIS, "마이크", "보카로 하이브 잔해에서 발견된 노래의 파편. 중독성 있는 멜로디가 흘러나올 것 같습니다.", false},
    {ITEM_MAGIC_WAND, "마법 지팡이", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_HAMMER, "망치", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_SCISSOR, "가위", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_BASEBALL_BAT, "야구 배트", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_AXE, "도끼", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_STUN_GUN, "전기 충격기", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_SHOVEL, "삽", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_CHAIN_CUTTER, "체인 커터", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_CUTTER_KNIFE, "커터칼", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_KITCHEN_KNIFE, "식칼", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_KATANA_JAPAN, "일본도", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false},
    {ITEM_PICKAXE, "곡괭이", "보카로 하이브 잔해에서 발견된 노래의 파편. 미쿠의 사랑이 느껴집니다.", false}};

CraftRecipe RecipeDatabase[] = {
    // [일반 아이템 제작]
    {ITEM_TETO_FRAGMENT, 5, ITEM_OLD_FABRIC, 1, ITEM_BAGUETTE},             // 테토의 데이터5 + 천1 = 바게트
    {ITEM_BLUE_FLARE, 2, ITEM_BROKEN_CIRCUIT, 1, ITEM_FLUOROBONG},          // 인광2 + 회로1 = 형광봉
    {ITEM_MIKU_FRAGMENT, 10, ITEM_RUSTY_METAL, 1, ITEM_HYPNOSIS_COIN},      // 데이터10 + 금속1 = 최면 동전
    {ITEM_MYSTERIOUS_LIQUID, 3, ITEM_MIKU_FRAGMENT, 2, ITEM_MELT_ICECREAM}, // 액체3 + 데이터2 = 녹은 아이스크림
    {ITEM_BROKEN_CIRCUIT, 2, ITEM_MIKU_FRAGMENT, 5, ITEM_MIC_TETORIS},      // 회로2 + 데이터5 = 마이크

    // [Reincarnation 시리즈 제작 - 흉기류]
    {ITEM_RUSTY_METAL, 3, ITEM_MIKU_FRAGMENT, 2, ITEM_CUTTER_KNIFE},  // 금속3 + 데이터2 = 커터칼
    {ITEM_RUSTY_METAL, 5, ITEM_MIKU_FRAGMENT, 3, ITEM_KITCHEN_KNIFE}, // 금속5 + 데이터3 = 식칼
    {ITEM_RUSTY_METAL, 8, ITEM_BLUE_FLARE, 1, ITEM_KATANA_JAPAN},     // 금속8 + 인광1 = 일본도
    {ITEM_RUSTY_METAL, 4, ITEM_BROKEN_CIRCUIT, 1, ITEM_AXE},          // 금속4 + 회로1 = 도끼
    {ITEM_RUSTY_METAL, 6, ITEM_MIKU_FRAGMENT, 4, ITEM_PICKAXE},       // 금속6 + 데이터4 = 곡괭이

    // 1. 클래식 복원 (멜트 & 최면)
    {ITEM_MELT_ICECREAM, 1, ITEM_LEEK, 1, ITEM_FLUOROBONG},
    {ITEM_HYPNOSIS_COIN, 3, ITEM_SUNGLASSES, 1, ITEM_BAUMKUCHEN},

    // 2. 테토의 탄생
    {ITEM_BAGUETTE, 1, ITEM_MIC_TETORIS, 1, ITEM_KATANA_JAPAN},

    // 3. [위험] 미쿠의 지독한 사랑 (Reincarnation 시리즈 결합)
    // 날붙이들을 모아 더 날카로운 사랑을 만듭니다.
    {ITEM_CUTTER_KNIFE, 1, ITEM_KITCHEN_KNIFE, 1, ITEM_SCISSOR},
    {ITEM_AXE, 1, ITEM_HAMMER, 1, ITEM_BASEBALL_BAT},
    {ITEM_SHOVEL, 1, ITEM_PICKAXE, 1, ITEM_CHAIN_CUTTER},

    // 4. 최종 복원 (모든 사랑의 결정체)
    {ITEM_MAGIC_WAND, 1, ITEM_STUN_GUN, 1, ITEM_EXECUTION_GLOVES}};

int TotalRecipes = sizeof(RecipeDatabase) / sizeof(RecipeDatabase[0]);

// [ 게임 상태 변수 ]
InventorySlot MyBag[100] = { {ITEM_LEEK, 1}, {ITEM_MIKU_FRAGMENT, 10}, {ITEM_OLD_FABRIC, 5} };
SCREENS state_screen = SCREEN_MENU;
int pointer_position = 1;  // 메뉴 선택 포인터
int recipe_pointer = 0;    // 제작창 선택 포인터

// 알림 피드백 관련
char craftStatusMessage[100] = "";
float statusTimer = 0.0f;
Color statusColor = WHITE;

// 어드벤처 관련
bool isAdventuring = false;
float adventureTimer = 0.0f;
float playerHP = 100.0f;
float logTimer = 0.0f;
char adventureLogs[3][100] = { "", "", "" };
bool eventActive = false;
int activeEventType = 0;
float eventTimer = 0.0f;
int adventureStep = 0;

// [ 함수 정의 ]

// 로그 시스템: 새로운 로그를 추가하고 기존 로그를 한 칸씩 올림
void AddLog(const char* newLog) {
    strcpy(adventureLogs[0], adventureLogs[1]);
    strcpy(adventureLogs[1], adventureLogs[2]);
    strcpy(adventureLogs[2], newLog);
}

// 범위 제한 함수: 포인터 값이 Min~Max를 벗어나면 순환시킴
void ToAbleValue(int Min, int Max, int *Value) {
    if (*Value > Max) *Value = Min;
    else if (*Value < Min) *Value = Max;
}

// 인벤토리 관리 함수 (추가/제거)
bool BAG(BAG_OPTION Option, ITEMS ItemID, int Value) {
    if (Option == BAG_ADD) {
        // 중복 아이템 확인 및 수량 증가
        for (int i = 0; i < 100; i++) {
            if (MyBag[i].item_id == ItemID && MyBag[i].item_id != ITEM_BLANK) {
                MyBag[i].count += Value;
                return true;
            }
        }
        // 새 슬롯에 추가
        for (int i = 0; i < 100; i++) {
            if (MyBag[i].item_id == ITEM_BLANK) {
                MyBag[i].item_id = ItemID;
                MyBag[i].count = Value;
                return true;
            }
        }
    } else if (Option == BAG_SUB) {
        for (int i = 0; i < 100; i++) {
            if (MyBag[i].item_id == ItemID && MyBag[i].count >= Value) {
                MyBag[i].count -= Value;
                if (MyBag[i].count <= 0) MyBag[i].item_id = ITEM_BLANK;
                return true;
            }
        }
    }
    return false;
}

// 아이템 제작 로직
void CraftItem(CraftRecipe recipe) {
    bool hasMat1 = false, hasMat2 = (recipe.material2 == ITEM_BLANK);

    // 재료 확인
    for(int i=0; i<100; i++) {
        if(MyBag[i].item_id == recipe.material1 && MyBag[i].count >= recipe.count1) hasMat1 = true;
        if(recipe.material2 != ITEM_BLANK && MyBag[i].item_id == recipe.material2 && MyBag[i].count >= recipe.count2) hasMat2 = true;
    }

    if (hasMat1 && hasMat2) {
        BAG(BAG_SUB, recipe.material1, recipe.count1);
        if (recipe.material2 != ITEM_BLANK) BAG(BAG_SUB, recipe.material2, recipe.count2);
        BAG(BAG_ADD, recipe.result, 1);
        sprintf(craftStatusMessage, "[성공] %s 제작 완료!", ItemDatabase[recipe.result].name);
        statusColor = SKYBLUE;
    } else {
        sprintf(craftStatusMessage, "[실패] 재료가 부족합니다!");
        statusColor = RED;
    }
    statusTimer = 2.5f;
}

// 데이터 저장/불러오기
void SaveGame(void) {
    SaveData data;
    memcpy(data.bag, MyBag, sizeof(MyBag));
    data.playerHP = playerHP;
    FILE *file = fopen("savegame.dat", "wb");
    if (file) {
        fwrite(&data, sizeof(SaveData), 1, file);
        fclose(file);
        sprintf(craftStatusMessage, "[저장] 진행 상황이 저장되었습니다.");
        statusColor = LIME; statusTimer = 2.0f;
    }
}

void LoadGame(void) {
    SaveData data;
    FILE *file = fopen("savegame.dat", "rb");
    if (file) {
        fread(&data, sizeof(SaveData), 1, file);
        fclose(file);
        memcpy(MyBag, data.bag, sizeof(MyBag));
        playerHP = data.playerHP;
        sprintf(craftStatusMessage, "[로드] 데이터를 불러왔습니다.");
        statusColor = GOLD; statusTimer = 2.0f;
    }
}

// [ 메인 함수 ]
int main(void) {
    InitWindow(1280, 720, "Bokaro Hive v0.2.0");
    SetExitKey(KEY_NULL); // ESC 종료 방지

    // 폰트 및 리소스 로드 (한글 지원 설정)
    int ranges[2][2] = { {32, 126}, {0xAC00, 0xD7A3} };
    int countTotal = (126-32+1) + (0xD7A3-0xAC00+1);
    int *codepoints = (int *)malloc(sizeof(int) * countTotal);
    int offset = 0;
    for (int i = 0; i < 2; i++) 
        for (int cp = ranges[i][0]; cp <= ranges[i][1]; cp++) codepoints[offset++] = cp;

    nanumFont = LoadFontEx("fonts/NanumGothic.ttf", 32, codepoints, countTotal);
    free(codepoints);
    Texture2D leekTexture = LoadTexture("images/leek.png");

    SetTargetFPS(60);

    // 메인 게임 루프
    while (!WindowShouldClose()) {
        if (statusTimer > 0) statusTimer -= GetFrameTime();

        // 공통 입력: ESC (이전 화면)
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (state_screen == SCREEN_GAMES_MAIN) state_screen = SCREEN_MENU;
            else if (state_screen >= SCREEN_SETTINGS) state_screen = SCREEN_GAMES_MAIN;
        }

        // --- 화면별 로직 업데이트 및 입력 처리 ---
        switch (state_screen) {
            case SCREEN_MENU:
                if (IsKeyPressed(KEY_W)) { pointer_position--; ToAbleValue(1, 3, &pointer_position); }
                if (IsKeyPressed(KEY_S)) { pointer_position++; ToAbleValue(1, 3, &pointer_position); }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (pointer_position == 1) state_screen = SCREEN_GAMES_MAIN;
                    else if (pointer_position == 2) state_screen = SCREEN_SETTINGS;
                    else if (pointer_position == 3) goto EXIT_GAME;
                }
                break;

            case SCREEN_GAMES_MAIN:
                if (IsKeyPressed(KEY_B)) state_screen = SCREEN_GAMES_MAIN_BAG;
                if (IsKeyPressed(KEY_C)) state_screen = SCREEN_GAMES_MAIN_CREATE;
                if (IsKeyPressed(KEY_A)) state_screen = SCREEN_GAMES_MAIN_ADVENTURE;
                if (IsKeyPressed(KEY_S)) SaveGame();
                break;

            case SCREEN_GAMES_MAIN_CREATE:
                if (IsKeyPressed(KEY_W)) { recipe_pointer--; ToAbleValue(0, TotalRecipes - 1, &recipe_pointer); }
                if (IsKeyPressed(KEY_S)) { recipe_pointer++; ToAbleValue(0, TotalRecipes - 1, &recipe_pointer); }
                if (IsKeyPressed(KEY_ENTER)) CraftItem(RecipeDatabase[recipe_pointer]);
                break;

            case SCREEN_GAMES_MAIN_ADVENTURE:
                // 1. 배경 및 제목
                DrawTextEx(nanumFont, "--- [ 데이터 하이브 탐사 ] ---", (Vector2){100, 60}, 35, 2, SKYBLUE);
                
                // 2. 동력(HP) 게이지 그리기
                DrawTextEx(nanumFont, TextFormat("탐사 동력: %d%%", (int)playerHP), (Vector2){950, 70}, 20, 2, playerHP < 30 ? RED : GREEN);
                DrawRectangle(950, 100, 200, 15, DARKGRAY);
                DrawRectangle(950, 100, (int)(200 * (playerHP / 100.0f)), 15, playerHP < 30 ? RED : GREEN);

                // 3. 로그 박스 (중앙 검은 상자)
                DrawRectangle(100, 150, 1080, 400, Fade(BLACK, 0.7f));
                DrawRectangleLines(100, 150, 1080, 400, isAdventuring ? SKYBLUE : GRAY);

                // 4. 실시간 로그 출력 (3줄)
                for (int i = 0; i < 3; i++) {
                    // 아래로 갈수록 진해지는 연출
                    Color logColor = Fade(WHITE, 0.3f + (i * 0.35f));
                    DrawTextEx(nanumFont, adventureLogs[i], (Vector2){150, 250 + (i * 60)}, 26, 2, logColor);
                }

                // 5. 하단 상태 알림
                if (isAdventuring) {
                    float progress = (5.0f - adventureTimer) / 5.0f;
                    DrawRectangle(150, 480, (int)(980 * progress), 10, SKYBLUE); // 진행바
                    DrawTextEx(nanumFont, "데이터 동기화 및 회수 중...", (Vector2){500, 510}, 20, 2, SKYBLUE);
                } else {
                    if (playerHP > 10.0f) {
                        DrawTextEx(nanumFont, "SPACE 키를 눌러 탐사를 시작하세요", (Vector2){450, 480}, 22, 2, YELLOW);
                    } else {
                        DrawTextEx(nanumFont, "동력이 부족합니다! 기지에서 휴식하세요.", (Vector2){430, 480}, 22, 2, RED);
                    }
                }

                DrawTextEx(nanumFont, "ESC: 기지로 귀환", (Vector2){100, 650}, 18, 2, DARKGRAY);
                break;
            case SCREEN_SETTINGS:
            case SCREEN_GAMES:
            case SCREEN_GAMES_MAIN_BAG:
            case SCREEN_EXIT:
                // 이 화면들에서는 별도의 전용 키 입력 처리가 없음 (ESC 공통 처리 제외)
                break;
            default: break;
        }

        // --- 렌더링 시작 ---
        BeginDrawing();
        ClearBackground(BLACK);

            switch (state_screen)
            {
                case SCREEN_MENU:
                    DrawTextEx(nanumFont, "보카로 하이브 (BOKARO HIVE)", (Vector2){400, 100}, 40, 2, SKYBLUE);
                    DrawTextEx(nanumFont, pointer_position == 1 ? "> 시작하기" : "  시작하기", (Vector2){540, 350}, 30, 2, (pointer_position == 1) ? YELLOW : WHITE);
                    DrawTextEx(nanumFont, pointer_position == 2 ? "> 도움말" : "  도움말", (Vector2){540, 400}, 30, 2, (pointer_position == 2) ? YELLOW : WHITE);
                    DrawTextEx(nanumFont, pointer_position == 3 ? "> 종료" : "  종료", (Vector2){540, 450}, 30, 2, (pointer_position == 3) ? YELLOW : WHITE);
                    break;

                case SCREEN_GAMES_MAIN:
                    if (leekTexture.id > 0) DrawTextureEx(leekTexture, (Vector2){540, 200}, 0, 5.0f, WHITE);
                    DrawTextEx(nanumFont, "메인 기지 가동 중...", (Vector2){510, 450}, 25, 2, RAYWHITE);
                    DrawTextEx(nanumFont, "[A] 탐험 | [B] 가방 | [C] 제작 | [S] 저장 | [ESC] 뒤로", (Vector2){300, 600}, 20, 2, GRAY);
                    if (statusTimer > 0) {
                        float alpha = (statusTimer > 0.5f) ? 1.0f : statusTimer * 2.0f;
                        DrawTextEx(nanumFont, craftStatusMessage, (Vector2){500, 550}, 20, 2, Fade(statusColor, alpha));
                    }
                    break;
                    
                case SCREEN_GAMES_MAIN_BAG:
                    DrawTextEx(nanumFont, "--- [ 인벤토리 ] ---", (Vector2){100, 80}, 35, 2, GREEN);
                    DrawTextEx(nanumFont, "ESC를 눌러 돌아가기", (Vector2){100, 650}, 20, 2, DARKGRAY);
                    int itemY = 160;
                    for (int i = 0; i < 100; i++) {
                        if (MyBag[i].item_id != ITEM_BLANK) {
                            DrawTextEx(nanumFont, TextFormat("- %s (x%d)", ItemDatabase[MyBag[i].item_id].name, MyBag[i].count), (Vector2){120, itemY}, 25, 2, WHITE);
                            itemY += 40;
                        }
                    }
                    break;

                case SCREEN_GAMES_MAIN_CREATE:
                    DrawTextEx(nanumFont, "--- [ 아이템 제작 ] ---", (Vector2){100, 60}, 35, 2, ORANGE);
                    DrawTextEx(nanumFont, "W/S: 선택, ENTER: 제작, ESC: 뒤로가기", (Vector2){120, 110}, 18, 2, GRAY);
                    
                    // --- [ 수정된 리스트 출력 로직 ] ---
                    int maxDisplay = 10; // 화면에 한 번에 보여줄 최대 개수
                    int startIdx = 0;
                    
                    // 선택된 커서가 화면 중간쯤 오도록 시작 인덱스 조절
                    if (recipe_pointer >= maxDisplay) {
                        startIdx = recipe_pointer - (maxDisplay / 2);
                    }
                    // 인덱스가 범위를 벗어나지 않도록 방어 코드
                    if (startIdx + maxDisplay > TotalRecipes) {
                        startIdx = TotalRecipes - maxDisplay;
                    }
                    if (startIdx < 0) startIdx = 0;

                    int recipeY = 160;
                    for (int i = startIdx; i < startIdx + maxDisplay && i < TotalRecipes; i++) {
                        Color textColor = (i == recipe_pointer) ? YELLOW : WHITE;
                        const char* pointer = (i == recipe_pointer) ? "> " : "  ";
                        
                        ItemInfo res = ItemDatabase[RecipeDatabase[i].result];
                        ItemInfo m1 = ItemDatabase[RecipeDatabase[i].material1];
                        
                        char recipeText[256];
                        if (RecipeDatabase[i].material2 != ITEM_BLANK) {
                            ItemInfo m2 = ItemDatabase[RecipeDatabase[i].material2];
                            sprintf(recipeText, "%s[%s] 필요: %s(%d) + %s(%d)", 
                                    pointer, res.name, m1.name, RecipeDatabase[i].count1, m2.name, RecipeDatabase[i].count2);
                        } else {
                            sprintf(recipeText, "%s[%s] 필요: %s(%d)", 
                                    pointer, res.name, m1.name, RecipeDatabase[i].count1);
                        }

                        DrawTextEx(nanumFont, recipeText, (Vector2){120, recipeY}, 22, 2, textColor);
                        recipeY += 40; // 간격을 조금 더 띄움
                    }
                    
                    // --- [ 설명창 영역 (위치 고정) ] ---
                    // 리스트와 확실히 분리하기 위해 박스를 조금 더 아래로 내리거나 높이를 조절합니다.
                    DrawRectangle(100, 580, 1080, 100, Fade(DARKGRAY, 0.8f)); 
                    DrawRectangleLines(100, 580, 1080, 100, ORANGE); // 테두리 추가로 시인성 확보
                    
                    // --- [ 추가: 제작 결과 알림 출력 ] ---
                    if (statusTimer > 0) {
                        // 메시지가 서서히 투명해지도록 설정
                        float alpha = (statusTimer > 0.5f) ? 1.0f : statusTimer * 2.0f; 
                        DrawTextEx(nanumFont, craftStatusMessage, (Vector2){120, 540}, 22, 2, Fade(statusColor, alpha));
                    }

                    // 현재 선택된 아이템의 결과물 정보 가져오기
                    ItemInfo currentItem = ItemDatabase[RecipeDatabase[recipe_pointer].result];
                    
                    DrawTextEx(nanumFont, TextFormat("[ %s ]", currentItem.name), (Vector2){120, 595}, 24, 2, YELLOW);
                    DrawTextEx(nanumFont, TextFormat("상세 설명: %s", currentItem.desc), (Vector2){120, 630}, 20, 2, RAYWHITE);
                    break;
                    
                case SCREEN_GAMES_MAIN_ADVENTURE:
                    if (!isAdventuring && IsKeyPressed(KEY_SPACE)) {
                        if (playerHP <= 10.0f) {
                            AddLog("동력이 부족하여 탐사할 수 없습니다! (기지에서 휴식 필요)");
                        } else {
                            isAdventuring = true;
                            adventureTimer = 5.0f; // 탐사 시간 증가
                            adventureStep = 0;
                            playerHP -= 15.0f;    // 탐사 시 동력 소모
                            for(int i=0; i<3; i++) strcpy(adventureLogs[i], "");
                            AddLog(">> 하이브 심부로 진입합니다...");
                        }
                    }

                    if (isAdventuring) {
                        if (!eventActive) {
                            adventureTimer -= GetFrameTime();
                            logTimer += GetFrameTime();

                            // 1.5초마다 이벤트 발생 확률 체크
                            if (logTimer >= 1.5f) {
                                logTimer = 0.0f;
                                if (GetRandomValue(1, 100) > 60) { // 40% 확률로 돌발 이벤트
                                    eventActive = true;
                                    eventTimer = 2.0f; // 2초 안에 반응해야 함!
                                    activeEventType = GetRandomValue(0, 1);
                                    if (activeEventType == 0) AddLog("!! 경고: 데이터 폭풍 발생! [L]회피 [R]강행");
                                    else AddLog("!! 발견: 버려진 상자! [L]무시 [R]조사");
                                } else {
                                    // 기존 일반 로그
                                    AddLog("..하이브 내부를 전진 중..");
                                }
                            }
                        } else {
                            // 이벤트가 활성화된 경우 (선택 대기)
                            eventTimer -= GetFrameTime();
                            
                            if (IsKeyPressed(KEY_L)) {
                                if (activeEventType == 0) { AddLog("-> 안전하게 회피했습니다. (동력 소모)"); playerHP -= 2; }
                                else { AddLog("-> 상자를 지나쳤습니다."); }
                                eventActive = false;
                            } 
                            else if (IsKeyPressed(KEY_R)) {
                                if (activeEventType == 0) { 
                                    if(GetRandomValue(0,1)) { AddLog("-> 강행 성공! 데이터 획득."); BAG(BAG_ADD, ITEM_MIKU_FRAGMENT, 2); }
                                    else { AddLog("-> 폭풍에 휘말려 동력 손실!"); playerHP -= 10; }
                                } else { 
                                    AddLog("-> 상자에서 재료를 발견했습니다!"); BAG(BAG_ADD, ITEM_RUSTY_METAL, 1); 
                                }
                                eventActive = false;
                            }

                            if (eventTimer <= 0) { // 시간 초과
                                AddLog("-> 대응하지 못해 피해를 입었습니다!");
                                playerHP -= 5;
                                eventActive = false;
                            }
                        }
                        adventureTimer -= GetFrameTime();
                        logTimer += GetFrameTime();

                        // 1초마다 무작위 탐사 로그 출력
                        if (logTimer >= 1.2f) {
                            logTimer = 0.0f;
                            int event = GetRandomValue(0, 3);
                            if (event == 0) AddLog("..노이즈 섞인 노랫소리가 들립니다.");
                            else if (event == 1) AddLog("..데이터 파편들이 휘몰아칩니다!");
                            else if (event == 2) AddLog("..오래된 서버 렉을 발견했습니다.");
                            else AddLog("..주변을 정밀 스캔 중...");
                        }

                        if (adventureTimer <= 0) {
                            isAdventuring = false;
                            // 보상 결정 (확률)
                            int luck = GetRandomValue(1, 100);
                            if (luck > 90) { // 10% 확률 대박
                                BAG(BAG_ADD, ITEM_BLUE_FLARE, 2);
                                BAG(BAG_ADD, ITEM_MIKU_FRAGMENT, 5);
                                AddLog("★ 희귀한 블루 플레어와 대량의 데이터를 회수했습니다!");
                            } else if (luck > 40) { // 일반 성공
                                ITEMS found = (ITEMS)GetRandomValue(ITEM_MIKU_FRAGMENT, ITEM_OLD_FABRIC);
                                BAG(BAG_ADD, found, 2);
                                AddLog(TextFormat("탐사 성공: [%s]를 획득했습니다.", ItemDatabase[found].name));
                            } else { // 꽝 또는 소량
                                AddLog("회수할 수 있는 유효한 데이터가 거의 없습니다..");
                            }
                        }
                    } else {
                        // 기지(메인화면)에서 쉬면 동력이 찬다는 컨셉을 위해 메인에서 회복 로직 추가 가능
                        if (playerHP < 100.0f) playerHP += GetFrameTime() * 0.5f; 
                    }
                    break;

                case SCREEN_SETTINGS:
                    DrawTextEx(nanumFont, "--- HOW TO PLAY ---", (Vector2){500, 100}, 40, 2, RAYWHITE);
                    DrawTextEx(nanumFont, "W / S : 메뉴 이동", (Vector2){500, 250}, 25, 2, WHITE);
                    DrawTextEx(nanumFont, "ENTER : 선택", (Vector2){500, 300}, 25, 2, WHITE);
                    DrawTextEx(nanumFont, "B : 가방 열기 / C : 제작창", (Vector2){500, 350}, 25, 2, WHITE);
                    DrawTextEx(nanumFont, "ESC : 이전 화면으로", (Vector2){500, 450}, 20, 2, YELLOW);
                    break;

                default: break;
            }

        EndDrawing();
    }
    // 리소스 해제
    UnloadTexture(leekTexture);
    UnloadFont(nanumFont);
    CloseWindow();
    
    EXIT_GAME:
    return 0;
}