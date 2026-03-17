#!/bin/bash

# 1. 设置启动的客户端数量（比如先来 20 个）
CLIENT_COUNT=20

echo "开始启动 $CLIENT_COUNT 个客户端进行压测..."

for i in $(seq 1 $CLIENT_COUNT)
do
    # 2. 模拟用户交互逻辑
    # 利用括号 ( ) 将命令组合，通过 | 传给 ChatClient
    # & 符号让这个客户端在后台运行，不会阻塞循环
    (
        echo "User_$i"             # 第一步：注册用户名
        sleep 0.5                  # 稍微停顿，确保服务器处理完注册
        echo "Server 你好，我是 $i" # 第二步：按照 <用户名> <内容> 格式发消息
        sleep 1                    # 维持连接 1 秒
        echo "exit"                # 第三步：退出
    ) | ./ChatClient > /dev/null 2>&1 & 

    echo "客户端 User_$i 已启动"
done

# 3. 等待所有后台进程结束
wait
echo "压测任务执行完毕！"