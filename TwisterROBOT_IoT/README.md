# WiFi_BLE

## 简介

该工程实现了使用ESP32芯片 **同时进行**WiFi及BLE的连接

在该示例代码中, WiFi实现了网络授时及http长链接(socket)的功能, BLE实现了与手机的配对连接(注意修改MTU的大小)

在两种通信的过程中, http服务器与手机都会向ESP32发送一串**JSON格式字符串**, 代表各个命令信息, 命令信息见"cmd.md"文件

ESP32会使用cJSON将其解析, 并做出对应的响应, 如修改变量值等等.

同时, ESP32会不定时地向两者发送心跳包, 包内存储各类数据信息, 默认时间为100s发送一次, 可被命令修改

## 项目难点

1. 两种通信使用同一根射频天线, 资源的争夺.	--	**分时复用**

   >好消息是, 在arduino框架下, 最底层已封装了硬件资源的互斥锁
   >
   >**我们仅需直接调用, 无需进行任何处理**

2. BLE通信协议

   > 之前没有了解过的新协议, 有"UUID","特征值"等概念, 一般情况下我们只需对其进行声明即可
   >
   > 在初始化完成后, 需要进行"蓝牙广播"以及"server端广播"
   >
   > **如果不进行server端广播的话, 下位机可能仅仅只能实现连接蓝牙, 而不能获取其信号数据**
   >
   > 同时, 在BLE的server广播中, 数据也被分为了几种类型, **"读/写/通知(notify)"**, 这几种类型可以同时被定义
   >
   > 其中较为特殊的是"notify", 当广播信号被定义为此时, 下位机可以通过"订阅通知"的方式来实现长线收取, 否则必须手动发起接收请求, 才会捕获一段信号. 心跳包便是使用了notify来实现的
   >
   > > 目前仍不明白的点:
   > >
   > > 1. 特征码的作用是什么?
   > > 2. UUID的作用是什么?

3. http, TCP通信协议

   > 这一协议用的比较多, 尤其是物联网项目, 本人暂时没有深入了解
   >
   > ESP32对物联网服务器发起数据请求, 主要利用的是代码中的**http.get()**函数
   >
   > 这一函数可以实现从指定的http服务器进行数据请求, 一般情况下我们需要其返回一串json串
   >
   > 解析json串的方式同BLE
   >
   > 向http服务器发送数据使用的是post指令, 发送一串字符串过去, 建议为json格式

4. 对json包的解析与响应

   > 这里使用了cJSON库函数, 用起来还是很方便的
   >
   > 首先一定要创建一个root指针
   >
   > **cJSON *root = cJSON_Parse(your_json_string);**
   >
   >  注意在解析完后对其进行**仅一次的delete操作!**
   >
   > 获取到这个root之后, 便可以对其进行**指定字符串名称的解包**
   >
   > 如从"cmd"后抓取一个int型变量, 从"serverip"后抓取一个char*变量等等
   >
   > 用起来很方便, 但是要**注意json包格式的正确性**, 否则很容易指针跑飞, 系统宕机.

5. 组合并发送json心跳包, 包含自身数据信息

   > 同样利用了cJSON库, 将字符串名称与想要发送的变量结合起来, 组合成一长串json格式的数据
   >
   > 首先创建一个json串的root, 使用createObject函数.	**root目前是不含任何数据信息的!**
   >
   > 此后, 便可以在此root上进行json串的串接. 
   >
   > 格式可以从解包那里理解, 先为其赋一个字符串昵称, 而后在后面利用cJSON的函数创建对象,
   >
   > 如**cJSON_AddItemToObject(root, "cmd", cJSON_CreateNumber(19));**
   >
   > 第三个形参一定要用cJSON库中给出的函数, 可以create很多变量, 如string, double等等
   >
   > **难点:**
   >
   > 要组合json串里的json串, 要用到AddArray
   >
   > **步骤1: 在创建好cJSON的root指针之后, 创建一个cJSON的array指针**
   >
   > cJSON *array = cJSON_CreateArray();
   >
   > **步骤2: 将这一array对象添加入初级json串的root中**
   >
   > cJSON_AddItemToObject(root, "wifi", tx_wifi_array);
   >
   > **步骤3: 将次级json包的对象添加入这个array**
   >
   > cJSON *objcet = cJSON_CreateObject();
   >
   > cJSON_AddItemToArray(array, objcet);
   >
   > **步骤4: 将普通变量添加进这个object, 填充次级json包的内容**
   >
   > cJSON_AddItemToObject(objcet, "ssid", cJSON_CreateString(your_string));
   >
   > **步骤5: 生成json串**
   >
   > char *json_string = cJSON_Print(root);
   >
   > **此后, 便从root得到了一个完整的json串, 名为json_string**