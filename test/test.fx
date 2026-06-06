function main() {
    int a;
    a = 10;

    print("--- 사용자 정의 함수 호출 테스트 ---");
    // ppap 함수에 a(10)를 전달하여 결과 출력
    print(ppap(a)); 
}

// 매개변수 이름에서 '-'를 제거하고 ';'를 삭제함
function ppap(int arg1) {
    // 10 - 5 = 5를 반환
    return(arg1 - 5);
}
