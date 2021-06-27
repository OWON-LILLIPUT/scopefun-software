dir /s /b *.cpp > cpp.text
dir /s /b *.c   > c.text
dir /s /b *.h   > h.text
dir /s /b *.hpp > hpp.text
for /F "tokens=*" %%A in (cpp.text) do astyle --options=./scopefun.ini %%A
for /F "tokens=*" %%A in (c.text)   do astyle --options=./scopefun.ini %%A
for /F "tokens=*" %%A in (h.text)   do astyle --options=./scopefun.ini %%A
for /F "tokens=*" %%A in (hpp.text) do astyle --options=./scopefun.ini %%A
dir /s /b *.orig > orig.text
for /F "tokens=*" %%A in (orig.text) do del %%A
del cpp.text
del hpp.text
del c.text
del h.text
del orig.text
pause