import os
import random
from PyQt6.QtWidgets import (QMainWindow, QPushButton, QSlider, QLabel,
                             QListWidget, QListWidgetItem, QHBoxLayout,
                             QWidget, QFileDialog, QMessageBox)
from PyQt6.QtCore import (Qt, QTimer, QPropertyAnimation, QTime,
                          QDateTime, QEasingCurve, QPoint, QSize)
from PyQt6.QtGui import QIcon, QPixmap, QPalette, QColor
from PyQt6.QtMultimedia import QMediaPlayer, QAudioOutput

from lyrices import Lyrics
from lyricswidget import LyricsWidget
from settings import Settings
from resources import resources


class MainWindow(QMainWindow):
    ORDER_MODE = 0
    RANDOM_MODE = 1
    CIRCLE_MODE = 2

    def __init__(self, parent=None):
        super().__init__(parent)
        self.mode = self.ORDER_MODE
        self.last_click_time = 0
        self.music_path = ""

        # 预先定义所有控件变量为 None
        self.output = None
        self.player = None
        self.timer = None
        self.slider = None
        self.process_label = None
        self.lyrics_widget = None
        self.volume_btn = None
        self.volume_slider = None
        self.list_widget = None  # 确保这个被定义
        self.previous_btn = None
        self.play_btn = None
        self.next_btn = None
        self.mode_btn = None
        self.list_btn = None
        self.control_widget = None
        self.animation = None
        self.btn_list = []

        # 先设置一个默认背景色，确保不会一片白
        self.setStyleSheet("QMainWindow { background-color: #2C3E50; }")

        print("开始初始化资源...")
        self.init_resource()
        print("资源初始化完成，开始初始化UI...")
        self.init_ui()
        print("UI初始化完成，开始连接信号...")
        self.connections()
        print("MainWindow初始化完成")

    def init_resource(self):
        self.setMouseTracking(True)
        self.setWindowTitle("EastMusic+")

        print("初始化媒体播放器...")
        # 媒体播放器
        self.output = QAudioOutput()
        self.player = QMediaPlayer()
        self.player.setAudioOutput(self.output)

        # 定时器
        self.timer = QTimer()
        self.timer.setInterval(100)

        print("初始化UI控件...")
        # 控件 - 确保所有控件都赋值给实例变量
        self.slider = QSlider(Qt.Orientation.Horizontal, self)
        self.process_label = QLabel(self)
        self.lyrics_widget = LyricsWidget(self)
        self.volume_btn = QPushButton(self)
        self.volume_slider = QSlider(Qt.Orientation.Vertical, self)
        self.list_widget = QListWidget(self)  # 这个很重要！

        # 控制按钮
        self.previous_btn = QPushButton(self)
        self.play_btn = QPushButton(self)
        self.next_btn = QPushButton(self)
        self.mode_btn = QPushButton(self)
        self.list_btn = QPushButton(self)

        self.btn_list = [self.previous_btn, self.play_btn, self.next_btn,
                         self.mode_btn, self.list_btn]

        # 歌词相关
        self.lyrics = Lyrics()
        self.lyrics_map = {}

        print(f"资源初始化完成 - list_widget: {self.list_widget is not None}")

    def init_ui(self):
        # 设置背景 - 先检查文件是否存在
        bg_path = resources.get_path("background.png")
        if os.path.exists(bg_path):
            self.set_background(bg_path)
            print(f"背景图片加载成功: {bg_path}")
        else:
            # 使用纯色背景作为备选
            self.setStyleSheet("""
                QMainWindow {
                    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                              stop:0 #667eea, stop:1 #764ba2);
                }
            """)
            print(f"背景图片不存在，使用默认背景: {bg_path}")

        # 进度显示
        self.process_label.setText("00:00/00:00")
        palette = self.process_label.palette()
        palette.setColor(QPalette.ColorRole.WindowText, QColor("#FF66CC"))
        self.process_label.setPalette(palette)
        self.process_label.setStyleSheet("color: #FF66CC; font-size: 14px;")

        # 歌词显示控件
        self.lyrics_widget.setFixedSize(700, 300)

        # 播放列表设置
        self.list_widget.setFixedWidth(300)
        self.list_widget.setFixedHeight(400)
        self.list_widget.setStyleSheet("""
                QListWidget {
                    background-color: rgba(255, 255, 255, 0.8);
                    border: none;
                    border-radius: 20px;
                    padding: 10px;
                }
                QListWidget::item {
                    background-color: rgba(255, 255, 255, 0.6);
                    border-radius: 10px;
                    padding: 8px 12px;
                    margin: 4px 0;
                    color: #333;
                    min-height: 50px;
                    font-size: 16px;
                }
                QListWidget::item:hover {
                    background-color: rgba(255, 255, 255, 0.8);
                    color: #000;
                }
                QListWidget::item:selected {
                    background-color: rgba(255, 102, 204, 0.7);
                    color: white;
                    border: 1px solid rgba(255, 102, 204, 0.9);
                }
            """)
        self.list_widget.setFrameShape(QListWidget.Shape.NoFrame)
        self.list_widget.setUniformItemSizes(True)
        self.list_widget.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)

        # 确保列表初始位置在窗口右侧外部
        self.list_widget.move(self.width(), 0)
        self.list_widget.hide()

        # 初始化动画 - 确保在这里创建动画对象
        self.animation = QPropertyAnimation(self.list_widget, b"pos")
        self.animation.setDuration(300)
        self.animation.setEasingCurve(QEasingCurve.Type.OutQuad)

        print(f"动画初始化完成，列表控件: {self.list_widget is not None}, 动画: {self.animation is not None}")

        # 音量按钮
        volume_icon_path = resources.get_path("volume.png")
        if os.path.exists(volume_icon_path):
            self.volume_btn.setIcon(QIcon(volume_icon_path))
        else:
            self.volume_btn.setText("🔊")
        self.volume_btn.setFixedSize(30, 30)
        self.volume_btn.setProperty("status", "yes")
        self.volume_btn.setProperty("volume", 100)
        self.volume_btn.setStyleSheet("""
            QPushButton:hover { background-color: lightpink; }
            QPushButton:pressed { background-color: white; }
            QPushButton { 
                background-color: transparent; 
                border: none;
            }
        """)

        # 音量滑块
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setValue(100)
        self.volume_slider.hide()
        self.volume_slider.setFixedHeight(80)
        self.volume_slider.setStyleSheet("""
            QSlider::groove:vertical {
                background: #ccc;
                border-radius: 5px;
                width: 5px;
                margin: 0 5px;
            }
            QSlider::handle:vertical {
                background: #FF66CC;
                border: 1px solid #FF3399;
                width: 15px;
                height: 15px;
                border-radius: 7px;
                margin: -5px 0;
            }
        """)

        # 播放列表
        self.list_widget.setFixedWidth(300)
        self.list_widget.setFixedHeight(400)
        self.list_widget.setStyleSheet("""
            QListWidget {
                background-color: rgba(255, 255, 255, 0.8);
                border: none;
                border-radius: 20px;
                padding: 10px;
            }
            QListWidget::item {
                background-color: rgba(255, 255, 255, 0.6);
                border-radius: 10px;
                padding: 8px 12px;
                margin: 4px 0;
                color: #333;
                min-height: 50px;
                font-size: 16px;
            }
            QListWidget::item:hover {
                background-color: rgba(255, 255, 255, 0.8);
                color: #000;
            }
            QListWidget::item:selected {
                background-color: rgba(255, 102, 204, 0.7);
                color: white;
                border: 1px solid rgba(255, 102, 204, 0.9);
            }
        """)
        self.list_widget.setFrameShape(QListWidget.Shape.NoFrame)
        self.list_widget.setUniformItemSizes(True)
        self.list_widget.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.list_widget.hide()
        self.list_widget.move(self.width(), 0)

        # 进度条样式
        self.slider.setFixedWidth(400)
        self.slider.setStyleSheet("""
            QSlider::groove:horizontal {
                background: #ccc;
                height: 6px;
                border-radius: 3px;
            }
            QSlider::handle:horizontal {
                background: #FF66CC;
                border: 1px solid #FF3399;
                width: 15px;
                height: 15px;
                border-radius: 7px;
                margin: -5px 0;
            }
            QSlider::sub-page:horizontal {
                background: #FF66CC;
                border-radius: 3px;
            }
        """)

        # 初始化按钮
        self.init_buttons()

        # 确定控件位置
        self.position_widgets()

        # 加载音乐
        self.load_music(Settings.load_last_used_directory())

    def init_buttons(self):
        # 按钮布局
        layout = QHBoxLayout()
        layout.addWidget(self.previous_btn)
        layout.addWidget(self.play_btn)
        layout.addWidget(self.next_btn)
        layout.addWidget(self.mode_btn)
        layout.addWidget(self.list_btn)
        layout.setSpacing(20)

        self.control_widget = QWidget(self)
        self.control_widget.setLayout(layout)

        # 设置按钮样式
        self.set_button_style(self.previous_btn, "previous.png")
        self.set_button_style(self.play_btn, "play.png")
        self.set_button_style(self.next_btn, "next.png")
        self.set_button_style(self.mode_btn, "order.png")
        self.set_button_style(self.list_btn, "list.png")

        # 动画
        self.animation = QPropertyAnimation(self.list_widget, b"pos")
        self.animation.setDuration(300)
        self.animation.setEasingCurve(QEasingCurve.Type.OutQuad)

    def set_button_style(self, button, icon_name):
        button.setFixedSize(50, 50)

        icon_path = resources.get_path(icon_name)
        if os.path.exists(icon_path):
            # 设置较小的图标尺寸，留出边距
            icon_size = 30  # 比按钮小一些
            button.setIcon(QIcon(icon_path))
            button.setIconSize(QSize(icon_size, icon_size))
        else:
            # 备用文本
            icon_texts = {
                "previous.png": "⏮",
                "play.png": "▶",
                "next.png": "⏭",
                "order.png": "🔀",
                "list.png": "📋"
            }
            button.setText(icon_texts.get(icon_name, "?"))
            button.setStyleSheet("""
                QPushButton {
                    background-color: rgba(255, 255, 255, 0.3);
                    border: 2px solid #FF66CC;
                    border-radius: 25px;
                    color: #FF66CC;
                    font-size: 20px;
                    font-weight: bold;
                }
                QPushButton:hover {
                    background-color: #FF66CC;
                    color: white;
                }
                QPushButton:pressed {
                    background-color: white;
                    color: #FF66CC;
                }
            """)
            return

        # 有图标时的样式
        button.setStyleSheet("""
            QPushButton {
                background-color: rgba(255, 255, 255, 0.3);
                border: 2px solid #FF66CC;
                border-radius: 25px;
            }
            QPushButton:hover {
                background-color: #FF66CC;
                border: 2px solid #FF66CC;
            }
            QPushButton:pressed {
                background-color: white;
                border: 2px solid #FF66CC;
            }
            QPushButton:focus {
                outline: none;
            }
        """)

    def set_background(self, image_path):
        """设置背景图片路径"""
        self.background_path = image_path
        self.update_background()

    def update_background(self):
        """更新背景图片尺寸"""
        if hasattr(self, 'background_label'):
            self.background_label.deleteLater()

        self.background_label = QLabel(self)
        pixmap = QPixmap(self.background_path)

        if not pixmap.isNull():
            # 缩放图片以适应窗口，同时保持宽高比
            scaled_pixmap = pixmap.scaled(self.size(), Qt.AspectRatioMode.KeepAspectRatioByExpanding,
                                          Qt.TransformationMode.SmoothTransformation)
            self.background_label.setPixmap(scaled_pixmap)
        else:
            # 使用纯色背景
            self.background_label.setStyleSheet(
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);")

        self.background_label.setGeometry(0, 0, self.width(), self.height())
        self.background_label.lower()

    def position_widgets(self):
        w, h = self.width(), self.height()

        # 控制按钮
        control_width = 350
        control_height = 60
        control_x = (w - control_width) // 2
        control_y = h - 100
        self.control_widget.setGeometry(control_x, control_y, control_width, control_height)

        # 进度条
        slider_width = self.slider.width()
        slider_height = self.slider.height()
        slider_x = (w - slider_width) // 2
        slider_y = h - 150
        self.slider.setGeometry(slider_x, slider_y, slider_width, slider_height)

        # 音量按钮
        volume_width = self.volume_btn.width()
        volume_height = self.volume_btn.height()
        self.volume_btn.setGeometry(slider_x - volume_width - 20, slider_y,
                                    volume_width, volume_height)

        # 音量滑块
        volume_slider_width = 30
        volume_slider_height = 80
        self.volume_slider.setGeometry(slider_x - volume_slider_width - 10,
                                       slider_y - volume_slider_height,
                                       volume_slider_width, volume_slider_height)

        # 进度标签
        self.process_label.setFixedWidth(100)
        self.process_label.setGeometry(slider_x + slider_width + 10, slider_y,
                                       100, 30)

        # 歌词控件
        lyrics_width = 700
        lyrics_height = 300
        lyrics_x = (w - lyrics_width) // 2
        lyrics_y = 80
        self.lyrics_widget.setGeometry(lyrics_x, lyrics_y, lyrics_width, lyrics_height)

        # 播放列表
        self.list_widget.setFixedHeight(h - 100)

    def connections(self):
        """连接所有信号和槽"""
        # 定时器更新播放状态
        self.timer.timeout.connect(self.update_play_state)

        # 列表点击
        self.list_widget.itemClicked.connect(self.on_list_item_clicked)

        # 按钮连接
        for i, btn in enumerate(self.btn_list):
            btn.setProperty("btnType", i)
            btn.clicked.connect(self.on_button_clicked)

        # 进度条和音量设置
        self.setup_sliders()

        # 列表按钮双击事件
        self.list_btn.installEventFilter(self)

        # 启动定时器
        self.timer.start()

    def update_play_state(self):
        """更新播放状态"""
        if self.player.playbackState() == QMediaPlayer.PlaybackState.PlayingState:
            play_icon_path = resources.get_path("pause.png")
            if os.path.exists(play_icon_path):
                self.play_btn.setIcon(QIcon(play_icon_path))
            else:
                self.play_btn.setText("❚❚")
        else:
            play_icon_path = resources.get_path("play.png")
            if os.path.exists(play_icon_path):
                self.play_btn.setIcon(QIcon(play_icon_path))
            else:
                self.play_btn.setText("▶")

    def on_list_item_clicked(self, item):
        """列表项点击事件"""
        if item:
            music_file = f"{self.music_path}/{item.text()}.mp3"
            lyrics_file = f"{self.music_path}/{item.text()}.lrc"

            print(f"正在播放: {music_file}")
            if os.path.exists(music_file):
                # 修复：将字符串路径转换为 QUrl
                from PyQt6.QtCore import QUrl
                music_url = QUrl.fromLocalFile(music_file)
                self.player.setSource(music_url)
                self.player.play()

                # 加载歌词
                if os.path.exists(lyrics_file):
                    self.lyrics.read_lyrics_file(lyrics_file)
                    self.lyrics_map = self.lyrics.get_map()
                    print(f"已加载歌词: {len(self.lyrics_map)} 行")
                else:
                    self.lyrics_map = {}
                    print("未找到歌词文件")
            else:
                print(f"音乐文件不存在: {music_file}")
    def on_button_clicked(self):
        """按钮点击事件"""
        btn = self.sender()
        if not btn:
            return

        btn_type = btn.property("btnType")

        if btn_type == 0:
            self.previous_music()
        elif btn_type == 1:
            self.toggle_play()
        elif btn_type == 2:
            self.next_music()
        elif btn_type == 3:
            self.change_mode()
        elif btn_type == 4:
            self.toggle_playlist()

    def setup_sliders(self):
        """设置进度条和音量条连接"""
        # 进度条
        self.player.durationChanged.connect(self.on_duration_changed)
        self.player.positionChanged.connect(self.on_position_changed)
        self.slider.sliderMoved.connect(self.on_slider_moved)

        # 歌词更新
        self.player.positionChanged.connect(self.update_lyrics)

        # 音量
        self.volume_btn.clicked.connect(self.toggle_volume)
        self.volume_slider.valueChanged.connect(self.on_volume_changed)

    def on_duration_changed(self, duration):
        """持续时间改变"""
        self.slider.setRange(0, duration)
        total_time = QTime(0, 0).addMSecs(duration)
        self.all_time = total_time
        self.process_label.setText(f"00:00/{total_time.toString('mm:ss')}")

    def on_position_changed(self, position):
        """位置改变"""
        self.slider.setValue(position)
        current_time = QTime(0, 0).addMSecs(position)
        if hasattr(self, 'all_time'):
            self.process_label.setText(f"{current_time.toString('mm:ss')}/{self.all_time.toString('mm:ss')}")
        else:
            self.process_label.setText(f"{current_time.toString('mm:ss')}/00:00")

    def on_slider_moved(self, position):
        """进度条拖动"""
        self.player.setPosition(position)

    def toggle_volume(self):
        """切换音量状态"""
        if self.volume_btn.property("status") == "yes":
            current_volume = self.volume_slider.value()
            self.volume_btn.setProperty("volume", current_volume)

            no_volume_icon = resources.get_path("noVolume.png")
            if os.path.exists(no_volume_icon):
                self.volume_btn.setIcon(QIcon(no_volume_icon))
            else:
                self.volume_btn.setText("🔇")

            self.volume_btn.setProperty("status", "no")
            self.output.setVolume(0)
            self.volume_slider.setValue(0)
        else:
            saved_volume = self.volume_btn.property("volume")
            if saved_volume <= 0:
                saved_volume = 50

            volume_icon = resources.get_path("volume.png")
            if os.path.exists(volume_icon):
                self.volume_btn.setIcon(QIcon(volume_icon))
            else:
                self.volume_btn.setText("🔊")

            self.volume_btn.setProperty("status", "yes")
            self.output.setVolume(saved_volume / 100.0)
            self.volume_slider.setValue(saved_volume)

    def on_volume_changed(self, value):
        """音量改变"""
        self.volume_btn.setProperty("volume", value)
        self.output.setVolume(value / 100.0)

        if value == 0 and self.volume_btn.property("status") == "yes":
            self.toggle_volume()
        elif value > 0 and self.volume_btn.property("status") == "no":
            self.toggle_volume()

    def load_music(self, path):
        """加载音乐文件"""
        if not path or not os.path.exists(path):
            # 使用默认音乐目录
            default_path = os.path.expanduser("./music")
            if os.path.exists(default_path):
                path = default_path
            else:
                path = os.path.expanduser("~")

        self.music_path = path

        try:
            files = [f for f in os.listdir(path) if f.lower().endswith('.mp3')]
        except PermissionError:
            QMessageBox.warning(self, "错误", f"没有权限访问文件夹: {path}")
            return
        except Exception as e:
            QMessageBox.warning(self, "错误", f"无法读取文件夹: {str(e)}")
            return

        self.list_widget.clear()
        for file in files:
            base_name = os.path.splitext(file)[0]
            self.list_widget.addItem(base_name)

        if files:
            self.list_widget.setCurrentRow(0)

        Settings.save_last_used_directory(path)
        print(f"已加载 {len(files)} 首音乐文件从: {path}")

    def toggle_play(self):
        """播放/暂停"""
        if self.list_widget.count() == 0:
            QMessageBox.information(self, "提示", "播放列表为空，请先添加音乐文件")
            return

        if self.player.playbackState() == QMediaPlayer.PlaybackState.PlayingState:
            self.player.pause()
        else:
            # 如果没有当前播放的音乐，播放第一首
            if not self.player.source().toString():
                current_item = self.list_widget.currentItem()
                if current_item:
                    music_file = f"{self.music_path}/{current_item.text()}.mp3"
                    if os.path.exists(music_file):
                        self.player.setSource(music_file)
                    else:
                        QMessageBox.warning(self, "错误", f"音乐文件不存在: {music_file}")
                        return
            self.player.play()

    def previous_music(self):
        """上一首"""
        if self.list_widget.count() == 0:
            return

        current_row = self.list_widget.currentRow()
        next_row = 0

        if self.mode == self.ORDER_MODE:
            next_row = current_row - 1 if current_row > 0 else self.list_widget.count() - 1
        elif self.mode == self.RANDOM_MODE:
            while True:
                next_row = random.randint(0, self.list_widget.count() - 1)
                if next_row != current_row or self.list_widget.count() == 1:
                    break
        else:  # CIRCLE_MODE
            next_row = current_row

        self.list_widget.setCurrentRow(next_row)
        self.on_list_item_clicked(self.list_widget.currentItem())

    def next_music(self):
        """下一首"""
        if self.list_widget.count() == 0:
            return

        current_row = self.list_widget.currentRow()
        next_row = 0

        if self.mode == self.ORDER_MODE:
            next_row = current_row + 1 if current_row < self.list_widget.count() - 1 else 0
        elif self.mode == self.RANDOM_MODE:
            while True:
                next_row = random.randint(0, self.list_widget.count() - 1)
                if next_row != current_row or self.list_widget.count() == 1:
                    break
        else:  # CIRCLE_MODE
            next_row = current_row

        self.list_widget.setCurrentRow(next_row)
        self.on_list_item_clicked(self.list_widget.currentItem())

    def change_mode(self):
        """切换播放模式"""
        if self.mode == self.ORDER_MODE:
            self.mode = self.RANDOM_MODE
            mode_icon = resources.get_path("shuffle.png")
            if os.path.exists(mode_icon):
                self.mode_btn.setIcon(QIcon(mode_icon))
            else:
                self.mode_btn.setText("🔀")
        elif self.mode == self.RANDOM_MODE:
            self.mode = self.CIRCLE_MODE
            mode_icon = resources.get_path("circle.png")
            if os.path.exists(mode_icon):
                self.mode_btn.setIcon(QIcon(mode_icon))
            else:
                self.mode_btn.setText("🔁")
        else:
            self.mode = self.ORDER_MODE
            mode_icon = resources.get_path("order.png")
            if os.path.exists(mode_icon):
                self.mode_btn.setIcon(QIcon(mode_icon))
            else:
                self.mode_btn.setText("▶")

    def toggle_playlist(self):
        """显示/隐藏播放列表"""
        print("点击列表按钮")

        # 调试信息：检查所有相关对象
        print(f"list_widget exists: {hasattr(self, 'list_widget')}")
        if hasattr(self, 'list_widget'):
            print(f"list_widget: {self.list_widget}")
            print(f"list_widget is hidden: {self.list_widget.isHidden()}")

        # 确保列表控件存在
        if not hasattr(self, 'list_widget') or self.list_widget is None:
            print("错误: 列表控件未初始化，正在重新创建...")
            self.list_widget = QListWidget(self)
            # 重新设置列表样式和位置
            self.list_widget.setFixedWidth(300)
            self.list_widget.setFixedHeight(400)
            self.list_widget.move(self.width(), 0)
            self.list_widget.hide()

        # 确保动画存在
        if not hasattr(self, 'animation') or self.animation is None:
            print("创建动画对象")
            self.animation = QPropertyAnimation(self.list_widget, b"pos")
            self.animation.setDuration(300)
            self.animation.setEasingCurve(QEasingCurve.Type.OutQuad)

        will_show = self.list_widget.isHidden()
        print(f"切换播放列表: 当前{'隐藏' if will_show else '显示'} -> 将{'显示' if will_show else '隐藏'}")

        # 停止当前动画
        self.animation.stop()

        # 清除之前的完成连接
        try:
            self.animation.finished.disconnect()
        except:
            pass

        if will_show:
            # 显示播放列表
            start_x = self.width()
            end_x = self.width() - self.list_widget.width()
            print(f"显示动画: {start_x} -> {end_x}")

            self.animation.setStartValue(QPoint(start_x, 0))
            self.animation.setEndValue(QPoint(end_x, 0))

            # 显示并开始动画
            self.list_widget.show()
            self.list_widget.raise_()
            self.animation.start()

        else:
            # 隐藏播放列表
            start_x = self.width() - self.list_widget.width()
            end_x = self.width()
            print(f"隐藏动画: {start_x} -> {end_x}")

            self.animation.setStartValue(QPoint(start_x, 0))
            self.animation.setEndValue(QPoint(end_x, 0))

            def on_hide_finished():
                print("隐藏动画完成")
                self.list_widget.hide()
                try:
                    self.animation.finished.disconnect(on_hide_finished)
                except:
                    pass

            self.animation.finished.connect(on_hide_finished)
            self.animation.start()

    def update_lyrics(self, position):
        """更新歌词显示"""
        if not self.lyrics_map:
            # 如果没有歌词，显示占位文本
            self.lyrics_widget.set_lyrics("", "暂无歌词", "")
            return

        # 简单的歌词匹配
        times = sorted(self.lyrics_map.keys())
        current_line = ""
        prev_line = ""
        next_line = ""

        for i, time in enumerate(times):
            if position >= time:
                current_line = self.lyrics_map[time]
                prev_line = self.lyrics_map[times[i - 1]] if i > 0 else ""
                next_line = self.lyrics_map[times[i + 1]] if i < len(times) - 1 else ""

        self.lyrics_widget.set_lyrics(prev_line, current_line, next_line)

    def eventFilter(self, obj, event):
        """事件过滤器"""
        if obj == self.list_btn and event.type() == event.Type.MouseButtonPress:
            if event.button() == Qt.MouseButton.LeftButton:
                current_time = QDateTime.currentMSecsSinceEpoch()
                if current_time - self.last_click_time < 500:
                    directory = QFileDialog.getExistingDirectory(self, "选择音乐文件夹")
                    if directory:
                        self.music_path = directory
                        Settings.save_last_used_directory(directory)
                        self.load_music(directory)
                        self.last_click_time = 0
                        return True
                self.last_click_time = current_time

        return super().eventFilter(obj, event)

    def mousePressEvent(self, event):
        """鼠标点击事件"""
        if (event.button() == Qt.MouseButton.LeftButton and
                not self.list_widget.geometry().contains(event.pos()) and
                self.list_widget.isVisible()):
            self.toggle_playlist()

        super().mousePressEvent(event)

    def mouseMoveEvent(self, event):
        """鼠标移动事件"""
        mouse_pos = event.pos()
        volume_rect = self.volume_btn.geometry()
        slider_rect = self.volume_slider.geometry()

        should_show = (volume_rect.contains(mouse_pos) or
                       (self.volume_slider.isVisible() and slider_rect.contains(mouse_pos)))

        if should_show and not self.volume_slider.isVisible():
            self.volume_slider.show()
            self.volume_slider.raise_()
        elif not should_show and self.volume_slider.isVisible():
            self.volume_slider.hide()

        super().mouseMoveEvent(event)

    def resizeEvent(self, event):
        """窗口大小改变时更新背景"""
        self.position_widgets()
        if hasattr(self, 'background_path'):
            self.update_background()
        super().resizeEvent(event)