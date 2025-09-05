<template>
    <div class="switch-manager">
        <h3>开关管理</h3>

        <!-- 按键列表 -->
        <el-table :data="localSwitches" border style="width: 100%">
            <el-table-column prop="ch" label="ch" width="80">
                <template #default="scope">
                    <el-input v-model="scope.row.ch" size="small" @change="emitUpdate" />
                </template>
            </el-table-column>

            <el-table-column prop="name" label="名称" min-width="150">
                <template #default="scope">
                    <el-input v-model="scope.row.name" size="small" @change="emitUpdate" />
                </template>
            </el-table-column>

            <el-table-column label="操作" width="80">
                <template #default="scope">
                    <el-button type="danger" size="small" @click="deleteSwitch(scope.$index)">删除</el-button>
                </template>
            </el-table-column>
        </el-table>

        <!-- 添加新按键 -->
        <div class="add-switch">
            <el-input type="number" v-model="newCh" placeholder="序号 ch" size="small" style="width: 100px;" />
            <el-input v-model="newName" placeholder="按键名称" size="small" style="width: 220px;" />
            <el-button type="primary" size="small" @click="addSwitch">添加</el-button>
        </div>
    </div>
</template>

<script>
export default {
    name: "SwitchManager",
    props: {
        switches: {
            type: Array,
            default: () => []
        }
    },
    data() {
        return {
            localSwitches: [],
            newCh: null,
            newName: ""
        };
    },
    watch: {
        switches: {
            deep: true,
            immediate: true,
            handler(val) {
                this.localSwitches = val.map(s => ({ ...s }));
            }
        }
    },
    methods: {
        addSwitch() {
            const ch = Number(this.newCh);
            const name = this.newName.trim();
            if (!ch || !name) return;

            // 如果存在 ch，更新 name
            const exists = this.localSwitches.find(s => s.ch === ch);
            if (exists) {
                exists.name = name;
            } else {
                this.localSwitches.push({ ch, name });
            }

            this.newCh = null;
            this.newName = "";
            this.emitUpdate();
        },
        deleteSwitch(index) {
            this.localSwitches.splice(index, 1);
            this.emitUpdate();
        },
        emitUpdate() {
            this.$emit("update-switches", JSON.parse(JSON.stringify(this.localSwitches)));
        }
    }
};
</script>

<style scoped>
.switch-manager {
    padding: 12px;
    margin: auto;
    margin-bottom: 68px;
    width: 40%;
}


@media (max-width: 1000px) {
    .switch-manager {
        width: 90%;
    }
}


.add-switch {
    margin-top: 12px;
    display: flex;
    align-items: center;
    gap: 8px;
    justify-content: center;
}
</style>