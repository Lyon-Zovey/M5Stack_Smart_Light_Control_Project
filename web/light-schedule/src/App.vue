<template>
  <el-container style="height: 100vh">
    <el-main>
      <!-- 模板标题 -->
      <div class="template-header">
        <span class="template-title">灯光开关计划</span>
      </div>

      <!-- 电池状态 -->
      <div class="battery-progress">
        <el-progress :percentage="batteryPercent" color="#67C23A" :text-inside="true" :stroke-width="22" />
        <div class="battery-voltage">
          电池电压: {{ batteryVoltage.toFixed(2) }} V
        </div>
      </div>

      <!-- 日程模板 -->
      <day-template-tabs :switches="switches.SwitchInfo" :schedule="schedule" @update-schedule="updateSchedule" />

      <!-- 按键管理 -->
      <switch-manager :switches="switches.SwitchInfo" @update-switches="updateSwitches" />

      <!-- 浮动操作按钮 -->
      <div class="floating-buttons">
        <el-button round type="success" @click="saveTemplate">
          保存
        </el-button>
        <el-button round type="warning" @click="refreshTemplate">
          刷新
        </el-button>
      </div>
    </el-main>
  </el-container>
</template>

<script>
import { ref, computed, onMounted, onBeforeUnmount } from "vue";
import { ElLoading, ElMessage } from "element-plus";
import DayTemplateTabs from "./components/DayTemplateTabs.vue";
import SwitchManager from "./components/SwitchManager.vue";

