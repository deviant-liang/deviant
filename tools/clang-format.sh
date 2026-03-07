#!/bin/bash
# ---------------------------------------------
# format.sh
# 用 clang-format 排版專案根目錄、include/ 和 src/ 的 .cpp/.hpp
# ---------------------------------------------

CLANG_FORMAT=clang-format  # 可改成 clang-format-20

# 1. 根目錄下的 .cpp/.hpp
ROOT_FILES=$(find . -maxdepth 1 -type f \( -name "*.cpp" -o -name "*.hpp" \))

# 2. include/ 下的所有子目錄檔案
INCLUDE_FILES=$(find include -type f \( -name "*.cpp" -o -name "*.hpp" \))

# 3. src/ 下的所有子目錄檔案
SRC_FILES=$(find src -type f \( -name "*.cpp" -o -name "*.hpp" \))

# 合併
FILES="$ROOT_FILES $INCLUDE_FILES $SRC_FILES"

if [ -z "$FILES" ]; then
    echo "沒有找到任何 .cpp 或 .hpp 檔案"
    exit 0
fi

echo "排版以下檔案："
echo "$FILES"

for f in $FILES; do
    $CLANG_FORMAT -i "$f"
done

echo "完成排版！"