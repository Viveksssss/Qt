from PyQt6.QtWidgets import QWidget
from PyQt6.QtGui import QPainter, QColor, QFont
from PyQt6.QtCore import Qt


class LyricsWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.previous = ""
        self.current = ""
        self.next = ""

    def set_lyrics(self, pre_text, cur_text, next_text):
        self.previous = pre_text
        self.current = cur_text
        self.next = next_text
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)

        # 透明背景
        painter.fillRect(self.rect(), QColor(0, 0, 0, 0))

        center_y = self.height() // 2
        line_height = 70

        # 上一行歌词
        pre_font = QFont()
        pre_font.setPointSize(18)
        painter.setFont(pre_font)
        painter.setPen(QColor("#FF66CC"))
        painter.drawText(0, center_y - line_height, self.width(), line_height,
                         Qt.AlignmentFlag.AlignCenter, self.previous)

        # 当前行歌词
        current_font = QFont()
        current_font.setPointSize(25)
        current_font.setBold(True)
        painter.setFont(current_font)
        painter.setPen(QColor("#FF0033"))
        painter.drawText(0, center_y, self.width(), line_height,
                         Qt.AlignmentFlag.AlignCenter, self.current)

        # 下一行歌词
        next_font = QFont()
        next_font.setPointSize(18)
        painter.setFont(next_font)
        painter.setPen(QColor("#FF3399"))
        painter.drawText(0, center_y + line_height, self.width(), line_height,
                         Qt.AlignmentFlag.AlignCenter, self.next)