from PyQt6.QtCore import QSettings
from PyQt6.QtCore import QDir


class Settings:
    @staticmethod
    def save_last_used_directory(path):
        settings = QSettings("Vivek", "myMusic")
        settings.setValue("LastUsedDirectory", path)

    @staticmethod
    def load_last_used_directory():
        settings = QSettings("Vivek", "myMusic")
        return settings.value("LastUsedDirectory", QDir.homePath(), type=str)