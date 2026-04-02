#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "Arche/World.h"
#include "Script/LuaScriptFramework.h"
#include "Script/DemoComponents.h"

#pragma comment(lib, "lua54.lib")

void ClearConsoleScreen() {
#if defined(_WIN32)
    int ClearResult{ std::system("cls") };
#else
    int ClearResult{ std::system("clear") };
#endif
    if (ClearResult == -1) {
        std::cout << "\n";
    }
}

void PrintBanner() {
    std::cout << "========================================" << std::endl;
    std::cout << "      Lua Entity Interactive Demo       " << std::endl;
    std::cout << "========================================" << std::endl;
}

void PrintHelp() {
    std::cout << "[명령어 목록]" << std::endl;
    std::cout << "  help" << std::endl;
    std::cout << "  status" << std::endl;
    std::cout << "  update <DeltaTime>" << std::endl;
    std::cout << "  tick <Count> <DeltaTime>" << std::endl;
    std::cout << "  velocity <A|B|D> <X> <Y> <Z>" << std::endl;
    std::cout << "  health <A|B|C> <Value>" << std::endl;
    std::cout << "  energy <A|B|D> <Current> <Drain> <Regen>" << std::endl;
    std::cout << "  faction <A|B|C> <TeamId>" << std::endl;
    std::cout << "  exit" << std::endl;
}

bool TryResolveEntity(const std::string& EntityName, const Arche::EntityID EntityA, const Arche::EntityID EntityB, const Arche::EntityID EntityC, const Arche::EntityID EntityD, Arche::EntityID& TargetEntity) {
    if (EntityName == "A") {
        TargetEntity = EntityA;
        return true;
    }

    if (EntityName == "B") {
        TargetEntity = EntityB;
        return true;
    }

    if (EntityName == "C") {
        TargetEntity = EntityC;
        return true;
    }

    if (EntityName == "D") {
        TargetEntity = EntityD;
        return true;
    }

    return false;
}

