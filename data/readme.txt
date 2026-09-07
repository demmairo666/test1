# S.T.A.L.K.E.R. PS3 Demo - Extended (PSL1GHT / tiny3D)

Короткая техно-демка в стиле Clear Sky для PS3. Собирается в .pkg для CFW/HEN.

## Что внутри (расширенная версия)
- Корридор 20x4 метра с туманом и мигающей лампой (шейдер)
- FPS камера как в Сталкере (левый стик - ходьба, правый - обзор)
- Фонарик (вкл/выкл - треугольник)
- Оружие в руках (квадрат - выстрел, вибрация, вспышка)
- Прицел точка + счетчик радиации/патронов (2D overlay)
- Эмбиент (ветер + счетчик Гейгера) через аудиотред
- Выход - PS button

## Как собрать в .pkg

### Вариант 1 - Docker (рекомендуется, ничего ставить не надо)
```powershell
docker run --rm -v ${PWD}:/src -w /src ps3dev/ps3dev make pkg
# на выходе: stalker_demo.pkg в корне проекта
```

### Вариант 2 - локальный PSL1GHT (Windows MSYS2)
```bash
pacman -S mingw-w64-x86_64-ps3dev
make pkg
```

### Вариант 3 - WSL / Linux
```bash
sudo apt install ps3dev
make pkg
```

## Установка на PS3
1. Скопируй `stalker_demo.pkg` на флешку FAT32
2. На PS3: Install Package Files -> выбери pkg
3. Запуск из XMB -> Stalker Demo

## Тест без консоли
```bash
rpcs3 stalker_demo.pkg  # или запусти EBOOT.BIN напрямую
```

## Управление
- Левый стик - движение (WASD)
- Правый стик - обзор мышью
- L2/R2 - стрейф
- Треугольник - фонарик
- Квадрат - выстрел
- Крест - перезарядка (анимация)
- Start - пауза

## Файлы проекта
- source/main.cpp - инициализация tiny3D + главный цикл
- source/camera.h - FPS камера
- source/level.h - геометрия корридора + туман
- source/weapon.h - оружие и прицел

Требует tiny3D + PSL1GHT 2.0.0+, разрешение 1280x720
