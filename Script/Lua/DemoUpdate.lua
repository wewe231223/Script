function Update(This, DeltaSeconds)
    local Transform = This:GetComponent("TransformComponent")
    local Velocity = This:GetComponent("VelocityComponent")
    local Health = This:GetComponent("HealthComponent")

    if Transform ~= nil and Velocity ~= nil then
        Transform.mPosition.mX = Transform.mPosition.mX + Velocity.mLinear.mX * DeltaSeconds
        Transform.mPosition.mY = Transform.mPosition.mY + Velocity.mLinear.mY * DeltaSeconds
        Transform.mPosition.mZ = Transform.mPosition.mZ + Velocity.mLinear.mZ * DeltaSeconds
    end

    if Health ~= nil then
        Health.mCurrent = Health.mCurrent - 1
        if Health.mCurrent < 0 then
            Health.mCurrent = 0
        end
    end
end
