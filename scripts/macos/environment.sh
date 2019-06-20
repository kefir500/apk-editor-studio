if [ -z "$CI" ]; then PATH=$PATH:~/Qt5.*/5.*/clang_64; fi
if [ -z "$VERSION" ]; then VERSION=$(cat $(dirname "$BASH_SOURCE")/../../VERSION); fi
