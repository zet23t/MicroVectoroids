local fp = assert(io.open("assets/hemifont.fnt","r"))
local fnt = fp:read "*a"
fp:close()
local fontmap = {
	lineheight = tonumber((fnt:match("size=(%d+)")))
}
local function pattern(sample)
	return (sample:gsub("%s+","%%s%+"):gsub("%d+","(%%-*%%d+)"))
end
local function tonumbers(...)
	local tab = {...}
	for i=1,#tab do tab[i]= tonumber(tab[i]) end
	return unpack(tab)
end

for id,x,y,width,height,xoffset,yoffset,xadvance in 
	fnt:gmatch(pattern "char id=32   x=37    y=51    width=4     height=3     xoffset=-1    yoffset=11    xadvance=3") 
do
	local c = string.char(id)
	local x,y,width,height,xoffset,yoffset,xadvance = tonumbers(x,y,width,height,xoffset,yoffset,xadvance)
	x = x - 0
	y = y - 0
	width = width + 0
	height = height + 0
	fontmap[#fontmap + 1] = {
		c = c,
		u = x, v = y, w = width, h = height,
		offsetX = xoffset + 0,
		offsetY = yoffset + 0,
		spacing = xadvance
	}
	print(string.char(id),x,y,yoffset)
end

return {
	format = "rgb565";
	transparent_color = 0xff00ff;
	fontmap = fontmap
}