# Сборка через GitHub (без Docker на ПК)

1. Создай пустой репозиторий на github.com (например `stalker-ps3-demo`)
2. Залей туда содержимое этого архива:
   ```powershell
   cd stalker_demo
   git init
   git add .
   git commit -m "initial"
   git branch -M main
   git remote add origin https://github.com/ТВОЙ_НИК/stalker-ps3-demo.git
   git push -u origin main
   ```
3. На GitHub открой вкладку `Actions` -> выбери `Build PS3 PKG` -> дождись зеленой галки (~2-3 минуты)
4. Внизу страницы сборки будет `Artifacts` -> скачай `stalker_demo-ps3-pkg.zip` -> внутри `stalker_demo.pkg`
5. Закинь `stalker_demo.pkg` на флешку FAT32 -> PS3 -> Install Package Files

Повторная сборка запускается автоматически при каждом `git push` или вручную кнопкой `Run workflow`.
