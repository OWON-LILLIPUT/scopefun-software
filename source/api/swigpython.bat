@echo on

rem swig
swig -python -outdir "%cd%" -o "%cd%\scopefunapi_wrap_python.c" "%cd%\scopefunapi.i"

pause
