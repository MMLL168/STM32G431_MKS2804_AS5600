import math
import queue
import threading
import time
import tkinter as tk
from tkinter import messagebox, ttk

try:
    import serial
    import serial.tools.list_ports
except ImportError as exc:  # pragma: no cover
    raise SystemExit(
        "Missing dependency: pyserial. Install with: pip install pyserial"
    ) from exc


class SerialReader(threading.Thread):
    def __init__(self, port, baudrate, output_queue, stop_event):
        super().__init__(daemon=True)
        self.port = port
        self.baudrate = baudrate
        self.output_queue = output_queue
        self.stop_event = stop_event

    def run(self):
        try:
            with serial.Serial(self.port, self.baudrate, timeout=0.2) as ser:
                while not self.stop_event.is_set():
                    raw = ser.readline()
                    if not raw:
                        continue
                    try:
                        line = raw.decode("ascii", errors="ignore").strip()
                    except UnicodeDecodeError:
                        continue
                    if line:
                        self.output_queue.put(("line", line))
        except Exception as exc:  # pragma: no cover
            self.output_queue.put(("error", str(exc)))


class AngleMonitorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Knob / Follower Angle Monitor")
        self.root.geometry("920x680")
        self.root.minsize(860, 620)
        self.root.configure(bg="#ece6db")

        self.queue = queue.Queue()
        self.stop_event = None
        self.reader = None
        self.last_frame_time = 0.0
        self.knob_deg = 0.0
        self.follow_deg = 0.0

        self.port_var = tk.StringVar()
        self.baud_var = tk.StringVar(value="115200")
        self.status_var = tk.StringVar(value="Disconnected")
        self.knob_angle_var = tk.StringVar(value="0.0 deg")
        self.follow_angle_var = tk.StringVar(value="0.0 deg")
        self.error_var = tk.StringVar(value="0.0 deg")
        self.pulse_var = tk.StringVar(value="0 us")
        self.frame_var = tk.StringVar(value="0")
        self.last_line_var = tk.StringVar(value="-")

        self._build_ui()
        self.refresh_ports()
        self.root.after(50, self.process_queue)
        self.root.after(500, self.update_link_status)

    def _build_ui(self):
        root = self.root
        root.columnconfigure(0, weight=1)
        root.rowconfigure(2, weight=1)

        style = ttk.Style()
        style.theme_use("clam")
        style.configure("App.TFrame", background="#ece6db")
        style.configure("Card.TLabelframe", background="#f8f4eb", foreground="#3d3325")
        style.configure("Card.TLabelframe.Label", background="#f8f4eb", foreground="#3d3325")
        style.configure("Status.TLabel", background="#ece6db", foreground="#4a4032")
        style.configure("Hint.TLabel", background="#ece6db", foreground="#5f5548")
        style.configure("Value.TLabel", background="#f8f4eb", foreground="#1f1a15")

        top = ttk.Frame(root, padding=12, style="App.TFrame")
        top.grid(row=0, column=0, sticky="ew")
        top.columnconfigure(5, weight=1)

        ttk.Label(top, text="COM").grid(row=0, column=0, sticky="w")
        self.port_combo = ttk.Combobox(top, textvariable=self.port_var, width=14, state="readonly")
        self.port_combo.grid(row=0, column=1, padx=(6, 12), sticky="w")

        ttk.Button(top, text="Refresh", command=self.refresh_ports).grid(row=0, column=2, padx=(0, 12))

        ttk.Label(top, text="Baud").grid(row=0, column=3, sticky="w")
        ttk.Entry(top, textvariable=self.baud_var, width=10).grid(row=0, column=4, padx=(6, 12), sticky="w")

        self.connect_button = ttk.Button(top, text="Connect", command=self.toggle_connection)
        self.connect_button.grid(row=0, column=5, sticky="w")

        status = ttk.Label(root, textvariable=self.status_var, padding=(12, 0, 12, 8), style="Status.TLabel")
        status.grid(row=1, column=0, sticky="ew")

        cards = ttk.Frame(root, padding=12, style="App.TFrame")
        cards.grid(row=2, column=0, sticky="nsew")
        cards.columnconfigure(0, weight=1)
        cards.columnconfigure(1, weight=1)
        cards.rowconfigure(0, weight=1)

        self.knob_canvas = self._build_visual_card(
            cards,
            0,
            0,
            "旋鈕角度",
            self.knob_angle_var,
            accent="#d8732a",
            mode="knob",
        )
        self.follow_canvas = self._build_visual_card(
            cards,
            0,
            1,
            "跟隨馬達角度",
            self.follow_angle_var,
            accent="#2b7a78",
            mode="motor",
        )
        self._update_dial(self.knob_canvas, 0.0, "#d8732a", "knob")
        self._update_dial(self.follow_canvas, 0.0, "#2b7a78", "motor")

        detail = ttk.Frame(root, padding=12, style="App.TFrame")
        detail.grid(row=3, column=0, sticky="ew")
        for idx in range(4):
            detail.columnconfigure(idx, weight=1)

        self._build_detail(detail, 0, "角度誤差", self.error_var)
        self._build_detail(detail, 1, "PWM 脈寬", self.pulse_var)
        self._build_detail(detail, 2, "Frame Count", self.frame_var)
        self._build_detail(detail, 3, "Last Line", self.last_line_var)

        hint = ttk.Label(
            root,
            padding=(12, 0, 12, 12),
            text="Telemetry format: ANG,<knob_deg10>,<follower_deg10>,<err_deg10>,<pulse_us>",
            style="Hint.TLabel",
        )
        hint.grid(row=4, column=0, sticky="ew")

    def _build_visual_card(self, parent, row, column, title, value_var, accent, mode):
        frame = ttk.LabelFrame(parent, text=title, padding=16, style="Card.TLabelframe")
        frame.grid(row=row, column=column, padx=8, pady=8, sticky="nsew")
        frame.columnconfigure(0, weight=1)
        frame.rowconfigure(0, weight=1)
        canvas = tk.Canvas(
            frame,
            width=360,
            height=270,
            bg="#f8f4eb",
            bd=0,
            highlightthickness=0,
        )
        canvas.grid(row=0, column=0, sticky="nsew")
        self._draw_dial_base(canvas, accent, mode)
        ttk.Label(
            frame,
            textvariable=value_var,
            font=("Segoe UI", 27, "bold"),
            anchor="center",
            style="Value.TLabel",
        ).grid(row=1, column=0, sticky="ew", pady=(8, 0))
        return canvas

    def _draw_dial_base(self, canvas, accent, mode):
        cx, cy = 180, 124
        outer_r = 96
        inner_r = 74
        canvas.create_rectangle(8, 8, 352, 252, fill="#fbf8f2", outline="#ddd2c1", width=1)
        canvas.create_oval(
            cx - outer_r,
            cy - outer_r,
            cx + outer_r,
            cy + outer_r,
            fill="#efe6d5",
            outline="#cabca8",
            width=3,
        )
        canvas.create_oval(
            cx - inner_r,
            cy - inner_r,
            cx + inner_r,
            cy + inner_r,
            fill="#fffdf8",
            outline="#dacdb9",
            width=2,
        )

        for deg in range(0, 360, 30):
            angle = math.radians(deg - 90)
            x1 = cx + math.cos(angle) * (inner_r - 6)
            y1 = cy + math.sin(angle) * (inner_r - 6)
            x2 = cx + math.cos(angle) * (outer_r - 10)
            y2 = cy + math.sin(angle) * (outer_r - 10)
            width = 3 if deg % 90 == 0 else 2
            color = "#8e7f6d" if deg % 90 == 0 else "#b8a894"
            canvas.create_line(x1, y1, x2, y2, fill=color, width=width, capstyle=tk.ROUND)

        canvas.create_arc(
            cx - outer_r + 6,
            cy - outer_r + 6,
            cx + outer_r - 6,
            cy + outer_r - 6,
            start=90,
            extent=0,
            style=tk.ARC,
            outline=accent,
            width=9,
            tags="progress",
        )

        if mode == "knob":
            canvas.create_text(cx, 34, text="KNOB", fill="#875c34", font=("Segoe UI", 12, "bold"))
            canvas.create_oval(cx - 24, cy - 24, cx + 24, cy + 24, fill="#d7b491", outline="#9d6c3c", width=2)
            canvas.create_oval(cx - 12, cy - 12, cx + 12, cy + 12, fill="#fffaf2", outline="#9d6c3c", width=2)
            canvas.create_line(cx, cy - 10, cx, cy - 36, fill="#9d6c3c", width=4, capstyle=tk.ROUND)
        else:
            canvas.create_text(cx, 34, text="FOLLOWER", fill="#215b59", font=("Segoe UI", 12, "bold"))
            for deg in range(0, 360, 60):
                angle = math.radians(deg - 90)
                x = cx + math.cos(angle) * 50
                y = cy + math.sin(angle) * 50
                canvas.create_rectangle(x - 8, y - 12, x + 8, y + 12, fill="#b9d9d8", outline="#2b7a78", width=2)
            canvas.create_oval(cx - 30, cy - 30, cx + 30, cy + 30, fill="#dff1f0", outline="#2b7a78", width=2)
            canvas.create_line(cx, cy, cx, cy - 28, fill="#2b7a78", width=4, tags="rotor")

        canvas.create_line(cx, cy, cx, cy - 66, fill=accent, width=6, capstyle=tk.ROUND, tags="needle")
        canvas.create_oval(cx - 8, cy - 8, cx + 8, cy + 8, fill=accent, outline="#ffffff", width=2)
        canvas.create_text(cx, 224, text="0.0°", fill="#4a4032", font=("Consolas", 13, "bold"), tags="dial_text")

    def _build_detail(self, parent, column, title, value_var):
        frame = ttk.LabelFrame(parent, text=title, padding=12, style="Card.TLabelframe")
        frame.grid(row=0, column=column, padx=6, pady=6, sticky="ew")
        ttk.Label(frame, textvariable=value_var, font=("Consolas", 13, "bold")).grid(
            row=0, column=0, sticky="w"
        )

    def _update_dial(self, canvas, angle_deg, accent, mode):
        cx, cy = 180, 124
        needle_r = 66
        angle = math.radians((angle_deg % 360.0) - 90.0)
        x = cx + math.cos(angle) * needle_r
        y = cy + math.sin(angle) * needle_r

        canvas.coords("needle", cx, cy, x, y)
        canvas.itemconfigure("progress", extent=-(angle_deg % 360.0))
        canvas.itemconfigure("dial_text", text=f"{angle_deg:.1f}°")
        canvas.itemconfigure("needle", fill=accent)
        canvas.itemconfigure("progress", outline=accent)

        if mode == "motor":
            rotor_r = 28
            rx = cx + math.cos(angle) * rotor_r
            ry = cy + math.sin(angle) * rotor_r
            canvas.coords("rotor", cx, cy, rx, ry)

    def refresh_ports(self):
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo["values"] = ports
        if ports and self.port_var.get() not in ports:
            self.port_var.set(ports[0])
        elif not ports:
            self.port_var.set("")

    def toggle_connection(self):
        if self.reader is None:
            self.connect()
        else:
            self.disconnect()

    def connect(self):
        if not self.port_var.get():
            messagebox.showwarning("Port", "Please select a COM port first.")
            return
        try:
            baudrate = int(self.baud_var.get())
        except ValueError:
            messagebox.showwarning("Baud", "Baud rate must be an integer.")
            return

        self.stop_event = threading.Event()
        self.reader = SerialReader(self.port_var.get(), baudrate, self.queue, self.stop_event)
        self.reader.start()
        self.last_frame_time = 0.0
        self.connect_button.config(text="Disconnect")
        self.status_var.set(f"Connected: {self.port_var.get()} @ {baudrate}")

    def disconnect(self):
        if self.stop_event is not None:
            self.stop_event.set()
        self.reader = None
        self.stop_event = None
        self.connect_button.config(text="Connect")
        self.status_var.set("Disconnected")

    def process_queue(self):
        while True:
            try:
                kind, payload = self.queue.get_nowait()
            except queue.Empty:
                break

            if kind == "error":
                self.status_var.set(f"Serial error: {payload}")
                self.disconnect()
                break

            self.last_line_var.set(payload)
            self._handle_line(payload)

        self.root.after(50, self.process_queue)

    def _handle_line(self, line):
        if not line.startswith("ANG,"):
            return

        parts = line.split(",")
        if len(parts) != 5:
            return

        try:
            knob_deg10 = int(parts[1])
            follow_deg10 = int(parts[2])
            err_deg10 = int(parts[3])
            pulse_us = int(parts[4])
        except ValueError:
            return

        self.knob_angle_var.set(f"{knob_deg10 / 10.0:.1f} deg")
        self.follow_angle_var.set(f"{follow_deg10 / 10.0:.1f} deg")
        self.error_var.set(f"{err_deg10 / 10.0:.1f} deg")
        self.pulse_var.set(f"{pulse_us} us")
        self.knob_deg = knob_deg10 / 10.0
        self.follow_deg = follow_deg10 / 10.0
        self._update_dial(self.knob_canvas, self.knob_deg, "#d8732a", "knob")
        self._update_dial(self.follow_canvas, self.follow_deg, "#2b7a78", "motor")

        current_frames = int(self.frame_var.get())
        self.frame_var.set(str(current_frames + 1))
        self.last_frame_time = time.monotonic()

    def update_link_status(self):
        if self.reader is not None and self.last_frame_time > 0.0:
            age = time.monotonic() - self.last_frame_time
            if age > 0.5:
                self.status_var.set(
                    f"Connected: {self.port_var.get()} @ {self.baud_var.get()} (waiting for telemetry)"
                )
            else:
                self.status_var.set(
                    f"Connected: {self.port_var.get()} @ {self.baud_var.get()} (live)"
                )
        self.root.after(500, self.update_link_status)


def main():
    root = tk.Tk()
    ttk.Style().theme_use("clam")
    app = AngleMonitorApp(root)

    def on_close():
        app.disconnect()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_close)
    root.mainloop()


if __name__ == "__main__":
    main()