void PrintStatus(Arche::World& MainWorld, const Arche::EntityID EntityA, const Arche::EntityID EntityB, const Arche::EntityID EntityC, const Arche::EntityID EntityD) {
    TransformComponent* TransformA{ MainWorld.GetComponent<TransformComponent>(EntityA) };
    VelocityComponent* VelocityA{ MainWorld.GetComponent<VelocityComponent>(EntityA) };
    HealthComponent* HealthA{ MainWorld.GetComponent<HealthComponent>(EntityA) };
    EnergyComponent* EnergyA{ MainWorld.GetComponent<EnergyComponent>(EntityA) };
    FactionComponent* FactionA{ MainWorld.GetComponent<FactionComponent>(EntityA) };
    ValueOnlyComponent* ValueOnlyA{ MainWorld.GetComponent<ValueOnlyComponent>(EntityA) };

    TransformComponent* TransformB{ MainWorld.GetComponent<TransformComponent>(EntityB) };
    VelocityComponent* VelocityB{ MainWorld.GetComponent<VelocityComponent>(EntityB) };
    HealthComponent* HealthB{ MainWorld.GetComponent<HealthComponent>(EntityB) };
    EnergyComponent* EnergyB{ MainWorld.GetComponent<EnergyComponent>(EntityB) };
    FactionComponent* FactionB{ MainWorld.GetComponent<FactionComponent>(EntityB) };
    ValueOnlyComponent* ValueOnlyB{ MainWorld.GetComponent<ValueOnlyComponent>(EntityB) };

    TransformComponent* TransformC{ MainWorld.GetComponent<TransformComponent>(EntityC) };
    HealthComponent* HealthC{ MainWorld.GetComponent<HealthComponent>(EntityC) };
    FactionComponent* FactionC{ MainWorld.GetComponent<FactionComponent>(EntityC) };
    ValueOnlyComponent* ValueOnlyC{ MainWorld.GetComponent<ValueOnlyComponent>(EntityC) };

    TransformComponent* TransformD{ MainWorld.GetComponent<TransformComponent>(EntityD) };
    VelocityComponent* VelocityD{ MainWorld.GetComponent<VelocityComponent>(EntityD) };
    EnergyComponent* EnergyD{ MainWorld.GetComponent<EnergyComponent>(EntityD) };
    ValueOnlyComponent* ValueOnlyD{ MainWorld.GetComponent<ValueOnlyComponent>(EntityD) };

    if (TransformA == nullptr || VelocityA == nullptr || HealthA == nullptr || EnergyA == nullptr || FactionA == nullptr || ValueOnlyA == nullptr || TransformB == nullptr || VelocityB == nullptr || HealthB == nullptr || EnergyB == nullptr || FactionB == nullptr || ValueOnlyB == nullptr || TransformC == nullptr || HealthC == nullptr || FactionC == nullptr || ValueOnlyC == nullptr || TransformD == nullptr || VelocityD == nullptr || EnergyD == nullptr || ValueOnlyD == nullptr) {
        std::cout << "[오류] 상태 조회 실패" << std::endl;
        return;
    }

    std::cout << "\n[엔티티 상태]" << std::endl;
    std::cout << "A | Pos(" << TransformA->mPosition.mX << ", " << TransformA->mPosition.mY << ", " << TransformA->mPosition.mZ << ") Vel(" << VelocityA->mLinear.mX << ", " << VelocityA->mLinear.mY << ", " << VelocityA->mLinear.mZ << ") HP(" << HealthA->mCurrent << "/" << HealthA->mMax << ") EN(" << EnergyA->mCurrent << ") Team(" << FactionA->mTeamId << ") Value(" << ValueOnlyA->mValue << ")" << std::endl;
    std::cout << "B | Pos(" << TransformB->mPosition.mX << ", " << TransformB->mPosition.mY << ", " << TransformB->mPosition.mZ << ") Vel(" << VelocityB->mLinear.mX << ", " << VelocityB->mLinear.mY << ", " << VelocityB->mLinear.mZ << ") HP(" << HealthB->mCurrent << "/" << HealthB->mMax << ") EN(" << EnergyB->mCurrent << ") Team(" << FactionB->mTeamId << ") Value(" << ValueOnlyB->mValue << ")" << std::endl;
    std::cout << "C | Pos(" << TransformC->mPosition.mX << ", " << TransformC->mPosition.mY << ", " << TransformC->mPosition.mZ << ") HP(" << HealthC->mCurrent << "/" << HealthC->mMax << ") Team(" << FactionC->mTeamId << ") Value(" << ValueOnlyC->mValue << ")" << std::endl;
    std::cout << "D | Pos(" << TransformD->mPosition.mX << ", " << TransformD->mPosition.mY << ", " << TransformD->mPosition.mZ << ") Vel(" << VelocityD->mLinear.mX << ", " << VelocityD->mLinear.mY << ", " << VelocityD->mLinear.mZ << ") EN(" << EnergyD->mCurrent << ") Value(" << ValueOnlyD->mValue << ")" << std::endl;
}

void RenderConsole(Arche::World& MainWorld, const Arche::EntityID EntityA, const Arche::EntityID EntityB, const Arche::EntityID EntityC, const Arche::EntityID EntityD, const std::string& LastResultMessage) {
    ClearConsoleScreen();
    PrintBanner();
    PrintHelp();
    PrintStatus(MainWorld, EntityA, EntityB, EntityC, EntityD);
    std::cout << "\n[이전 결과] " << LastResultMessage << std::endl;
}

