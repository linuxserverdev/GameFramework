local debug = require "debug"

local _trace = trace
local _print = print
local _warning = warning
local _error = error

local MAX_STRING_LENGTH = 512

local function buildOutStr(...)
    local t = {}
	for i = 1, select("#", ...) do
		table.insert(t, tostring(select(i, ...)))
	end
	local str = string.gsub(table.concat(t, " "), "%%", "%%%%")
	local info = debug.getinfo(3,"nSl");
	if info then
		local filename = "";
		local nIndex = string.find(info.short_src,".lua",1,true);
		local nIndexString = string.find(info.short_src,"string",1,true)
		if nIndex and nIndexString and nIndex > 1 and nIndexString > 1 then
            filename = string.sub(info.short_src,nIndexString + 8,nIndex + 3);
		end
		str = str..' - '..(filename or "unknown")..':'..tostring(info.currentline or -1)
	end
	return str
end

--@note：把字符串按最大长度拆分输出
local function output(str, fn_print)
	if not fn_print then fn_print = _print end

	if str == nil then
		fn_print(nil)
	else
		local len = #str
		if len < MAX_STRING_LENGTH then
			fn_print(str)
		elseif len > 0 then
			local idx = 1
			local substr = str
			while #substr > 0 do
				local s = string.substr(substr, 1, MAX_STRING_LENGTH)
				fn_print(s)
				substr = string.substr(substr, #s+1)
			end
		end
	end
end

--@note：重新定义print，替换%,避免出现Trace参数个数与格式不匹配
function print(...)
	local str = buildOutStr(...)
	output(str)
end

function warning(...)
	local str = buildOutStr(...)
	output(str, _warning)
end

function trace(...)
	local str = buildOutStr(...)
	output(str, _trace)
end

function error(...)
	local str = buildOutStr(...)
	output(str, _error)
end