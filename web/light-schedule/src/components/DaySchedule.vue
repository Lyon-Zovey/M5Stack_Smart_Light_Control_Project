<template>
  <div class="day-schedule">
    <div v-for="(row, rowIndex) in localSchedule" :key="rowIndex" class="row-wrapper" :class="{ selected: row.selected }"
      @click="selectRow(rowIndex)">
      <div class="row-head">
        <el-time-picker v-model="row.time" value-format="HH:mm" format="HH:mm" placeholder="选择时间" size="small"
          @change="emitUpdate" :editable="false" :clearable="false" :popper-append-to-body="false" />
        <el-button type="danger" size="small" @click="confirmDeleteItem(rowIndex)">删除</el-button>
      </div>

      <div class="cmd-list">
        <div v-for="cmd in row.cmd" :key="cmd.ch" class="cmd-item">
          <span class="cmd-label">
            <span :class="['status-dot', `state-${cmd.onoff}`]"></span>
            {{ getSwitchName(cmd.ch) }}
          </span>
          <div class="cmd-actions">
            <el-select v-model="cmd.onoff" size="small" class="state-select" @change="emitUpdate">
              <el-option label="ON" value="on" />
              <el-option label="IDLE" value="idle" />
              <el-option label="OFF" value="off" />
            </el-select>
            <!-- 传 rowIndex 和 cmd.ch -->
            <el-button type="danger" size="small" @click.stop="confirmDeleteCmd(rowIndex, cmd.ch)">删除</el-button>
          </div>
        </div>
      </div>
    </div>

    <div class="actions">
      <el-button type="primary" size="small" @click="addItem">+ 添加计划</el-button>

      <template v-if="showCmdInput">
        <el-select v-model="newCmdCh" placeholder="选择开关" size="small" class="new-ch-input">
          <el-option v-for="sw in switches" :key="sw.ch" :label="sw.name" :value="sw.ch" />
        </el-select>
        <el-button type="primary" size="small" @click="handleAddCmd">确定</el-button>
        <el-button size="small" @click="cancelAddCmd">取消</el-button>
      </template>

      <el-button type="success" size="small" @click="showAddCmdInput">+ 添加开关</el-button>
    </div>
  </div>
</template>

<script>
import { ElMessageBox } from "element-plus";

export default {
  name: "DaySchedule",
  props: {
    schedule: { type: Array, default: () => [] },
    switches: { type: Array, default: () => [] }
  },
  data() {
    return {
      localSchedule: [],
      showCmdInput: false,
      newCmdCh: ""
    };
  },
  watch: {
    schedule: {
      deep: true,
      immediate: true,
      handler(newVal) {
        this.localSchedule = (newVal || []).map(row => ({
          time: row.time || "12:00",
          cmd: row.cmd ? row.cmd.map(c => ({ ...c })) : [],
          selected: false // 本地选中状态
        }));
      }
    }
  },
  methods: {
    selectRow(index) {
      this.localSchedule.forEach((row, i) => {
        row.selected = i === index;
      });
    },
    addItem() {
      const newRow = {
        time: "12:00",
        cmd: [],
        selected: false
      };
      this.localSchedule.push(newRow);
      this.emitUpdate();
    },
    confirmDeleteItem(index) {
      ElMessageBox.confirm("确定要删除该计划吗？", "提示", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(() => this.deleteItem(index));
    },
    deleteItem(index) {
      this.localSchedule.splice(index, 1);
      this.emitUpdate();
    },
    // ✅ 修改：只删除当前行的开关
    confirmDeleteCmd(rowIndex, cmdCh) {
      ElMessageBox.confirm("确定要删除该开关吗？", "提示", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(() => {
        this.localSchedule[rowIndex].cmd =
          this.localSchedule[rowIndex].cmd.filter(c => c.ch !== cmdCh);
        this.emitUpdate();
      });
    },
    showAddCmdInput() {
      const hasSelected = this.localSchedule.some(row => row.selected);
      if (!hasSelected) {
        this.$message.warning("请先选择要插入开关的计划");
        return;
      }
      this.showCmdInput = true;
    },
    cancelAddCmd() {
      this.showCmdInput = false;
      this.newCmdCh = "";
    },
    handleAddCmd() {
      const ch = this.newCmdCh;
      if (!ch) return;

      this.localSchedule.forEach(row => {
        if (row.selected) {
          const exists = row.cmd.some(c => c.ch === ch);
          if (!exists) row.cmd.push({ ch, onoff: "idle" });
        }
      });

      this.showCmdInput = false;
      this.newCmdCh = "";
      this.emitUpdate();
    },
    emitUpdate() {
      // 完整深拷贝，避免引用传递问题
      const cleaned = JSON.parse(JSON.stringify(this.localSchedule.map(row => ({
        time: row.time,
        cmd: row.cmd.map(c => ({ ch: c.ch, onoff: c.onoff }))
      }))));
      this.$emit("update", cleaned);
    },
    getSwitchName(ch) {
      try {
        const sw = this.switches.find(s => s.ch == ch);
        return sw ? sw.name : ch;
      } catch (error) {
        console.log(error)
      }
      return `CH-${ch}`;
    }
  }
};
</script>


<style scoped>
.day-schedule {
  display: flex;
  flex-direction: column;
  gap: 12px;
  font-size: 14px;
}

.row-wrapper {
  display: flex;
  flex-direction: column;
  gap: 8px;
  padding: 0;
  border: 1px solid #ebeef5;
  border-radius: 6px;
  overflow: hidden;
  cursor: pointer;
}

.row-wrapper.selected {
  border-color: #409eff;
  background-color: #f0f9ff;
}

.row-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 6px 12px;
  background-color: #f5f7fa;
  border-bottom: 1px solid #ebeef5;
  font-weight: 500;
}

.cmd-list {
  display: flex;
  flex-direction: column;
  gap: 6px;
  padding: 6px 6px 12px;
}

.cmd-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 6px;
  border-bottom: 1px solid #e3e3e3;
}

.cmd-item:last-child {
  border-bottom: none;
}

.cmd-label {
  flex-shrink: 0;
  /* width: 150px; */
  font-weight: 500;
  display: flex;
  align-items: center;
}

.cmd-actions {
  display: flex;
  align-items: center;
  gap: 8px;
}

.state-select {
  width: 100px;
  text-align: center;
  font-weight: 500;
}

.status-dot {
  display: inline-block;
  width: 12px;
  height: 12px;
  border-radius: 50%;
  margin-right: 6px;
  vertical-align: middle;
  /* 去掉边框 */
  border: none;
  /* 发光效果 */
  box-shadow: 0 0 6px rgba(0, 0, 0, 0.1);
  transition: background-color 0.3s, box-shadow 0.3s;
}

.state-on {
  background-color: #67c23a;
  box-shadow: 0 0 8px #67c23a80;
}

.state-idle {
  background-color: #909399;
  box-shadow: 0 0 6px #90939980;
}

.state-off {
  background-color: #f56c6c;
  box-shadow: 0 0 8px #f56c6c80;
}



.actions {
  display: flex;
  align-items: center;
  gap: 12px;
  flex-wrap: wrap;
  justify-content: center;
  margin-top: 12px;
}

.new-ch-input {
  width: 120px;
}
</style>
