local inited=false

local ip="127.0.0.1"
local port=20131
local logid=66

UDPLOG_LEVEL_I=1
UDPLOG_LEVEL_D=2
UDPLOG_LEVEL_W=3
UDPLOG_LEVEL_E=4

local log=function(level,msg,tag)
    if not inited then
        udplog_resetDst(ip,port,logid)
        inited=true
    end
    if not tag then
        tag=logid
    end
    udplog(level,tag,msg)
end

-------------------------------------------------------------------------
-- @description 设置log输出地址，如果未设置，则使用默认地址
-- @param aIp ip地址
-- @param aPort 端口
-- @param aLogid 此log源的id 
-------------------------------------------------------------------------
function log_set(aIp,aPort,aLogid)
    ip=aIp
    port=aPort
    logid=aLogid
end

-------------------------------------------------------------------------
-- @description 输出等级为information的日志
-- @param msg 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function logi(msg,tag)
    log(UDPLOG_LEVEL_I,msg,tag)
end

-------------------------------------------------------------------------
-- @description 输出等级为debug的日志
-- @param msg 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function logd(msg,tag)
    log(UDPLOG_LEVEL_D,msg,tag)
end

-------------------------------------------------------------------------
-- @description 输出等级为warning的日志
-- @param msg 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function logw(msg,tag)
    log(UDPLOG_LEVEL_W,msg,tag)
end

-------------------------------------------------------------------------
-- @description 输出等级为error的日志
-- @param msg 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function loge(msg,tag)
    log(UDPLOG_LEVEL_E,msg,tag)
end

-------------------------------------------------------------------------
-- @description 打印table
-- @param level 等级
-- @param table 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function logTable(level,table,tag)
    local msg="{<br/>"
    for k,v in pairs(table)do
        if(type(v)=="string")then
            v='@' .. v
        end
        msg=msg .. "&nbsp" .. k .. "&nbsp" .. ":" .. "&nbsp" .. v .. "&nbsp" .. ",<br/>"
    end
    msg=msg .. "}"
    log(level,msg,tag)
end

-------------------------------------------------------------------------
-- @description 打印数组
-- @param level 等级
-- @param array 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function logArray(level,array,tag)
    local msg="["
    local dot=""
    for k,v in ipairs(array)do
        if(type(v)=="string")then
            v='@' .. v
        end
        msg=msg .. dot .. "&nbsp" .. v
        dot=","
    end
    msg=msg .. "&nbsp]"
    log(level,msg,tag)
end

-------------------------------------------------------------------------
-- @description 打印16进制
-- @param level 等级
-- @param bytes 信息主体
-- @param tag 标记
-------------------------------------------------------------------------
function log0x(level,bytes,tag)
    local msg;
    if(type(bytes)=="table") then
        msg="["
        local dot=""
        for k,v in ipairs(bytes)do
            v=string.format("%x",v)
            msg=msg .. dot .. "&nbsp" .. v
            dot=","
        end
        msg=msg .. "&nbsp]"
    else
        msg=string.format("%x",bytes)
    end
        log(level,msg,tag)
end