int main(void) {
    Arche::World MainWorld{};

    TransformComponent TransformA{};
    TransformA.mPosition = Vec3{ 0.0f, 0.0f, 0.0f };
    TransformA.mScale = Vec3{ 1.0f, 1.0f, 1.0f };

    VelocityComponent VelocityA{};
    VelocityA.mLinear = Vec3{ 1.0f, 0.0f, 0.0f };

    AccelerationComponent AccelerationA{};
    AccelerationA.mLinear = Vec3{ 1.0f, 0.0f, 0.0f };

    HealthComponent HealthA{};
    HealthA.mCurrent = 20;
    HealthA.mMax = 20;

    EnergyComponent EnergyA{};
    EnergyA.mCurrent = 30.0f;
    EnergyA.mDrainPerSecond = 4.0f;
    EnergyA.mRegenPerSecond = 1.0f;

    FactionComponent FactionA{};
    FactionA.mTeamId = 1;

    ValueOnlyComponent ValueOnlyA{};
    ValueOnlyA.mValue = 11;

    Arche::EntityID EntityA{ MainWorld.CreateEntity<TransformComponent, VelocityComponent, AccelerationComponent, HealthComponent, EnergyComponent, FactionComponent, ValueOnlyComponent>(TransformA, VelocityA, AccelerationA, HealthA, EnergyA, FactionA, ValueOnlyA) };

    TransformComponent TransformB{};
    TransformB.mPosition = Vec3{ 10.0f, 0.0f, 0.0f };
    TransformB.mScale = Vec3{ 2.0f, 2.0f, 2.0f };

    VelocityComponent VelocityB{};
    VelocityB.mLinear = Vec3{ -2.0f, 1.0f, 0.0f };

    AccelerationComponent AccelerationB{};
    AccelerationB.mLinear = Vec3{ 0.0f, 1.0f, 0.0f };

    HealthComponent HealthB{};
    HealthB.mCurrent = 10;
    HealthB.mMax = 12;

    EnergyComponent EnergyB{};
    EnergyB.mCurrent = 40.0f;
    EnergyB.mDrainPerSecond = 3.0f;
    EnergyB.mRegenPerSecond = 6.0f;

    FactionComponent FactionB{};
    FactionB.mTeamId = 2;

    ValueOnlyComponent ValueOnlyB{};
    ValueOnlyB.mValue = 22;

    Arche::EntityID EntityB{ MainWorld.CreateEntity<TransformComponent, VelocityComponent, AccelerationComponent, HealthComponent, EnergyComponent, FactionComponent, ValueOnlyComponent>(TransformB, VelocityB, AccelerationB, HealthB, EnergyB, FactionB, ValueOnlyB) };

    TransformComponent TransformC{};
    TransformC.mPosition = Vec3{ 1.0f, 1.0f, 1.0f };
    TransformC.mScale = Vec3{ 1.0f, 1.0f, 1.0f };

    HealthComponent HealthC{};
    HealthC.mCurrent = 3;
    HealthC.mMax = 5;

    FactionComponent FactionC{};
    FactionC.mTeamId = 0;

    ValueOnlyComponent ValueOnlyC{};
    ValueOnlyC.mValue = 33;

    Arche::EntityID EntityC{ MainWorld.CreateEntity<TransformComponent, HealthComponent, FactionComponent, ValueOnlyComponent>(TransformC, HealthC, FactionC, ValueOnlyC) };

    TransformComponent TransformD{};
    TransformD.mPosition = Vec3{ 3.0f, 0.0f, 0.0f };
    TransformD.mScale = Vec3{ 1.0f, 1.0f, 1.0f };

    VelocityComponent VelocityD{};
    VelocityD.mLinear = Vec3{ 0.0f, 2.0f, 0.0f };

    EnergyComponent EnergyD{};
    EnergyD.mCurrent = 70.0f;
    EnergyD.mDrainPerSecond = 2.0f;
    EnergyD.mRegenPerSecond = 4.0f;

    ValueOnlyComponent ValueOnlyD{};
    ValueOnlyD.mValue = 0;

    Arche::EntityID EntityD{ MainWorld.CreateEntity<TransformComponent, VelocityComponent, EnergyComponent, ValueOnlyComponent>(TransformD, VelocityD, EnergyD, ValueOnlyD) };

    Script::LuaScriptFramework Framework{};
    Framework.Initialize(&MainWorld);
    Framework.OpenDefaultLibraries();

    Framework.RegisterComponentByDefinition<Vec3>();
    Framework.RegisterComponentByDefinition<TransformComponent>();
    Framework.RegisterComponentByDefinition<VelocityComponent>();
    Framework.RegisterComponentByDefinition<AccelerationComponent>();
    Framework.RegisterComponentByDefinition<HealthComponent>();
    Framework.RegisterComponentByDefinition<EnergyComponent>();
    Framework.RegisterComponentByDefinition<FactionComponent>();
    Framework.RegisterComponentByDefinition<ValueOnlyComponent>();
    Framework.RegisterTypeByDefinition<SimpleMath::Matrix4x4>();

    bool IsAttachedA{ Framework.AttachScriptFromFile(EntityA, "Script/Lua/DemoUpdate.lua", 1u) };
    bool IsAttachedB{ Framework.AttachScriptFromFile(EntityB, "Script/Lua/DemoUpdate.lua", 2u) };
    bool IsAttachedC{ Framework.AttachScriptFromFile(EntityC, "Script/Lua/DemoSupport.lua", 3u) };
    bool IsAttachedD{ Framework.AttachScriptFromFile(EntityD, "Script/Lua/DemoMatrix.lua", 4u) };

    if (IsAttachedA == false || IsAttachedB == false || IsAttachedC == false || IsAttachedD == false) {
        std::cout << "[오류] 스크립트 부착 실패" << std::endl;
        return 1;
    }

    std::string LastResultMessage{ "준비 완료" };

    while (true) {
        RenderConsole(MainWorld, EntityA, EntityB, EntityC, EntityD, LastResultMessage);
        std::cout << "\nCommand > ";

        std::string InputLine{};
        std::getline(std::cin, InputLine);

        if (std::cin.good() == false) {
            return 0;
        }

        std::istringstream InputStream{ InputLine };
        std::string Command{};
        InputStream >> Command;

        if (Command.empty()) {
            LastResultMessage = "빈 명령은 처리하지 않습니다.";
            continue;
        }

        if (Command == "help") {
            LastResultMessage = "도움말은 화면 상단에 항상 표시됩니다.";
            continue;
        }

        if (Command == "status") {
            LastResultMessage = "상태를 새로 고침했습니다.";
            continue;
        }

        if (Command == "update") {
            float DeltaTime{ 0.0f };
            InputStream >> DeltaTime;

            if (InputStream.fail() || DeltaTime <= 0.0f) {
                LastResultMessage = "실패: update <DeltaTime> 형식으로 입력하세요.";
                continue;
            }

            Framework.Update(DeltaTime);
            LastResultMessage = "완료: update 실행";
            continue;
        }

        if (Command == "tick") {
            int Count{ 0 };
            float DeltaTime{ 0.0f };
            InputStream >> Count >> DeltaTime;

            if (InputStream.fail() || Count <= 0 || DeltaTime <= 0.0f) {
                LastResultMessage = "실패: tick <Count> <DeltaTime> 형식으로 입력하세요.";
                continue;
            }

            int Index{ 0 };

            while (Index < Count) {
                Framework.Update(DeltaTime);
                Index = Index + 1;
            }

            LastResultMessage = "완료: tick 실행";
            continue;
        }

        if (Command == "velocity") {
            std::string EntityName{};
            float VelocityX{ 0.0f };
            float VelocityY{ 0.0f };
            float VelocityZ{ 0.0f };
            InputStream >> EntityName >> VelocityX >> VelocityY >> VelocityZ;

            if (InputStream.fail()) {
                LastResultMessage = "실패: velocity <A|B|D> <X> <Y> <Z> 형식으로 입력하세요.";
                continue;
            }

            Arche::EntityID TargetEntity{};
            bool IsEntityValid{ TryResolveEntity(EntityName, EntityA, EntityB, EntityC, EntityD, TargetEntity) };

            if (IsEntityValid == false || EntityName == "C") {
                LastResultMessage = "실패: velocity 는 A, B, D만 지원합니다.";
                continue;
            }

            VelocityComponent* TargetVelocity{ MainWorld.GetComponent<VelocityComponent>(TargetEntity) };

            if (TargetVelocity == nullptr) {
                LastResultMessage = "실패: VelocityComponent가 없습니다.";
                continue;
            }

            TargetVelocity->mLinear = Vec3{ VelocityX, VelocityY, VelocityZ };
            LastResultMessage = "완료: velocity 변경";
            continue;
        }

        if (Command == "health") {
            std::string EntityName{};
            int HealthValue{ 0 };
            InputStream >> EntityName >> HealthValue;

            if (InputStream.fail()) {
                LastResultMessage = "실패: health <A|B|C> <Value> 형식으로 입력하세요.";
                continue;
            }

            Arche::EntityID TargetEntity{};
            bool IsEntityValid{ TryResolveEntity(EntityName, EntityA, EntityB, EntityC, EntityD, TargetEntity) };

            if (IsEntityValid == false || EntityName == "D") {
                LastResultMessage = "실패: health 는 A, B, C만 지원합니다.";
                continue;
            }

            HealthComponent* TargetHealth{ MainWorld.GetComponent<HealthComponent>(TargetEntity) };

            if (TargetHealth == nullptr) {
                LastResultMessage = "실패: HealthComponent가 없습니다.";
                continue;
            }

            TargetHealth->mCurrent = HealthValue;
            LastResultMessage = "완료: health 변경";
            continue;
        }

        if (Command == "energy") {
            std::string EntityName{};
            float EnergyCurrent{ 0.0f };
            float EnergyDrain{ 0.0f };
            float EnergyRegen{ 0.0f };
            InputStream >> EntityName >> EnergyCurrent >> EnergyDrain >> EnergyRegen;

            if (InputStream.fail()) {
                LastResultMessage = "실패: energy <A|B|D> <Current> <Drain> <Regen> 형식으로 입력하세요.";
                continue;
            }

            Arche::EntityID TargetEntity{};
            bool IsEntityValid{ TryResolveEntity(EntityName, EntityA, EntityB, EntityC, EntityD, TargetEntity) };

            if (IsEntityValid == false || EntityName == "C") {
                LastResultMessage = "실패: energy 는 A, B, D만 지원합니다.";
                continue;
            }

            EnergyComponent* TargetEnergy{ MainWorld.GetComponent<EnergyComponent>(TargetEntity) };

            if (TargetEnergy == nullptr) {
                LastResultMessage = "실패: EnergyComponent가 없습니다.";
                continue;
            }

            TargetEnergy->mCurrent = EnergyCurrent;
            TargetEnergy->mDrainPerSecond = EnergyDrain;
            TargetEnergy->mRegenPerSecond = EnergyRegen;
            LastResultMessage = "완료: energy 변경";
            continue;
        }

        if (Command == "faction") {
            std::string EntityName{};
            int TeamId{ 0 };
            InputStream >> EntityName >> TeamId;

            if (InputStream.fail()) {
                LastResultMessage = "실패: faction <A|B|C> <TeamId> 형식으로 입력하세요.";
                continue;
            }

            Arche::EntityID TargetEntity{};
            bool IsEntityValid{ TryResolveEntity(EntityName, EntityA, EntityB, EntityC, EntityD, TargetEntity) };

            if (IsEntityValid == false || EntityName == "D") {
                LastResultMessage = "실패: faction 은 A, B, C만 지원합니다.";
                continue;
            }

            FactionComponent* TargetFaction{ MainWorld.GetComponent<FactionComponent>(TargetEntity) };

            if (TargetFaction == nullptr) {
                LastResultMessage = "실패: FactionComponent가 없습니다.";
                continue;
            }

            TargetFaction->mTeamId = TeamId;
            LastResultMessage = "완료: faction 변경";
            continue;
        }

        if (Command == "exit") {
            return 0;
        }

        LastResultMessage = "실패: 알 수 없는 명령입니다.";
    }
}
