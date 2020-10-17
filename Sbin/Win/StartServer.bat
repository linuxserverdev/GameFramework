echo off

taskkill /f /im Gateway.exe

start "Gateway" Gateway.exe

exit