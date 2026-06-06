class Robot {
    string name;
    int battery;

    function init(string n) {
        this.name = n;
        this.battery = 100;
        printf("로봇 {this.name}이(가) 활성화되었습니다. (배터리: {this.battery}%)");
    }

    function work() {
        if (this.battery > 0) {
            this.battery = this.battery - 20;
            printf("{this.name} 로봇이 작업 중입니다... (남은 배터리: {this.battery}%)");
        } else {
            print("배터리가 없어 작업을 수행할 수 없습니다.");
        }
    }
}

function main() {
    print("--- 클래스 메서드 및 this 테스트 ---");
    
    Robot r;
    r = new Robot();
    
    // 메서드 호출 테스트
    r.init("Flux-Bot");
    r.work();
    r.work();

    printf("최종 로봇 상태 - 이름: {r.name}, 배터리: {r.battery}%");
}
