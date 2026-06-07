// ==========================================
//  flux + v4.0 범용 언어 기능 검증 테스트
// ==========================================

// 1. 구조체 정의 (데이터 묶음)
struct Player {
    string name;
    int level;
    int hp;
}

// 2. 클래스 정의 (속성 및 메서드 기반 구조)
class System {
    string version;
    bool is_running;
}

function main() {
    print("--- [1] 구조체(Struct) 생성 및 멤버 접근 테스트 ---");
    Player user;
    user = new Player();
    user.name = "Flux_Master";
    user.level = 10;
    user.hp = 100;

    printf("플레이어 생성: {user.name} (Lv.{user.level})");
    printf("현재 체력: {user.hp}");

    print("");
    print("--- [2] 클래스(Class) 인스턴스화 테스트 ---");
    System sys;
    sys = new System();
    sys.version = "v4.0.0-alpha";
    sys.is_running = true;

    printf("시스템 버전: {sys.version}");
    printf("상태: {sys.is_running}");

    print("");
    print("--- [3] 예외 처리(Exception) 및 흐름 제어 테스트 ---");
    try {
        print("데이터 처리를 시작합니다...");
        
        int input_val;
        input_val = 50;
        
        if (input_val < 100) {
            print("경고: 입력값이 너무 낮습니다. 예외를 던집니다!");
            throw("ValueUnderflowError: 입력값이 100보다 작음");
        }
        
        print("이 문장은 실행되지 않아야 합니다.");
    } catch (e) {
        console.color("red");
        printf(">>> 예외 포착: {e}");
        console.color("reset");
    }

    print("");
    print("--- [4] 복합 검증 (객체 대입 및 변경) ---");
    user.level = user.level + 5;
    printf("{user.name}님이 레벨업했습니다! 새 레벨: {user.level}");

    print("");
    print("==========================================");
    print(" 모든 v4.0 범용 기능 테스트가 정상 종료되었습니다.");
    print("==========================================");
}
