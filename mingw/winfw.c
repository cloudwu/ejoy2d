#include "opengl.h"
#include "ejoy2dgame.h"
#include "fault.h"
#include "screen.h"
#include "winfw.h"

#include <windows.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <stdio.h>

struct WINDOWGAME {
	struct game *game;
	int intouch;
};

static struct WINDOWGAME *G = NULL;

static const char * startscript =
"local ej = require 'ejoy2d'\n"
"local fw = require 'ejoy2d.framework'\n"
"local pack = require 'ejoy2d.simplepackage'\n"
"local matrix = require 'ejoy2d.matrix'\n"

"local config = require 'examples.asset.bird_config'\n"

"-- print('work path = '..fw.WorkDir)\n"

"fw.WorkDir = 'E:/GitBash/ejoy2d/ejoy2d/'\n"

"pack.load {\n"
" pattern = fw.WorkDir..[[examples/asset/?]],\n"
" 'birds',\n"
"}\n"

"local DEBUG = false\n"

"local screen_width = 1024\n"
"local screen_height = 768\n"

"-- all sprites\n"
"local sky1 = ej.sprite('birds', 'sky_bg')\n"
"local sky2 = ej.sprite('birds', 'sky_bg')\n"
"local land1 = ej.sprite('birds', 'land_bg')\n"
"local land2 = ej.sprite('birds', 'land_bg')\n"

"local scoreboard = ej.sprite('birds', 'scoreboard')\n"
"scoreboard.text = '0'\n"

"-- util\n"
"local function _width(s, scale)\n"
"  return -s:aabb({x=0,y=0, scale=scale})\n"
"end\n"

"local function _half_height(s, scale)\n"
"  local _, h1, _, h2 = s:aabb({x=0,y=0, scale=scale})\n"
"  return (-h1 + h2)/2\n"
"end\n"

"local function _real_width(s, scale)\n"
"  local w1, _, w2, _ = s:aabb({x=0,y=0, scale=scale})\n"
"  return -w1 + w2\n"
"end\n"

"-- draw\n"
"local movingBg = {}\n"
"movingBg.__index = movingBg\n"
"function movingBg.new(obj1, obj2, y)\n"
"  local obj = {}\n"
"  obj.obj1 = obj1\n"
"  obj.obj2 = obj2\n"
"  print(obj1, obj2, x, y)\n"

"  -- init x, y\n"
"  obj.x = _width(obj1) \n"
"  obj.y = y\n"

"  obj.diff = 0\n"
"  obj.speed = 0\n"

"  obj.width = _real_width(obj1)\n"
"  assert(obj.width > screen_width)\n"

"  return setmetatable(obj, movingBg)\n"
"end\n"

"function movingBg:set_speed(speed)\n"
"  self.speed = speed\n"
"end\n"

"function movingBg:get_speed()\n"
"  return self.speed\n"
"end\n"

"function movingBg:draw()\n"
"  self.obj1:draw({x=self.x, y=self.y})\n"
"  self.obj2:draw({x=self.x + self.width, y=self.y})\n"
"end\n"

"function movingBg:update()\n"
"  self.diff = self.diff + self.speed\n"
"  self.diff = self.diff % (2 * self.width)\n"
"  if self.diff > (2*self.width - screen_width) then\n"
"    self.obj1:ps(2*self.width - self.diff, 0)\n"
"  else\n"
"    self.obj1:ps(-self.diff, 0)\n"
"  end\n"
"  self.obj2:ps(-self.diff, 0)\n"
"end\n"


"local half_land_height = _half_height(land1)\n"
"local land_height = 2 * half_land_height\n"
"local sky_height = _half_height(sky2)\n"

"-- gen_matrix for birds_annimation\n"
"-- 顶部向上拉长4倍\n"
"-- 底部向下拉长4倍\n"
"local function gen_matrix()\n"
"  local scale = 4\n"
"  local blank_height = config.blank_height * config.pipe_scale\n"
"  local header_height = config.pipe_scale * config.header_height\n"
"  local tail_height = config.pipe_scale * config.tail_height\n"

"  local offset1 = (blank_height + tail_height)*0.5 + header_height\n"
"  local offset2 = blank_height * 0.5 + header_height + scale * 0.5 * tail_height\n"

"  local m1 = matrix{x=0, y=offset1}\n"
"  m1 = m1:mul(matrix{sx=1,sy=scale, x=0, y=-offset2})\n"
"  local m2 = matrix({x=0, y=-offset1})\n"
"  m2 = m2:mul(matrix{sx=1, sy=scale, x=0, y=offset2})\n"
"  print('----------m1:', m1:export())\n"
"  print('----------m2:', m2:export())\n"
"end\n"
"gen_matrix()\n"

