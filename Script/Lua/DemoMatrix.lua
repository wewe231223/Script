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
    local Energy = This:GetComponent("EnergyComponent")
    local ValueOnly = This:GetComponent("ValueOnlyComponent")

    if Transform == nil or Velocity == nil or Energy == nil or ValueOnly == nil then
        return
    end

    local Matrix = SimpleMathMatrix4x4.new()
    Matrix:SetDiagonal(2.0 + DeltaSeconds)

    local Trace = Matrix:GetTrace()
    local TraceScale = Trace * 0.05

    Velocity.mLinear.mX = Velocity.mLinear.mX + TraceScale
    Velocity.mLinear.mY = Velocity.mLinear.mY - TraceScale * 0.5
    Transform.mPosition.mX = Transform.mPosition.mX + Velocity.mLinear.mX * DeltaSeconds
    Transform.mPosition.mY = Transform.mPosition.mY + Velocity.mLinear.mY * DeltaSeconds

    local EnergyDelta = Energy.mRegenPerSecond * DeltaSeconds - Energy.mDrainPerSecond * DeltaSeconds * 0.5
    Energy.mCurrent = Clamp(Energy.mCurrent + EnergyDelta, 0.0, 100.0)

    local MixedValue = Transform.mPosition.mX + Transform.mPosition.mY + Velocity.mLinear.mX + Energy.mCurrent * 0.1 + Trace
    ValueOnly.mValue = math.floor(MixedValue)
end
