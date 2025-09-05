# Light Schedule

## 简介

本项目结合 Ezdta Websocket 接口，可用于配置一周内的灯光开关计划。页面打开时将自动通过 Ezdata Device Token 查询获取设备的开关计划数据字段`light_schedule`，电池状态数据字段
`battery_voltage`，通过页面修改，可以重新写入配置。设备端，处于低功耗定时唤醒状态，当唤醒后，将自动获取最新的`light_schedule`数据，结合当前的时间，做出对应的开关灯动作。

## NodeJS 版本

- 22.18.0

## 补充说明

修改灯光后，将产生以下数据格式内容，点击保存后写入`light_schedule`字段。


```json
{
    "Mon": [
        {
            "time": "8:45",
            "cmd": [
                {"ch":0,"onoff":"on"},
                {"ch":1,"onoff":"on"},
                {"ch":2,"onoff":"off"},
                {"ch":3,"onoff":"on"},
            ]
        },
        {
            "time": "13:15",
            "cmd": [
                {"ch":0,"onoff":"on"},
                {"ch":1,"onoff":"on"},
                {"ch":2,"onoff":"off"},
                {"ch":3,"onoff":"on"},
            ]
        }
    ],
    "Tue": [
        ...
    ],
    "Wed": [
        ...
    ],
    "Thu": [
        ...
    ],
    "Fri": [
        ...
    ],
    "Sat": [
        ...
    ],
    "Sun": [
        ...
    ]
}
```

修改开关后，将产生以下数据格式内容，点击保存后写入`light_switch_info`字段。

```json
{
    "SwitchInfo": [
        {
            "ch": "1",
            "name": "3F-研发办公区"
        }
    ]
}
```


- 连接 Ezdata Websocket 后发送以下内容进行登录。

```json
{"deviceToken":"device_token"}
```

- 发送104指令进行字段数据查询

```json
{"deviceToken":"device_token","body":{"name":"battery_voltage","requestType":104}}
```

- 发送101指令进行字段数据写入

```json
{"deviceToken": "device_token", "body": { "name": "light_schedule", "value": "{data}", "requestType": 101 }}
```