"local pipe_obj = ej.sprite('birds', 'pipe')\n"

"math.randomseed(os.time())\n"
"local pipe = {}\n"
"pipe.__index = pipe\n"

"pipe.blank_height = config.pipe_scale * config.blank_height\n"
"pipe.init_x = -100\n"
"pipe.min_init_y = pipe.blank_height * 0.5 + config.header_height * config.pipe_scale + 30\n"
"pipe.max_init_y = screen_height - (0.5 * pipe.blank_height + land_height + config.header_height*config.pipe_scale + 10)\n"

"function pipe.new()\n"
"  local obj = {}\n"
"  obj.sprite = ej.sprite('birds', 'pipe')\n"
"  obj.x = pipe.init_x\n"
"  obj.y = math.random(pipe.min_init_y, pipe.max_init_y)\n"

"  print('--------- sprite:', obj.x, obj.y)\n"
"  --\n"
"  obj.offset = 0\n"
"  return setmetatable(obj, pipe)\n"
"end\n"

"function pipe:get_init_x()\n"
"  return self.x\n"
"end\n"

"function pipe:get_x()\n"
"  return self.x + self.offset\n"
"end\n"

"function pipe:get_y()\n"
"  return self.y\n"
"end\n"

"function pipe:set_x(x)\n"
"  self.offset = x - self.x\n"
"end\n"

"--\n"
"function pipe:update(dist)\n"
"  -- print('----------update:', dist, self.sprite, self.offset, dist, self.x, self.y)\n"
"  self.offset = self.offset + dist\n"
"  self.sprite:ps(self.offset, 0)\n"
"end\n"

"function pipe:draw()\n"
"  self.sprite:draw({x=self.x, y=self.y})\n"
"end\n"

"local pipes = {}\n"
"pipes.pool_length = 100\n"
"pipes.pool = {} -- pipe pool\n"
"pipes.choosed = {} -- choosed pipes\n"
"pipes.width = 0 -- pipe width\n"
"pipes.space = 190 -- pipe space\n"
"pipes.speed = 0\n"
"pipes.init_offset = screen_width + 200\n"
"--pipes.init_offset = screen_width\n"
"pipes.half_blank_height = pipe.blank_height / 2\n"

"function pipes:init()\n"
"  for i=1, self.pool_length do\n"
"    self.pool[i] = pipe.new()\n"
"  end\n"
"  self.width = _real_width(self.pool[1].sprite)\n"
"  self.max_pipes = math.ceil(screen_width / (self.width + self.space)) + 3\n"
"  assert(self.max_pipes < self.pool_length)\n"
"end\n"

"function pipes:choose_pipe()\n"
"  local i = math.random(#self.pool)\n"
"  local p = table.remove(self.pool, i)\n"
"  if #self.choosed == 0 then\n"
"    p:set_x(self.init_offset)\n"
"  else\n"
"    local offset = self.choosed[#self.choosed]:get_x() + self.space + self.width\n"
"    p:set_x(offset)\n"
"  end\n"
"  table.insert(self.choosed, p)\n"
"  print('---------------pipe:', #self.choosed, self.choosed[#self.choosed]:get_x(), self.choosed[#self.choosed]:get_y())\n"
"end\n"

"function pipes:reset()\n"
"  self.offset = self.init_offset\n"

"  while #self.choosed > 0 do\n"
"    local p = table.remove(self.choosed)\n"
"    table.insert(self.pool, p)\n"
"  end\n"
"  assert(#self.pool == self.pool_length)\n"

"  for i=1, self.max_pipes do\n"
"    self:choose_pipe()\n"
"  end\n"
"end\n"

"function pipes:set_speed(speed)\n"
"  self.speed = speed\n"
"end\n"

"function pipes:update()\n"
"  if #self.choosed == 0 then\n"
"    return\n"
"  end\n"

"  for _, p in ipairs(self.choosed) do\n"
"    p:update(-self.speed)\n"
"  end\n"

"  local p1 = self.choosed[1]\n"
"  local x = p1:get_x()\n"
"  if x + self.width / 2 < 0 then\n"
"    -- move out screen\n"
"    -- choose new one\n"
"    self:choose_pipe()\n"
"    -- put back to pool\n"
"    table.remove(self.choosed, 1)\n"
"    table.insert(self.pool, p1)\n"
"  end\n"
"end\n"

