local screen = platform.window
local w = screen:width()
local h = screen:height()
local FOV = 0.698

function distance(x, y)
    local diffX = x[1]-y[1]
    local diffY = x[2]-y[2]
    local diffZ = x[3]-y[3]
    return math.sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ)
end

function mod(x, n)
    return {x[1]%n, x[2]%n, x[3]%n}
end

function fractalDistance(x)
    return distance(mod(x, 1), {0.5, 0.5, 0.5}) - 0.2
end

function lightIntensity(x, light, camera)
    local fog = 1/math.pow(distance(x, camera) + 1, 0.2)
    return 1/math.pow(distance(mod(x, 1), light), 5) * fog
end

function norm(x)
    local mag = math.sqrt(x[1]*x[1] + x[2]*x[2] + x[3]*x[3])
    return {x[1]/mag, x[2]/mag, x[3]/mag}
end

function raytrace(startPos, direction, rayPos)
    rayPos[1] = startPos[1]
    rayPos[2] = startPos[2]
    rayPos[3] = startPos[3]

    local d = 1
    while(d > 0.05) do
        rayPos[1] = rayPos[1] + direction[1] * d
        rayPos[2] = rayPos[2] + direction[2] * d
        rayPos[3] = rayPos[3] + direction[3] * d
        d = fractalDistance(rayPos)

        if(distance(rayPos, startPos) > 30) then
            return false
        end
    end
    return true
end

function FragmentShader(cameraPos, lightPos, x, y)
    local angleX = FOV * (2*x/w - 1)
    local angleY = FOV * (2*y/h - 1)
    local direction = norm({math.tan(angleX), math.tan(angleY), 1})

    local rayResult = {0, 0, 0}
    local found = raytrace(cameraPos, direction, rayResult)
    if(not found) then
        return {0, 0, 0}
    end
    local shade = lightIntensity(rayResult, lightPos, cameraPos)
    return {shade, 0, 0}
end

function Draw(gc)
    local cameraPos = {0, 0, -3}
    local lightPos = {0, 1, -0.5}
    for x=1, w do
        for y=1, h do
            color = FragmentShader(cameraPos, lightPos, x, y)
            gc:setColorRGB(math.min(color[1]*255, 255), math.min(color[2]*255), math.min(color[3]*255, 255))
            gc:fillRect(x, y, 1, 1)
        end
    end
end

function on.paint(gc)
    Draw(gc)
end
