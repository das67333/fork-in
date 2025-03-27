## fork-in

Клонирование:
```
git clone --recurse-submodules https://github.com/das67333/fork-in.git
```

Для файлов с расширениями "go", "rs", "js", "ts" вызывается ./fork-in-dt, для фейкового расширения "lsp" ./fork-in-lsp, для "md" - ./fork-in-regex, а токенизаторы для "json", "c", "h", "cpp", "hpp", "cxx", "py" встроены в репозиторий из tree-sitter.

Тесты в [fork-in_test.sh](fork-in_test.sh), [fork-in-tree-sitter_test](fork-in-tree-sitter_test.sh). Там же можно посмотреть команды сборки.