"function pipes:draw()\n"
"  for _, p in ipairs(self.choosed) do\n"
"    p:draw()\n"
"  end\n"
"end\n"

"function pipes:find_clamp(x)\n"
"  local ret\n"
"  for i, p in ipairs(self.choosed) do\n"
"    if p:get_x() >= x then\n"
"      print('++++++ find clamp:', i, x, p:get_x(), p:get_y())\n"
"      return p, ret\n"
"    end\n"
"    ret = p\n"
"  end\n"
"end\n"

"pipes:init()\n"

"local bg = {}\n"
"bg.land = movingBg.new(land1, land2, screen_height - half_land_height)\n"
"bg.sky = movingBg.new(sky1, sky2, screen_height - land_height - sky_height)\n"
"bg.pipes = pipes\n"

"function bg:stop()\n"
"  bg.sky:set_speed(0)\n"
"  bg.pipes:set_speed(0)\n"
"  bg.land:set_speed(0)\n"
"end\n"

"function bg:begin()\n"
"  pipes:reset()\n"
"  bg.sky:set_speed(2)\n"
"  bg.pipes:set_speed(8)\n"
"  bg.land:set_speed(8)\n"
"end\n"

"function bg:is_moving()\n"
"  return bg.land:get_speed() > 0\n"
"end\n"

"function bg:draw()\n"
"  bg.sky:draw()\n"
"  bg.pipes:draw()\n"
"  bg.land:draw()\n"
"end\n"

"function bg:update()\n"
"  bg.sky:update()\n"
"  bg.pipes:update()\n"
"  bg.land:update()\n"
"end\n"

"local bird = {}\n"
"bird.sprite = ej.sprite('birds', 'bird')\n"
"bird.x = 350\n"
"bird.half_width = _width(bird.sprite)\n"
"bird.half_height = _half_height(bird.sprite)\n"
"bird.y = screen_height - land_height - bird.half_height\n"

"-- const\n"
"bird.touch_speed = 13.5\n"
"bird.g = 1.5 -- 重力加速度\n"

"-- variable\n"
"bird.speed = 0\n"
"bird.dt = bird.g\n"

"bird.altitude = 0\n"

"-- 死亡后保护一段时间才能开始游戏\n"
"bird.guard_time = 0\n"

"-- pipe\n"
"bird.behind = nil\n"
"bird.score = 0\n"

"-- debug\n"
"bird.stop = false\n"

"function bird:draw()\n"
"  self.sprite:draw({x=self.x, y=self.y})\n"
"end\n"

"function bird:reset()\n"
"  -- init height\n"
"  self.altitude = 50\n"
"  self.dt = self.g\n"

"  self.guard_time = 0\n"

"  self.behind = nil\n"
"  self.score = 0\n"
"end\n"

"function bird:update_altitude()\n"
"  -- print('jump:', self.speed, self.altitude)\n"
"  if self.speed > 0 then\n"
"    self.sprite:sr(360 - (self.speed/self.touch_speed) * 30)\n"
"  elseif self.speed == 0 then\n"
"    self.sprite:sr(0)\n"
"  else\n"
"    self.sprite:sr((-self.speed /(self.touch_speed * 5))*75 % 75)\n"
"  end\n"

"  self.altitude = self.altitude + self.speed\n"
"  if self.altitude > 0 then\n"
"    self.speed = self.speed - self.dt\n"
"  else\n"
"    self.altitude = 0\n"
"    self.speed = 0\n"
"    bg:stop()\n"
"  end\n"
"  self.sprite:ps(0, -self.altitude)\n"
"end\n"

"function bird:crash_with(p)\n"
"  if not p then\n"
"    return false\n"
"  end\n"

"  -- local offset_x = pipes.width + self.width\n"
"  -- local offset_y = pipes.half_blank_height - self.height\n"
"  local offset_x = pipes.width/2\n"
"  local offset_y = pipes.half_blank_height\n"
"  local x = p:get_x()\n"
"  local y = p:get_y()\n"

"  local bird_x = self.x\n"
"  local bird_y = self.y - self.altitude\n"

"  if x-offset_x <= bird_x and bird_x <= x + offset_x then\n"
"    if y-offset_y <= bird_y and bird_y <= y+offset_y then\n"
"      return false\n"
"    end\n"
"    print('-------------- crash -------------')\n"
"    print(string.format('bird x:%s, y:%s, altitude:%s, half height:%s', bird_x, bird_y, self.altitude, self.half_height))\n"
"    print(string.format('pipe x:%d, y:%d, offset_x:%d, offset_y:%d', x, y, offset_x, offset_y))\n"
"    print('-------------- crash end-------------')\n"
"    return true\n"
"  end\n"
"  return false\n"
"end\n"

