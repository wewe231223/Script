function CreateArraySample()
    local Values = Float3Array.new()
    Values[0] = 1.25
    Values[1] = 2.5
    Values[2] = 3.75

    local Sum = 0.0
    local Count = Values:Size()
    local Index = 0

    while Index < Count do
        Sum = Sum + Values[Index]
        Index = Index + 1
    end

    return Sum
end
