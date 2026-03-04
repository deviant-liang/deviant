@echo off
echo Formatting C++ files with clang-format...

echo Processing header files (.hpp)...
for %%f in (include\*.hpp main.cpp) do (
    echo Formatting %%f
    clang-format -i %%f
)

echo Processing source files (.cpp)...
for %%f in (src\*.cpp) do (
    echo Formatting %%f
    clang-format -i %%f
)

echo All files formatted successfully!
pause