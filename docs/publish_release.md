# Небольшая памятка (для себя) по созданию новых релизов

Для пунктов 2-3 можно запустить `increment_version.bat`

Для пункта 8 - `ant rename_sis`

Для п. 5 - `ant restore_api_keys`

1) Подтянуть все изменения, объединить ветки
   ```
   git checkout master
   git pull
   git merge wip
   ```
2) Обновить номер версии в 3-х местах
   - inc/defs.h (7 строка): `const TVersion KProgramVersion(X, XX, X);`
   - sis/S60Maps_symbian9.1.pkg (13 строка): `#{"S60Maps"},(0xED689B88),X,XX,X`
   - sis/S60Maps_symbian9.2-9.4.pkg (13 строка): `#{"S60Maps"},(0xED689B88),X,XX,X`

3) Найти все строки (в основном в data/S60Maps.lXX файлах) `// Added in version X.XX` и поставить номер текущей версии

4) Сделать commit и push
   ```
   commit -a -m 'vX.XX.X'
   git tag -a vX.XX.X
   git push
   ```

5) Скопировать содержимое `inc/ApiKeys.h.bak` в `inc/ApiKeys.h`

6) Собрать sis-пакет **для symbian 9.1**
   - Project => Build configurations => Set active => Phone **release** (GCCE) [**S60_3rd**]
   - Project => Clean...
   ![](/docs/images/create_release/2026-04-06_130830.png)
   - Через некоторое время в окошке "Console" должно появиться `***SIS Creation Complete`

7) Собрать sis-пакет **для symbian 9.2 и выше**
   - Всё аналогично, только нужно выбрать Phone **release** (GCCE) [**S60_3rd_fp1**]

8) Переместить файлы и добавить номер версии:
   - `sis/S60Maps_symbian9.1.sis` => `sis/releases/S60Maps_symbian9.1_vX_XX_X.sis`
   - `sis/S60Maps_symbian9.2-9.4.sis` => `sis/releases/S60Maps_symbian9.2-9.4_vX_XX_X.sis`

9) Собрать архив с полными исходниками
   ```
   cd "C:\Symbian\Carbide\workspace\S60Maps"
   ant source_zip
   ```
   Ждём сообщения `BUILD SUCCESSFUL`, в папке `sources` получаем файл `S60Maps_full_sources_vX.XX.X.zip`

10) Находим [тут](https://github.com/artem78/s60-maps/tags) последний тег, нажимаем `Create release`
   - Пишем, что изменилось
     ```
     Changes:
     - Updated blablabla...
     - Fixed blablabla...
     ```
   - Прикрепляем полученные 3 файла (2 sis и zip-архив)
   - Жмём кнопку `Publish release`

11) Вернуться на ту ветку, где были и обновить её
   ```
   git checkout wip
   git merge master --ff-only
   git push
   ```

12) Желательно обновить таблицу состояния переводов [здесь](https://github.com/artem78/s60-maps/blob/master/docs/CONTRIBUTING.md#localization)
    
    скрипт - https://github.com/artem78/SymLocInsert/blob/master/TOOLS/get_translation_stats.py

13) Всё, конец
