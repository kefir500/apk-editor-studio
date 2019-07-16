if [ -z "$CI" ]; then PATH=$PATH:~/Qt5.6.3/5.6.3/clang_64; fi
if [ -z "$VERSION" ]; then VERSION=$(cat $(dirname "$BASH_SOURCE")/../../VERSION); fi
