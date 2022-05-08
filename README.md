# sticker-MOOC

仿腾讯桌面整理外观设计。以时间为轴的任务便签，支持中国大学慕课测验 / 作业 / 考试 / 互评任务导入并自动刷新。暂时不适配屏幕缩放，这意味着一些高分辨率屏幕可能无法使用。暂时不支持颜色自定义。

## 使用说明

### 运行 sticker-MOOC.exe 打开应用。

### 按钮说明：
   
   - ![delButton](/src/icon/delButton.png) 双击按钮，清空已做
   - ![addButton](/src/icon/addButton.png) 添加待做
   - ![settingButton](/src/icon/settingButton.png) 设置慕课自动获取
   - ![foldButoon](/src/icon/foldButton.png) ![openButton](/src/icon/openButton.png) 折叠 / 展开格子
   - ![lockButton](/src/icon/lockButton-locked.png) 锁定格子，点击之后无法移动 / 调整大小
   - ![unlockButton](/src/icon/lockButton-unlocked.png) 解锁格子
   - ![prevPage](/src/icon/prevPage.png) ![nextPage](/src/icon/nextPage.png) 上一页 / 下一页
   - ~[flushButton](/src/icon/flushButton.png) 刷新慕课

### 慕课获取说明

   1. 这需要自行抓包获取 `mob-token`. **警告：请避免随意泄露你的 `mob-token`，这可能导致你被有心人恶搞，后果可能是不可挽回的。**
   
   2. 手机 / 平板下载任意一款抓包软件，如 ios 的 Stream，以下将以该软件为示例进行演示。
   
   3. 打开软件，首次使用通常需要为其进行配置准备，软件会有详细的操作步骤。点击开始抓包。

        <img src="/image/1.PNG" width="50%" ></img>

   4. 切换到 “中国大学MOOC”，进入底下的 “学习” 页面，等待课程加载完毕。

        <img src="/image/2.PNG" width="50%" ></img>

   5. 回到软件，结束抓包，进入抓包历史，点击最近一条记录。

        <img src="/image/3.PNG" width="50%" ></img>

        <img src="/image/4.PNG" width="50%" ></img>

   6. 点击搜索，搜索 `getAllMyCourse` 关键词，进入搜索结果。

        <img src="/image/5.PNG" width="50%" ></img>

   7. 切换到请求页面，查看请求主体（即图中的查看表单），即可看到 `mob-token`，它通常由整数与 `a` 到 `f` 的小写字母组成。

        <img src="/image/6.PNG" width="50%" ></img>

### 文件说明：

   - `config.ini` 记录窗口基本信息
   - `courseList.json` 记录个人慕课课程列表
   - `taskList.json` 记录个人慕课测验 / 作业 / 考试 / 互评列表
   - `data.json` 记录除慕课外的任务列表
   - `moocConfig.json` 记录个人慕课 mob-token

## 源码说明

   1. 使用 QT 5.15 与 Python 3.10 编写。
   
   2. 本人非计算机系学生，所以未了解太多命名方面的规范，以及对象之间的关系可能有些混乱。
   
   3. **debug 时需要自行去 `gin.pro` 更改 Python 路径**
   
   4. 因为不会用 QT 的 Network 模块，所以改用了 Python 的 Requests 模块。在尝试发布 Release 版本时需要自行添加 Python 的依赖库并自行删除多余依赖库，否则文件体积会很大，这一步骤挺麻烦的。考虑到 Python 代码体积不大，所以直接将要调用的函数打包成了 `.exe` 文件，由 QT 阻塞运行。
   
   5. 调用 Python 的两个地方分别在 `void MainWindow::moocAutoEvent()` 以及 `void MoocSetting::getAllCourse()`. 需要注意的是，源码中并未提供这两个文件，需要自行在 Python 中分别调用 `mooc.getAllMyCouresList` 与 `mooc.getCourseTaskList` 并自行打包成 `.exe` 文件，可以使用 `pyinstaller -F -w -i icon.ico xxxx.py`.
   
   6. 如果不想这样做，可以按照以下方式改写代码直接调用 Python 函数。[参考文章](https://zhuanlan.zhihu.com/p/450318119) [参考文档](https://docs.python.org/3/)

       ```cpp
       // QT 与 Python 的宏定义冲突了
       #undef slots
       #include <Python.h>
       #define slots Q_SLOTS
       -------------------------
       // 调用前必须初始化，可以只写在 main 函数中
       Py_Initialize();
       PyRun_SimpleString("import sys");
       PyRun_SimpleString("sys.path.append('./')");
       // 函数调用语句
       PyObject* pModule = PyImport_ImportModule("mooc"); // 导入模块 mooc.py
       PyObject* pClass = PyObject_GetAttrString(pModule, "mooc"); // 获取类名
       PyObject* pObject = PyEval_CallObject(pClass, NULL);  // 实例化对象
       PyObject* pFunc = PyObject_GetAttrString(pObject, "getAllMyCouresList"); // 获取成员函数
       PyEval_CallObject(pFunc, NULL); // 调用函数
       // 结束
       Py_Finalize();
       ```
