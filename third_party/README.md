# Сторонние файлы

## stb_truetype.h

- Источник: https://github.com/nothings/stb
- Файл: `stb_truetype.h`
- Лицензия: public domain / MIT (текст в заголовке файла)
- Назначение: растеризация текста в `src/render/font.cpp`

## Roboto

- Источник: https://github.com/googlefonts/roboto
- Файл: `Roboto-Regular.ttf`
- Лицензия: Apache License 2.0, текст в `Roboto-LICENSE.txt`
- Назначение: шрифт с кириллицей для `Renderer::draw_text`

Оба файла лежат в репозитории, чтобы сборка не зависела от системных шрифтов и
не требовала скачивания бинарников.