export default {
  name: "App",
  components: { DayTemplateTabs, SwitchManager },
  setup() {
    const ws = ref(null);
    const wsConnected = ref(false);
    const pingTimer = ref(null);

    // ✅ 区分两类 loading
    const connectLoading = ref(null);
    const dataLoading = ref(null);

    const deviceToken = "xxxxxxxx";

    // 超时重试定时器
    const refreshTimeout = ref(null);

    // 按键信息
    const switches = ref({
      SwitchInfo: [
        { ch: 1, name: "3F-活动区-1" },
        { ch: 2, name: "3F-过道" },
        { ch: 3, name: "3F-活动区-2" },
        { ch: 4, name: "3F-研发办公区-1" },
        { ch: 5, name: "3F-研发办公区-2" }
      ]
    });

    const updateSwitches = (val) => {
      switches.value = { SwitchInfo: val };
      console.log("更新 SwitchInfo:", switches.value);
    };

    // 日程
    const schedule = ref({ Mon: [], Tue: [], Wed: [], Thu: [], Fri: [], Sat: [], Sun: [] });
    const updateSchedule = (day, val) => { schedule.value[day] = val; };

    // 电池电压 & 电量百分比
    const batteryVoltage = ref(0);
    const batteryPercent = computed(() => {
      const minV = 3.0, maxV = 4.2;
      let percent = ((batteryVoltage.value - minV) / (maxV - minV)) * 100;
      return Math.max(0, Math.min(100, Math.round(percent)));
    });

    // --- Loading 控制 ---
    const showConnectLoading = () => {
      if (!connectLoading.value) {
        connectLoading.value = ElLoading.service({
          lock: true,
          text: "正在连接...",
          background: "rgba(0,0,0,0.7)"
        });
      }
    };

    const hideConnectLoading = () => {
      if (connectLoading.value) {
        connectLoading.value.close();
        connectLoading.value = null;
      }
    };

    const showDataLoading = (title) => {
      if (!dataLoading.value) {
        dataLoading.value = ElLoading.service({
          lock: true,
          text: title,
          background: "rgba(0,0,0,0.7)"
        });
      }
    };

    const hideDataLoading = () => {
      if (dataLoading.value) {
        dataLoading.value.close();
        dataLoading.value = null;
      }
    };

    // --- WebSocket 发送 ---
    const sendWS = (payload) => {
      const str = JSON.stringify(payload);
      console.log("发送 WS:", str);
      if (ws.value && ws.value.readyState === WebSocket.OPEN) ws.value.send(str);
      else console.warn("WebSocket 未连接，无法发送");
    };

    // 保存计划 + 开关信息
    const saveTemplate = () => {
      if (!wsConnected.value) {
        console.warn("WebSocket 未连接，无法保存");
        return;
      }
      showDataLoading("正在保存...");

      const cleanedschedule = JSON.parse(JSON.stringify(schedule.value));
      sendWS({ deviceToken, body: { name: "light_schedule", value: cleanedschedule, requestType: 101 } });

      saveSwitchInfo();
    };

    const saveSwitchInfo = () => {
      if (!wsConnected.value) return;
      const cleanedSwitches = JSON.parse(JSON.stringify(switches.value));
      sendWS({ deviceToken, body: { name: "light_switch_info", value: cleanedSwitches, requestType: 101 } });
    };

    // 通用请求 + 重试逻辑
    const requestWithRetry = (payload, retryFn) => {
      if (!wsConnected.value) return;
      sendWS(payload);

      if (refreshTimeout.value) clearTimeout(refreshTimeout.value);
      refreshTimeout.value = setTimeout(() => {
        console.warn("6 秒无响应，重试请求:", payload.body.name);
        retryFn();
      }, 6000);
    };

    const refreshTemplate = () => {
      if (!wsConnected.value) return;
      showDataLoading("加载中...");
      requestWithRetry(
        { deviceToken, body: { name: "light_schedule", requestType: 104 } },
        refreshTemplate
      );
    };

    const refreshSwitchInfo = () => {
      if (!wsConnected.value) return;
      requestWithRetry(
        { deviceToken, body: { name: "light_switch_info", requestType: 104 } },
        refreshSwitchInfo
      );
    };

    const clearRefreshTimeout = () => {
      if (refreshTimeout.value) {
        clearTimeout(refreshTimeout.value);
        refreshTimeout.value = null;
      }
    };

    const startPing = () => {
      if (pingTimer.value) clearInterval(pingTimer.value);
      const payload = { deviceToken, body: { name: "battery_voltage", requestType: 104 } };
      sendWS(payload);
      pingTimer.value = setInterval(() => sendWS(payload), 5000);
    };

    const connectWS = () => {
      if (ws.value) {
        try { ws.value.close(); } catch { }
        ws.value = null;
      }

      showConnectLoading();

      ws.value = new WebSocket("wss://ezdata2.m5stack.com/ws");

      ws.value.onopen = () => {
        console.log("WebSocket 已连接");
        wsConnected.value = true;
        hideConnectLoading();   // ✅ 连接成功立刻关掉连接 loading
        sendWS({ deviceToken });
        setTimeout(() => {
          refreshTemplate();
          refreshSwitchInfo();
          startPing();
        }, 200);
      };

      ws.value.onmessage = (event) => {
        let msg;
        try { msg = JSON.parse(event.data); } catch { return; }
        if (!msg.body) return;
        console.log("收到 WS:", msg);

        if ((msg.cmd === 101 || msg.cmd === 104) && msg.body.name) {
          switch (msg.body.name) {
            case "light_schedule":
              const newSchedule = typeof msg.body.value === "string" ? JSON.parse(msg.body.value) : msg.body.value;
              schedule.value = {
                Mon: newSchedule.Mon || [], Tue: newSchedule.Tue || [],
                Wed: newSchedule.Wed || [], Thu: newSchedule.Thu || [],
                Fri: newSchedule.Fri || [], Sat: newSchedule.Sat || [],
                Sun: newSchedule.Sun || []
              };
              hideDataLoading();
              ElMessage.success("开关计划同步完成");
              clearRefreshTimeout();
              break;
            case "light_switch_info":
              const newSwitches = typeof msg.body.value === "string" ? JSON.parse(msg.body.value) : msg.body.value;
              switches.value = newSwitches;
              console.log("刷新 SwitchInfo:", switches.value);
              hideDataLoading();
              clearRefreshTimeout();
              ElMessage.success("开关信息同步完成");
              break;
            case "battery_voltage":
              batteryVoltage.value = parseFloat(msg.body.value);
              break;
          }
        }
      };

      ws.value.onerror = (err) => {
        console.error("WebSocket 错误:", err);
        wsConnected.value = false;
      };

      ws.value.onclose = () => {
        console.log("WebSocket 已关闭");
        showConnectLoading();
        wsConnected.value = false;
        setTimeout(() => connectWS(), 3000);
      };
    };

    onMounted(() => { connectWS(); });
    onBeforeUnmount(() => {
      if (ws.value) ws.value.close();
      if (pingTimer.value) clearInterval(pingTimer.value);
      if (refreshTimeout.value) clearTimeout(refreshTimeout.value);
    });

    return {
      schedule, switches,
      saveTemplate, refreshTemplate, updateSchedule,
      batteryVoltage, batteryPercent,
      updateSwitches, saveSwitchInfo, refreshSwitchInfo
    };
  }
};
</script>


<style scoped>
.template-header {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-bottom: 12px;
  font-weight: bold;
  font-size: 16px;
  gap: 12px;
}

.battery-progress {
  margin: 20px auto;
  width: 40%;
  text-align: center;
}

@media (max-width: 1000px) {
  .battery-progress {
    width: 90%;
  }
}

.battery-voltage {
  margin-top: 6px;
  font-size: 13px;
  color: #666;
}

.floating-buttons {
  position: fixed;
  bottom: 20px;
  left: 50%;
  transform: translateX(-50%);
  display: flex;
  gap: 12px;
  background: white;
  border-radius: 30px;
  padding: 8px 16px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.15);
  z-index: 999;
}
</style>
