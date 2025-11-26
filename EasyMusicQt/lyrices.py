import re
from PyQt6.QtCore import QFile, QIODevice


class Lyrics:
    def __init__(self, lyrics_path=None):
        self.lyrics_path = lyrics_path
        self.list_lyrics_text = []
        self.list_lyrics_time = []
        self.lyrics_map = {}

    def read_lyrics_file(self, lyrics_path):
        self.lyrics_path = lyrics_path
        self.lyrics_map.clear()
        self.list_lyrics_text.clear()
        self.list_lyrics_time.clear()

        file = QFile(lyrics_path)
        if not file.open(QIODevice.OpenModeFlag.ReadOnly | QIODevice.OpenModeFlag.Text):
            return False

        line = ""
        while not file.atEnd():
            line = file.readLine().data().decode('utf-8').strip()
            if line:
                self.analysis_lyrics_file(line)

        file.close()
        return True

    def analysis_lyrics_file(self, line):
        if not line:
            return False

        pattern = r"\[(\d+)?:(\d+)?(\.\d+)?\](.*)?"
        match = re.match(pattern, line)

        if match:
            minutes = int(match.group(1)) if match.group(1) else 0
            seconds = int(match.group(2)) if match.group(2) else 0
            total_time = minutes * 60000 + seconds * 1000

            current_text = match.group(4) if match.group(4) else ""

            self.list_lyrics_text.append(current_text)
            self.list_lyrics_time.append(total_time)
            self.lyrics_map[total_time] = current_text
            return True

        return False

    def get_list_lyrics_text(self):
        return self.list_lyrics_text

    def get_list_lyrics_time(self):
        return self.list_lyrics_time

    def get_map(self):
        return self.lyrics_map