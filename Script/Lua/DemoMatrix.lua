function Update(This, DeltaSeconds)
    local ValueOnly = This:GetComponent("ValueOnlyComponent")

    if ValueOnly == nil then
        return
    end

    local Matrix = SimpleMathMatrix4x4.new()
    Matrix:SetDiagonal(3.0)

    local MatrixTrace = Matrix:GetTrace()
    local IgnoredDeltaSeconds = DeltaSeconds

    ValueOnly.mValue = math.floor(MatrixTrace)
end
