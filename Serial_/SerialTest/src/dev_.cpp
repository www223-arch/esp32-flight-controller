#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

// ===================== 配置项（改这里！）=====================
const char* WIFI_SSID = "你的手机热点名称";    // 替换成你的手机热点名
const char* WIFI_PWD  = "你的手机热点密码";    // 替换成你的热点密码
const uint16_t WEB_PORT = 80;                 // Web服务器端口（默认80）
const char* WS_PATH = "/ws";                  // WebSocket路径

// ===================== 全局变量 =====================
AsyncWebServer server(WEB_PORT);              // 异步Web服务器对象
AsyncWebSocket ws(WS_PATH);                   // WebSocket对象

// 无人机控制参数（后续对接电机/IMU）
int throttle_target = 0;    // 油门目标值（0-100）
float pitch = 0.0f;         // 俯仰角（模拟数据）
float roll = 0.0f;          // 横滚角（模拟数据）

// ===================== WebSocket事件处理（核心通信逻辑）=====================
// 处理WebSocket的连接、断开、数据接收事件
void onWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len) {
  // 1. 事件类型：连接建立
  if (type == WS_EVT_CONNECT) {
    Serial.printf("客户端[%u]已连接\n", client->id());
    // 连接成功后，给客户端发初始数据
    String init_data = String("throttle:") + throttle_target + 
                       ",pitch:" + pitch + ",roll:" + roll;
    client->text(init_data);
  }
  // 2. 事件类型：连接断开
  else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("客户端[%u]已断开\n", client->id());
  }
  // 3. 事件类型：收到客户端（手机）的数据
  else if (type == WS_EVT_DATA) {
    // 解析收到的字节数据为字符串
    String cmd = String((char*)data).substring(0, len);
    Serial.printf("收到客户端指令：%s\n", cmd.c_str());

    // 解析指令：格式为"指令名:值"
    if (cmd.startsWith("throttle:")) {
      // 提取油门值（比如"throttle:50" → 50）
      throttle_target = cmd.substring(9).toInt();
      // 限制油门范围（0-100，避免超量程）
      throttle_target = constrain(throttle_target, 0, 100);
    } 
    else if (cmd.startsWith("pitch:")) {
      // 提取俯仰角（比如"pitch:2.5" → 2.5）
      pitch = cmd.substring(6).toFloat();
      pitch = constrain(pitch, -10.0f, 10.0f); // 限制范围
    } 
    else if (cmd.startsWith("roll:")) {
      // 提取横滚角（比如"roll:-1.2" → -1.2）
      roll = cmd.substring(5).toFloat();
      roll = constrain(roll, -10.0f, 10.0f); // 限制范围
    }
  }
}

