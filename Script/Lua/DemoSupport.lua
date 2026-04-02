HotReloadRevision = 1
HotReloadMoveBoost = 4.0
HotReloadValueOffset = 700

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
    local Health = This:GetComponent("HealthComponent")
    local Faction = This:GetComponent("FactionComponent")
    local ValueOnly = This:GetComponent("ValueOnlyComponent")

    if Health == nil then
        return
    end

    local TeamId = 0
    if Faction ~= nil then
        TeamId = Faction.mTeamId
    end

    local RegenAmount = 1

    if TeamId == 0 then
        RegenAmount = 2
    elseif TeamId == 1 then
        RegenAmount = 1
    else
        RegenAmount = 0
    end

    local TickBoost = math.floor(DeltaSeconds * 2.0 + HotReloadRevision)
    Health.mCurrent = Clamp(Health.mCurrent + RegenAmount + TickBoost, 0, Health.mMax)

    if Transform ~= nil then
        local MoveBias = 0.25

        if TeamId == 0 then
            MoveBias = 0.4
        end

        Transform.mPosition.mX = Transform.mPosition.mX + DeltaSeconds * MoveBias * HotReloadMoveBoost
        Transform.mPosition.mY = Transform.mPosition.mY + DeltaSeconds * (MoveBias * 0.5) * HotReloadMoveBoost
    end

    if ValueOnly ~= nil then
        local PositionScore = 0.0

        if Transform ~= nil then
            PositionScore = math.abs(Transform.mPosition.mX) + math.abs(Transform.mPosition.mY) + math.abs(Transform.mPosition.mZ)
        end

        ValueOnly.mValue = math.floor(Health.mCurrent * 10 + PositionScore + TeamId + HotReloadValueOffset + HotReloadRevision * 100)
    end
end
