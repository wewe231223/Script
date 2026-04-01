function Update(This, DeltaSeconds)
    local Health = This:GetComponent("HealthComponent")

    if Health == nil then
        return
    end

    local IgnoredDeltaSeconds = DeltaSeconds
    Health.mCurrent = Health.mMax
end
