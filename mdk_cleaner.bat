@echo off
del /s *.uvguix.*
del /s *.mxproject
for /f "delims=" %%d in ('dir /b /s /ad "MDK-ARM"') do (
    for /d %%s in ("%%d\*") do rd /s /q "%%s"
)
