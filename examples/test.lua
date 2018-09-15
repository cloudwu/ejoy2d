local ej = require "ejoy2d"
local fw = require "ejoy2d.framework"
local pack = require "ejoy2d.simplepackage"

pack.load {
	pattern = fw.WorkDir..[[examples/asset/?]],
    "box",
}

local box = ej.sprite("box", "box1")

box:ps(400,300)

local game = {}

function game.update()

end

function game.drawframe()
	ej.clear(0xff808080)
    box:draw()
end

function game.touch(what, x, y)
end

function game.message(...)
end

function game.handle_error(...)
end

function game.on_resume()
end

function game.on_pause()
end

ej.start(game)