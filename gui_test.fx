import gui;
import time;

function main() {
    print("GUI 모듈 테스트를 시작합니다...");

    // 1. 메시지 박스 테스트
    gui.msgbox("flux + GUI", "환영합니다! 이제 flux + 에서 창을 띄울 수 있습니다.");

    // 2. 윈도우 창 생성 테스트
    print("창을 생성합니다: 800x600");
    gui.window("My Flux Application", 800, 600);

    print("창이 열렸습니다. 창을 닫으면 프로그램이 종료됩니다.");
    
    // 3. 이벤트 루프 (창이 닫힐 때까지 대기)
    gui.loop();

    print("프로그램이 정상적으로 종료되었습니다.");
}
