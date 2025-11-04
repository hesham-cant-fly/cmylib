#!/usr/bin/env sh

USERNAME="hesham-cant-fly"
REPO="cmylib"
BRANCH="main"

get-file-url()
{
    fileName=$1
    echo "https://raw.githubusercontent.com/$USERNAME/$REPO/refs/heads/$BRANCH/$fileName"
}

get-file()
{
    fileName=$1
    url=$(get-file-url $fileName)
    curl -s $url
}

get-cmake-template()
{
    get-file cmake-template.txt
}

get-main-c()
{
    echo "#include <stdio.h>"
    echo ""
    echo "int main(int argc, char **argv) {"
    echo "  printf(\"Hello World\\n\");"
    echo "  return 0;"
    echo "}"
}

files=(
    "cmylib.h"
    "cli_tools.h"
    "my_array.h"
    "my_commons.h"
    "my_string.h"
    "my_termcolor.h"
    "utf8.h"
)

mkdir include/
mkdir src/
touch .project
get-main-c > main.c

get-cmake-template > CMakeLists.txt

for i in "${files[@]}"; do   # The quotes are necessary here
    get-file $i > include/$i
done

echo "#define CMYLIB_IMPLEMENTATION
#include \"cmylib.h\"" > src/cmylib.c

