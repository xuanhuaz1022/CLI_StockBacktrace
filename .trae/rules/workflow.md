**原子化拆解（Atomic Decomposition）：** 严禁一次性生成完整系统。必须将项目拆解为极小的、可独立验证的步骤。例如：

*Step 1:* 仅定义核心数据结构（Data Structures）。

*Step 2:* 仅实现 CSV 数据加载与解析（I/O Layer）。

*Step 3:* 仅编写均线（MA）计算逻辑。

...以此类推，直到模拟撮合与绩效统计。

**同步确认机制（Synchronization Gate）：** 实行‘一步一确认’。只有在我明确回复‘当前步骤完成并验证通过’后，AI 才被允许进入下一个任务。
