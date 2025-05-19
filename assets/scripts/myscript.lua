-- an example script with some things to test.

--lua comment; lua's global variable
some_variable = 7 * 6 + 1
username = "Sacha"

print("Hello, "..username)

if some_variable==43 then
    --..=concatenation
    print("The value of the variable is "..some_variable)
end

--lua table, the datastructure
--it contains values, necessary to initialise the game engine
config = {
    title = "Title",
    fullscreen = false,
    resolution = {
        width = 800,
        height = 600
    }
}

--lua functions
function factorial (n)
    local i = n
    local result = 1
    while i>0 do
        result = result * i
        i = i - 1
    end
    return result
end

function factorial_rec (n)
    if n <= 0 then
        return 1
    else
        return n * factorial_rec(n-1)
    end
end

--invoiking the a function from C++
print("Cube of 3 is "..cube(3))