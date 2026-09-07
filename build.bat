@echo off
echo Building STALKER PS3 demo via Docker...
docker run --rm -v "%CD%:/src" -w /src ps3dev/ps3dev make pkg
if exist stalker_demo.pkg (
 echo [OK] stalker_demo.pkg собран!
) else (
 echo [ERR] Сборка не удалась, смотри лог выше
)
pause
