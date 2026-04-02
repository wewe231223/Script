HotReloadRevision = 0
HotReloadPositionBoost = -1.0
HotReloadValueOffset = 1000

function Clamp(Value, MinValue, MaxValue)
    if Value < MinValue then
        return MinValue
    end

    if Value > MaxValue then
        return MaxValue
    end

    return Value
end

function Update(This, DeltaSeconds)
    local Transform = This:GetComponent("TransformComponent")
    local Velocity = This:GetComponent("VelocityComponent")
    local Acceleration = This:GetComponent("AccelerationComponent")
    local Health = This:GetComponent("HealthComponent")
    local Energy = This:GetComponent("EnergyComponent")
    local Faction = This:GetComponent("FactionComponent")
    local ValueOnly = This:GetComponent("ValueOnlyComponent")

    local TeamId = 0
    if Faction ~= nil then
        TeamId = Faction.mTeamId
    end

    if Velocity ~= nil and Acceleration ~= nil then
        local TeamAccelBias = 1.0

        if TeamId == 1 then
            TeamAccelBias = 1.3
        elseif TeamId == 2 then
            TeamAccelBias = 0.7
        end

        Velocity.mLinear.mX = Velocity.mLinear.mX + Acceleration.mLinear.mX * DeltaSeconds * TeamAccelBias
        Velocity.mLinear.mY = Velocity.mLinear.mY + Acceleration.mLinear.mY * DeltaSeconds * TeamAccelBias
        Velocity.mLinear.mZ = Velocity.mLinear.mZ + Acceleration.mLinear.mZ * DeltaSeconds * TeamAccelBias
    end

    if Transform ~= nil and Velocity ~= nil then
        local Damping = 1.0

        if TeamId == 1 then
            Damping = 0.85
        elseif TeamId == 2 then
            Damping = 1.15
        end

        Transform.mPosition.mX = Transform.mPosition.mX + Velocity.mLinear.mX * DeltaSeconds * Damping * HotReloadPositionBoost
        Transform.mPosition.mY = Transform.mPosition.mY + Velocity.mLinear.mY * DeltaSeconds * Damping * HotReloadPositionBoost
        Transform.mPosition.mZ = Transform.mPosition.mZ + Velocity.mLinear.mZ * DeltaSeconds * Damping * HotReloadPositionBoost
    end

    if Health ~= nil and Health:IsAlive() then
        local Damage = 1

        if TeamId == 1 then
            Damage = 2
        elseif TeamId == 2 then
            Damage = 0
        end

        if Energy ~= nil and Energy.mCurrent < 10.0 then
            Damage = Damage + 1
        end

        Health.mCurrent = Clamp(Health.mCurrent - Damage, 0, Health.mMax)
    end

    if Energy ~= nil then
        local DeltaEnergy = 0.0

        if TeamId == 2 then
            DeltaEnergy = Energy.mRegenPerSecond * DeltaSeconds
        else
            DeltaEnergy = -Energy.mDrainPerSecond * DeltaSeconds
        end

        if Health ~= nil and Health:IsAlive() == false then
            DeltaEnergy = DeltaEnergy + Energy.mRegenPerSecond * DeltaSeconds * 0.5
        end

        Energy.mCurrent = Clamp(Energy.mCurrent + DeltaEnergy, 0.0, 100.0)
    end

    if ValueOnly ~= nil then
        local PositionScore = 0.0
        local HealthScore = 0
        local EnergyScore = 0.0

        if Transform ~= nil then
            PositionScore = math.abs(Transform.mPosition.mX) + math.abs(Transform.mPosition.mY) + math.abs(Transform.mPosition.mZ)
        end

        if Health ~= nil then
            HealthScore = Health.mCurrent
        end

        if Energy ~= nil then
            EnergyScore = Energy.mCurrent * 0.1
        end

        ValueOnly.mValue = math.floor(PositionScore + HealthScore + EnergyScore + TeamId + HotReloadValueOffset + HotReloadRevision * 100)
    end
end
