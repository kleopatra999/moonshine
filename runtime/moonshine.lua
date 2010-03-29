require "moonshine.prelude"

local term   = require "moonshine.ui.term"
term.init()

local log    = require "moonshine.log"
local loop   = require "moonshine.loop"
local keymap = new "moonshine.keymap"
local entry  = new "moonshine.ui.entry"
local buffer = new "moonshine.ui.buffer"
local label  = new "moonshine.ui.label"
local Format = new "moonshine.ui.format"


Format:define_color("aqua", 0, 454, 1000)
Format:define_style("topic", "white", "color12")
Format:define_style("happy", "yellow", "black")
Format:define_style("debug", "blue", "black")

Format:define("topic", "%{style topic}%1")
Format:define("debug", "[debug] %{style debug}%1%^")
Format:define("info",  "%{debug %(we have %{style happy}%1%^ colors and %{style happy}%2%^ styles)}...")

label:set(Format:apply('info', term.colors(), term.styles()))
buffer:print(Format:apply('info', term.colors(), term.styles()))

local r, c = term.dimensions()
label:render(0)
buffer:render(1, r - 2)
entry:render("foo: ")
term.refresh()

function on_input(key)
    if key == 'x' then
        loop.quit()
    else
        entry:keypress(key)
    end
    label:render(0)
    buffer:render(1, r - 2)
    entry:render("foo: ")
    term.refresh()
end

loop.run()

term.reset()