"function bird:crash()\n"
"  local front, behind = pipes:find_clamp(self.x)\n"
"  if self:crash_with(front) or self:crash_with(behind) then\n"
"    return true\n"
"  end\n"

"  if self.behind ~= behind then\n"
"    self.behind = behind\n"
"    self.score = self.score + 1\n"
"  end\n"

"  return false\n"
"end\n"

"function bird:update()\n"
"  if self.stop then\n"
"    return\n"
"  end\n"

"  if self.guard_time > 0 then\n"
"    self.guard_time = self.guard_time - 1\n"
"  end\n"

"  if bg:is_moving() and self:crash() then\n"
"    if DEBUG then\n"
"      self.stop = true\n"
"    else\n"
"      self.dt = 10 * self.g\n"
"      self.guard_time = 15\n"
"    end\n"
"    bg:stop()\n"
"  end\n"

"  self:update_altitude()\n"
"  scoreboard.text = tostring(self.score)\n"
"end\n"

"function bird:touch()\n"
"  if self.stop or self.guard_time > 0 then\n"
"    return\n"
"  end\n"

"  if not bg:is_moving() then\n"
"    self:reset()\n"
"    bg:begin()\n"
"  else\n"
"    self.speed = self.touch_speed\n"
"  end\n"
"end\n"

"local game = {}\n"
"function game.update()\n"
"  bg:update()\n"
"  bird:update()\n"
"end\n"

"function game.drawframe()\n"
" ej.clear(0xff51c0c9)  -- clear blue\n"

"  bg:draw()\n"
"  bird:draw()\n"
"  scoreboard:draw{x=screen_width/2, y=150}\n"
"end\n"

"function game.touch(what, x, y)\n"
"  if what == 'BEGIN' then\n"
"    bird:touch()\n"
"  end\n"
"end\n"

"function game.message(...)\n"
"end\n"

"function game.handle_error(...)\n"
"end\n"

"function game.on_resume()\n"
"end\n"

"function game.on_pause()\n"
"end\n"

"ej.start(game)\n"
;

static struct WINDOWGAME *
create_game() {
	struct WINDOWGAME * g = (struct WINDOWGAME *)malloc(sizeof(*g));
	g->game = ejoy2d_game();
	g->intouch = 0;
	return g;
}

static int
traceback(lua_State *L) {
	const char *msg = lua_tostring(L, 1);
	if (msg == NULL) {
	if (luaL_callmeta(L, 1, "__tostring") &&
		lua_type(L, -1) == LUA_TSTRING)
		return 1; 
	else
		msg = lua_pushfstring(L, "(error object is a %s value)",
								luaL_typename(L, 1));
	}
	luaL_traceback(L, L, msg, 1); 
	return 1;
}

void
ejoy2d_win_init(int argc, char *argv[]) {
	G = create_game();
	screen_init(WIDTH,HEIGHT,1.0f);
	lua_State *L = ejoy2d_game_lua(G->game);
	lua_pushcfunction(L, traceback);
	int tb = lua_gettop(L);
	int err = luaL_loadstring(L, startscript);
	if (err) {
		const char *msg = lua_tostring(L,-1);
		fault("%s", msg);
	}

	char modname[1024];

	int sz = GetModuleFileNameA(0,  modname, 1024);

	lua_pushlstring(L, modname, sz);

	int i;
	for (i=1;i<argc;i++) {
		lua_pushstring(L, argv[i]);
	}

	err = lua_pcall(L, argc, 0, tb);
	if (err) {
		const char *msg = lua_tostring(L,-1);
		fault("%s", msg);
	}

	lua_pop(L,1);

	ejoy2d_game_start(G->game);
}

void
ejoy2d_win_update() {
	ejoy2d_game_update(G->game, 0.01f);
}

void
ejoy2d_win_frame() {
	ejoy2d_game_drawframe(G->game);
}

void
ejoy2d_win_touch(int x, int y,int touch) {
	switch (touch) {
	case TOUCH_BEGIN:
		G->intouch = 1;
		break;
	case TOUCH_END:
		G->intouch = 0;
		break;
	case TOUCH_MOVE:
		if (!G->intouch) {
			return;
		}
		break;
	}
	// windows only support one touch id (0)
	int id = 0;
	ejoy2d_game_touch(G->game, id, x,y,touch);
}

