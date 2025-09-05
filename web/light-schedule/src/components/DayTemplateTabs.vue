<template>
  <div class="template-tabs-wrapper">

    <!-- 星期选择器（Segmented） -->
    <el-segmented v-model="activeDay" :options="days" size="large" class="day-segmented" />

    <!-- 当前选择的日程 -->
    <day-schedule :switches="switches" :schedule="schedule[activeDay]"
      @update="(val) => $emit('update-schedule', activeDay, val)" class="day-schedule-wrapper" />
  </div>
</template>

<script>
import DaySchedule from "./DaySchedule.vue";

export default {
  name: "DayTemplateTabs",
  components: { DaySchedule },
  props: {
    schedule: Object,
    switches: Array
  },
  data() {
    return {
      activeDay: "Mon",
      days: ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]
    };
  }
};
</script>

<style scoped>
.template-tabs-wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 16px;
  margin-bottom: 60px;
}

.day-segmented {
  width: fit-content;
  margin: 12px auto 0;
}

.day-schedule-wrapper {
  width: 40%;
  margin-top: 12px;
}

@media (max-width: 1000px) {
  .day-schedule-wrapper {
    width: 90%;
  }
}
</style>
