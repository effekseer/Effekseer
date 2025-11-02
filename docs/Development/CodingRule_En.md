
# Coding Rules

Effekseer uses shared formatting and naming conventions to keep the code base consistent. Apply the rules in this document to every change unless the file is explicitly marked as third-party code.

## General Policy

- Use the formatting tools tracked in the repository; avoid hand-crafted indentation or spacing that fights the formatter.
- Limit formatting changes to the lines you touch so that diffs stay reviewable.
- Respect the existing style of imported third-party or auto-generated sources, even when it differs from the default Effekseer style.
- For large clean-ups or reformatting tasks, coordinate with the maintainers before submitting a pull request.

## C++ Guidelines

### Formatting

- Run `clang-format` with the configuration at `Dev/Cpp/.clang-format`. Integrate it into your editor or use `clang-format -i <file>`.
- The profile is derived from LLVM with Allman braces, tabs with width 4 (`UseTab = Always`, `IndentWidth = 4`), and no column limit. Break long lines manually when it improves readability.
- Keep the include order and whitespace that `clang-format` produces; avoid manual tweaks that reintroduce churn.
- Refrain from reformatting entire legacy files together with behavioral changes. If a file needs a wholesale cleanup, discuss it first or submit it as a dedicated formatting change.

### Naming

| Element | Convention | Example |
| --- | --- | --- |
| Public data members | PascalCase (`CamelCase`) | `RenderTarget` |
| Protected/private data members | lowerCamelCase with trailing `_` (`camelBack_`) | `currentIndex_` |
| Classes, structs, enums | PascalCase (`CamelCase`) | `ModelRenderer` |
| Member functions | PascalCase (`CamelCase`) | `UpdateBuffer()` |

- Keep namespace names consistent with the surrounding module (for example `Effekseer` or `EffekseerRendererGL`).
- Choose descriptive names and prefer whole words over unexplained abbreviations.

## C# Guidelines

### Formatting

- Follow the `.editorconfig` settings. For the editor project the file is located at `Dev/Editor/.editorconfig`; other C# projects ship their own configuration alongside the source.
- Current settings require tab indentation (`indent_style = tab`, `indent_size = 4`) and braces for control blocks.
- Apply formatting to the code you modify. In Visual Studio, run `Format Document` (`Ctrl+K, Ctrl+D`) after your edits or execute `dotnet format` in the project directory.
- When you edit code that relies on a different `.editorconfig`, let that local file drive the formatting.

### Naming

- Follow the .NET conventions enforced by `.editorconfig`: PascalCase for classes, structs, properties, events, and public methods; camelCase for private fields, optionally ending with `_` when consistent with the surrounding code.
- Prefix interfaces with `I` (for example `IRenderer`) and suffix asynchronous methods with `Async`.
- Use expressive identifiers; avoid abbreviations unless they are widely understood within the Effekseer project.

## Exceptions

- If you must merge a large block of code that temporarily violates these rules (such as an upstream third-party update), contact the maintainers beforehand or document the deviation in the pull request.
- Record any intentional style deviations in code comments or in the review description so reviewers understand the reasoning.
