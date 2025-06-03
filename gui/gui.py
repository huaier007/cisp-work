import tkinter as tk
from tkinter import ttk, filedialog
import subprocess
import os
from datetime import datetime
import psutil  

BASE_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "../prototype"))


MODULES = {
    "chkRaW": os.path.join(BASE_PATH, "chkRaW/loader"),
    "chkLog": os.path.join(BASE_PATH, "chkLog/loader"),
    "chkRS": os.path.join(BASE_PATH, "chkRS/loader"),
}

root = tk.Tk()
root.title("antiVirus 控制面板")
root.geometry("1000x600")  # 可手动调整分辨率
root.minsize(800, 500)

notebook = ttk.Notebook(root)
notebook.pack(fill='both', expand=True)

main_tab = ttk.Frame(notebook)
notebook.add(main_tab, text="控制台")

log_tabs = {}
log_areas = {}
module_entries = {}
module_paths = {}
status_labels = {}
processes = {}
run_counts = {name: 0 for name in MODULES}
module_start_times = {}
runtime_updaters = {}


dashboard_tab = ttk.Frame(notebook)
notebook.add(dashboard_tab, text="仪表盘")

dashboard_table = ttk.Treeview(dashboard_tab,
                               columns=("状态图标", "状态", "次数", "启动时间", "运行时长", "CPU", "内存"),
                               show="headings")
dashboard_table.heading("状态图标", text="状态图标")
dashboard_table.heading("状态", text="状态")
dashboard_table.heading("次数", text="次数")
dashboard_table.heading("启动时间", text="启动时间")
dashboard_table.heading("运行时长", text="运行时长")
dashboard_table.heading("CPU", text="CPU 使用率 (%)")
dashboard_table.heading("内存", text="内存使用率 (%)")

dashboard_table.column("状态图标", width=40, anchor="center")
dashboard_table.column("状态", width=60, anchor="center")
dashboard_table.column("次数", width=80, anchor="center")
dashboard_table.column("启动时间", width=150, anchor="center")
dashboard_table.column("运行时长", width=100, anchor="center")
dashboard_table.column("CPU", width=100, anchor="center")
dashboard_table.column("内存", width=100, anchor="center")

dashboard_table.pack(fill="both", expand=True, padx=10, pady=10)

for name in MODULES:
    dashboard_table.insert("", tk.END, iid=name, values=("🔴", "未运行", "0", "-", "0:00:00", "0.0", "0.0"))

def log(name, msg):
    area = log_areas.get(name)
    if area:
        area.insert(tk.END, msg + "\n")
        area.see(tk.END)

def choose_path(name):
    path = filedialog.askopenfilename(title=f"选择 {name} 模块的目标文件")
    if path:
        module_paths[name].set(path)

def run_module(name):
    path = module_paths[name].get()
    args = module_entries[name].get()
    if os.path.isfile(path):
        try:
            proc = subprocess.Popen([path] + args.split())
            processes[name] = proc
            log(name, f"[+] 模块 {name} 启动成功，参数: {args}")
            run_counts[name] += 1
            dashboard_table.set(name, "次数", str(run_counts[name]))
            dashboard_table.set(name, "状态", "运行中")
            dashboard_table.set(name, "状态图标", "🟢")

            # 启动时间
            start_time = datetime.now()
            module_start_times[name] = start_time
            dashboard_table.set(name, "启动时间", start_time.strftime("%H:%M:%S"))
            dashboard_table.set(name, "运行时长", "0:00:00")

            def update_runtime():
                now = datetime.now()
                duration = now - module_start_times.get(name, now)
                dashboard_table.set(name, "运行时长", str(duration).split('.')[0])
                runtime_updaters[name] = root.after(1000, update_runtime)

            update_runtime()

        except Exception as e:
            log(name, f"[!] 启动失败: {e}")
    else:
        log(name, f"[!] 未找到模块可执行文件: {path}")

def stop_module(name):
    proc = processes.get(name)
    if proc and proc.poll() is None:
        proc.terminate()
        log(name, f"[-] 模块 {name} 已停止")
        dashboard_table.set(name, "状态", "未运行")
        dashboard_table.set(name, "状态图标", "🔴")
        if name in runtime_updaters:
            root.after_cancel(runtime_updaters[name])
            del runtime_updaters[name]
        dashboard_table.set(name, "运行时长", "0:00:00")
        dashboard_table.set(name, "CPU", "0.0")
        dashboard_table.set(name, "内存", "0.0")

def run_all():
    for name in MODULES:
        run_module(name)

def stop_all():
    for name in MODULES:
        stop_module(name)


def refresh_resource_usage():
    for name in MODULES:
        proc = processes.get(name)
        if proc and proc.poll() is None:
            try:
                p = psutil.Process(proc.pid)
                cpu_percent = p.cpu_percent(interval=None)  # 非阻塞
                mem_percent = p.memory_percent()
                dashboard_table.set(name, "CPU", f"{cpu_percent:.1f}")
                dashboard_table.set(name, "内存", f"{mem_percent:.1f}")
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                dashboard_table.set(name, "CPU", "0.0")
                dashboard_table.set(name, "内存", "0.0")
        else:
            dashboard_table.set(name, "CPU", "0.0")
            dashboard_table.set(name, "内存", "0.0")

    root.after(2000, refresh_resource_usage)  

title = ttk.Label(main_tab, text="模块控制区域", font=("Arial", 14))
title.pack(pady=10)

for idx, name in enumerate(MODULES):
    frame = ttk.Frame(main_tab)
    frame.pack(pady=5, padx=20, anchor="w")

    ttk.Label(frame, text=name, width=8).grid(row=0, column=0)

    path_var = tk.StringVar(value=MODULES[name])
    module_paths[name] = path_var
    ttk.Entry(frame, textvariable=path_var, width=40).grid(row=0, column=1)
    ttk.Button(frame, text="选择路径", command=lambda n=name: choose_path(n)).grid(row=0, column=2, padx=5)

    entry = tk.StringVar()
    module_entries[name] = entry
    ttk.Entry(frame, textvariable=entry, width=30).grid(row=0, column=3, padx=5)

    ttk.Button(frame, text="启动", command=lambda n=name: run_module(n)).grid(row=0, column=4, padx=5)
    ttk.Button(frame, text="停止", command=lambda n=name: stop_module(n)).grid(row=0, column=5, padx=5)

 
    tab = ttk.Frame(notebook)
    notebook.add(tab, text=f"日志 - {name}")
    text = tk.Text(tab, height=15)
    text.pack(fill='both', expand=True)
    log_areas[name] = text

bottom_frame = ttk.Frame(main_tab)
bottom_frame.pack(pady=10)

tk.Button(bottom_frame, text="启动全部模块", command=run_all).grid(row=0, column=0, padx=10)
tk.Button(bottom_frame, text="停止全部模块", command=stop_all).grid(row=0, column=1, padx=10)
tk.Button(bottom_frame, text="退出程序", command=root.quit).grid(row=0, column=2, padx=10)

refresh_resource_usage()

root.mainloop()
