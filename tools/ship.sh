# target 是electron的版本
node-gyp configure
HOME=~/.electron-gyp node-gyp rebuild --target=7.1.4 --arch=x64 --dist-url=https://electronjs.org/headers