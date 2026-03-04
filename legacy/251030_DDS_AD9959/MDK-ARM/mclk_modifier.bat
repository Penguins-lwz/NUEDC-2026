@echo off
set "file=%~dp0..\Core\Src\main.c"
if not exist "%file%" exit /b
powershell -Command ^
    "$content = Get-Content -Path '%file%';" ^
    "$content = $content -replace 'PLLN\s*=\s*[^;]*;', 'PLLN = %1;';" ^
    "$content | Set-Content -Path '%file%'"
