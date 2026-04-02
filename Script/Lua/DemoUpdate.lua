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

    local TeamId = 0
    if Faction ~= nil then
        TeamId = Faction.mTeamId
    end

    if Velocity ~= nil and Acceleration ~= nil then
        Velocity.mLinear.mX = Velocity.mLinear.mX + Acceleration.mLinear.mX * DeltaSeconds
        Velocity.mLinear.mY = Velocity.mLinear.mY + Acceleration.mLinear.mY * DeltaSeconds
        Velocity.mLinear.mZ = Velocity.mLinear.mZ + Acceleration.mLinear.mZ * DeltaSeconds
    end

    if Transform ~= nil and Velocity ~= nil then
        local Damping = 1.0

        if TeamId == 1 then
            Damping = 0.8
        elseif TeamId == 2 then
            Damping = 1.2
        end

        Transform.mPosition.mX = Transform.mPosition.mX + Velocity.mLinear.mX * DeltaSeconds * Damping
        Transform.mPosition.mY = Transform.mPosition.mY + Velocity.mLinear.mY * DeltaSeconds * Damping
        Transform.mPosition.mZ = Transform.mPosition.mZ + Velocity.mLinear.mZ * DeltaSeconds * Damping
    end

    if Health ~= nil and Health:IsAlive() then
        local Damage = 1

        if TeamId == 1 then
            Damage = 2
        elseif TeamId == 2 then
            Damage = 0
        end

        Health.mCurrent = Clamp(Health.mCurrent - Damage, 0, Health.mMax)
    end

    if Energy ~= nil and Energy:HasEnergy() then
        local DeltaEnergy = 0.0

        if TeamId == 2 then
            DeltaEnergy = Energy.mRegenPerSecond * DeltaSeconds
        else
            DeltaEnergy = -Energy.mDrainPerSecond * DeltaSeconds
        end

        Energy.mCurrent = Clamp(Energy.mCurrent + DeltaEnergy, 0.0, 100.0)
    end
end
