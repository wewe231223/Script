function Update(This, DeltaSeconds)
    local Health = This:GetComponent("HealthComponent")

    if Health == nil or Health:IsAlive() == false then
        return
    end

    local IgnoredDeltaSeconds = DeltaSeconds
    Health.mCurrent = Health.mMax
end