// ===================== 提供网页内容（HTML/CSS/JS）=====================
// 把网页代码以字符串形式返回给浏览器
String getHtmlPage() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>无人机控制界面</title>
  <style>
    /* 简单美化界面（适配手机） */
    body { font-family: Arial; margin: 20px; background: #f0f0f0; }
    .control-box { background: white; padding: 20px; border-radius: 10px; margin-bottom: 20px; }
    .slider { width: 100%; margin: 10px 0; }
    .btn-group { display: flex; gap: 10px; margin: 10px 0; }
    .btn { flex: 1; padding: 10px; border: none; border-radius: 5px; background: #007bff; color: white; font-size: 16px; }
    .status-box { background: #333; color: white; padding: 15px; border-radius: 10px; font-size: 18px; }
  </style>
</head>
<body>
  <h1>无人机可视化控制</h1>

  <!-- 油门控制滑块 -->
  <div class="control-box">
    <h3>油门控制 (0-100)</h3>
    <input type="range" class="slider" id="throttleSlider" min="0" max="100" value="0">
    <p>当前油门：<span id="throttleValue">0</span></p>
  </div>

  <!-- 俯仰/横滚控制按钮 -->
  <div class="control-box">
    <h3>俯仰角控制 (±10°)</h3>
    <div class="btn-group">
      <button class="btn" onclick="sendPitch(-1)">俯仰-1°</button>
      <button class="btn" onclick="sendPitch(0)">俯仰归0</button>
      <button class="btn" onclick="sendPitch(1)">俯仰+1°</button>
    </div>

    <h3>横滚角控制 (±10°)</h3>
    <div class="btn-group">
      <button class="btn" onclick="sendRoll(-1)">横滚-1°</button>
      <button class="btn" onclick="sendRoll(0)">横滚归0</button>
      <button class="btn" onclick="sendRoll(1)">横滚+1°</button>
    </div>
  </div>

  <!-- 实时状态显示 -->
  <div class="status-box">
    <p>实时姿态：</p>
    <p>油门：<span id="statusThrottle">0</span></p>
    <p>俯仰角：<span id="statusPitch">0.0</span> °</p>
    <p>横滚角：<span id="statusRoll">0.0</span> °</p>
  </div>

  <script>
    // ===================== 前端WebSocket逻辑 =====================
    let ws;
    // 连接WebSocket（替换成ESP32的IP，这里用相对路径）
    function connectWebSocket() {
      ws = new WebSocket('ws://' + window.location.host + '/ws');
      
      // 连接成功
      ws.onopen = function() {
        console.log("WebSocket已连接");
      };
      
      // 收到ESP32的数据
      ws.onmessage = function(event) {
        // 解析数据：格式"throttle:50,pitch:2.5,roll:-1.2"
        const data = event.data.split(',');
        data.forEach(item => {
          const [key, value] = item.split(':');
          if (key === 'throttle') {
            document.getElementById('statusThrottle').textContent = value;
            document.getElementById('throttleSlider').value = value;
            document.getElementById('throttleValue').textContent = value;
          } else if (key === 'pitch') {
            document.getElementById('statusPitch').textContent = value;
          } else if (key === 'roll') {
            document.getElementById('statusRoll').textContent = value;
          }
        });
      };
      
      // 连接断开，自动重连
      ws.onclose = function() {
        console.log("WebSocket断开，5秒后重连");
        setTimeout(connectWebSocket, 5000);
      };
    }

    // ===================== 发送控制指令到ESP32 =====================
    // 发送油门值（滑块拖动时触发）
    document.getElementById('throttleSlider').addEventListener('input', function() {
      const value = this.value;
      document.getElementById('throttleValue').textContent = value;
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send('throttle:' + value);
      }
    });

    // 发送俯仰角指令
    function sendPitch(value) {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send('pitch:' + value);
      }
    }

    // 发送横滚角指令
    function sendRoll(value) {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send('roll:' + value);
      }
    }

    // 页面加载完成后，连接WebSocket
    window.onload = function() {
      connectWebSocket();
    };
  </script>
</body>
</html>
)HTML";
  return html;
}

// ===================== 初始化函数 =====================
void setup() {
  // 初始化串口（调试用）
  Serial.begin(115200);
  delay(100);

  // 1. 连接手机热点
  Serial.printf("正在连接WiFi：%s...\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // 连接成功，打印ESP32的IP（关键！手机要访问这个IP）
  Serial.println("\nWiFi连接成功！");
  Serial.printf("ESP32 IP地址：http://%s\n", WiFi.localIP().toString().c_str());

  // 2. 初始化WebSocket
  ws.onEvent(onWebSocketEvent); // 绑定事件处理函数
  server.addHandler(&ws);       // 把WebSocket添加到Web服务器

  // 3. 初始化Web服务器：根路径返回控制网页
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getHtmlPage());
  });

  // 4. 启动Web服务器
  server.begin();
  Serial.println("Web服务器已启动！");
}

// ===================== 主循环 =====================
void loop() {
  // 清理WebSocket的闲置连接（必须加，否则内存泄漏）
  ws.cleanupClients();

  // 模拟姿态数据变化（后续替换成真实IMU数据）
  // 这里只是演示，实际要读ICM42688的pitch/roll
  static unsigned long last_update = 0;
  if (millis() - last_update > 500) { // 每500ms更新一次模拟数据
    // 模拟微小的姿态波动
    pitch += (random(-10, 10) / 100.0f);
    roll += (random(-10, 10) / 100.0f);
    // 限制范围
    pitch = constrain(pitch, -10.0f, 10.0f);
    roll = constrain(roll, -10.0f, 10.0f);

    // 把最新状态发给所有连接的WebSocket客户端（手机）
    String status_data = String("throttle:") + throttle_target + 
                         ",pitch:" + pitch + ",roll:" + roll;
    ws.textAll(status_data);

    last_update = millis();
  }

  // 后续添加：把throttle_target/pitch/roll传给PID/电机混控逻辑
  // 比如：motor1 = throttle_target + pitch_comp - roll_comp;
  delay(10);
}