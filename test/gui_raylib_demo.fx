import gui;
import system;

// 글로벌 카운터 변수
int count = 0;

// 버튼 클릭 시 호출될 함수
function increment() {
    count = count + 1;
    printf("카운트 증가: {count}");
    gui.msgbox("카운터", "현재 카운트는 {count} 입니다.");
}

function reset_count() {
    count = 0;
    print("카운터가 초기화되었습니다.");
    gui.msgbox("초기화", "카운터를 0으로 돌렸습니다.");
}

function main() {
    print("Raylib 기반 통합 GUI 데모를 시작합니다.");

    // 1. 윈도우 생성 (제목, 너비, 높이)
    gui.window("Flux + Raylib GUI Demo", 500, 400);

    // 2. 레이블 추가 (텍스트, x, y, w, h)
    gui.label("Flux + 프로그래밍 언어", 150, 30, 200, 30);
    gui.label("Raylib을 이용한 크로스 플랫폼 GUI 테스트", 80, 70, 400, 25);

    // 3. 버튼 추가 (텍스트, x, y, w, h, 콜백함수명)
    gui.button("카운트 증가", 150, 150, 200, 50, "increment");
    gui.button("초기화", 150, 220, 200, 50, "reset_count");
    
    gui.label("아래 버튼을 누르면 프로그램이 종료됩니다.", 100, 300, 300, 25);
    gui.button("종료", 200, 330, 100, 40, "on_exit");

    print("이벤트 루프 진입...");
    
    // 4. GUI 이벤트 루프 시작
    gui.loop();

    print("데모가 종료되었습니다.");
}

function on_exit() {
    print("프로그램을 종료합니다.");
    system.exit(0);
}
