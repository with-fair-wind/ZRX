# 注意点

- 当开启自动uic时，对于 **`xxx.ui`** 文件，只需要在cpp文件中包含 **`<./ui_xxx.h>`** 则会自动在build目录下编译出 **`ui_xxx.h`** 文件头
- 不同的qt版本好像只需要指定 **`CMAKE_PREFIX_PATH`** 即可识别
- 对于生成的 **`.zrx`**，即使使用 **`.../msvc2017_64/bin/windepoly.exe`** 生成所需动态库，依然无法加载。必须添加环境变量 **.../msvc2017_64/bin**，才可以加载
- 必须设置环境变量 **`QT_QPA_PLATFORM_PLUGIN_PATH`**，Qt界面才能正常运行，不确定是否是因为qt版本安装过多的问题
- **`QApplication`** 的创建必须是一个全局变量在init和unload的时候分别创建和析构才不会导致重复加载卸载崩溃，以及非初次加载zrx创建widget崩溃问题
