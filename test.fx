import system;
import console;
import math;
import time;
import random;
import file;

function main() {
    console.title("flux + v2.0 Standard Modules Test");
    console.clear();
    console.hide_cursor();

    console.color("cyan");
    print("==========================================");
    print("     flux + v2.0 대규모 모듈 테스트      ");
    print("==========================================");
    console.color("reset");

    // 1. system & time
    print("");
    console.color("yellow"); print("[1] System & Time"); console.color("reset");
    printf("Platform: {system.platform()}");
    printf("Current Dir: {system.cwd()}");
    printf("Current Time: {time.format(\"%Y-%m-%d %H:%M:%S\")}");


    // 2. math
    print("");
    console.color("yellow"); print("[2] Math Constants & Funcs"); console.color("reset");
    printf("PI: {math.pi()}, E: {math.e()}");
    printf("sqrt(25): {math.sqrt(25.0)}, pow(2,10): {math.pow(2.0, 10.0)}");
    printf("round(3.7): {math.round(3.7)}, floor(3.7): {math.floor(3.7)}");

    // 3. random
    print("");
    console.color("yellow"); print("[3] Random & Array Choice"); console.color("reset");
    int fruits[];
    fruits = ["Apple", "Banana", "Cherry", "Dragonfruit"];
    string picked;
    picked = random.choice(fruits);
    printf("Random Fruit: {picked}");

    // 4. file system (std::filesystem 기반)
    print("");
    console.color("yellow"); print("[4] File System Operations"); console.color("reset");
    string test_dir;
    test_dir = "flux_test_folder";
    file.mkdir(test_dir);
    printf("Directory '{test_dir}' created: {file.exists(test_dir)}");

    string test_file;
    test_dir = "flux_test_folder/hello.txt"; // 경로 재활용
    file.write(test_dir, "Hello flux + v2.0 filesystem!");
    printf("File size: {file.size(test_dir)} bytes");

    print("Directory List:");
    print(file.list("."));

    // Cleanup
    print("");
    console.color("magenta");
    print("테스트 완료. 3초 후 커서를 복구하고 종료합니다...");
    console.color("reset");
    time.sleep(3000);
    console.show_cursor();
}
