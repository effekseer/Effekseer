
set OPTS=-f markdown-auto_identifiers -t html5 --section-divs -c ../base.css

for %%i in (*.md) do pandoc -s %%i %OPTS% -o %%~ni.html